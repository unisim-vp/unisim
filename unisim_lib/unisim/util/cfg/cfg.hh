/*
 *  Copyright (c) 2024,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

// A dynamic and incremental control flow graph (CFG) analysis.
//
// This is an implementation of algorithms from the following paper:
//
//   Andrei Rimsa, José Nelson Amaral, and Fernando Magno Quintão Pereira:
//   Practical dynamic reconstruction of control flow graphs.
//   In Journal of Software Practice and Experience, Volume 51, Number 2,
//   February 2021.

#ifndef __UNISIM_UTIL_CFG_CFG_HH__
#define __UNISIM_UTIL_CFG_CFG_HH__

#include <unisim/util/json/json.hh>
#include <unisim/util/ostream/ostream.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/instruction_collecting.hh>
#include <unordered_map>
#include <map>
#include <vector>
#include <array>
#include <set>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace unisim {
namespace util {
namespace cfg {

#if 0
struct CFGBuilderConfig
{
	typedef uint64_t ADDRESS;     // address type
	enum { MIN_OPCODE_SIZE = 4 }; // minimum opcode size (to optimize cache usage, see CACHE_SIZE)
	enum { MAX_OPCODE_SIZE = 4 }; // maximum opcode size (zero means unlimited)
	enum { CACHE_SIZE = 8 };      // set to > 0 to enable caching (per node) of the next working node based on the current working node and the incoming group of instructions
	enum { DEBUG = 0 };           // set to 1 for debugging
	enum { CHECK = 0 };           // set to 1 to verify expensive assertions
};

Builder<CFGBuilderConfig> cfg_builder;
...
std::ifstream previous_backup_file("cfgs.json");
if(!(previous_backup_file >> cfg_builder))
{
	std::cerr << "Loading failed" << std::endl;
	exit(1);
}
...
cfg_builder.Process(group1);
cfg_builder.Process(group2);
cfg_builder.Process(group3);
...
cfg_builder.End();
...
std::ofstream backup_file("cfgs.json");
backup_file << cfg_builder;
...
std::ostream graphviz_file("cfgs.dot")
cfg_builder.OutputGraph(graphviz_file);
#endif

///////////////////////////////////////////////////////////////////////////////
//                            Enumeration types                              //
///////////////////////////////////////////////////////////////////////////////

// Instruction types (see Definition 1 of paper)
enum
{
	// standard instruction flows to the next instruction
	STANDARD = unisim::service::interfaces::InstructionInfoBase::STANDARD,
	
	// jump(@target, mode: (direct | indirect)): unconditionally jumps to @target address, either directly or indirectly
	JUMP = unisim::service::interfaces::InstructionInfoBase::JUMP,
	
	// branch(@target, @fallthrough, mode: (direct | indirect)): conditionally branches to @target or @fallthrough address,
	//                                                           either directly or indirectly
	BRANCH = unisim::service::interfaces::InstructionInfoBase::BRANCH,
	
	// call(@target, @fallthrough, mode: (direct | indirect)): invokes the function stored at the @target address,
	//                                                         either directly or indirectly
	CALL = unisim::service::interfaces::InstructionInfoBase::CALL,
	
	// return instruction transfers control back to caller
	RETURN = unisim::service::interfaces::InstructionInfoBase::RETURN    
};

// Control flow instruction modes (see Definition 1 of paper)
enum
{
	DIRECT   = unisim::service::interfaces::InstructionInfoBase::DIRECT  , // direct
	INDIRECT = unisim::service::interfaces::InstructionInfoBase::INDIRECT  // indirect
};

// Node categories (see Definition 3 of paper)
// Note: added "unknown" category for indirect target node
enum
{
	ENTRY   = 0, // entry: marks the start of the CFG
	
	BLOCK   = 1, // block(group, calls, signals): is a basic block that contains:
	             //   - a group, an ordered sequence of instructions,
	             //     consecutive in the program, containing at least one instruction.
	             //   - a map of calls that associates the addresses of functions with pairs (CFG, count).
	             //     The first element in the pair is the CFG of a function, and the second is the number
	             //     of times that function was invoked by a call instruction in the group.
	             //   - a map of signals, similar to the map of calls, except that keys are signal ids,
	             //     and the CFG, in the pair (CFG, count) is a signal handler with how many times
	             //     it was invoked.
	             
	PHANTOM = 2, // phantom(@addr): is an undiscovered node represented by its address.
	
	EXIT    = 3, // exit: marks the return of control to the caller of this CFG.
	
	HALT    = 4, // halt: marks the stop of the execution of the program -- no further instructions can be
	             //       executed from this point forward.
	
	UNKNOWN = 5  // unknown: is the target of an indirect jump, branch or call.
};

// Edge tag
enum
{
	EMPTY       = 0, // nothing special
	TARGET      = 1, // target branch outcome
	FALLTHROUGH = 2  // fallthrough branch outcome
};

// Extension type
enum ExtensionType
{
	NODE_EXT = 0, // node extension
	EDGE_EXT = 1, // edge extension
	CFG_EXT  = 2  // CFG extension
};

///////////////////////////////////////////////////////////////////////////////
//                          Forward declarations                             //
///////////////////////////////////////////////////////////////////////////////

struct Type;
struct Mode;
struct Category;
template <typename CONFIG> struct Config;
struct InstructionBase;
template <unsigned MAX_OPCODE_SIZE> struct Opcode;
template <typename CONFIG> struct Instruction;
template <typename CONFIG> struct StandardInstruction;
template <typename CONFIG> struct JumpInstruction;
template <typename CONFIG> struct BranchInstruction;
template <typename CONFIG> struct CallInstruction;
template <typename CONFIG> struct ReturnInstruction;
template <typename CONFIG> struct Group;
template <typename CONFIG> struct CacheAccess;
template <typename CONFIG> struct CacheEntry;
template <typename CONFIG> struct Cache;
struct ExtensionBase;
template <typename CONFIG, ExtensionType EXT_TYPE> struct TypedExtension;
template <typename CONFIG, ExtensionType EXT_TYPE, typename T> struct Extension;
template <typename CONFIG, typename T> struct NodeExtension;
template <typename CONFIG, typename T> struct EdgeExtension;
template <typename CONFIG, typename T> struct CFGExtension;
struct ExtensionFactory;
struct NodeBase;
struct EdgeValueBase;
template <typename CONFIG> struct EdgeValue;
template <typename CONFIG> struct Node;
template <typename CONFIG> struct CallMapEntry;
template <typename CONFIG> struct SignalMapEntry;
template <typename CONFIG> struct Block;
template <typename CONFIG> struct Phantom;
template <typename CONFIG> struct Entry;
template <typename CONFIG> struct Exit;
template <typename CONFIG> struct Halt;
template <typename CONFIG> struct Unknown;
struct CFGBase;
template <typename CONFIG> struct CFG;
template <typename CONFIG> struct Mapping;
template <typename CONFIG> struct Current;
template <typename CONFIG> struct Call;
template <typename CONFIG> struct CallStack;
template <typename CONFIG> struct State;
template <typename CONFIG> struct Builder;

using unisim::util::ostream::ToString;
using unisim::util::ostream::FScope;
using unisim::util::json::JSON_Value;
using unisim::util::json::JSON_Object;
using unisim::util::json::JSON_Member;
using unisim::util::json::JSON_Array;

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

// Allocate an extension ID of a particular extension type
unsigned AllocateExtensionId(ExtensionType ext_type);

// Get number of extension IDs for a particular extension type
unsigned ExtensionIds(ExtensionType ext_type);

// Helper function to convert a string to HTML
std::string StringToHTML(const std::string& s);

// Helper class for instruction type
struct Type
{
	Type() : value(RETURN + 1) {}
	Type(const std::string& s) : value(RETURN + 1) { Set(s); }
	Type(unsigned _value) : value(_value) {}
	void Print(std::ostream& stream) const;
	Type& operator = (const std::string& s) { Set(s); return *this; }
	bool Valid() const { return (value >= STANDARD) && (value <= RETURN); }
	operator unsigned() const { return value; }
	
private:
	void Set(const std::string& s);
	
	unsigned value;
};

inline std::ostream& operator << (std::ostream& stream, const Type& type) { type.Print(stream); return stream; }

// Helper class for control flow mode
struct Mode
{
	Mode() : value(INDIRECT + 1) {}
	Mode(const std::string& s) : value(INDIRECT + 1) { Set(s); }
	Mode(unsigned _value) : value(_value) {}
	void Print(std::ostream& stream) const;
	Mode& operator = (const std::string& s) { Set(s); return *this; }
	bool Valid() const { return (value >= DIRECT) && (value <= INDIRECT); }
	operator unsigned() const { return value; }
	
private:
	void Set(const std::string& s);
	
	unsigned value;
};

inline std::ostream& operator << (std::ostream& stream, const Mode& mode) { mode.Print(stream); return stream; }

// Helper class for node category
struct Category
{
	Category() : value(UNKNOWN + 1) {}
	Category(const std::string& s) : value(UNKNOWN + 1) { Set(s); }
	Category(unsigned _value) : value(_value) {}
	void Print(std::ostream& stream) const;
	Category& operator = (const std::string& s) { Set(s); return *this; }
	bool Valid() const { return (value >= ENTRY) && (value <= UNKNOWN); }
	operator unsigned() const { return value; }
	
private:
	void Set(const std::string& s);
	
	unsigned value;
};

inline std::ostream& operator << (std::ostream& stream, const Category& category) { category.Print(stream); return stream; }

// Helper class for edge tag
struct EdgeTag
{
	EdgeTag() : value(FALLTHROUGH + 1) {}
	EdgeTag(const std::string& s) : value(FALLTHROUGH + 1) { Set(s); }
	EdgeTag(unsigned _value) : value(_value) {}
	void Print(std::ostream& stream) const;
	EdgeTag& operator = (const std::string& s) { Set(s); return *this; }
	bool Valid() const { return (value >= EMPTY) && (value <= FALLTHROUGH); }
	operator unsigned() const { return value; }
	
private:
	void Set(const std::string& s);
	
	unsigned value;
};

inline std::ostream& operator << (std::ostream& stream, const EdgeTag& edge_tag) { edge_tag.Print(stream); return stream; }

////////////////////////////////// Config<> ///////////////////////////////////

// run-time configuration
template <typename CONFIG>
struct Config
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	unisim::service::interfaces::Disassembly<ADDRESS> *disassembly_if;
	unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if;
	std::string graphviz_color;
	std::string graphviz_fillcolor;
	std::string graphviz_branch_target_color;
	std::string graphviz_branch_fallthrough_color;
	std::string graphviz_call_color;
	std::string graphviz_fontname;
	std::string graphviz_block_fontname;
	std::string graphviz_phantom_fontname;
	bool graphviz_enable_call_edges;
	
	Config();
};

////////////////////////////// InstructionBase ////////////////////////////////

struct InstructionBase
{
	InstructionBase() : type(STANDARD), size(0), mode(DIRECT) {}
	InstructionBase(unsigned _type, unsigned _size) : type(_type), size(_size), mode(DIRECT) {}
	InstructionBase(unsigned _type, unsigned _size, unsigned _mode) : type(_type), size(_size), mode(_mode) {}
	InstructionBase(const InstructionBase& instr) : type(instr.type), size(instr.size), mode(instr.mode) {}
	InstructionBase(InstructionBase&& instr) : type(std::move(instr.type)), size(std::move(instr.size)), mode(std::move(instr.mode)) {}
	
	inline void Initialize(unsigned _type, unsigned _size, unsigned _mode);
	inline InstructionBase& operator = (const InstructionBase& other);
	inline InstructionBase& operator = (InstructionBase&& other);
	
	unsigned GetType() const { return type; }
	unsigned GetSize() const { return size; }
	unsigned GetMode() const { return mode; }
	
protected:
	unsigned type; // one of { STANDARD, JUMP, BRANCH, CALL, RETURN }
	unsigned size; // size in bytes
	unsigned mode; // one of { DIRECT, INDIRECT }
};

////////////////////////////////// Opcode<> ///////////////////////////////////

// array-based implementation of opcode
template <unsigned MAX_OPCODE_SIZE>
struct Opcode
{
	typedef uint8_t value_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef value_type *iterator;
	typedef const value_type *const_iterator;
	
	Opcode(size_type n) : opcode_size(n), opcode_storage() {}
	Opcode(Opcode<MAX_OPCODE_SIZE>&& other) : opcode_size(std::move(other.opcode_size)), opcode_storage()
	{ ::memcpy(&opcode_storage[0], &other.opcode_storage[0], opcode_size); }
	Opcode(const Opcode<MAX_OPCODE_SIZE>& other) : opcode_size(other.opcode_size), opcode_storage()
	{ ::memcpy(&opcode_storage[0], &other.opcode_storage[0], opcode_size); }
	
	template <class InputIterator> void assign(InputIterator first, InputIterator last);
	void resize(size_type n);
	Opcode<MAX_OPCODE_SIZE>& operator = (const  Opcode<MAX_OPCODE_SIZE>& other);
	Opcode<MAX_OPCODE_SIZE>& operator = (Opcode<MAX_OPCODE_SIZE>&& other);
	size_type size() const { return opcode_size; }
	iterator begin() { return &opcode_storage[0]; }
	const_iterator begin() const { return &opcode_storage[0]; }
	iterator end() { return &opcode_storage[0] + opcode_size; }
	const_iterator end() const { return &opcode_storage[0] + opcode_size; }
	reference operator[] (size_type n) { return opcode_storage[n]; }
	const_reference operator[] (size_type n) const { return opcode_storage[n]; }
private:
	size_type opcode_size;
	value_type opcode_storage[MAX_OPCODE_SIZE];
};

// vector-based implementation of opcode
template <> struct Opcode<0> : std::vector<uint8_t>
{
	Opcode(size_type n) : std::vector<uint8_t>(n) {}
	Opcode(Opcode<0>&& other) : std::vector<uint8_t>(other) {}
	Opcode(const Opcode<0>& other) : std::vector<uint8_t>(other) {}
};

/////////////////////////////// Instruction<> /////////////////////////////////

// Note: See Definition 1 of paper

template <typename CONFIG>
struct Instruction : InstructionBase
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	Instruction() : InstructionBase(), addr(-1), target(-1), fallthrough(-1) {}
	Instruction(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info);
	Instruction(unsigned _type, ADDRESS _addr, unsigned _size, const uint8_t *_opcode);
	Instruction(unsigned _type, ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, unsigned _mode);
	Instruction(unsigned _type, ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode);
	Instruction(const Instruction<CONFIG>& other);
	Instruction(Instruction<CONFIG>&& other);
	void Initialize(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info);
	Instruction<CONFIG>& operator = (const Instruction<CONFIG>& other);
	Instruction<CONFIG>& operator = (Instruction<CONFIG>&& other);
	const uint8_t *GetOpcode() const { return &opcode[0]; }
	ADDRESS GetAddress() const { return addr; }
	ADDRESS GetTarget() const { return target; }
	ADDRESS GetFallthrough() const { return fallthrough; }
	bool operator == (const Instruction<CONFIG>& other) const { return addr == other.addr; }
	bool operator != (const Instruction<CONFIG>& other) const { return addr != other.addr; }
	bool DeepCompare(const Instruction<CONFIG>& other) const;
	void Print(std::ostream& stream) const;
	JSON_Value *Save(const Config<CONFIG>& config) const;
	static Instruction<CONFIG> *Load(const JSON_Value& value);
	
protected:
	typedef Opcode<CONFIG::MAX_OPCODE_SIZE> OPCODE;
	OPCODE  opcode;      // opcode
	ADDRESS addr;        // @addr
	ADDRESS target;      // @target
	ADDRESS fallthrough; // @fallthrough
};

template <typename CONFIG>
std::ostream& operator << (std::ostream& stream, const Instruction<CONFIG>& instruction) { instruction.Print(stream); return stream; }

/////////////////////////// StandardInstruction<> /////////////////////////////

// Standard instruction that flows to the next instruction
template <typename CONFIG>
struct StandardInstruction : Instruction<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	StandardInstruction(ADDRESS _addr, unsigned _size, const uint8_t *_opcode) : Instruction<CONFIG>(STANDARD, _addr, _size, _opcode) {}
};

///////////////////////////// JumpInstruction<> ///////////////////////////////

// Unconditional jump, either direct or indirect
template <typename CONFIG>
struct JumpInstruction : Instruction<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	JumpInstruction(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, unsigned _mode) : Instruction<CONFIG>(JUMP, _addr, _size, _opcode, _target, _mode) {}
};

//////////////////////////// BranchInstruction<> //////////////////////////////

// Conditional branch, either direct or indirect
template <typename CONFIG>
struct BranchInstruction : Instruction<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	BranchInstruction(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode) : Instruction<CONFIG>(BRANCH, _addr, _size, _opcode, _target, _fallthrough, _mode) {}
};

///////////////////////////// CallInstruction<> ///////////////////////////////

// Call, either direct or indirect
template <typename CONFIG>
struct CallInstruction : Instruction<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	CallInstruction(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode) : Instruction<CONFIG>(CALL, _addr, _size, _opcode, _target, _fallthrough, _mode) {}
};

//////////////////////////// ReturnInstruction<> //////////////////////////////

// Return, either direct or indirect
template <typename CONFIG>
struct ReturnInstruction : Instruction<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	ReturnInstruction(ADDRESS _addr, unsigned _size, const uint8_t *_opcode) : Instruction<CONFIG>(RETURN, _addr, _size, _opcode) {}
};

////////////////////////////////// Group<> ////////////////////////////////////

// Note: See Definition 2 of paper

template <typename CONFIG>
struct Group
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
private:
	typedef std::vector<Instruction<CONFIG> > Instructions;
	Instructions instructions;
	typename Instructions::size_type leader_idx;
public:
	typedef typename Instructions::value_type             value_type;
	typedef typename Instructions::allocator_type         allocator_type;
	typedef typename Instructions::reference              reference;
	typedef typename Instructions::const_reference        const_reference;
	typedef typename Instructions::pointer                pointer;
	typedef typename Instructions::const_pointer          const_pointer;
	typedef typename Instructions::iterator               iterator;
	typedef typename Instructions::const_iterator         const_iterator;
	typedef typename Instructions::reverse_iterator       reverse_iterator;
	typedef typename Instructions::const_reverse_iterator const_reverse_iterator;
	typedef typename Instructions::difference_type        difference_type;
	typedef typename Instructions::size_type              size_type;
	
	Group() : instructions(), leader_idx(0) {}
	Group(const Group<CONFIG>& group) : instructions(group.instructions), leader_idx(group.leader_idx) {}
	Group(Instruction<CONFIG>&& instr) : instructions(), leader_idx(0) { AddInstruction(std::move(instr)); }
	Group(const Instruction<CONFIG>& instr) : instructions(), leader_idx(0) { AddInstruction(instr); }
	Group(Group<CONFIG>& group, ADDRESS addr);
	~Group() {}
	
	void Standard(ADDRESS _addr, unsigned _size, const uint8_t *_opcode) { AddInstruction(StandardInstruction<CONFIG>(_addr, _size, _opcode)); }
	void Jump(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, unsigned _mode) { AddInstruction(JumpInstruction<CONFIG>(_addr, _size, _opcode, _target, _mode)); }
	void Branch(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode) { AddInstruction(BranchInstruction<CONFIG>(_addr, _size, _opcode, _target, _fallthrough, _mode)); }
	void Call(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode) { AddInstruction(CallInstruction<CONFIG>(_addr, _size, _opcode, _target, _fallthrough, _mode)); }
	void Return(ADDRESS _addr, unsigned _size, const uint8_t *_opcode) { AddInstruction(ReturnInstruction<CONFIG>(_addr, _size, _opcode)); }
	
	bool operator == (const Group<CONFIG>& other) const { return (GetAddress() == other.GetAddress()) && (Length() == other.Length()); }
	bool operator != (const Group<CONFIG>& other) const { return (GetAddress() != other.GetAddress()) || (Length() != other.Length()); }
	bool DeepCompare(const Group<CONFIG>& other) const;
	ADDRESS GetAddress() const { return Leader().GetAddress(); }
	bool Has(ADDRESS addr) const;
	bool Conflicts(const Group<CONFIG>& group) const;
	bool Valid() const { return leader_idx < instructions.size(); }
	size_type Length() const { return Valid() ? (instructions.size() - leader_idx) : 0; }
	const Instruction<CONFIG>& operator [] (size_type idx) const { return instructions[leader_idx + idx]; }
	const_iterator begin() const { return instructions.begin() + leader_idx; }
	const_iterator end() const { return instructions.end(); }
	const Instruction<CONFIG>& Leader() const { return instructions[leader_idx]; }
	const Instruction<CONFIG>& Tail() const { return instructions.back(); }
	const Instruction<CONFIG> *Next(const Instruction<CONFIG>& instruction) const;
	void AddInstruction(Instruction<CONFIG>&& instr);
	void AddInstruction(const Instruction<CONFIG>& instr);
	void PopLeader() { ++leader_idx; }
	void Clear() { leader_idx = 0; instructions.clear(); }
	void Print(std::ostream& stream) const;
	JSON_Value *Save(const Config<CONFIG>& config) const;
	static Group<CONFIG> *Load(const JSON_Value& value);
	template <typename VISITOR, typename T = bool> T ScanInstructions(VISITOR& visitor) const;
};

template <typename CONFIG>
std::ostream& operator << (std::ostream& stream, const Group<CONFIG>& group) { group.Print(stream); return stream; }

////////////////////////////// CacheAccess<> //////////////////////////////////

template <typename CONFIG>
struct CacheAccess
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	CacheAccess(const Group<CONFIG>& _group) : group(_group), idx(0), cache_entry(0) {}
	
	const Group<CONFIG>& group;      // key
	unsigned idx;                    // hashed address of group
	CacheEntry<CONFIG> *cache_entry; // cache entry
};

/////////////////////////////// CacheEntry<> //////////////////////////////////

template <typename CONFIG>
struct CacheEntry
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	CacheEntry() : cached_group(), cached_working(0), cached_count(0) {}
	CacheEntry(const Group<CONFIG>& _cached_group, Node<CONFIG> *_cached_working, uint64_t _cached_count) : cached_group(_cached_group), cached_working(_cached_working), cached_count(_cached_count) {}
	
	Group<CONFIG> cached_group;   // key
	Node<CONFIG> *cached_working; // next working node
	uint64_t cached_count;        // execution count
};

////////////////////////////////// Cache<> ////////////////////////////////////

// Note: See algorithm 4 of paper

template <typename CONFIG>
struct Cache
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	Cache() : cache_entries() {}
	bool Access(CacheAccess<CONFIG>& cache_access, uint64_t count);
	void Replace(CacheAccess<CONFIG>& cache_access, Node<CONFIG> *working, uint64_t count);
	void FlushCounts(Node<CONFIG> *src);
	
private:
	typedef std::array<CacheEntry<CONFIG>, CONFIG::CACHE_SIZE> CacheEntries;
	CacheEntries cache_entries;
};

//////////////////////////////// ExtensionBase ////////////////////////////////

struct ExtensionBase
{
	virtual void Free();
	virtual unsigned GetId() const = 0;
	virtual const char *GetName() const = 0;
	virtual JSON_Value *Save() const = 0;
	virtual void Print(std::ostream& stream) const = 0;
protected:
	virtual ~ExtensionBase();
};

inline std::ostream& operator << (std::ostream& stream, const ExtensionBase& ext) { ext.Print(stream); return stream; }

////////////////////////////// TypedExtension<> ///////////////////////////////

template <typename CONFIG, ExtensionType EXT_TYPE> struct TypedExtension {};

template <typename CONFIG>
struct TypedExtension<CONFIG, NODE_EXT> : ExtensionBase
{
	static const ExtensionType Type;
	
	virtual void Fix(const Mapping<CONFIG>& mapping, const CFG<CONFIG>& cfg, const Node<CONFIG>& node, const JSON_Value& value) {};
};

template <typename CONFIG>
struct TypedExtension<CONFIG, EDGE_EXT> : ExtensionBase
{
	static const ExtensionType Type;
	
	virtual void Fix(const Mapping<CONFIG>& mapping, const CFG<CONFIG>& cfg, const Node<CONFIG>& node, const EdgeValue<CONFIG>& edge_value, const JSON_Value& value) {};
};

template <typename CONFIG>
struct TypedExtension<CONFIG, CFG_EXT> : ExtensionBase
{
	static const ExtensionType Type;
	
	virtual void Fix(const Mapping<CONFIG>& mapping, const CFG<CONFIG>& cfg, const JSON_Value& value) {};
};

//////////////////////////// NodeExtensionBase<> //////////////////////////////

template <typename CONFIG>
using NodeExtensionBase = TypedExtension<CONFIG, NODE_EXT>;

//////////////////////////// EdgeExtensionBase<> //////////////////////////////

template <typename CONFIG>
using EdgeExtensionBase = TypedExtension<CONFIG, EDGE_EXT>;

//////////////////////////// CFGExtensionBase<> ///////////////////////////////

template <typename CONFIG>
using CFGExtensionBase = TypedExtension<CONFIG, CFG_EXT>;

///////////////////////////////// Extension<> /////////////////////////////////

template <typename CONFIG, ExtensionType EXT_TYPE, typename T>
struct Extension : TypedExtension<CONFIG, EXT_TYPE>
{
	const static unsigned ID;
	virtual unsigned GetId() const { return T::ID; }
	virtual const char *GetName() const { return T::NAME; }
};

template <typename CONFIG, typename T> struct NodeExtension : Extension<CONFIG, NODE_EXT, T> {};
template <typename CONFIG, typename T> struct EdgeExtension : Extension<CONFIG, EDGE_EXT, T> {};
template <typename CONFIG, typename T> struct CFGExtension  : Extension<CONFIG, CFG_EXT , T> {};

////////////////////////////// ExtensionFactory ///////////////////////////////

struct ExtensionFactory
{
	virtual ~ExtensionFactory() {}
	virtual ExtensionBase *LoadExtension(ExtensionType ext_type, const std::string& ext_name, const JSON_Value& value) const = 0;
};

////////////////////////////////// CFGBase ////////////////////////////////////

struct CFGBase
{
	CFGBase() {}
	virtual ~CFGBase() {}
	
	virtual std::string GetName() const = 0;
	virtual std::string GetGraphName() const = 0;
};

////////////////////////////////// NodeBase ///////////////////////////////////

struct NodeBase
{
	NodeBase(unsigned _category) : category(_category) {}
	virtual ~NodeBase() {}
	
	unsigned GetCategory() const { return category; }
	virtual std::string GetName() const { return ToString(Category(category)); }
	virtual std::string GetGraphNodeName(const CFGBase& cfg) const { return cfg.GetGraphName() + "_" + ToString(Category(category)); }
	virtual void Print(std::ostream& stream) const = 0;
	JSON_Object *SaveAsObject() const;
	
private:
	unsigned category; // one of { ENTRY, BLOCK, PHANTOM, EXIT, HALT }
};

inline std::ostream& operator << (std::ostream& stream, const NodeBase& node) { node.Print(stream); return stream; }

///////////////////////////////// EdgeValueBase ///////////////////////////////

// Value on a edge: (count, tag)
struct EdgeValueBase
{
	EdgeValueBase() : count(0), tag(EMPTY) {}
	EdgeValueBase(uint64_t _count) : count(_count), tag(EMPTY) {}
	EdgeValueBase(uint64_t _count, unsigned _tag) : count(_count), tag(_tag) {}
	EdgeValueBase(const EdgeValueBase& other) : count(other.count), tag(other.tag) {}
	EdgeValueBase(const JSON_Value& value);
	
	uint64_t GetCount() const { return count; }
	unsigned GetTag() const { return tag; }
	
	EdgeValueBase& operator = (const EdgeValueBase& other) { count = other.count; tag = other.tag; return *this; }
	
	void Update(const EdgeValueBase& other) { count += other.count; }
	void Update(uint64_t _count) { count += _count; }
	
	void Print(std::ostream& stream) const { stream << "EdgeValueBase(count=" << std::dec << count << ",tag=" << EdgeTag(tag) << ")"; }
	
	void Save(JSON_Value& value) const;
	
private:
	uint64_t count; // profiling information: how many times this edge was visited during execution
	unsigned tag;   // one of { EMPTY, TARGET, FALLTHROUGH }
};

inline std::ostream& operator << (std::ostream& stream, const EdgeValueBase& edge_value) { edge_value.Print(stream); return stream; }

////////////////////////////////// EdgeValue<> ////////////////////////////////

template <typename CONFIG>
struct EdgeValue : EdgeValueBase
{
	EdgeValue() : EdgeValueBase(), extensions(ExtensionIds(EDGE_EXT)) {}
	EdgeValue(uint64_t _count) : EdgeValueBase(_count), extensions(ExtensionIds(EDGE_EXT)) {}
	EdgeValue(uint64_t _count, unsigned _tag) : EdgeValueBase(_count, _tag), extensions(ExtensionIds(EDGE_EXT)) {}
	EdgeValue(const EdgeValue& other) : EdgeValueBase(other), extensions(ExtensionIds(EDGE_EXT)) {}
	EdgeValue(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {});
	~EdgeValue();
	
	EdgeValue<CONFIG>& operator = (const EdgeValue<CONFIG>& other) { EdgeValueBase::operator = (other); return *this; }
	
	void Save(JSON_Value& value) const;
	
	template <typename T> T *SetExtension(T *ext);
	template <typename T> void GetExtension(T *& ext) const;
	template <typename T> T *GetExtension() const;
	
	void InvalidateExtensions();
	void OutputGraphEdgeExtensions(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
private:
	friend struct Node<CONFIG>;
	
	typedef std::vector<EdgeExtensionBase<CONFIG> *> Extensions;
	Extensions extensions; // extensions
	
	EdgeExtensionBase<CONFIG> *SetExtension(unsigned ext_id, ExtensionBase *ext);
	
	void Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories);
	
	void Fix(const Mapping<CONFIG>& mapping, const CFG<CONFIG>& cfg, const Node<CONFIG>& node, const JSON_Value& value);
};

template <typename CONFIG>
std::ostream& operator << (std::ostream& stream, const EdgeValue<CONFIG>& edge_value) { edge_value.Print(stream); return stream; }

/////////////////////////////////// Node<> ////////////////////////////////////

// A CFG node
template <typename CONFIG>
struct Node : NodeBase
{
	typedef typename CONFIG::ADDRESS ADDRESS;

	friend struct CFG<CONFIG>;
	friend struct Builder<CONFIG>;
	
	Node(unsigned _category) : NodeBase(_category), edges(), predecessors(), has_unknown_successor(false), cache(), extensions(ExtensionIds(NODE_EXT)) {}
	virtual ~Node();
	
	Block<CONFIG>& AsBlock() { assert(this->GetCategory() == BLOCK); return *static_cast<Block<CONFIG> *>(this); }
	const Block<CONFIG>& AsBlock() const { assert(this->GetCategory() == BLOCK); return *static_cast<const Block<CONFIG> *>(this); }
	Phantom<CONFIG>& AsPhantom() { assert(this->GetCategory() == PHANTOM); return *static_cast<Phantom<CONFIG> *>(this); }
	const Phantom<CONFIG>& AsPhantom() const { assert(this->GetCategory() == PHANTOM); return *static_cast<const Phantom<CONFIG> *>(this); }
	Unknown<CONFIG>& AsUnknown() { assert(this->GetCategory() == UNKNOWN); return *static_cast<Unknown<CONFIG> *>(this); }
	const Unknown<CONFIG>& AsUnknown() const { assert(this->GetCategory() == UNKNOWN); return *static_cast<const Unknown<CONFIG> *>(this); }
	void ConnectTo(Node<CONFIG> *successor, EdgeValue<CONFIG>&& edge_value);
	void ConnectTo(Node<CONFIG> *successor, uint64_t count = 0, unsigned tag = EMPTY);
	std::size_t SuccessorCount() const { return edges.size(); }
	std::size_t PredecessorCount() const { return predecessors.size(); }
	bool HasSuccessors() const { return !edges.empty(); }
	bool HasPredecessors() const { return !predecessors.empty(); }
	bool HasPhantomSuccessor() const;
	bool HasUnknownSuccessor() const { return has_unknown_successor; }
	uint64_t Reconnect(Node<CONFIG> *old_node, Node<CONFIG> *node);
	void PrintEdges(std::ostream& stream) const;
	virtual bool HasGroup(const Group<CONFIG>& group) const { return false; }
	void FlushCounts(Group<CONFIG>& group, Node<CONFIG> *dst, uint64_t count);
	void FlushCounts() { cache.FlushCounts(this); }
	bool AccessCache(CacheAccess<CONFIG>& cache_access, uint64_t count) { return cache.Access(cache_access, count); }
	void ReplaceCache(CacheAccess<CONFIG>& cache_access, Node<CONFIG> *working, uint64_t count) { cache.Replace(cache_access, working, count); }
	virtual void OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const = 0;
	virtual void OutputGraphEdges(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
	virtual void OutputGraphNodeExtensions(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
	JSON_Object *SaveEdge(Node<CONFIG> *node, const EdgeValue<CONFIG>& edge_value) const;
	JSON_Object *SaveAsObject() const;
	virtual JSON_Value *Save(const Config<CONFIG>& config) const;
	static Node<CONFIG> *Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {});
	virtual bool Fix(Mapping<CONFIG>& mapping, CFG<CONFIG>& cfg, const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {});
	virtual void Check() const;
	bool HasExtensions() const { for(auto extension : extensions) { if(extension) return true; } return false; }
	void InvalidateExtensions();
	template <typename VISITOR, typename T = bool> T ScanEdges(VISITOR& visitor) const;
	template <typename VISITOR, typename T = bool> T ScanEdgesOrdered(VISITOR& visitor) const;
	template <typename VISITOR, typename T = bool> T ScanSuccessors(VISITOR& visitor) const;
	template <typename VISITOR, typename T = bool> T ScanPredecessors(VISITOR& visitor) const;
	
	template <typename T> T *SetExtension(T *ext);
	template <typename T> void GetExtension(T *& ext) const;
	template <typename T> T *GetExtension() const;
private:
	typedef std::map<Node<CONFIG> *, EdgeValue<CONFIG> > Edges;
	typedef std::vector<Node<CONFIG> *> Predecessors;
	Edges edges;                // edges: (node, count)
	Predecessors predecessors;  // predecessor nodes
	bool has_unknown_successor; // Whether there is an edge toward an unknown node (i.e. toward an indirect target node)
	Cache<CONFIG> cache;        // per node cache of the next working node based on the current working node and the incoming group of instructions
	typedef std::vector<NodeExtensionBase<CONFIG> *> Extensions;
	Extensions extensions; // extensions

	NodeExtensionBase<CONFIG> *SetExtension(unsigned ext_id, ExtensionBase *ext);
};

/////////////////////////////// CallMapEntry<> ////////////////////////////////

template <typename CONFIG>
struct CallMapEntry
{
	CallMapEntry(CFG<CONFIG> *_cfg, uint64_t _count) : cfg(_cfg), count(_count) {}
	
	CFG<CONFIG> *cfg;
	uint64_t count;
};

/////////////////////////////// SignalMapEntry<> ////////////////////////////////

template <typename CONFIG>
struct SignalMapEntry
{
	SignalMapEntry(CFG<CONFIG> *_cfg, uint64_t _count) : cfg(_cfg), count(_count) {}
	
	CFG<CONFIG> *cfg;
	uint64_t count;
};

/////////////////////////////////// Block<> ///////////////////////////////////

// A basic block
template <typename CONFIG>
struct Block : Node<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename Group<CONFIG>::value_type             value_type;
	typedef typename Group<CONFIG>::allocator_type         allocator_type;
	typedef typename Group<CONFIG>::reference              reference;
	typedef typename Group<CONFIG>::const_reference        const_reference;
	typedef typename Group<CONFIG>::pointer                pointer;
	typedef typename Group<CONFIG>::const_pointer          const_pointer;
	typedef typename Group<CONFIG>::iterator               iterator;
	typedef typename Group<CONFIG>::const_iterator         const_iterator;
	typedef typename Group<CONFIG>::reverse_iterator       reverse_iterator;
	typedef typename Group<CONFIG>::const_reverse_iterator const_reverse_iterator;
	typedef typename Group<CONFIG>::difference_type        difference_type;
	typedef typename Group<CONFIG>::size_type              size_type;
	
	Block(Group<CONFIG> *_group) : Node<CONFIG>(BLOCK), group(_group), call_map(), signal_map() {}
	Block(Block<CONFIG> *block, ADDRESS addr) : Node<CONFIG>(BLOCK), group(new Group(*block->group, addr)), call_map(), signal_map() {}
	virtual ~Block() { delete group; }
	
	virtual std::string GetName() const;
	virtual std::string GetGraphNodeName(const CFGBase& cfg) const;
	ADDRESS GetAddress() const { return group->GetAddress(); }
	bool Has(ADDRESS addr) const { return group->Has(addr); }
	bool Conflicts(const Group<CONFIG>& _group) const { return group->Conflicts(_group); }
	bool Conflicts(const Block<CONFIG>& block) const { return group->Conflicts(*block.group); }
	bool Valid() const { return group->Valid(); }
	size_type Length() const { return group->Length(); }
	const Instruction<CONFIG>& operator [] (size_type idx) const { return (*group)[idx]; }
	const_iterator begin() const { return group->begin(); }
	const_iterator end() const { return group->end(); }
	bool HasCalls() const { return !call_map.empty(); }
	void AddCall(CFG<CONFIG> *called, uint64_t count);
	bool HasSignals() const { return !signal_map.empty(); }
	void AddSignal(unsigned sigid, CFG<CONFIG> *called, uint64_t count);
	const Instruction<CONFIG>& Leader() const { return group->Leader(); }
	const Instruction<CONFIG>& Tail() const { return group->Tail(); }
	const Instruction<CONFIG> *Next(const Instruction<CONFIG>& instruction) const { return group->Next(instruction); }
	void AddInstruction(Instruction<CONFIG>&& instr) { group->AddInstruction(std::move(instr)); }
	void AddInstruction(const Instruction<CONFIG>& instr) { group->AddInstruction(instr); }
	void PopLeader() { group->PopLeader(); }
	virtual bool HasGroup(const Group<CONFIG>& _group) const { return *group == _group; }
	virtual void Print(std::ostream& stream) const;
	virtual void OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
	virtual void OutputGraphEdges(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
	virtual JSON_Value *Save(const Config<CONFIG>& config) const;
	static Block<CONFIG> *Load(const JSON_Value& value);
	virtual bool Fix(Mapping<CONFIG>& mapping, CFG<CONFIG>& cfg, const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {});
private:
	typedef std::map<ADDRESS, CallMapEntry<CONFIG> > CallMap;
	typedef std::map<unsigned, SignalMapEntry<CONFIG> > SignalMap;
	
	Group<CONFIG> *group; // ordered sequence of instructions
	CallMap call_map;     // call map: @addr -> (cfg, count)
	SignalMap signal_map; // signal map: sigid -> (cfg, count)
};

////////////////////////////////// Phantom<> //////////////////////////////////

// An undiscovered node
template <typename CONFIG>
struct Phantom : Node<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;

	Phantom(ADDRESS _addr) : Node<CONFIG>(PHANTOM), addr(_addr) {}

	virtual std::string GetName() const;
	virtual std::string GetGraphNodeName(const CFGBase& cfg) const;
	ADDRESS GetAddress() const { return addr; }
	virtual void Print(std::ostream& stream) const;
	virtual void OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
	virtual JSON_Value *Save(const Config<CONFIG>& config) const;
	static Phantom<CONFIG> *Load(const JSON_Value& value);
	
private:
	ADDRESS addr; // address of phantom node
};

/////////////////////////////////// Entry<> ///////////////////////////////////

// CFG entry node
template <typename CONFIG>
struct Entry : Node<CONFIG>
{
	Entry() : Node<CONFIG>(ENTRY) {}
	virtual void Print(std::ostream& stream) const;
	virtual void OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
};

//////////////////////////////////// Exit<> ///////////////////////////////////

// CFG exit node
template <typename CONFIG>
struct Exit : Node<CONFIG>
{
	Exit() : Node<CONFIG>(EXIT) {}
	virtual void Print(std::ostream& stream) const;
	virtual void OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
};

//////////////////////////////////// Halt<> ///////////////////////////////////

// CFG halt node
template <typename CONFIG>
struct Halt : Node<CONFIG>
{
	Halt() : Node<CONFIG>(HALT) {}
	virtual void Print(std::ostream& stream) const;
	virtual void OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
};

////////////////////////////////// Unknown<> //////////////////////////////////

// Indirect target node
template <typename CONFIG>
struct Unknown : Node<CONFIG>
{
	Unknown() : Node<CONFIG>(UNKNOWN), id(0) {}
	Unknown(unsigned _id) : Node<CONFIG>(UNKNOWN), id(_id) {}
	unsigned GetId() const { return id; }
	virtual std::string GetName() const;
	virtual std::string GetGraphNodeName(const CFGBase& cfg) const;
	virtual void Print(std::ostream& stream) const;
	virtual void OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const;
	virtual JSON_Value *Save(const Config<CONFIG>& config) const;
	static Unknown<CONFIG> *Load(const JSON_Value& value);
private:
	unsigned id; // identifier of unknown node
};

///////////////////////////////////// CFG<> ///////////////////////////////////

// Note: See Definition 3 of paper

// Filters (to combine with a bitwise OR) when outputting graph for Graphviz
enum
{
	GRAPH = 1, // digraph or subgraph
	NODES = 2, // nodes
	EDGES = 4  // edges
};

// Control Flow Graph (see Definition 3 of paper)
template <typename CONFIG>
struct CFG : CFGBase
{
	typedef typename CONFIG::ADDRESS ADDRESS;

	CFG() : CFGBase(), has_addr(false), addr(0), nodes(), unknowns(), entry(0), exit(0), halt(0), extensions(ExtensionIds(CFG_EXT)) {}
	CFG(ADDRESS _addr) : CFGBase(), has_addr(true), addr(_addr), nodes(), unknowns(), entry(0), exit(0), halt(0), extensions(ExtensionIds(CFG_EXT)) {}
	virtual ~CFG();
	
	virtual std::string GetName() const;
	virtual std::string GetGraphName() const;
	bool IsComplete() const; // see Definition 4 of paper
	bool HasAddress() const { return has_addr; }
	ADDRESS GetAddress() const { assert(has_addr); return addr; }
	Block<CONFIG> *AddNode(Block<CONFIG> *block);
	Phantom<CONFIG> *AddNode(Phantom<CONFIG> *phantom);
	Unknown<CONFIG> *AddNode(Unknown<CONFIG> *unknown);
	bool HasNode(Node<CONFIG> *node) const;
	void AddEdge(Node<CONFIG> *src, Node<CONFIG> *dst, EdgeValue<CONFIG>&& edge_value);
	void AddEdge(Node<CONFIG> *src, Node<CONFIG> *dst, uint64_t count = 0, unsigned tag = EMPTY);
	Node<CONFIG> *FindNodeWithAddr(ADDRESS node_addr) const;
	Node<CONFIG> *FindUnknownWithId(unsigned id) const;
	Block<CONFIG> *PhantomToBlock(Phantom<CONFIG> *phantom, Block<CONFIG> *block);
	Block<CONFIG> *SplitBlock(Block<CONFIG> *block, ADDRESS instr_addr);
	bool HasEntry() const { return entry != 0; }
	Node<CONFIG> *GetEntry() const { return entry ? entry : (entry = new Entry<CONFIG>()); }
	bool HasExit() const { return exit != 0; }
	Node<CONFIG> *GetExit() const { return exit ? exit : (exit = new Exit<CONFIG>()); }
	bool HasHalt() const { return halt != 0; }
	Node<CONFIG> *GetHalt() const { return halt ? halt : (halt = new Halt<CONFIG>()); }
	void Check();
	void InvalidateExtensions();
	void Print(std::ostream& stream) const;
	void OutputGraphNodes(std::ostream& stream, const std::string& indent, const Config<CONFIG>& config, bool subgraph) const;
	void OutputGraphEdges(std::ostream& stream, const std::string& indent, const Config<CONFIG>& config, bool subgraph) const;
	void OutputGraph(std::ostream& stream, const std::string& indent, const Config<CONFIG>& config, bool subgraph = false, unsigned filter = GRAPH | NODES | EDGES) const;
	JSON_Value *Save(const Config<CONFIG>& config) const;
	static CFG<CONFIG> *Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {});
	template <typename VISITOR, typename T = bool> T ScanNodes(VISITOR& visitor) const;
	
	template <typename T> T *SetExtension(T *ext);
	template <typename T> void GetExtension(T *& ext) const;
	template <typename T> T *GetExtension() const;
private:
	friend struct Mapping<CONFIG>;
	
	bool Fix(Mapping<CONFIG>& mapping, const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {});
	
	struct ReversedAddressCompare { bool operator() (ADDRESS a, ADDRESS b) const { return a > b; } };
	typedef std::map<ADDRESS, Node<CONFIG> *, ReversedAddressCompare> Nodes;
	
	typedef std::vector<Unknown<CONFIG> *> Unknowns;
	
	bool has_addr;                // Whether CFG has (yet) an address
	ADDRESS addr;                 // address of CFG
	Nodes nodes;                  // Block and phantom nodes ordered backwards by address
	Unknowns unknowns;            // Unknown nodes
	mutable Node<CONFIG> *entry; // Entry node of CFG
	mutable Node<CONFIG> *exit;   // Exit node of CFG
	mutable Node<CONFIG> *halt;   // Halt node of CFG
	typedef std::vector<CFGExtensionBase<CONFIG> *> Extensions;
	Extensions extensions; // extensions
	
	CFGExtensionBase<CONFIG> *SetExtension(unsigned ext_id, ExtensionBase *ext);
};

template <typename CONFIG>
std::ostream& operator << (std::ostream& stream, const CFG<CONFIG>& cfg) { cfg.Print(stream); return stream; }

/////////////////////////////////// Mapping<> /////////////////////////////////

// CFG mapping: @addr -> CFG
template <typename CONFIG>
struct Mapping
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	Mapping() : map() {}
	virtual ~Mapping() { Clear(); }
	
	void Clear();
	CFG<CONFIG> *Get(ADDRESS addr) { typename Map::const_iterator itr = map.find(addr); return (itr != map.end()) ? (*itr).second : 0; }
	void Put(ADDRESS addr, CFG<CONFIG> *cfg) { map.emplace(addr, cfg); }
	void OutputGraph(std::ostream& stream, const Config<CONFIG>& config) const;
	JSON_Value *Save(const Config<CONFIG>& config) const;
	bool Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {});
	void Check();
	void InvalidateExtensions();
	template <typename VISITOR, typename T = bool> T ScanCFGs(VISITOR& visitor) const;
	template <typename VISITOR, typename T = bool> T ScanCFGsOrdered(VISITOR& visitor) const;
private:
	typedef std::unordered_map<ADDRESS, CFG<CONFIG> *> Map;
	Map map;
};

/////////////////////////////////// Current<> /////////////////////////////////

template <typename CONFIG>
struct Current
{
	CFG<CONFIG> *cfg;
	Node<CONFIG> *working;
	
	Current() : cfg(0), working(0) {}
	Current(CFG<CONFIG> *_cfg, Node<CONFIG> *_working) : cfg(_cfg), working(_working) {}
	Current<CONFIG>& operator = (const Current<CONFIG>& current) { cfg = current.cfg; working = current.working; return *this; }
	operator bool() const { return cfg && working; }
	void Print(std::ostream& stream) const;
};

template <typename CONFIG>
std::ostream& operator << (std::ostream& stream, const Current<CONFIG>& current) { current.Print(stream); return stream; }

/////////////////////////////// CallStackEntry<> //////////////////////////////

template <typename CONFIG>
struct CallStackEntry
{
	typedef typename CONFIG::ADDRESS ADDRESS;

	Current<CONFIG> current;
	ADDRESS ret_addr;
	
	CallStackEntry(Current<CONFIG> const& _current, ADDRESS _ret_addr) : current(_current), ret_addr(_ret_addr) {}
};

///////////////////////////////// CallStack<> /////////////////////////////////

template <typename CONFIG>
struct CallStack
{
	typedef typename CONFIG::ADDRESS ADDRESS;

	CallStack() : stack() {}
	~CallStack() { Clear(); }
	bool Empty() const { return stack.empty(); }
	void Push(Current<CONFIG> const& current, ADDRESS ret_addr) { stack.emplace_back(current, ret_addr); }
	CallStackEntry<CONFIG>& Top() { return stack.back(); }
	void Pop() { stack.pop_back(); }
	void Clear() { stack.clear(); }
	bool Has(ADDRESS ret_addr) const;
private:
	typedef std::vector<CallStackEntry<CONFIG> > Stack;
	Stack stack;
};

/////////////////////////////////// State<> ///////////////////////////////////

template <typename CONFIG>
struct State
{
	Current<CONFIG> current;
	CallStack<CONFIG> callstack;
	
	State() : current(), callstack() {} // S = (NIL, [])
	~State() { Clear(); }
	void Clear() { current = Current<CONFIG>(); callstack.Clear(); }
};

////////////////////////////////// Builder<> //////////////////////////////////

// dynamic and incremental control flow graph (CFG) builder
template <typename CONFIG>
struct Builder
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	Builder();
	virtual ~Builder();
	
	// run-time configuration
	void SetDisasmInterface(unisim::service::interfaces::Disassembly<ADDRESS> *disassembly_if) { config.disassembly_if = disassembly_if; }
	void SetSymbolTableLookupInterface(unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if) { config.symbol_table_lookup_if = symbol_table_lookup_if; }
	void SetColor(const std::string& graphviz_color) { config.graphviz_color = graphviz_color; }
	void SetFillColor(const std::string& graphviz_fillcolor) { config.graphviz_fillcolor = graphviz_fillcolor; }
	void SetBranchTargetColor(const std::string& graphviz_branch_target_color) { config.graphviz_branch_target_color = graphviz_branch_target_color; }
	void SetBranchFallthroughColor(const std::string& graphviz_branch_fallthrough_color) { config.graphviz_branch_fallthrough_color = graphviz_branch_fallthrough_color; }
	void SetCallColor(const std::string& graphviz_call_color) { config.graphviz_call_color = graphviz_call_color; }
	void SetFontName(const std::string& graphviz_fontname) { config.graphviz_fontname = graphviz_fontname; }
	void SetBlockFontName(const std::string& graphviz_block_fontname) { config.graphviz_block_fontname = graphviz_block_fontname; }
	void SetPhantomFontName(const std::string& graphviz_phantom_fontname) { config.graphviz_phantom_fontname = graphviz_phantom_fontname; }
	void EnableCallEdges(bool flag = true) { config.graphviz_enable_call_edges = flag; }
	const Config<CONFIG>& GetConfig() const { return config; }
	
	// Convenient methods for implicitely building groups from executed instructions and process them
	void Standard(ADDRESS _addr, unsigned _size, const uint8_t *_opcode) { Process(StandardInstruction<CONFIG>(_addr, _size, _opcode)); }
	void Jump(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, unsigned _mode) { Process(JumpInstruction<CONFIG>(_addr, _size, _opcode, _target, _mode)); }
	void Branch(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode) { Process(BranchInstruction<CONFIG>(_addr, _size, _opcode, _target, _fallthrough, _mode)); }
	void Call(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode) { Process(CallInstruction<CONFIG>(_addr, _size, _opcode, _target, _fallthrough, _mode)); }
	void Return(ADDRESS _addr, unsigned _size, const uint8_t *_opcode) { Process(ReturnInstruction<CONFIG>(_addr, _size, _opcode)); }
	void Process(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info);
	void Process(Instruction<CONFIG>&& instr);
	void Process(const Instruction<CONFIG>& instr);
	
	// Methods for processing groups as they are encountered during execution
	void Process(const Group<CONFIG>& group); // see algorithms 1 and 4 (PROCESS_PROGRAM) of paper
	void End();                               // see algorithms 1 and 4 (PROCESS_PROGRAM) of paper
	
	// Result: all computed CFGs, by address
	const Mapping<CONFIG>& GetMapping() const { return mapping; }
	
	// I/O methods
	void OutputGraph(std::ostream& stream) const; // output all CFGs in a .dot file for further use with Graphviz
	JSON_Value *Save() const;                     // saving (as JSON value) for incremental construction of CFGs
	void Save(std::ostream& stream) const;        // saving (as "textual" JSON) for incremental construction of CFGs
	bool Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories = {}); // loading (from a JSON value) for incremental construction of CFGs
	void Load(std::istream& stream, const std::initializer_list<ExtensionFactory *>& extension_factories = {});    // loading (from "textual" JSON) for incremental construction of CFGs
	
private:
	unsigned /* tag */ ProcessTail(const Instruction<CONFIG>& tail, ADDRESS target_addr); // see algorithm 2 (PROCESS_TYPE) of paper
	void ProcessBranch(ADDRESS addr, ADDRESS target_addr, unsigned tag); // see algorithm 2 (PROCESS_TYPE) of paper
	Block<CONFIG> *ProcessGroup(CFG<CONFIG> *cfg, Node<CONFIG> *working, const Group<CONFIG>& group, unsigned tag); // see algorithm 3 (PROCESS_GROUP) of paper
	void InvalidateExtensions();
	
	State<CONFIG> state;     // S = (current, callstack)
	                         // callstack = (current, ret_addr)
	                         // current = (CFG, working)
	Mapping<CONFIG> mapping; // CFG mapping: @addr -> CFG
	Group<CONFIG> new_group; // implicit built group
	unsigned next_unknown_id;
	bool under_construction;
protected:
	Config<CONFIG> config;
};

template <typename CONFIG>
std::ostream& operator << (std::ostream& stream, const Builder<CONFIG>& builder) { builder.Save(stream); return stream; }

template <typename CONFIG>
std::istream& operator >> (std::istream& stream, Builder<CONFIG>& builder) { builder.Load(stream); return stream; }

///////////////////////////////////////////////////////////////////////////////
//                               Definitions                                 //
///////////////////////////////////////////////////////////////////////////////

////////////////////////////// InstructionBase ////////////////////////////////

inline void InstructionBase::Initialize(unsigned _type, unsigned _size, unsigned _mode)
{
	type = _type;
	size = _size;
	mode = _mode;
}

inline InstructionBase& InstructionBase::operator = (const InstructionBase& other)
{
	type = other.type;
	size = other.size;
	mode = other.mode;
	return *this;
}

inline InstructionBase& InstructionBase::operator = (InstructionBase&& other)
{
	type = std::move(other.type);
	size = std::move(other.size);
	mode = std::move(other.mode);
	return *this;
}

} // end of namespace cfg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_CFG_CFG_HH__
