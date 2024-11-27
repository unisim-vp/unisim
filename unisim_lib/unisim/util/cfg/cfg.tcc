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

#ifndef __UNISIM_UTIL_CFG_CFG_TCC__
#define __UNISIM_UTIL_CFG_CFG_TCC__

#include <unisim/util/cfg/cfg.hh>
#include <unisim/util/json/json.tcc>
#include <algorithm>

namespace unisim {
namespace util {
namespace cfg {

using unisim::util::ostream::ToString;
using unisim::util::json::JSON_Value;
using unisim::util::json::JSON_String;
using unisim::util::json::JSON_Number;
using unisim::util::json::JSON_Integer;
using unisim::util::json::JSON_Float;
using unisim::util::json::JSON_Boolean;
using unisim::util::json::JSON_Null;
using unisim::util::json::JSON_Object;
using unisim::util::json::JSON_Member;
using unisim::util::json::JSON_Array;
using unisim::util::json::JSON_Parser;
using unisim::util::json::JSON_AST_Builder;

///////////////////////////////////////////////////////////////////////////////
//                               Definitions                                 //
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Config<> ///////////////////////////////////

template <typename CONFIG>
Config<CONFIG>::Config()
	: disassembly_if(0)
	, symbol_table_lookup_if(0)
	, graphviz_color("black")
	, graphviz_fillcolor("white")
	, graphviz_branch_target_color("#008000")      // green
	, graphviz_branch_fallthrough_color("#cd0000") // red
	, graphviz_call_color("#0000cd")               // blue
	, graphviz_fontname("Helvetica")               // sans-serif font
	, graphviz_block_fontname("Courier")           // monospace font
	, graphviz_phantom_fontname("Courier")         // monospace font
	, graphviz_enable_call_edges(false)            // disable generating edges for calls
{
}

////////////////////////////////// Opcode<> ///////////////////////////////////

template <unsigned MAX_OPCODE_SIZE>
template <class InputIterator>
void Opcode<MAX_OPCODE_SIZE>::assign(InputIterator first, InputIterator last)
{
	for(iterator itr = begin(); (itr != end()) && (first != last); ++itr, ++first)
	{
		*itr = *first;
	}
	assert(first == last);
}

template <unsigned MAX_OPCODE_SIZE>
void Opcode<MAX_OPCODE_SIZE>::resize(size_type n)
{
	assert(n <= MAX_OPCODE_SIZE);
	opcode_size = n;
}

template <unsigned MAX_OPCODE_SIZE>
Opcode<MAX_OPCODE_SIZE>& Opcode<MAX_OPCODE_SIZE>::operator = (const  Opcode<MAX_OPCODE_SIZE>& other)
{
	opcode_size = other.opcode_size;
	::memcpy(&opcode_storage[0], &other.opcode_storage[0], opcode_size);
	return *this;
}

template <unsigned MAX_OPCODE_SIZE>
Opcode<MAX_OPCODE_SIZE>& Opcode<MAX_OPCODE_SIZE>::operator = (Opcode<MAX_OPCODE_SIZE>&& other)
{
	opcode_size = std::move(other.opcode_size);
	::memcpy(&opcode_storage[0], &other.opcode_storage[0], opcode_size);
	return *this;
}

/////////////////////////////// Instruction<> /////////////////////////////////

template <typename CONFIG>
Instruction<CONFIG>::Instruction(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info)
	: InstructionBase(instr_info.type, instr_info.size, instr_info.mode)
	, opcode(instr_info.size)
	, addr(instr_info.addr)
	, target(instr_info.target)
	, fallthrough(instr_info.fallthrough)
{
	if(instr_info.opcode) opcode.assign(instr_info.opcode, instr_info.opcode + size);
}

template <typename CONFIG>
Instruction<CONFIG>::Instruction(unsigned _type, ADDRESS _addr, unsigned _size, const uint8_t *_opcode)
	: InstructionBase(_type, _size)
	, opcode(_size)
	, addr(_addr)
	, target(-1)
	, fallthrough(-1)
{
	if(_opcode) opcode.assign(_opcode, _opcode + _size);
}

template <typename CONFIG>
Instruction<CONFIG>::Instruction(unsigned _type, ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, unsigned _mode)
	: InstructionBase(_type, _size, _mode)
	, opcode(_size)
	, addr(_addr)
	, target(_target)
	, fallthrough(-1)
{
	if(_opcode) opcode.assign(&_opcode[0], &_opcode[0] + size);
}

template <typename CONFIG>
Instruction<CONFIG>::Instruction(unsigned _type, ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode)
	: InstructionBase(_type, _size, _mode)
	, opcode(_size)
	, addr(_addr)
	, target(_target)
	, fallthrough(_fallthrough)
{
	if(_opcode) opcode.assign(&_opcode[0], &_opcode[0] + size);
}

template <typename CONFIG>
Instruction<CONFIG>::Instruction(const Instruction<CONFIG>& other)
	: InstructionBase(other)
	, opcode(other.opcode)
	, addr(other.addr)
	, target(other.target)
	, fallthrough(other.fallthrough)
{
}

template <typename CONFIG>
Instruction<CONFIG>::Instruction(Instruction<CONFIG>&& other)
	: InstructionBase(std::move(other))
	, opcode(std::move(other.opcode))
	, addr(std::move(other.addr))
	, target(std::move(other.target))
	, fallthrough(std::move(other.fallthrough))
{
}

template <typename CONFIG>
void Instruction<CONFIG>::Initialize(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info)
{
	InstructionBase::Initialize(instr_info.type, instr_info.size, instr_info.mode);
	if(instr_info.opcode)
		opcode.assign(instr_info.opcode, instr_info.opcode + size);
	else
		std::fill(opcode.begin(), opcode.end(), uint8_t(0));
	
	addr = instr_info.addr;
	target = instr_info.target;
	fallthrough = instr_info.fallthrough;
}

template <typename CONFIG>
Instruction<CONFIG>& Instruction<CONFIG>::operator = (const Instruction<CONFIG>& other)
{
	InstructionBase::operator = ((const InstructionBase &) other);
	opcode.resize(size);
	opcode.assign(&other.opcode[0], &other.opcode[0] + size);
	addr = other.addr;
	target = other.target;
	fallthrough = other.fallthrough;
	return *this;
}

template <typename CONFIG>
Instruction<CONFIG>& Instruction<CONFIG>::operator = (Instruction<CONFIG>&& other)
{
	InstructionBase::operator = (std::move(other));
	opcode.resize(size);
	opcode.assign(&other.opcode[0], &other.opcode[0] + size);
	addr = std::move(other.addr);
	target = std::move(other.target);
	fallthrough = std::move(other.fallthrough);
	return *this;
}

template <typename CONFIG>
bool Instruction<CONFIG>::DeepCompare(const Instruction<CONFIG>& other) const
{
	if(addr != other.addr) return false;
	if(size != other.size) return false;
	if(!std::equal(opcode.begin(), opcode.end(), other.opcode.begin())) return false;
	if(CONFIG::CHECK)
	{
		assert(type == other.type);
		if((type == JUMP) || (type == BRANCH) || (type == CALL))
		{
			assert(mode == other.mode);
		}
		if((type == BRANCH) || (type == CALL))
		{
			assert(fallthrough == other.fallthrough);
		}
		if(((type == JUMP) || (type == BRANCH) || (type == CALL)) && (mode == DIRECT))
		{
			assert(target == other.target);
		}
	}
	return true;
}

template <typename CONFIG>
void Instruction<CONFIG>::Print(std::ostream& stream) const
{
	FScope<std::ostream> fscope(stream);
	stream << "Instruction(addr=@0x" << std::hex << addr << std::dec << ", type=" << Type(type) << ", size=" << size << ", opcode=[";
	for(unsigned idx = 0; idx < size; ++idx)
	{
		if(idx) stream << " ";
		stream << "0x" << std::hex << +opcode[idx] << std::dec;
	}
	stream << "]";
	if((type == JUMP) || (type == BRANCH) || (type == CALL))
	{
		stream << ", target=@0x" << std::hex << target << std::dec;
	}
	if((type == BRANCH) || (type == CALL))
	{
		stream << ", fallthrough=@0x" << std::hex << fallthrough << std::dec;
	}
	if((type == JUMP) || (type == BRANCH) || (type == CALL))
	{
		stream << ", mode=" << Mode(mode);
	}
	stream << ")";
}

template <typename CONFIG>
JSON_Value *Instruction<CONFIG>::Save(const Config<CONFIG>& config) const
{
	JSON_Object *object = new JSON_Object();
	object->Add(new JSON_Member("kind", new JSON_String("instruction")));
	object->Add(new JSON_Member("type", new JSON_String(ToString(Type(type)))));
	object->Add(new JSON_Member("address", new JSON_Integer(addr)));
	object->Add(new JSON_Member("size", new JSON_Integer(size)));
	if((type == JUMP) || (type == BRANCH) || (type == CALL))
	{
		object->Add(new JSON_Member("target", new JSON_Integer(target)));
	}
	if((type == BRANCH) || (type == CALL))
	{
		object->Add(new JSON_Member("fallthrough", new JSON_Integer(fallthrough)));
	}
	if((type == JUMP) || (type == BRANCH) || (type == CALL))
	{
		object->Add(new JSON_Member("mode", new JSON_String(ToString(Mode(mode)))));
	}
	JSON_Array *opcode_array = new JSON_Array();
	for(unsigned idx = 0; idx < size; ++idx)
	{
		opcode_array->Push(new JSON_Integer(opcode[idx]));
	}
	object->Add(new JSON_Member("opcode", opcode_array));
	if(config.disassembly_if)
	{
		ADDRESS next_addr = 0;
		object->Add(new JSON_Member("disasm", new JSON_String(config.disassembly_if->Disasm(addr, next_addr) /*disassembly_if->Disasm(addr, &opcode[0], size)*/)));
	}
	return object;
}

template <typename CONFIG>
Instruction<CONFIG> *Instruction<CONFIG>::Load(const JSON_Value& value)
{
	try
	{
		const JSON_Object& object = value.AsObject();
		
		const JSON_String& kind = object["kind"].AsString();
		if((const std::string&) kind != "instruction") kind.Bad("Expected \"instruction\" as \"kind\"");
		const JSON_String& type_string = object["type"].AsString();
		Type type_struct((const std::string&) type_string);
		if(!type_struct.Valid()) type_string.Bad("Invalid type");
		unsigned type = type_struct;
		
		ADDRESS addr = object["address"].AsInteger();
		
		unsigned size = object["size"].AsInteger();
		
		ADDRESS target = ((type == JUMP) || (type == BRANCH) || (type == CALL)) ? ADDRESS(object["target"].AsInteger()) : ADDRESS(0);
		
		ADDRESS fallthrough = ((type == BRANCH) || (type == CALL)) ? ADDRESS(object["fallthrough"].AsInteger()) : ADDRESS(0);
		
		unsigned mode = DIRECT;
		if((type == JUMP) || (type == BRANCH) || (type == CALL))
		{
			const JSON_String& mode_string = object["mode"].AsString();
			Mode mode_struct((const std::string&) mode_string);
			if(!mode_struct.Valid()) mode_string.Bad("Invalid mode");
			mode = mode_struct;
		}
		
		std::vector<uint8_t> opcode(size);
		const JSON_Array& opcode_array = object["opcode"].AsArray();
		if(opcode_array.Length() != size) opcode_array.Bad("bad opcode size");
		for(unsigned idx = 0; idx < size; ++idx)
		{
			const JSON_Integer& byte = opcode_array[idx].AsInteger();
			opcode[idx] = byte;
		}
		
		if(type == STANDARD)
			return new StandardInstruction<CONFIG>(addr, size, &opcode[0]);
		else if(type == JUMP)
			return new JumpInstruction<CONFIG>(addr, size, &opcode[0], target, mode);
		else if(type == BRANCH)
			return new BranchInstruction<CONFIG>(addr, size, &opcode[0], target, fallthrough, mode);
		else if(type == CALL)
			return new CallInstruction<CONFIG>(addr, size, &opcode[0], target, fallthrough, mode);
		else if(type == RETURN)
			return new ReturnInstruction<CONFIG>(addr, size, &opcode[0]);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

////////////////////////////////// Group<> ////////////////////////////////////

template <typename CONFIG>
Group<CONFIG>::Group(Group<CONFIG>& group, ADDRESS addr)
	: instructions()
	, leader_idx(0)
{
	while(group.Valid())
	{
		const Instruction<CONFIG>& leader = group.Leader();
		if(leader.GetAddress() == addr) break;
		AddInstruction(leader);
		group.PopLeader();
	}
	if(CONFIG::CHECK)
	{
		assert(group.Valid());
		assert(!Conflicts(group));
		assert((Tail().GetAddress() + Tail().GetSize()) == group.Leader().GetAddress());
	}
}

template <typename CONFIG>
bool Group<CONFIG>::DeepCompare(const Group<CONFIG>& other) const
{
	if(GetAddress() != other.GetAddress()) return false;
	if(Length() != other.Length()) return false;
	for(typename Instructions::const_iterator itr1 = instructions.begin() + leader_idx, itr2 = other.instructions.begin() + other.leader_idx; (itr1 != instructions.end()) && (itr2 != other.instructions.end()); ++itr1, ++itr2)
	{
		const Instruction<CONFIG>& instr1 = *itr1;
		const Instruction<CONFIG>& instr2 = *itr2;
		if(!instr1.DeepCompare(instr2)) return false;
	}
	
	return true;
}

template <typename CONFIG>
bool Group<CONFIG>::Has(ADDRESS addr) const
{
	for(typename Instructions::const_iterator itr = instructions.begin() + leader_idx; itr != instructions.end(); ++itr)
	{
		if(itr->GetAddress() == addr) return true;
	}
	return false;
}

template <typename CONFIG>
bool Group<CONFIG>::Conflicts(const Group<CONFIG>& group) const
{
	for(typename Instructions::const_iterator itr = instructions.begin() + leader_idx; itr != instructions.end(); ++itr)
	{
		const Instruction<CONFIG>& instr = *itr;
		if(group.Has(instr.GetAddress())) return true;
	}
	return false;
}

template <typename CONFIG>
const Instruction<CONFIG> *Group<CONFIG>::Next(const Instruction<CONFIG>& instruction) const
{
	for(typename Instructions::const_iterator itr = instructions.begin() + leader_idx; itr != instructions.end(); ++itr)
	{
		const Instruction<CONFIG>& curr_instr = *itr;
		if(curr_instr == instruction)
		{
			++itr;
			if(itr == instructions.end()) break;
			return &(*itr);
		}
	}
	return 0;
}

template <typename CONFIG>
void Group<CONFIG>::AddInstruction(Instruction<CONFIG>&& instr)
{
	if(Valid())
	{
		const Instruction<CONFIG>& tail = Tail();
		assert((tail.GetAddress() + tail.GetSize()) == instr.GetAddress());
	}
	instructions.push_back(std::move(instr));
}

template <typename CONFIG>
void Group<CONFIG>::AddInstruction(const Instruction<CONFIG>& instr)
{
	if(Valid())
	{
		const Instruction<CONFIG>& tail = Tail();
		assert((tail.GetAddress() + tail.GetSize()) == instr.GetAddress());
	}
	instructions.push_back(instr);
}

template <typename CONFIG>
void Group<CONFIG>::Print(std::ostream& stream) const
{
	stream << "Group(" << std::endl;
	for(typename Instructions::const_iterator itr = instructions.begin() + leader_idx; itr != instructions.end(); ++itr)
	{
		stream << "\t" << *itr << std::endl;
	}
	stream << ")";
}

template <typename CONFIG>
JSON_Value *Group<CONFIG>::Save(const Config<CONFIG>& config) const
{
	JSON_Array *array = new JSON_Array();
	for(typename Instructions::const_iterator itr = instructions.begin() + leader_idx; itr != instructions.end(); ++itr)
	{
		const Instruction<CONFIG>& instruction = *itr;
		array->Push(instruction.Save(config));
	}
	return array;
}

template <typename CONFIG>
Group<CONFIG> *Group<CONFIG>::Load(const JSON_Value& value)
{
	Group<CONFIG> *group = new Group<CONFIG>();
	try
	{
		const JSON_Array& array = value.AsArray();
		
		for(JSON_Array::size_type idx = 0; idx < array.Length(); ++idx)
		{
			const JSON_Value& instruction_value = array[idx];
			Instruction<CONFIG> *instruction = Instruction<CONFIG>::Load(instruction_value);
			if(!instruction) throw std::runtime_error("group loading aborted");
			group->AddInstruction(*instruction);
			delete instruction;
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		delete group;
		group = 0;
	}
	return group;
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Group<CONFIG>::ScanInstructions(VISITOR& visitor) const
{
	for(typename Instructions::const_iterator itr = instructions.begin() + leader_idx; itr != instructions.end(); ++itr)
	{
		T ret = visitor.Visit(*itr);
		if(ret) return ret;
	}
	return T();
}

////////////////////////////////// Cache<> ////////////////////////////////////

template <typename CONFIG>
bool Cache<CONFIG>::Access(CacheAccess<CONFIG>& cache_access, uint64_t count)
{
	cache_access.idx = (cache_access.group.GetAddress() / CONFIG::MIN_OPCODE_SIZE) % CONFIG::CACHE_SIZE;
	CacheEntry<CONFIG>& cache_entry = cache_entries[cache_access.idx];
	cache_access.cache_entry = &cache_entry;
	if(cache_entry.cached_group.Valid() && (cache_entry.cached_group == cache_access.group))
	{
		if(CONFIG::CHECK) assert(cache_entry.cached_group.DeepCompare(cache_access.group));
		cache_entry.cached_count += count;
		return true;
	}
	return false;
}

template <typename CONFIG>
void Cache<CONFIG>::Replace(CacheAccess<CONFIG>& cache_access, Node<CONFIG> *working, uint64_t count)
{
	CacheEntry<CONFIG>& cache_entry = cache_entries[cache_access.idx];
	cache_entry = CacheEntry<CONFIG>(cache_access.group, working, count);
}

template <typename CONFIG>
void Cache<CONFIG>::FlushCounts(Node<CONFIG> *src)
{
	for(typename CacheEntries::iterator itr = cache_entries.begin(); itr != cache_entries.end(); ++itr)
	{
		CacheEntry<CONFIG>& cache_entry = *itr;
		if(cache_entry.cached_group.Valid() && (cache_entry.cached_count > 0))
		{
			src->FlushCounts(cache_entry.cached_group, cache_entry.cached_working, cache_entry.cached_count);
			assert(!cache_entry.cached_group.Valid());
		}
	}
}

////////////////////////////// TypedExtension<> ///////////////////////////////

template <typename CONFIG>
const ExtensionType TypedExtension<CONFIG, NODE_EXT>::Type = NODE_EXT;

template <typename CONFIG>
const ExtensionType TypedExtension<CONFIG, EDGE_EXT>::Type = EDGE_EXT;

template <typename CONFIG>
const ExtensionType TypedExtension<CONFIG, CFG_EXT>::Type = CFG_EXT;

///////////////////////////////// Extension<> /////////////////////////////////

template <typename CONFIG, ExtensionType EXT_TYPE, typename T>
const unsigned Extension<CONFIG, EXT_TYPE, T>::ID = AllocateExtensionId(EXT_TYPE);

/////////////////////////////// EdgeValue<> ///////////////////////////////////

template <typename CONFIG>
EdgeValue<CONFIG>::EdgeValue(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
	: EdgeValueBase(value)
	, extensions(ExtensionIds(EDGE_EXT))
{
	const JSON_Object& edge_object = value.AsObject();
	
	if(edge_object.HasProperty("extensions"))
	{
		const JSON_Object& extensions_object = edge_object["extensions"].AsObject();
		
		struct MemberVisitor
		{
			EdgeValue<CONFIG>& self;
			const std::initializer_list<ExtensionFactory *>& extension_factories;
			
			MemberVisitor(EdgeValue<CONFIG>& _self, const std::initializer_list<ExtensionFactory *>& _extension_factories) : self(_self), extension_factories(_extension_factories) {}
			
			bool Visit(const JSON_Member& member)
			{
				const std::string& name = member.GetName();
				for(ExtensionFactory *extension_factory : extension_factories)
				{
					ExtensionBase *ext = extension_factory->LoadExtension(EDGE_EXT, name, member.GetValue());
					if(ext)
					{
						self.SetExtension(ext->GetId(), ext);
						return false; // keep scanning members
					}
				}
				
				std::cerr << "WARNING! can't load edge extension " << name << std::endl;
				return false; // keep scanning members
			}
		};
		
		MemberVisitor member_visitor(*this, extension_factories);
		extensions_object.Scan(member_visitor);
	}
}

template <typename CONFIG>
EdgeValue<CONFIG>::~EdgeValue()
{
	InvalidateExtensions();
}

template <typename CONFIG>
void EdgeValue<CONFIG>::Save(JSON_Value& value) const
{
	EdgeValueBase::Save(value);
	
	JSON_Object& edge_object = value.AsObject();
	
	if(!extensions.empty())
	{
		JSON_Object *extensions_object = new JSON_Object();
		
		for(typename Extensions::const_iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
		{
			EdgeExtensionBase<CONFIG> *ext = *itr;
			if(ext)
			{
				extensions_object->Add(new JSON_Member(ext->GetName(), ext->Save()));
			}
		}
		
		edge_object.Add(new JSON_Member("extensions", extensions_object));
	}
}

template <typename CONFIG>
void EdgeValue<CONFIG>::Fix(const Mapping<CONFIG>& mapping, const CFG<CONFIG>& cfg, const Node<CONFIG>& node, const JSON_Value& value)
{
	const JSON_Object& edge_object = value.AsObject();
	
	if(edge_object.HasProperty("extensions"))
	{
		const JSON_Object& extensions_object = edge_object["extensions"].AsObject();
		
		for(typename Extensions::iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
		{
			EdgeExtensionBase<CONFIG> *ext = *itr;
			if(ext)
			{
				const JSON_Value& ext_value = extensions_object[ext->GetName()].AsValue();
				ext->Fix(mapping, cfg, node, *this, ext_value);
			}
		}
	}
}

template <typename CONFIG>
template <typename T>
T *EdgeValue<CONFIG>::SetExtension(T *ext)
{
	EdgeExtensionBase<CONFIG> *old_ext = extensions[T::ID];
	extensions[T::ID] = ext;
	return static_cast<T *>(old_ext);
}

template <typename CONFIG>
template <typename T>
void EdgeValue<CONFIG>::GetExtension(T *& ext) const
{
	ext = this->template GetExtension<T>();
}

template <typename CONFIG>
template <typename T>
T *EdgeValue<CONFIG>::GetExtension() const
{
	return static_cast<T *>(extensions[T::ID]);
}

template <typename CONFIG>
EdgeExtensionBase<CONFIG> *EdgeValue<CONFIG>::SetExtension(unsigned ext_id, ExtensionBase *ext)
{
	EdgeExtensionBase<CONFIG> *old_ext = (ext_id < extensions.size()) ? extensions[ext_id] : 0;
	EdgeExtensionBase<CONFIG> *edge_ext = dynamic_cast<EdgeExtensionBase<CONFIG> *>(ext);
	assert(edge_ext != 0);
	extensions[ext_id] = edge_ext;
	return old_ext;
}

template <typename CONFIG>
void EdgeValue<CONFIG>::InvalidateExtensions()
{
	for(typename Extensions::iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
	{
		EdgeExtensionBase<CONFIG> *ext = *itr;
		if(ext) ext->Free();
		*itr = 0;
	}
}

template <typename CONFIG>
void EdgeValue<CONFIG>::OutputGraphEdgeExtensions(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	for(typename Extensions::const_iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
	{
		EdgeExtensionBase<CONFIG> *extension = *itr;
		
		if(extension)
		{
			stream << indent << "\t\t<TR>" << std::endl
			       << indent << "\t\t\t<TD>" << StringToHTML(ToString(*extension)) << "</TD>" << std::endl
			       << indent << "\t\t</TR>" << std::endl;
		}
	}
}

/////////////////////////////////// Node<> ////////////////////////////////////

template <typename CONFIG>
Node<CONFIG>::~Node()
{
	InvalidateExtensions();
}

template <typename CONFIG>
void Node<CONFIG>::ConnectTo(Node<CONFIG> *successor, EdgeValue<CONFIG>&& edge_value)
{
	assert((GetCategory() == ENTRY) || (GetCategory() == BLOCK));
	typename Edges::iterator itr = edges.find(successor);
	if(itr != edges.end())
	{
		(*itr).second.Update(edge_value);
	}
	else
	{
		edges.emplace(successor, std::move(edge_value));
		successor->predecessors.push_back(this);
		if(successor->GetCategory() == UNKNOWN)
		{
			assert(!has_unknown_successor);
			has_unknown_successor = true;
		}
	}
}

template <typename CONFIG>
void Node<CONFIG>::ConnectTo(Node<CONFIG> *successor, uint64_t count, unsigned tag)
{
	assert((GetCategory() == ENTRY) || (GetCategory() == BLOCK));
	typename Edges::iterator itr = edges.find(successor);
	if(itr != edges.end())
	{
		(*itr).second.Update(count);
	}
	else
	{
		edges.emplace(successor, EdgeValue<CONFIG>(count, tag));
		successor->predecessors.push_back(this);
		if(successor->GetCategory() == UNKNOWN)
		{
			assert(!has_unknown_successor);
			has_unknown_successor = true;
		}
	}
}

template <typename CONFIG>
uint64_t Node<CONFIG>::Reconnect(Node<CONFIG> *node, Node<CONFIG> *new_node)
{
	assert((GetCategory() == ENTRY) || (GetCategory() == BLOCK));
	assert((node->GetCategory() == BLOCK) || (node->GetCategory() == PHANTOM));
	uint64_t count = 0;
	typename Edges::iterator itr = edges.find(node);
	if(itr != edges.end())
	{
		EdgeValue<CONFIG> edge_value((*itr).second);
		count = edge_value.GetCount();
		edges.erase(itr);
		edges.emplace(new_node, edge_value);
		new_node->predecessors.push_back(this);
	}
	return count;
}

template <typename CONFIG>
bool Node<CONFIG>::HasPhantomSuccessor() const
{
	for(typename Edges::const_iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		Node<CONFIG> *node = (*itr).first;
		if(node->GetCategory() == PHANTOM) return true;
	}
	return false;
}

template <typename CONFIG>
void Node<CONFIG>::PrintEdges(std::ostream& stream) const
{
	stream << "Edges(";
	for(typename Edges::const_iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		if(itr != edges.begin()) stream << ",";
		Node<CONFIG> *node = (*itr).first;
		const EdgeValue<CONFIG>& edge_value = (*itr).second;
		stream << "Edge(node=" << node->GetName() << ",edge_value=" << edge_value << ")";
	}
	stream << ")";
}

template <typename CONFIG>
void Node<CONFIG>::FlushCounts(Group<CONFIG>& group, Node<CONFIG> *dst, uint64_t count)
{
	assert(group.Valid());
	for(typename Edges::iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		Node<CONFIG> *node = (*itr).first;
		EdgeValue<CONFIG>& edge_value = (*itr).second;
		if(node->GetCategory() == BLOCK)
		{
			Block<CONFIG>& block = node->AsBlock();
			// Match instructions
			const Instruction<CONFIG>& leader = block.Leader();
			typename Block<CONFIG>::const_iterator itr = block.begin();
			if(leader == group.Leader())
			{
				edge_value.Update(count);
				const Instruction<CONFIG> *instr = &leader;
				do
				{
					group.PopLeader();
					++itr;
					const Instruction<CONFIG> *next_instr = (itr != block.end()) ? &(*itr) : 0;
					if(CONFIG::CHECK) assert(block.Next(*instr) == next_instr);
					instr = next_instr;
				}
				while(instr && group.Valid() && ((*instr) == group.Leader()));
				assert(instr == 0); // no more instructions in this node
				if(group.Valid()) // path is not finished?
				{
					// follow edge
					node->FlushCounts(group, dst, count);
				}
				return;
			}
		}
	}
	throw std::runtime_error("Internal error!");
}

template <typename CONFIG>
void Node<CONFIG>::OutputGraphEdges(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	// Note: edges are ordered by pointer, but this shall not affect the order in the .dot output
	struct EdgeVisitor
	{
		const Node<CONFIG>& self;
		std::ostream& stream;
    const std::string& indent;
		const CFG<CONFIG>& cfg;
		const Config<CONFIG>& config;
		bool subgraph;
		
		EdgeVisitor(
			const Node<CONFIG>& _self,
			std::ostream& _stream,
			const std::string& _indent,
			const CFG<CONFIG>& _cfg,
			const Config<CONFIG>& _config,
			bool _subgraph)
			: self(_self)
			, stream(_stream)
			, indent(_indent)
			, cfg(_cfg)
			, config(_config)
			, subgraph(_subgraph)
		{
		}
		
		bool Visit(Node<CONFIG> *node, const EdgeValue<CONFIG>& edge_value)
		{
			uint64_t count = edge_value.GetCount();
			unsigned tag = edge_value.GetTag();
			const std::string& color = (tag == TARGET) ? config.graphviz_branch_target_color
			                                           : ((tag == FALLTHROUGH) ? config.graphviz_branch_fallthrough_color
			                                                                   : config.graphviz_color);
			bool straight = (node->GetCategory() == BLOCK) || (node->GetCategory() == EXIT) || (node->GetCategory() == HALT);
			stream << indent << self.GetGraphNodeName(cfg)
			       << " -> "
			       << node->GetGraphNodeName(cfg)
			       << " [style=\"" << ((node->GetCategory() == UNKNOWN) ? "dashed" : "solid")
			       << (count ? ",bold" : "") << "\""
			       << ",color=\"" << color << "\""
			       << ",weight=" << (straight ? "1000": "1")
			       << ((node == &self) ? ",dir=\"back\"" : "")
			       << ",constraint=true"
			       << ",fontname=\"" << config.graphviz_fontname << "\""
			       << ",label=";
			
			if(node->GetCategory() != UNKNOWN)
			{
				stream << "<" << std::endl
				       << indent << "\t<TABLE CELLPADDING=\"2\" BORDER=\"0\" CELLBORDER=\"0\">" << std::endl
				       << indent << "\t\t<TR>" << std::endl
				       << indent << "\t\t\t<TD>" << std::dec << count << "</TD>" << std::endl
				       << indent << "\t\t</TR>" << std::endl;
				
				edge_value.OutputGraphEdgeExtensions(stream, indent, cfg, config, subgraph);
				
				stream << indent << "\t</TABLE>" << std::endl
				       << indent << ">";
			}
			else
			{
				stream << "\"\"";
			}
			stream << "];" << std::endl;
			return false; // keep scanning edges
		}
	};
	
	EdgeVisitor edge_visitor(*this, stream, indent, cfg, config, subgraph);
	ScanEdgesOrdered(edge_visitor);
}

template <typename CONFIG>
void Node<CONFIG>::OutputGraphNodeExtensions(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	for(typename Extensions::const_iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
	{
		NodeExtensionBase<CONFIG> *extension = *itr;
		
		if(extension)
		{
			stream << indent << "\t\t<HR/>" << std::endl
			       << indent << "\t\t<TR>" << std::endl
			       << indent << "\t\t\t<TD ALIGN=\"LEFT\"><B>[" << StringToHTML(extension->GetName()) << "]</B></TD>" << std::endl
			       << indent << "\t\t</TR>" << std::endl
			       << indent << "\t\t<TR>" << std::endl
			       << indent << "\t\t\t<TD ALIGN=\"LEFT\">" << StringToHTML(ToString(*extension)) << "</TD>" << std::endl
			       << indent << "\t\t</TR>" << std::endl;
		}
	}
}

template <typename CONFIG>
JSON_Object *Node<CONFIG>::SaveEdge(Node<CONFIG> *node, const EdgeValue<CONFIG>& edge_value) const
{
	JSON_Object *edge_object = new JSON_Object();
	edge_object->Add(new JSON_Member("kind", new JSON_String("edge")));
	edge_value.Save(*edge_object);
	JSON_Object *ref_object = new JSON_Object();
	if((node->GetCategory() == BLOCK) || (node->GetCategory() == PHANTOM))
	{
		ADDRESS node_addr = (node->GetCategory() == BLOCK) ? node->AsBlock().GetAddress()
		                                                   : node->AsPhantom().GetAddress();
		ref_object->Add(new JSON_Member("kind", new JSON_String("address")));
		ref_object->Add(new JSON_Member("address", new JSON_Integer(node_addr)));
	}
	else if(node->GetCategory() == UNKNOWN)
	{
		unsigned unknown_id = node->AsUnknown().GetId();
		ref_object->Add(new JSON_Member("kind", new JSON_String("id")));
		ref_object->Add(new JSON_Member("id", new JSON_Integer(unknown_id)));
	}
	else
	{
		ref_object->Add(new JSON_Member("kind", new JSON_String("name")));
		ref_object->Add(new JSON_Member("name", new JSON_String(ToString(Category(node->GetCategory())))));
	}
	edge_object->Add(new JSON_Member("ref", ref_object));
	return edge_object;
}

template <typename CONFIG>
JSON_Object *Node<CONFIG>::SaveAsObject() const
{
	JSON_Object *object = NodeBase::SaveAsObject();
	
	if(!edges.empty())
	{
		JSON_Array *edges_array = new JSON_Array();
		
		// Note: edges are ordered by pointer, but this shall not affect the order in the JSON output
		
		struct EdgeVisitor
		{
			const Node<CONFIG>& self;
			JSON_Array *edges_array;
			
			EdgeVisitor(const Node<CONFIG>& _self, JSON_Array *_edges_array) : self(_self), edges_array(_edges_array) {}
			
			bool Visit(Node<CONFIG> *node, const EdgeValue<CONFIG>& edge_value)
			{
				edges_array->Push(self.SaveEdge(node, edge_value));
				return false; // keep scanning edges
			}
		};
		
		EdgeVisitor edge_visitor(*this, edges_array);
		ScanEdgesOrdered(edge_visitor);

		object->Add(new JSON_Member("edges", edges_array));
	}
	
	if(!extensions.empty())
	{
		JSON_Object *extensions_object = new JSON_Object();
		
		for(typename Extensions::const_iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
		{
			NodeExtensionBase<CONFIG> *ext = *itr;
			if(ext)
			{
				extensions_object->Add(new JSON_Member(ext->GetName(), ext->Save()));
			}
		}
		
		object->Add(new JSON_Member("extensions", extensions_object));
	}
	
	return object;
}

template <typename CONFIG>
JSON_Value *Node<CONFIG>::Save(const Config<CONFIG>& config) const
{
	return SaveAsObject();
}

template <typename CONFIG>
Node<CONFIG> *Node<CONFIG>::Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	Node<CONFIG> *node = 0;
	try
	{
		const JSON_Object& object = value.AsObject();
		
		const JSON_String& kind = object["kind"].AsString();
		if((const std::string&) kind != "node") kind.Bad("Expected \"node\" as \"kind\"");
		const JSON_String& category_string = object["category"].AsString();
		Category category_struct((const std::string&) category_string);
		if(!category_struct.Valid()) category_string.Bad("Invalid category");
		unsigned category = category_struct;
		if(category == ENTRY)
			node = new Entry<CONFIG>();
		else if(category == EXIT)
			node = new Exit<CONFIG>();
		else if(category == HALT)
			node = new Halt<CONFIG>();
		else if(category == BLOCK)
			node = Block<CONFIG>::Load(value);
		else if(category == PHANTOM)
			node = Phantom<CONFIG>::Load(value);
		else if(category == UNKNOWN)
			node = Unknown<CONFIG>::Load(value);
		else
			throw std::runtime_error("Internal error!");
		
		if(object.HasProperty("extensions"))
		{
			const JSON_Object& extensions_object = object["extensions"].AsObject();
			
			struct MemberVisitor
			{
				Node<CONFIG>& self;
				const std::initializer_list<ExtensionFactory *>& extension_factories;
				
				MemberVisitor(Node<CONFIG>& _self, const std::initializer_list<ExtensionFactory *>& _extension_factories) : self(_self), extension_factories(_extension_factories) {}
				
				bool Visit(const JSON_Member& member)
				{
					const std::string& name = member.GetName();
					for(ExtensionFactory *extension_factory : extension_factories)
					{
						ExtensionBase *ext = extension_factory->LoadExtension(NODE_EXT, name, member.GetValue());
						if(ext)
						{
							self.SetExtension(ext->GetId(), ext);
							return false; // keep scanning members
						}
					}
					
					std::cerr << "WARNING! can't load node extension " << name << std::endl;
					return false; // keep scanning members
				}
			};
			
			MemberVisitor member_visitor(*node, extension_factories);
			extensions_object.Scan(member_visitor);
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		if(node)
		{
			delete node;
			node = 0;
		}
	}
	return node;
}

template <typename CONFIG>
bool Node<CONFIG>::Fix(Mapping<CONFIG>& mapping, CFG<CONFIG>& cfg, const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	try
	{
		const JSON_Object& object = value.AsObject();
		
		if(object.HasProperty("edges"))
		{
			const JSON_Array& edges_array = object["edges"].AsArray();
			for(JSON_Array::size_type idx = 0; idx < edges_array.Length(); ++idx)
			{
				const JSON_Object& edge_object = edges_array[idx].AsObject();
				const JSON_String& edge_kind = edge_object["kind"].AsString();
				if((const std::string&) edge_kind != "edge") edge_kind.Bad("Expected \"edge\" as \"kind\"");
				
				const JSON_Object& edge_ref = edge_object["ref"].AsObject();
				const JSON_String& edge_ref_kind = edge_ref["kind"].AsString();
				if((const std::string&) edge_ref_kind == "address")
				{
					const JSON_Integer& edge_ref_address = edge_ref["address"].AsInteger();
					ADDRESS successor_addr = ADDRESS(edge_ref_address);
					Node<CONFIG> *successor = cfg.FindNodeWithAddr(successor_addr);
					if(!successor) edge_ref_address.Bad("successor of category block or phantom not found");
					cfg.AddEdge(this, successor, EdgeValue<CONFIG>(edge_object, extension_factories));
				}
				else if((const std::string&) edge_ref_kind == "id")
				{
					const JSON_Integer& edge_ref_id = edge_ref["id"].AsInteger();
					unsigned unknown_id = unsigned(edge_ref_id);
					Node<CONFIG> *successor = cfg.FindUnknownWithId(unknown_id);
					if(!successor) edge_ref_id.Bad("successor of category unknown not found");
					cfg.AddEdge(this, successor, EdgeValue<CONFIG>(edge_object, extension_factories));
				}
				else if((const std::string&) edge_ref_kind == "name")
				{
					const JSON_String& edge_ref_name = edge_ref["name"].AsString();
					const std::string& successor_name = (const std::string&) edge_ref_name;
					Category successor_category_struct(successor_name);
					unsigned successor_category = successor_category_struct;
					if(successor_category == EXIT) cfg.AddEdge(this, cfg.GetExit(), EdgeValue<CONFIG>(edge_object, extension_factories));
					else if(successor_category == HALT) cfg.AddEdge(this, cfg.GetHalt(), EdgeValue<CONFIG>(edge_object, extension_factories));
					else edge_ref_name.Bad("successor is not of category exit or halt");
				}
				else
				{
					edge_ref_kind.Bad("Invalid kind");
				}
			}
		}
		
		if(object.HasProperty("extensions"))
		{
			const JSON_Object& extensions_object = object["extensions"].AsObject();
			
			for(typename Extensions::iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
			{
				NodeExtensionBase<CONFIG> *ext = *itr;
				if(ext)
				{
					const JSON_Value& ext_value = extensions_object[ext->GetName()].AsValue();
					ext->Fix(mapping, cfg, *this, ext_value);
				}
			}
		}
		
		if(object.HasProperty("edges"))
		{
			const JSON_Array& edges_array = object["edges"].AsArray();
			JSON_Array::size_type idx = 0;
			for(typename Edges::iterator itr = edges.begin(); itr != edges.end(); ++itr, ++idx)
			{
				EdgeValue<CONFIG>& edge_value = (*itr).second;
				const JSON_Value& edge_value_value = edges_array[idx].AsValue();
				edge_value.Fix(mapping, cfg, *this, edge_value_value);
			}
		}
		
		return true;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return false;
}

template <typename CONFIG>
void Node<CONFIG>::Check() const
{
	if((GetCategory() != ENTRY) && (GetCategory() != EXIT) && (GetCategory() != HALT))
	{
		bool check_in_out_count = false;
		
		for(typename Edges::const_iterator itr = edges.begin(); itr != edges.end(); ++itr)
		{
			const Node<CONFIG> *node = (*itr).first;
			if((node->GetCategory() != PHANTOM) && (node->GetCategory() != UNKNOWN))
			{
				check_in_out_count = true;
				break;
			}
		}
	
		if(check_in_out_count)
		{
			uint64_t in_count = 0;
			for(typename Predecessors::const_iterator p_itr = predecessors.begin(); p_itr != predecessors.end(); ++p_itr)
			{
				const Node<CONFIG> *predecessor = *p_itr;
				for(typename Edges::const_iterator itr = predecessor->edges.begin(); itr != predecessor->edges.end(); ++itr)
				{
					const Node<CONFIG> *node = (*itr).first;
					const EdgeValue<CONFIG>& edge_value = (*itr).second;
					if(node == this)
					{
						uint64_t count = edge_value.GetCount();
						in_count += count;
					}
				}
			}
			
			uint32_t out_count = 0;
			for(typename Edges::const_iterator itr = edges.begin(); itr != edges.end(); ++itr)
			{
				const EdgeValue<CONFIG>& edge_value = (*itr).second;
				uint64_t count = edge_value.GetCount();
				out_count += count;
			}
			
			if(in_count != out_count)
			{
				std::cerr << "node=" << *this << std::endl;
				std::cerr << "in_count=" << std::dec << in_count << ", out_count=" << out_count << std::endl;
			}
			assert(in_count == out_count);
		}
	}
}

template <typename CONFIG>
void Node<CONFIG>::InvalidateExtensions()
{
	for(typename Edges::iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		EdgeValue<CONFIG>& edge_value = (*itr).second;
		edge_value.InvalidateExtensions();
	}
	
	for(typename Extensions::iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
	{
		NodeExtensionBase<CONFIG> *ext = *itr;
		if(ext) ext->Free();
		*itr = 0;
	}
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Node<CONFIG>::ScanEdges(VISITOR& visitor) const
{
	for(typename Edges::iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		T ret = visitor.Visit((*itr).first, (*itr).second);
		if(ret) return ret;
	}
	return T();
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Node<CONFIG>::ScanEdgesOrdered(VISITOR& visitor) const
{
	// edges toward exit and halt
	for(unsigned node_category: { EXIT, HALT, UNKNOWN })
	{
		for(typename Edges::const_iterator itr = edges.begin(); itr != edges.end(); ++itr)
		{
			Node<CONFIG> *node = (*itr).first;
			const EdgeValue<CONFIG>& edge_value = (*itr).second;
			if(node->GetCategory() == node_category)
			{
				T ret = visitor.Visit(node, edge_value);
				if(ret) return ret;
			}
		}
	}
	
	// edges toward blocks and phantoms by address
	typedef std::map<ADDRESS, typename Edges::value_type> _Edges;
	_Edges _edges;
	for(typename Edges::const_iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		Node<CONFIG> *node = (*itr).first;
		if((node->GetCategory() == BLOCK) || (node->GetCategory() == PHANTOM))
		{
			ADDRESS node_addr = (node->GetCategory() == BLOCK) ? node->AsBlock().GetAddress()
			                                                   : node->AsPhantom().GetAddress();
			_edges.emplace(node_addr, *itr);
		}
	}
	
	for(typename _Edges::const_iterator itr = _edges.begin(); itr != _edges.end(); ++itr)
	{
		typename Edges::value_type pair = (*itr).second;
		Node<CONFIG> *node = pair.first;
		const EdgeValue<CONFIG>& edge_value = pair.second;
		
		T ret = visitor.Visit(node, edge_value);
		if(ret) return ret;
	}
	
	return T();
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Node<CONFIG>::ScanSuccessors(VISITOR& visitor) const
{
	for(typename Edges::const_iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		T ret = visitor.Visit((*itr).first);
		if(ret) return ret;
	}
	return T();
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Node<CONFIG>::ScanPredecessors(VISITOR& visitor) const
{
	for(typename Predecessors::const_iterator itr = predecessors.begin(); itr != predecessors.end(); ++itr)
	{
		T ret = visitor.Visit(*itr);
		if(ret) return ret;
	}
	return T();
}

template <typename CONFIG>
template <typename T>
T *Node<CONFIG>::SetExtension(T *ext)
{
	return static_cast<T *>(SetExtension(T::ID, ext));
}

template <typename CONFIG>
template <typename T>
void Node<CONFIG>::GetExtension(T *& ext) const
{
	ext = this->template GetExtension<T>();
}

template <typename CONFIG>
template <typename T>
T *Node<CONFIG>::GetExtension() const
{
	return static_cast<T *>(extensions[T::ID]);
}

template <typename CONFIG>
NodeExtensionBase<CONFIG> *Node<CONFIG>::SetExtension(unsigned ext_id, ExtensionBase *ext)
{
	NodeExtensionBase<CONFIG> *old_ext = (ext_id < extensions.size()) ? extensions[ext_id] : 0;
	NodeExtensionBase<CONFIG> *node_ext = dynamic_cast<NodeExtensionBase<CONFIG> *>(ext);
	assert(node_ext != 0);
	extensions[ext_id] = node_ext;
	return old_ext;
}

/////////////////////////////////// Block<> ///////////////////////////////////

template <typename CONFIG>
std::string Block<CONFIG>::GetName() const
{
	std::ostringstream sstr; sstr << "block @0x" << std::hex << GetAddress(); return sstr.str();
}

template <typename CONFIG>
std::string Block<CONFIG>::GetGraphNodeName(const CFGBase& cfg) const
{
	std::ostringstream sstr; sstr << cfg.GetGraphName() << "_block_0x" << std::hex << GetAddress(); return sstr.str();
}

template <typename CONFIG>
void Block<CONFIG>::AddCall(CFG<CONFIG> *called, uint64_t count)
{
	ADDRESS called_addr = called->GetAddress();
	typename CallMap::iterator itr = call_map.find(called_addr);
	if(itr != call_map.end())
	{
		CallMapEntry<CONFIG>& call_map_entry = (*itr).second;
		call_map_entry.count += count;
	}
	else
	{
		call_map.emplace(called_addr, CallMapEntry(called, count));
	}
}

template <typename CONFIG>
void Block<CONFIG>::AddSignal(unsigned sigid, CFG<CONFIG> *called, uint64_t count)
{
	typename SignalMap::iterator itr = signal_map.find(sigid);
	if(itr != signal_map.end())
	{
		SignalMapEntry<CONFIG>& signal_map_entry = (*itr).second;
		signal_map_entry.count += count;
	}
	else
	{
		signal_map.emplace(sigid, SignalMapEntry(called, count));
	}
}

template <typename CONFIG>
void Block<CONFIG>::Print(std::ostream& stream) const
{
	stream << "Block(" << std::endl;
	stream << *group;
	stream << "," << std::endl;
	this->PrintEdges(stream);
	stream << ")";
}

template <typename CONFIG>
void Block<CONFIG>::OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	stream << indent << GetGraphNodeName(cfg)
	       << " [shape=none,margin=0,style=filled,color=\"" << config.graphviz_color << "\",fillcolor=\"" << config.graphviz_fillcolor << "\""
	       << ",fontname=\"" << config.graphviz_block_fontname << "\",label=<" << std::endl
	       << indent << "\t<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\">" << std::endl
	       << indent << "\t\t<TR>" << std::endl
	       << indent << "\t\t\t<TD ALIGN=\"LEFT\"><B>block</B></TD>" << std::endl
	       << indent << "\t\t</TR>" << std::endl
	       << indent << "\t\t<HR/>" << std::endl
	       << indent << "\t\t<TR>" << std::endl
	       << indent << "\t\t\t<TD ALIGN=\"LEFT\"><B>[group]</B></TD>" << std::endl
	       << indent << "\t\t</TR>" << std::endl
	       << indent << "\t\t<TR>" << std::endl
	       << indent << "\t\t\t<TD ALIGN=\"LEFT\">";
	
	struct Visitor
	{
		std::ostream& stream;
		const std::string& indent;
		const Config<CONFIG>& config;
		bool not_first;
		
		Visitor(
			std::ostream& _stream,
			const std::string& _indent,
			const Config<CONFIG>& _config)
			: stream(_stream)
			, indent(_indent)
			, config(_config)
			, not_first(false)
		{
		}
		
		bool Visit(const Instruction<CONFIG>& instr)
		{
			if(not_first)
			{
				stream << "</TD>" << std::endl
				       << indent << "\t\t</TR>" << std::endl
				       << indent << "\t\t<TR>" << std::endl
				       << indent << "\t\t\t<TD ALIGN=\"LEFT\">";
			}
			not_first = true;
			std::ostringstream sstr;
			ADDRESS next_addr = 0;
			sstr << "0x" << std::hex << instr.GetAddress() << std::dec << " [" << instr.GetSize() << "]: " << 
				// config.disassembly_if->Disasm(instr.GetAddress(), instr.GetOpcode(), instr.GetSize());
				config.disassembly_if->Disasm(instr.GetAddress(), next_addr);
			stream << StringToHTML(sstr.str());
			return false;
		}
	};
	
	Visitor visitor(stream, indent, config);
	if(config.disassembly_if) group->ScanInstructions(visitor);
	
	stream << "</TD>" << std::endl
	       << indent << "\t\t</TR>" << std::endl;
	
	if(!call_map.empty())
	{
		stream << indent << "\t\t<HR/>" << std::endl
		       << indent << "\t\t<TR>" << std::endl
		       << indent << "\t\t\t<TD ALIGN=\"LEFT\"><B>[calls]</B></TD>" << std::endl
		       << indent << "\t\t</TR>" << std::endl
		       << indent << "\t\t<TR>" << std::endl
		       << indent << "\t\t\t<TD ALIGN=\"LEFT\">";
		for(typename CallMap::const_iterator itr = call_map.begin(); itr != call_map.end(); ++itr)
		{
			if(itr != call_map.begin())
			{
				stream << "</TD>" << std::endl
				       << indent << "\t\t</TR>" << std::endl
				       << indent << "\t\t<TR>" << std::endl
				       << indent << "\t\t\t<TD ALIGN=\"LEFT\">";
			}
			const CallMapEntry<CONFIG>& call_map_entry = (*itr).second;
			std::ostringstream sstr;
			sstr << "0x" << std::hex << call_map_entry.cfg->GetAddress() << std::dec << " {" << call_map_entry.count << "}";
			if(config.symbol_table_lookup_if)
			{
				const unisim::util::debug::Symbol<ADDRESS> *symbol = config.symbol_table_lookup_if->FindSymbolByAddr(call_map_entry.cfg->GetAddress(), unisim::util::debug::SymbolBase::SYM_FUNC);
				if(symbol)
				{
					sstr << " (" << symbol->GetName() << ")";
				}
			}
			stream << StringToHTML(sstr.str());
		}
		stream << "</TD>" << std::endl
		       << indent << "\t\t</TR>" << std::endl;
	}
	
	if(!signal_map.empty())
	{
		stream << indent << "\t\t<HR/>" << std::endl
		       << indent << "\t\t<TR>" << std::endl
		       << indent << "\t\t\t<TD ALIGN=\"LEFT\"><B>[signals]</B></TD>" << std::endl
		       << indent << "\t\t</TR>" << std::endl
		       << indent << "\t\t<TR>" << std::endl
		       << indent << "\t\t\t<TD ALIGN=\"LEFT\">";
		for(typename SignalMap::const_iterator itr = signal_map.begin(); itr != signal_map.end(); ++itr)
		{
			unsigned sigid = (*itr).first;
			if(itr != signal_map.begin())
			{
				stream << "</TD>" << std::endl
				       << indent << "\t\t</TR>" << std::endl
				       << indent << "\t\t<TR>" << std::endl
				       << indent << "\t\t\t<TD ALIGN=\"LEFT\">";
			}
			const SignalMapEntry<CONFIG>& signal_map_entry = (*itr).second;
			std::ostringstream sstr;
			sstr << sigid << ": 0x" << std::hex << signal_map_entry.cfg->GetAddress() << std::dec << " {" << signal_map_entry.count << "}";
			stream << StringToHTML(sstr.str());
		}
		stream << "</TD>" << std::endl
		       << indent << "\t\t</TR>" << std::endl;
	}
	
	this->OutputGraphNodeExtensions(stream, indent, cfg, config, subgraph);
	
	stream << indent << "\t</TABLE>" << std::endl
	       << indent << ">];" << std::endl;
}

template <typename CONFIG>
void Block<CONFIG>::OutputGraphEdges(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	Node<CONFIG>::OutputGraphEdges(stream, indent, cfg, config, subgraph);
	
	if(config.graphviz_enable_call_edges && subgraph)
	{
		for(typename CallMap::const_iterator itr = call_map.begin(); itr != call_map.end(); ++itr)
		{
			const CallMapEntry<CONFIG>& call_map_entry = (*itr).second;
			CFG<CONFIG> *target_cfg = call_map_entry.cfg;
			stream << indent << GetGraphNodeName(cfg)
			       << " -> "
			       << target_cfg->GetEntry()->GetGraphNodeName(*target_cfg)
			       << " [style=\"solid,bold\",color=\"" << config.graphviz_call_color << "\""
			       << ",weight=1"
			       << ",constraint=true,fontname=\"" << config.graphviz_fontname << "\""
			       << ",lhead=\"cluster_" << target_cfg->GetGraphName() << "\""
			       << ",label=<<TABLE CELLPADDING=\"2\" BORDER=\"0\" CELLBORDER=\"0\"><TR><TD>" << std::dec << call_map_entry.count << "</TD></TR></TABLE>>"
			       << "];" << std::endl;
		}
	}
}

template <typename CONFIG>
JSON_Value *Block<CONFIG>::Save(const Config<CONFIG>& config) const
{
	JSON_Object *object = this->SaveAsObject();
	object->Add(new JSON_Member("address", new JSON_Integer(group->GetAddress())));
	object->Add(new JSON_Member("group", group->Save(config)));
	
	if(!call_map.empty())
	{
		JSON_Array *call_map_array = new JSON_Array();
		for(typename CallMap::const_iterator itr = call_map.begin(); itr != call_map.end(); ++itr)
		{
			const CallMapEntry<CONFIG>& call_map_entry = (*itr).second;
			ADDRESS cfg_addr = call_map_entry.cfg->GetAddress();
			JSON_Object *call_map_entry_object = new JSON_Object();
			call_map_entry_object->Add(new JSON_Member("kind", new JSON_String("call")));
			call_map_entry_object->Add(new JSON_Member("address", new JSON_Integer(cfg_addr)));
			if(config.symbol_table_lookup_if)
			{
				const unisim::util::debug::Symbol<ADDRESS> *symbol = config.symbol_table_lookup_if->FindSymbolByAddr(cfg_addr, unisim::util::debug::SymbolBase::SYM_FUNC);
				if(symbol)
				{
					call_map_entry_object->Add(new JSON_Member("name", new JSON_String(symbol->GetName())));
				}
			}
			call_map_entry_object->Add(new JSON_Member("count", new JSON_Integer(call_map_entry.count)));
			call_map_array->Push(call_map_entry_object);
		}
		object->Add(new JSON_Member("call_map", call_map_array));
	}
	
	if(!signal_map.empty())
	{
		JSON_Array *signal_map_array = new JSON_Array();
		for(typename SignalMap::const_iterator itr = signal_map.begin(); itr != signal_map.end(); ++itr)
		{
			unsigned sigid = (*itr).first;
			const SignalMapEntry<CONFIG>& signal_map_entry = (*itr).second;
			ADDRESS cfg_addr = signal_map_entry.cfg->GetAddress();
			JSON_Object *signal_map_entry_object = new JSON_Object();
			signal_map_entry_object->Add(new JSON_Member("kind", new JSON_String("signal")));
			signal_map_entry_object->Add(new JSON_Member("sigid", new JSON_Integer(sigid)));
			signal_map_entry_object->Add(new JSON_Member("address", new JSON_Integer(cfg_addr)));
			if(config.symbol_table_lookup_if)
			{
				const unisim::util::debug::Symbol<ADDRESS> *symbol = config.symbol_table_lookup_if->FindSymbolByAddr(cfg_addr, unisim::util::debug::SymbolBase::SYM_FUNC);
				if(symbol)
				{
					signal_map_entry_object->Add(new JSON_Member("name", new JSON_String(symbol->GetName())));
				}
			}
			signal_map_entry_object->Add(new JSON_Member("count", new JSON_Integer(signal_map_entry.count)));
			signal_map_array->Push(signal_map_entry_object);
		}
		object->Add(new JSON_Member("signal_map", signal_map_array));
	}
	
	return object;
}

template <typename CONFIG>
Block<CONFIG> *Block<CONFIG>::Load(const JSON_Value& value)
{
	try
	{
		const JSON_Object& object = value.AsObject();
		
		ADDRESS addr = object["address"].AsInteger();
		
		Group<CONFIG> *group = Group<CONFIG>::Load(object["group"].AsArray());
		if(!group) throw std::runtime_error("Block loading aborted");
		if(group->GetAddress() != addr) throw std::runtime_error("Internal error!");
		Block<CONFIG> *block = new Block<CONFIG>(group);
		return block;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

template <typename CONFIG>
bool Block<CONFIG>::Fix(Mapping<CONFIG>& mapping, CFG<CONFIG>& cfg, const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	if(!Node<CONFIG>::Fix(mapping, cfg, value, extension_factories)) return false;
	
	try
	{
		const JSON_Object& object = value.AsObject();
		
		if(object.HasProperty("call_map"))
		{
			const JSON_Array& call_map_array = object["call_map"].AsArray();
			for(JSON_Array::size_type idx = 0; idx < call_map_array.Length(); ++idx)
			{
				const JSON_Object& call_map_entry_object = call_map_array[idx].AsObject();
				const JSON_String& kind = call_map_entry_object["kind"].AsString();
				if((const std::string&) kind != "call") kind.Bad("Expected \"call\" as \"kind\"");
				ADDRESS cfg_addr = call_map_entry_object["address"].AsInteger();
				CFG<CONFIG> *cfg = mapping.Get(cfg_addr);
				uint64_t count = call_map_entry_object["count"].AsInteger();
				AddCall(cfg, count);
			}
		}
		
		if(object.HasProperty("signal_map"))
		{
			const JSON_Array& signal_map_array = object["signal_map"].AsArray();
			for(JSON_Array::size_type idx = 0; idx < signal_map_array.Length(); ++idx)
			{
				const JSON_Object& signal_map_entry_object = signal_map_array[idx].AsObject();
				const JSON_String& kind = signal_map_entry_object["kind"].AsString();
				if((const std::string&) kind != "signal") kind.Bad("Expected \"signal\" as \"kind\"");
				unsigned sigid = signal_map_entry_object["sigid"].AsInteger();
				ADDRESS cfg_addr = signal_map_entry_object["address"].AsInteger();
				CFG<CONFIG> *cfg = mapping.Get(cfg_addr);
				uint64_t count = signal_map_entry_object["count"].AsInteger();
				AddSignal(sigid, cfg, count);
			}
		}
		
		return true;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return false;
}

////////////////////////////////// Phantom<> //////////////////////////////////

template <typename CONFIG>
std::string Phantom<CONFIG>::GetName() const
{
	std::ostringstream sstr; sstr << "phantom @0x" << std::hex << GetAddress(); return sstr.str();
}

template <typename CONFIG>
std::string Phantom<CONFIG>::GetGraphNodeName(const CFGBase& cfg) const
{
	std::ostringstream sstr; sstr << cfg.GetGraphName() << "_phantom_0x" << std::hex << GetAddress(); return sstr.str();
}

template <typename CONFIG>
void Phantom<CONFIG>::Print(std::ostream& stream) const
{
	stream << "Phantom(addr=@0x" << std::hex << addr << std::dec << ")";
}

template <typename CONFIG>
void Phantom<CONFIG>::OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	FScope<std::ostream> fscope(stream);
	stream << indent << GetGraphNodeName(cfg) << " [shape=none,margin=0,style=\"dashed,filled\",color=\"" << config.graphviz_color << "\","
	       << "fillcolor=\"" << config.graphviz_fillcolor << "\",fontname=\"" << config.graphviz_phantom_fontname << "\",label=<" << std::endl
	       << indent << "\t<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" STYLE=\"DASHED\">" << std::endl
	       << indent << "\t\t<TR>" << std::endl
	       << indent << "\t\t\t<TD ALIGN=\"LEFT\" BORDER=\"1\" SIDES=\"B\" STYLE=\"DASHED\"><B>phantom</B></TD>" << std::endl
	       << indent << "\t\t</TR>" << std::endl
	       << indent << "\t\t<TR>" << std::endl
	       << indent << "\t\t\t<TD ALIGN=\"LEFT\"><B>[addr]</B></TD>" << std::endl
	       << indent << "\t\t</TR>" << std::endl
	       << indent << "\t\t<TR>" << std::endl
	       << indent << "\t\t\t<TD ALIGN=\"LEFT\">0x" << std::hex << addr << std::dec << "</TD>" << std::endl
	       << indent << "\t\t</TR>" << std::endl
	       << indent << "\t</TABLE>" << std::endl
	       << indent << ">];" << std::endl;
}

template <typename CONFIG>
JSON_Value *Phantom<CONFIG>::Save(const Config<CONFIG>& config) const
{
	JSON_Object *object = this->SaveAsObject();
	object->Add(new JSON_Member("address", new JSON_Integer(addr)));
	return object;
}

template <typename CONFIG>
Phantom<CONFIG> *Phantom<CONFIG>::Load(const JSON_Value& value)
{
	try
	{
		const JSON_Object& object = value.AsObject();
		
		ADDRESS addr = object["address"].AsInteger();
		
		Phantom<CONFIG> *phantom = new Phantom<CONFIG>(addr);
		return phantom;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

/////////////////////////////////// Entry<> ///////////////////////////////////

template <typename CONFIG>
void Entry<CONFIG>::Print(std::ostream& stream) const
{
	stream << "Entry(";
	this->PrintEdges(stream);
	stream << ")";
}

template <typename CONFIG>
void Entry<CONFIG>::OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	stream << indent << "{ rank=\"source\"; " << this->GetGraphNodeName(cfg)
	       << " [shape=circle,style=filled,color=\"" << config.graphviz_color << "\",fillcolor=\"" << config.graphviz_color << "\""
	       << ",width=0.3,height=0.3,fixedsize=true,label=\"\"]; }" << std::endl;
}

//////////////////////////////////// Exit<> ///////////////////////////////////

template <typename CONFIG>
void Exit<CONFIG>::Print(std::ostream& stream) const
{
	stream << "Exit()";
}

template <typename CONFIG>
void Exit<CONFIG>::OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	stream << indent << "{ rank=\"sink\"; " << this->GetGraphNodeName(cfg)
	       << " [shape=doublecircle,style=filled,color=\"" << config.graphviz_color << "\",fillcolor=\"" << config.graphviz_color << "\""
	       << ",width=0.3,height=0.3,fixedsize=true,label=\"\"]; }" << std::endl;
}

//////////////////////////////////// Halt<> ///////////////////////////////////

template <typename CONFIG>
void Halt<CONFIG>::Print(std::ostream& stream) const
{
	stream << "Halt()";
}

template <typename CONFIG>
void Halt<CONFIG>::OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	stream << indent << "{ rank=\"sink\"; " << this->GetGraphNodeName(cfg)
	       << " [shape=square,style=filled,color=\"" << config.graphviz_color << "\",fillcolor=\"" << config.graphviz_color << "\""
	       << ",width=0.3,height=0.3,fixedsize=true,peripheries=2,label=\"\"]; }" << std::endl;
}

////////////////////////////////// Unknown<> //////////////////////////////////

template <typename CONFIG>
std::string Unknown<CONFIG>::GetName() const
{
	std::ostringstream sstr; sstr << "unknown " << std::dec << id; return sstr.str();
}

template <typename CONFIG>
std::string Unknown<CONFIG>::GetGraphNodeName(const CFGBase& cfg) const
{
	std::ostringstream sstr; sstr << "unknown_" << std::dec << id; return sstr.str();
}

template <typename CONFIG>
void Unknown<CONFIG>::Print(std::ostream& stream) const
{
	stream << "Unknown(" << std::dec << id << ")";
}

template <typename CONFIG>
void Unknown<CONFIG>::OutputGraphNode(std::ostream& stream, const std::string& indent, const CFG<CONFIG>& cfg, const Config<CONFIG>& config, bool subgraph) const
{
	stream << indent << this->GetGraphNodeName(cfg)
	       << " [shape=plaintext,color=\"" << config.graphviz_color << "\",fontname=\"" << config.graphviz_fontname << "\""
	       << ",label=<?>];" << std::endl;
}

template <typename CONFIG>
JSON_Value *Unknown<CONFIG>::Save(const Config<CONFIG>& config) const
{
	JSON_Object *object = this->SaveAsObject();
	object->Add(new JSON_Member("id", new JSON_Integer(id)));
	return object;
}

template <typename CONFIG>
Unknown<CONFIG> *Unknown<CONFIG>::Load(const JSON_Value& value)
{
	try
	{
		const JSON_Object& object = value.AsObject();
		
		unsigned id = object["id"].AsInteger();
		
		Unknown<CONFIG> *unknown = new Unknown<CONFIG>(id);
		return unknown;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

///////////////////////////////////// CFG<> ///////////////////////////////////

template <typename CONFIG>
CFG<CONFIG>::~CFG()
{
	InvalidateExtensions();
	
	for(Node<CONFIG> *node : { (Node<CONFIG> *) entry, (Node<CONFIG> *) exit, (Node<CONFIG> *) halt })
	{
		if(node) delete node;
	}
	for(typename Nodes::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		delete (*itr).second;
	}
	for(typename Unknowns::const_iterator itr = unknowns.begin(); itr != unknowns.end(); ++itr)
	{
		delete (*itr);
	}
}

template <typename CONFIG>
std::string CFG<CONFIG>::GetName() const
{
	std::ostringstream sstr;
	sstr << "cfg @0x" << std::hex << GetAddress();
	return sstr.str();
}

template <typename CONFIG>
std::string CFG<CONFIG>::GetGraphName() const
{
	std::ostringstream sstr;
	sstr << "cfg_0x" << std::hex << GetAddress();
	return sstr.str();
}

template <typename CONFIG>
bool CFG<CONFIG>::IsComplete() const
{
	if(!entry) return false;
	if(!exit && !halt) return false;
  if(!unknowns.empty()) return false;
	for(typename Nodes::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		Node<CONFIG> *node = (*itr).second;
		if(node->GetCategory() == PHANTOM) return false;
	}
	
	return true;
}

template <typename CONFIG>
Block<CONFIG> *CFG<CONFIG>::AddNode(Block<CONFIG> *block)
{
	if(CONFIG::DEBUG) std::cerr << *this << "->AddNode(" << *block << ")" << std::endl;
	if(CONFIG::CHECK)
	{
		for(typename Nodes::iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
		{
			Node<CONFIG> *node = (*itr).second;
			if(node->GetCategory() == BLOCK)
			{
				assert(!node->AsBlock().Conflicts(*block));
			}
		}
	}
	
	ADDRESS block_addr = block->GetAddress();
	typename Nodes::iterator itr = nodes.find(block_addr);
	assert(itr == nodes.end());
	nodes.emplace(block_addr, block);
	if(!has_addr)
	{
		addr = block_addr;
		has_addr = true;
	}
	return block;
}

template <typename CONFIG>
Phantom<CONFIG> *CFG<CONFIG>::AddNode(Phantom<CONFIG> *phantom)
{
	if(CONFIG::DEBUG) std::cerr << *this << "->AddNode(" << *phantom << ")" << std::endl;
	ADDRESS phantom_addr = phantom->GetAddress();
	typename Nodes::iterator itr = nodes.find(phantom_addr);
	assert(itr == nodes.end());
	nodes.emplace(phantom_addr, phantom);
	if(!has_addr)
	{
		addr = phantom_addr;
		has_addr = true;
	}
	return phantom;
}

template <typename CONFIG>
Unknown<CONFIG> *CFG<CONFIG>::AddNode(Unknown<CONFIG> *unknown)
{
	unknowns.push_back(unknown);
	return unknown;
}

template <typename CONFIG>
bool CFG<CONFIG>::HasNode(Node<CONFIG> *node) const
{
	if(node->GetCategory() == ENTRY) return entry && (node == entry);
	if(node->GetCategory() == EXIT) return exit && (node == exit);
	if(node->GetCategory() == HALT) return halt && (node == halt);
	for(typename Nodes::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		Node<CONFIG> *existing_node = (*itr).second;
		if(existing_node == node) return true;
	}
	for(typename Unknowns::const_iterator itr = unknowns.begin(); itr != unknowns.end(); ++itr)
	{
		Node<CONFIG> *existing_node = *itr;
		if(existing_node == node) return true;
	}
	return false;
}

template <typename CONFIG>
void CFG<CONFIG>::AddEdge(Node<CONFIG> *src, Node<CONFIG> *dst, EdgeValue<CONFIG>&& edge_value)
{
	if(CONFIG::DEBUG)
	{
		std::cerr << *this << "->AddEdge(src=" << *src << ", dst=" << *dst << ", edge_value=" << edge_value << ")" << std::endl;
	}
	if(CONFIG::CHECK)
	{
		for(Node<CONFIG> *node: { src, dst })
		{
			assert(HasNode(node));
		}
	}
	
	src->ConnectTo(dst, std::move(edge_value));
}

template <typename CONFIG>
void CFG<CONFIG>::AddEdge(Node<CONFIG> *src, Node<CONFIG> *dst, uint64_t count, unsigned tag)
{
	if(CONFIG::DEBUG)
	{
		std::cerr << *this << "->AddEdge(src=" << *src << ", dst=" << *dst << ", count=" << count << ", tag=" << EdgeTag(tag) << ")" << std::endl;
	}
	if(CONFIG::CHECK)
	{
		for(Node<CONFIG> *node: { src, dst })
		{
			assert(HasNode(node));
		}
	}
	
	src->ConnectTo(dst, count, tag);
}

template <typename CONFIG>
Node<CONFIG> *CFG<CONFIG>::FindNodeWithAddr(ADDRESS node_addr) const
{
	if(CONFIG::DEBUG) { FScope<std::ostream> fscope(std::cerr); std::cerr << *this << "->FindNodeWithAddr(addr=@0x" << std::hex << node_addr << std::dec << ")" << std::endl; }
	
	typename Nodes::const_iterator itr = nodes.lower_bound(node_addr);
	if(itr != nodes.end())
	{
		Node<CONFIG> *node = (*itr).second;
		assert((node->GetCategory() == PHANTOM) || (node->GetCategory() == BLOCK));
		if(node->GetCategory() == PHANTOM)
		{
			if(node->AsPhantom().GetAddress() == node_addr) return node;
		}
		else
		{
			if(node->AsBlock().Has(node_addr)) return node;
		}
	}
	return 0;
}

template <typename CONFIG>
Node<CONFIG> *CFG<CONFIG>::FindUnknownWithId(unsigned id) const
{
	for(typename Unknowns::const_iterator itr = unknowns.begin(); itr != unknowns.end(); ++itr)
	{
		Unknown<CONFIG> *unknown = *itr;
		if(unknown->GetId() == id) return unknown;
	}
	return 0;
}

template <typename CONFIG>
Block<CONFIG> *CFG<CONFIG>::PhantomToBlock(Phantom<CONFIG> *phantom, Block<CONFIG> *block)
{
	if(CONFIG::DEBUG) std::cerr << *this << "->PhantomToBlock(phantom=" << *phantom << ", block=" << *block << ")" << std::endl;
	
	// remove the phantom
	typename Nodes::iterator itr = nodes.find(phantom->GetAddress());
	assert(itr != nodes.end());
	nodes.erase(itr);
	
	// update the edges in the predecessors of the phantom
	struct Visitor
	{
		Phantom<CONFIG> *phantom;
		Block<CONFIG> *block;
		
		Visitor(Phantom<CONFIG> *_phantom, Block<CONFIG> *_block) : phantom(_phantom), block(_block) {}
		
		bool Visit(Node<CONFIG> *predecessor)
		{
			predecessor->Reconnect(phantom, block);
			return false; // keep scanning predecessors
		}
	};
	
	Visitor visitor(phantom, block);
	phantom->ScanPredecessors(visitor);
	
	// delete the phantom
	delete phantom;
	// add the replacing block
	AddNode(block);
	if(CONFIG::CHECK && !(CONFIG::CACHE_SIZE > 0)) block->Check();
	return block;
}

template <typename CONFIG>
Block<CONFIG> *CFG<CONFIG>::SplitBlock(Block<CONFIG> *block, ADDRESS instr_addr)
{
	if(CONFIG::DEBUG) { FScope<std::ostream> fscope(std::cerr); std::cerr << *this << "->SplitBlock(block=" << *block << ", instr_addr=@0x" << std::hex << instr_addr << std::dec << ")" << std::endl; }
	if(CONFIG::CHECK && !(CONFIG::CACHE_SIZE > 0)) block->Check();
	
	// remove block
	typename Nodes::iterator itr = nodes.find(block->GetAddress());
	assert(itr != nodes.end());
	nodes.erase(itr);
  
	// split the block, stealing leadings instructions to fill a new block
	Block<CONFIG> *new_block = new Block<CONFIG>(block, instr_addr);
	// Note: new_block is just before block
	
	// update the edges in the predecessors of block being splitted
	struct Visitor
	{
		Block<CONFIG> *block;
		Block<CONFIG> *new_block;
		uint64_t count;
		
		Visitor(Block<CONFIG> *_block, Block<CONFIG> *_new_block) : block(_block), new_block(_new_block), count(0) {}
		
		bool Visit(Node<CONFIG> *predecessor)
		{
			count += predecessor->Reconnect(block, new_block);
			return false; // keep scanning predecessors
		}
	};
	
	Visitor visitor(block, new_block);
	block->ScanPredecessors(visitor);
	
	// add the two blocks
	AddNode(new_block);
	AddNode(block);
	// make an edge between the blocks
	AddEdge(new_block, block, visitor.count);
	if(CONFIG::CHECK && !(CONFIG::CACHE_SIZE > 0))
	{
		new_block->Check();
		block->Check();
	}
	return block;
}

template <typename CONFIG>
void CFG<CONFIG>::InvalidateExtensions()
{
	for(Node<CONFIG> *node : { (Node<CONFIG> *) entry, (Node<CONFIG> *) exit, (Node<CONFIG> *) halt })
	{
		if(node) node->InvalidateExtensions();
	}
	
	for(typename Nodes::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		Node<CONFIG> *node = (*itr).second;
		node->InvalidateExtensions();
	}
	
	for(typename Unknowns::const_iterator itr = unknowns.begin(); itr != unknowns.end(); ++itr)
	{
		Unknown<CONFIG> *node = *itr;
		node->InvalidateExtensions();
	}
	
	for(typename Extensions::iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
	{
		CFGExtensionBase<CONFIG> *ext = *itr;
		if(ext) ext->Free();
		*itr = 0;
	}
}

template <typename CONFIG>
void CFG<CONFIG>::Print(std::ostream& stream) const
{
	FScope<std::ostream> fscope(stream);
	stream << "CFG(";
	if(has_addr) stream << "addr=@" << std::hex << "0x" << addr << std::dec;
	stream << ")";
}

template <typename CONFIG>
void CFG<CONFIG>::OutputGraphNodes(std::ostream& stream, const std::string& indent, const Config<CONFIG>& config, bool subgraph) const
{
	// output graph nodes
	if(entry && entry->HasSuccessors()) entry->OutputGraphNode(stream, indent, *this, config, subgraph);
	for(typename Nodes::const_reverse_iterator itr = nodes.rbegin(); itr != nodes.rend(); ++itr)
	{
		Node<CONFIG> *node = (*itr).second;
		node->OutputGraphNode(stream, indent, *this, config, subgraph);
	}
	for(typename Unknowns::const_iterator itr = unknowns.begin(); itr != unknowns.end(); ++itr)
	{
		Node<CONFIG> *unknown = *itr;
		unknown->OutputGraphNode(stream, indent, *this, config, subgraph);
	}
	if(exit && exit->HasPredecessors()) exit->OutputGraphNode(stream, indent, *this, config, subgraph);
	if(halt && halt->HasPredecessors()) halt->OutputGraphNode(stream, indent, *this, config, subgraph);
}

template <typename CONFIG>
void CFG<CONFIG>::OutputGraphEdges(std::ostream& stream, const std::string& indent, const Config<CONFIG>& config, bool subgraph) const
{
	// output graph edges
	if(entry && entry->HasSuccessors()) entry->OutputGraphEdges(stream, indent, *this, config, subgraph);
	for(typename Nodes::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		Node<CONFIG> *node = (*itr).second;
		node->OutputGraphEdges(stream, indent, *this, config, subgraph);
	}
}

template <typename CONFIG>
void CFG<CONFIG>::OutputGraph(std::ostream& stream, const std::string& indent, const Config<CONFIG>& config, bool subgraph, unsigned filter) const
{
	FScope<std::ostream> fscope(stream);
	std::string next_indent(indent);
	
	if(filter & GRAPH)
	{
		next_indent += '\t';
		if(subgraph)
		{
			stream << indent << "subgraph \"cluster_" << GetGraphName() << "\" {" << std::endl;
			stream << next_indent << "style=\"dashed\";" << std::endl;
			stream << next_indent << "margin=32;" << std::endl;
		}
		else
		{
			stream << indent << "digraph \"" << GetGraphName() << "\" {" << std::endl;
			stream << next_indent << "overlap=false;" << std::endl;
			stream << next_indent << "splines=true;" << std::endl;
			stream << next_indent << "nodesep=0.5;" << std::endl;
			stream << next_indent << "colorscheme=\"X11\"" << std::endl;
		}
		
		stream << next_indent << "color=\"black\";" << std::endl;
		stream << next_indent << "fontname=\"" << config.graphviz_fontname << "\";" << std::endl;
		
		// output graph label
		if(has_addr)
		{
			stream << next_indent << "labelloc=\"t\";" << std::endl;
			stream << next_indent << "label=\"0x" << std::hex << GetAddress() << std::dec;
			if(config.symbol_table_lookup_if)
			{
				const unisim::util::debug::Symbol<ADDRESS> *symbol = config.symbol_table_lookup_if->FindSymbolByAddr(GetAddress(), unisim::util::debug::SymbolBase::SYM_FUNC);
				if(symbol)
				{
					stream << " (" << symbol->GetName() << ")";
				}
			}
			stream << "\";" << std::endl;
		}
	}
	
	if(filter & NODES)
	{
		stream << next_indent << std::endl << next_indent << "//////////////////////////////////// nodes ////////////////////////////////////" << std::endl << next_indent << std::endl;
		OutputGraphNodes(stream, next_indent, config, subgraph);
	}
	
	if(filter & EDGES)
	{
		stream << next_indent << std::endl << next_indent << "//////////////////////////////////// edges ////////////////////////////////////" << std::endl << next_indent << std::endl;
		OutputGraphEdges(stream, next_indent, config, subgraph);
	}
	
	if(filter & GRAPH)
	{
		stream << indent << "}" << std::endl;
	}
}

template <typename CONFIG>
JSON_Value *CFG<CONFIG>::Save(const Config<CONFIG>& config) const
{
	JSON_Object *object = new JSON_Object();
	object->Add(new JSON_Member("kind", new JSON_String("cfg")));
	if(has_addr)
	{
		object->Add(new JSON_Member("address", new JSON_Integer(addr)));
		if(config.symbol_table_lookup_if)
		{
			const unisim::util::debug::Symbol<ADDRESS> *symbol = config.symbol_table_lookup_if->FindSymbolByAddr(addr, unisim::util::debug::SymbolBase::SYM_FUNC);
			if(symbol)
			{
				object->Add(new JSON_Member("name", new JSON_String(symbol->GetName())));
			}
		}
	}
	
	object->Add(new JSON_Member("is_complete", new JSON_Boolean(IsComplete())));
	
	JSON_Array *nodes_array = new JSON_Array();
	if(entry) nodes_array->Push(entry->Save(config));
	if(exit) nodes_array->Push(exit->Save(config));
	if(halt) nodes_array->Push(halt->Save(config));
	for(typename Nodes::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		Node<CONFIG> *node = (*itr).second;
		nodes_array->Push(node->Save(config));
	}
	for(typename Unknowns::const_iterator itr = unknowns.begin(); itr != unknowns.end(); ++itr)
	{
		Node<CONFIG> *node = *itr;
		nodes_array->Push(node->Save(config));
	}
	object->Add(new JSON_Member("nodes", nodes_array));
	
	if(!extensions.empty())
	{
		JSON_Object *extensions_object = new JSON_Object();
		
		for(typename Extensions::const_iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
		{
			CFGExtensionBase<CONFIG> *ext = *itr;
			if(ext)
			{
				extensions_object->Add(new JSON_Member(ext->GetName(), ext->Save()));
			}
		}
		
		object->Add(new JSON_Member("extensions", extensions_object));
	}
	
	return object;
}

template <typename CONFIG>
CFG<CONFIG> *CFG<CONFIG>::Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	CFG<CONFIG> *cfg = 0;
	try
	{
		const JSON_Object& object = value.AsObject();
		
		const JSON_String& kind = object["kind"].AsString();
		if((const std::string&) kind != "cfg") kind.Bad("Expected \"cfg\" as \"kind\"");
		
		ADDRESS addr = object["address"].AsInteger();
		
		cfg = new CFG<CONFIG>(addr);
		
		const JSON_Array& nodes_array = object["nodes"].AsArray();
		
		for(JSON_Array::size_type idx = 0; idx < nodes_array.Length(); ++idx)
		{
			const JSON_Value& node_value = nodes_array[idx];
			Node<CONFIG> *node = Node<CONFIG>::Load(node_value, extension_factories);
			if(!node)
			{
				throw std::runtime_error("CFG loading aborted");
			}
			if(node->GetCategory() == ENTRY)
			{
				if(cfg->entry) node_value.Bad("Duplicated entry node");
				cfg->entry = node;
			}
			else if(node->GetCategory() == EXIT)
			{
				if(cfg->exit) node_value.Bad("Duplicated exit node");
				cfg->exit = node;
			}
			else if(node->GetCategory() == HALT)
			{
				if(cfg->halt) node_value.Bad("Duplicated halt node");
				cfg->halt = node;
			}
			else if(node->GetCategory() == BLOCK)
				cfg->AddNode(static_cast<Block<CONFIG> *>(node));
			else if(node->GetCategory() == PHANTOM)
				cfg->AddNode(static_cast<Phantom<CONFIG> *>(node));
			else if(node->GetCategory() == UNKNOWN)
				cfg->AddNode(static_cast<Unknown<CONFIG> *>(node));
			else
				throw std::runtime_error("Internal error!");
		}
		
		// cfg->LoadExtensions(object, extension_factories);
		if(object.HasProperty("extensions"))
		{
			const JSON_Object& extensions_object = object["extensions"].AsObject();
			
			struct MemberVisitor
			{
				CFG<CONFIG>& self;
				const std::initializer_list<ExtensionFactory *>& extension_factories;
				
				MemberVisitor(CFG<CONFIG>& _self, const std::initializer_list<ExtensionFactory *>& _extension_factories) : self(_self), extension_factories(_extension_factories) {}
				
				bool Visit(const JSON_Member& member)
				{
					const std::string& name = member.GetName();
					for(ExtensionFactory *extension_factory : extension_factories)
					{
						ExtensionBase *ext = extension_factory->LoadExtension(CFG_EXT, name, member.GetValue());
						if(ext)
						{
							self.SetExtension(ext->GetId(), ext);
							return false; // keep scanning members
						}
					}
					
					std::cerr << "WARNING! can't load CFG extension " << name << std::endl;
					return false; // keep scanning members
				}
			};
			
			MemberVisitor member_visitor(*cfg, extension_factories);
			extensions_object.Scan(member_visitor);
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		if(cfg)
		{
			delete cfg;
			cfg = 0;
		}
	}
	return cfg;
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T CFG<CONFIG>::ScanNodes(VISITOR& visitor) const
{
	for(Node<CONFIG> *node: { (Node<CONFIG> *) entry, (Node<CONFIG> *) exit, (Node<CONFIG> *) halt })
	{
		if(node)
		{
			T ret = visitor.Visit(node);
			if(ret) return ret;
		}
	}
	for(typename Nodes::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		T ret = visitor.Visit((*itr).second);
		if(ret) return ret;
	}
	for(typename Unknowns::const_iterator itr = unknowns.begin(); itr != unknowns.end(); ++itr)
	{
		T ret = visitor.Visit(*itr);
		if(ret) return ret;
	}
	return T();
}

template <typename CONFIG>
bool CFG<CONFIG>::Fix(Mapping<CONFIG>& mapping, const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	try
	{
		const JSON_Object& object = value.AsObject();
		const JSON_Array& nodes_array = object["nodes"].AsArray();
		for(JSON_Array::size_type idx = 0, len = nodes_array.Length(); idx < len; ++idx)
		{
			const JSON_Object& node_object = nodes_array[idx].AsObject();
			const JSON_String& category_string = node_object["category"].AsString();
			Category category_struct((const std::string&) category_string);
			if(!category_struct.Valid()) category_string.Bad("Invalid category");
			unsigned category = category_struct;
			Node<CONFIG> *node = 0;
			if(category == ENTRY)
			{
				node = entry;
			}
			else if(category == EXIT)
			{
				node = exit;
			}
			else if(category == HALT)
			{
				node = halt;
			}
			else if((category == BLOCK) || (category == PHANTOM))
			{
				ADDRESS node_addr = ADDRESS(node_object["address"].AsInteger());
				node = FindNodeWithAddr(node_addr);
			}
			else if(category == UNKNOWN)
			{
				unsigned unknown_id = unsigned(node_object["id"].AsInteger());
				node = FindUnknownWithId(unknown_id);
			}
			else
				throw std::runtime_error("Internal error!");
			
			assert(node != 0);
			if(!node->Fix(mapping, *this, node_object, extension_factories)) return false;
		}
		
		if(object.HasProperty("extensions"))
		{
			const JSON_Object& extensions_object = object["extensions"].AsObject();
			
			for(typename Extensions::iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
			{
				CFGExtensionBase<CONFIG> *ext = *itr;
				if(ext)
				{
					const JSON_Value& ext_value = extensions_object[ext->GetName()].AsValue();
					ext->Fix(mapping, *this, ext_value);
				}
			}
		}
		
		return true;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return false;
}

template <typename CONFIG>
void CFG<CONFIG>::Check()
{
	if(CONFIG::CHECK)
	{
		for(typename Nodes::iterator itr1 = nodes.begin(); itr1 != nodes.end(); ++itr1)
		{
			Node<CONFIG> *node1 = (*itr1).second;
			node1->Check();
			typename Nodes::iterator itr2 = itr1;
			for(++itr2; itr2 != nodes.end(); ++itr2)
			{
				Node<CONFIG> *node2 = (*itr2).second;
				if((node1->GetCategory() == BLOCK) && (node2->GetCategory() == BLOCK))
				{
					Block<CONFIG> *block1 = static_cast<Block<CONFIG> *>(node1);
					Block<CONFIG> *block2 = static_cast<Block<CONFIG> *>(node2);
					if(block1->Conflicts(*block2))
					{
						std::cerr << "block1=" << *block1 << std::endl;
						std::cerr << "block2=" << *block2 << std::endl;
					}
					assert(!block1->Conflicts(*block2));
				}
			}
		}
	}
}

template <typename CONFIG>
template <typename T>
T *CFG<CONFIG>::SetExtension(T *ext)
{
	CFGExtensionBase<CONFIG> *old_ext = extensions[T::ID];
	extensions[T::ID] = ext;
	return static_cast<T *>(old_ext);
}

template <typename CONFIG>
template <typename T>
void CFG<CONFIG>::GetExtension(T *& ext) const
{
	ext = this->template GetExtension<T>();
}

template <typename CONFIG>
template <typename T>
T *CFG<CONFIG>::GetExtension() const
{
	return static_cast<T *>(extensions[T::ID]);
}

template <typename CONFIG>
CFGExtensionBase<CONFIG> *CFG<CONFIG>::SetExtension(unsigned ext_id, ExtensionBase *ext)
{
	CFGExtensionBase<CONFIG> *old_ext = (ext_id < extensions.size()) ? extensions[ext_id] : 0;
	CFGExtensionBase<CONFIG> *node_ext = dynamic_cast<CFGExtensionBase<CONFIG> *>(ext);
	assert(node_ext != 0);
	extensions[ext_id] = node_ext;
	return old_ext;
}

/////////////////////////////////// Mapping<> /////////////////////////////////

template <typename CONFIG>
void Mapping<CONFIG>::Clear()
{
	for(typename Map::const_iterator itr = map.begin(); itr != map.end(); ++itr)
	{
		delete (*itr).second;
	}
	map.clear();
}

template <typename CONFIG>
void Mapping<CONFIG>::OutputGraph(std::ostream& stream, const Config<CONFIG>& config) const
{
	struct CFGVisitor
	{
		std::ostream& stream;
		const Config<CONFIG>& config;
		unsigned filter;
		
		CFGVisitor(std::ostream& _stream, const Config<CONFIG>& _config, unsigned _filter)
			: stream(_stream), config(_config), filter(_filter)
		{
		}
		
		bool Visit(const CFG<CONFIG> *cfg)
		{
			cfg->OutputGraph(stream, "\t", config, /* subgraph */ true, filter);
			return false; // keep scanning CFGs
		}
	};
	
	CFGVisitor cfg_visitor_graph_node(stream, config, GRAPH | NODES);
	ScanCFGsOrdered(cfg_visitor_graph_node);
	CFGVisitor cfg_visitor_edges(stream, config, EDGES);
	ScanCFGsOrdered(cfg_visitor_edges);
}

template <typename CONFIG>
JSON_Value *Mapping<CONFIG>::Save(const Config<CONFIG>& config) const
{
	JSON_Array *cfgs_array = new JSON_Array();
	
	struct CFGVisitor
	{
		JSON_Array *cfgs_array;
		const Config<CONFIG>& config;
		
		CFGVisitor(JSON_Array *_cfgs_array, const Config<CONFIG>& _config)
			: cfgs_array(_cfgs_array), config(_config)
		{
		}
		
		bool Visit(const CFG<CONFIG> *cfg)
		{
			cfgs_array->Push(cfg->Save(config));
			return false; // keep scanning CFGs
		}
	};
	
	CFGVisitor cfg_visitor(cfgs_array, config);
	ScanCFGsOrdered(cfg_visitor);
	
	return cfgs_array;
}

template <typename CONFIG>
bool Mapping<CONFIG>::Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	Clear();
	try
	{
		const JSON_Array& array = value.AsArray();
		
		std::vector<CFG<CONFIG> *> cfgs;
		for(JSON_Array::size_type idx = 0, len = array.Length(); idx < len; ++idx)
		{
			const JSON_Value& cfg_value = array[idx];
			CFG<CONFIG> *cfg = CFG<CONFIG>::Load(cfg_value, extension_factories);
			if(!cfg) throw std::runtime_error("Mapping loading aborted");
			Put(cfg->GetAddress(), cfg);
			cfgs.push_back(cfg);
		}
	
		for(JSON_Array::size_type idx = 0, len = array.Length(); idx < len; ++idx)
		{
			CFG<CONFIG> *cfg = cfgs[idx];
			const JSON_Value& cfg_value = array[idx];
			if(!cfg->Fix(*this, cfg_value, extension_factories)) return false;
		}
		
		return true;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return false;
}

template <typename CONFIG>
void Mapping<CONFIG>::Check()
{
	for(typename Map::const_iterator itr = map.begin(); itr != map.end(); ++itr)
	{
		CFG<CONFIG> *cfg = (*itr).second;
		cfg->Check();
	}
}

template <typename CONFIG>
void Mapping<CONFIG>::InvalidateExtensions()
{
	for(typename Map::const_iterator itr = map.begin(); itr != map.end(); ++itr)
	{
		CFG<CONFIG> *cfg = (*itr).second;
		cfg->InvalidateExtensions();
	}
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Mapping<CONFIG>::ScanCFGs(VISITOR& visitor) const
{
	for(typename Map::const_iterator itr = map.begin(); itr != map.end(); ++itr)
	{
		T ret = visitor.Visit((*itr).second);
		if(ret) return ret;
	}
	return T();
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Mapping<CONFIG>::ScanCFGsOrdered(VISITOR& visitor) const
{
	typedef std::map<ADDRESS, CFG<CONFIG> *> _Map;
	_Map _map(map.begin(), map.end());
	for(typename _Map::const_iterator itr = _map.begin(); itr != _map.end(); ++itr)
	{
		T ret = visitor.Visit((*itr).second);
		if(ret) return ret;
	}
	return T();
}

/////////////////////////////////// Current<> /////////////////////////////////

template <typename CONFIG>
void Current<CONFIG>::Print(std::ostream& stream) const
{
	if(*this)
		stream << "Current(cfg=" << *cfg << ", working=" << *working << ")";
	else
		stream << "Current()";
}

///////////////////////////////// CallStack<> /////////////////////////////////

template <typename CONFIG>
bool CallStack<CONFIG>::Has(ADDRESS ret_addr) const
{
	if(stack.empty()) return false;
	const CallStackEntry<CONFIG> *entry = &stack.back();
	const CallStackEntry<CONFIG> *bottom = &stack.front();
	do
	{
		if(entry->ret_addr == ret_addr) return true;
		if(entry == bottom) break;
		--entry;
	}
	while(1);
	return false;
}

////////////////////////////////// Builder<> //////////////////////////////////

template <typename CONFIG>
Builder<CONFIG>::Builder()
	: state()
	, mapping()
	, new_group()
	, next_unknown_id(0)
	, under_construction(false)
	, config()
{
}

template <typename CONFIG>
Builder<CONFIG>::~Builder()
{
}

template <typename CONFIG>
void Builder<CONFIG>::Process(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info)
{
	Process(Instruction<CONFIG>(instr_info));
}

template <typename CONFIG>
void Builder<CONFIG>::Process(const Instruction<CONFIG>& instr)
{
	new_group.AddInstruction(instr);
	if(instr.GetType() != STANDARD)
	{
		Process(new_group);
		new_group.Clear();
	}
}

template <typename CONFIG>
void Builder<CONFIG>::Process(Instruction<CONFIG>&& instr)
{
	new_group.AddInstruction(std::move(instr));
	if(instr.GetType() != STANDARD)
	{
		Process(new_group);
		new_group.Clear();
	}
}

template <typename CONFIG>
void Builder<CONFIG>::Process(const Group<CONFIG>& group)
{
	if(CONFIG::DEBUG) std::cerr << "Process(group=" << group << ")" << std::endl;
	
	if(!under_construction)
	{
		InvalidateExtensions();
		under_construction = true;
	}
	unsigned tag = EMPTY;
	ADDRESS addr = group.GetAddress();
	if(!state.current)
	{
		CFG<CONFIG> *initial = mapping.Get(addr);
		if(!initial)
		{
			initial = new CFG<CONFIG>();
			mapping.Put(addr, initial);
		}
		state.current = Current<CONFIG>(initial, initial->GetEntry());
	}
	else
	{
		tag = ProcessTail(state.current.working->AsBlock().Tail(), addr);
	}
	if(CONFIG::CACHE_SIZE > 0)
	{
		CacheAccess<CONFIG> cache_access(group);
		bool hit = state.current.working->AccessCache(cache_access, 1);
		CacheEntry<CONFIG> *cache_entry = cache_access.cache_entry;
		if(hit)
		{
			state.current.working = cache_entry->cached_working;
		}
		else
		{
			if(cache_entry->cached_count > 0)
			{
				state.current.working->FlushCounts(cache_entry->cached_group, cache_entry->cached_working, cache_entry->cached_count);
				assert(!cache_entry->cached_group.Valid()); // Note: FlushCounts alters cache_entry->cached_group which shall get invalidated
			}
			Node<CONFIG> *prev_working = state.current.working;
			state.current.working = ProcessGroup(state.current.cfg, state.current.working, group, tag);
			prev_working->ReplaceCache(cache_access, state.current.working, 0);
		}
	}
	else
	{
		state.current.working = ProcessGroup(state.current.cfg, state.current.working, group, tag);
	}
}

template <typename CONFIG>
void Builder<CONFIG>::End()
{
	if(under_construction)
	{
		if(CONFIG::CACHE_SIZE > 0)
		{
			struct CFGVisitor
			{
				bool Visit(CFG<CONFIG> *cfg)
				{
					struct NodeVisitor
					{
						CFG<CONFIG>& cfg;
						
						NodeVisitor(CFG<CONFIG>& _cfg) : cfg(_cfg) {}
						
						bool Visit(Node<CONFIG> *src)
						{
							src->FlushCounts();
							return false; // keep scanning nodes
						}
					};
					
					NodeVisitor node_visitor(*cfg);
					cfg->ScanNodes(node_visitor);
					return false; // keep scanning CFGs
				}
			};
			
			CFGVisitor cfg_visitor;
			mapping.ScanCFGs(cfg_visitor);
		}
		
		while(state.current)
		{
			state.current.cfg->AddEdge(state.current.working, state.current.cfg->GetHalt(), 1);
			if(state.callstack.Empty())
			{
				state.current = Current<CONFIG>();
			}
			else
			{
				state.current = state.callstack.Top().current;
				state.callstack.Pop();
			}
		}
		
		if(CONFIG::CHECK) mapping.Check();
	
		under_construction = false;
	}
}

template <typename CONFIG>
void Builder<CONFIG>::OutputGraph(std::ostream& stream) const
{
	stream << "digraph {" << std::endl;
	stream << "\toverlap=false;" << std::endl;
	stream << "\tcompound=true;" << std::endl;
	stream << "\tsplines=true;" << std::endl;
	stream << "\tnodesep=0.5;" << std::endl;
	stream << "\tfontname=\"" << config.graphviz_fontname << "\";" << std::endl;
	stream << "\tclusterrank=local;" << std::endl;
	stream << "\tcolorscheme=\"X11\"" << std::endl;
	stream << "\tnslimit=4;" << std::endl;
	stream << "\tnslimit1=4;" << std::endl;
	stream << "\tmaxiter=10000;" << std::endl;
	mapping.OutputGraph(stream, config);
	stream << "}" << std::endl;
}

template <typename CONFIG>
JSON_Value *Builder<CONFIG>::Save() const
{
	JSON_Object *object = new JSON_Object();
	object->Add(new JSON_Member("mapping", mapping.Save(config)));
	return object;
}

template <typename CONFIG>
void Builder<CONFIG>::Save(std::ostream& stream) const
{
	JSON_Value *backup = Save();
	stream << *backup;
	delete backup;
}

template <typename CONFIG>
bool Builder<CONFIG>::Load(const JSON_Value& value, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	new_group.Clear();
	state.Clear();
	try
	{
		const JSON_Object& object = value.AsObject();
		if(object.HasProperty("mapping"))
		{
			if(!mapping.Load(object["mapping"].AsValue(), extension_factories)) return false;
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	// restore next_unknown_id which is max({unknown.id}) + 1
	struct CFGVisitor
	{
		Builder<CONFIG>& builder;
		
		CFGVisitor(Builder<CONFIG>& _builder) : builder(_builder) {}
		
		bool Visit(CFG<CONFIG> *cfg)
		{
			struct NodeVisitor
			{
				Builder<CONFIG>& builder;
				
				NodeVisitor(Builder<CONFIG>& _builder) : builder(_builder) {}
				
				bool Visit(Node<CONFIG> *node)
				{
					return false; // keep scanning nodes
				}
				
				bool Visit(Unknown<CONFIG> *unknown)
				{
					unsigned id = unknown->GetId();
					if(id >= builder.next_unknown_id)
					{
						builder.next_unknown_id = id + 1;
					}
					
					return false; // keep scanning nodes
				}
			};
			
			NodeVisitor node_visitor(builder);
			cfg->ScanNodes(node_visitor);
			return false; // keep scanning CFGs
		}
	};

	CFGVisitor cfg_visitor(*this);
	mapping.ScanCFGs(cfg_visitor);
		
	return true;
}

template <typename CONFIG>
void Builder<CONFIG>::Load(std::istream& stream, const std::initializer_list<ExtensionFactory *>& extension_factories)
{
	JSON_Parser<JSON_AST_Builder> json_parser;
	JSON_AST_Builder json_ast_builder;
	
	JSON_Value *backup = json_parser.Parse(stream, json_ast_builder);
	
	if(backup)
	{
		bool load_status = Load(*backup, extension_factories);
		delete backup;
		if(load_status) return;
	}
	
	stream.setstate(std::ios::failbit);
}

template <typename CONFIG>
unsigned /* tag */ Builder<CONFIG>::ProcessTail(const Instruction<CONFIG>& tail, ADDRESS target_addr) // see PROCESS_TYPE
{
	if(CONFIG::DEBUG) { FScope<std::ostream> fscope(std::cerr); std::cerr << "ProcessTail(tail=" << tail << ", target_addr=@0x" << std::hex << target_addr << std::dec << ")" << std::endl; }
	unsigned tag = EMPTY;
	unsigned type = tail.GetType();
	if((tail.GetMode() == INDIRECT) && !state.current.working->HasUnknownSuccessor() && ((type == JUMP) || (type == BRANCH) || (type == CALL)))
	{
		state.current.cfg->AddEdge(state.current.working, state.current.cfg->AddNode(new Unknown<CONFIG>(next_unknown_id++)), 0);
	}
	if(type == JUMP)
	{
		// do nothing
	}
	else if(type == BRANCH)
	{
		ADDRESS branch_fallthrough = tail.GetFallthrough();
		if(branch_fallthrough != target_addr) ProcessBranch(branch_fallthrough, target_addr, FALLTHROUGH); else tag = FALLTHROUGH;
		if(tail.GetMode() == DIRECT)
		{
			ADDRESS branch_target = tail.GetTarget();
			if(branch_target != target_addr) ProcessBranch(branch_target, target_addr, TARGET); else tag = TARGET;
		}
	}
	else if(type == CALL)
	{
		CFG<CONFIG> *called = mapping.Get(target_addr);
		if(!called) mapping.Put(target_addr, called = new CFG<CONFIG>(target_addr));
		assert(state.current.working->GetCategory() == BLOCK);
		state.current.working->AsBlock().AddCall(called, 1);
		state.callstack.Push(state.current, tail.GetFallthrough());
		state.current = Current(called, called->GetEntry());
	}
	else if(type == RETURN)
	{
		if(state.callstack.Has(target_addr))
		{
			do
			{
				CallStackEntry<CONFIG>& call_stack_entry = state.callstack.Top();
				ADDRESS ret_addr = call_stack_entry.ret_addr;
				state.current.cfg->AddEdge(state.current.working, state.current.cfg->GetExit(), 1);
				state.current = call_stack_entry.current;
				state.callstack.Pop();
				if(ret_addr == target_addr) break;
			}
			while(!state.callstack.Empty());
		}
		else
		{
			// do nothing as if it was an unconditional jump
		}
	}
	else
	{
		throw std::runtime_error("Internal error!");
	}
	
	return tag;
}

template <typename CONFIG>
void Builder<CONFIG>::ProcessBranch(ADDRESS addr, ADDRESS target_addr, unsigned tag)
{
	assert(addr != target_addr);
	if(CONFIG::DEBUG) { FScope<std::ostream> fscope(std::cerr); std::cerr << "ProcessBranch(addr=@0x" << std::hex << addr << std::dec << ", target_addr=@0x" << std::hex << target_addr << std::dec << ")" << std::endl; }
	Node<CONFIG> *node = state.current.cfg->FindNodeWithAddr(addr);
	if(node)
	{
		if((node->GetCategory() == BLOCK))
		{
			Block<CONFIG> *block = static_cast<Block<CONFIG> *>(node);
			if(block->GetAddress() != addr)
			{
				node = state.current.cfg->SplitBlock(block, addr);
			}
		}
	}
	else
	{
		node = state.current.cfg->AddNode(new Phantom<CONFIG>(addr));
	}
	state.current.cfg->AddEdge(state.current.working, node, 0, tag);
}

template <typename CONFIG>
Block<CONFIG> *Builder<CONFIG>::ProcessGroup(CFG<CONFIG> *cfg, Node<CONFIG> *working, const Group<CONFIG>& group, unsigned tag)
{
	if(CONFIG::DEBUG) std::cerr << "ProcessGroup(cfg=" << *cfg << "), working=" << *working << ", group=" << group << ")" << std::endl;
	Block<CONFIG> *working_block = 0;
	const Instruction<CONFIG> *curr_instr = 0;
	typename Group<CONFIG>::const_iterator itr;
	for(typename Group<CONFIG>::size_type idx = 0, length = group.Length(); idx < length; ++idx)
	{
		const Instruction<CONFIG>& instr = group[idx];
		if(curr_instr)
		{
			assert(*curr_instr == instr);
		}
		else
		{
			Node<CONFIG> *node = cfg->FindNodeWithAddr(instr.GetAddress());
			if(node)
			{
				Block<CONFIG> *block;
				if(node->GetCategory() == PHANTOM)
				{
					Phantom<CONFIG> *phantom = static_cast<Phantom<CONFIG> *>(node);
					block = cfg->PhantomToBlock(phantom, new Block<CONFIG>(new Group<CONFIG>(instr)));
					assert(block->GetAddress() == instr.GetAddress());
				}
				else
				{
					block = static_cast<Block<CONFIG> *>(node);
					if(block->GetAddress() != instr.GetAddress())
					{
						block = cfg->SplitBlock(block, instr.GetAddress());
					}
					assert(block->GetAddress() == instr.GetAddress());
				}
				cfg->AddEdge(working, block, 1, tag);
				working = block;
				itr = block->begin();
			}
			else
			{
				if((instr != group.Leader()) &&
				   (working->GetCategory() == BLOCK) &&
				   (!working->AsBlock().HasCalls()) &&
				   (!working->AsBlock().HasSignals()) &&
				   (!working->AsBlock().HasSuccessors()))
				{
					Block<CONFIG> *working_block = static_cast<Block<CONFIG> *>(working);
					assert((working_block->Tail().GetAddress() + working_block->Tail().GetSize()) == instr.GetAddress());
					working_block->AddInstruction(instr);
					itr = --working_block->end();
				}
				else
				{
					Block<CONFIG> *block = cfg->AddNode(new Block<CONFIG>(new Group<CONFIG>(instr)));
					cfg->AddEdge(working, block, 1, tag);
					working = block;
					itr = block->begin();
				}
			}
		}
		working_block = static_cast<Block<CONFIG> *>(working);
		++itr;
		curr_instr = (itr != working_block->end()) ? &(*itr) : 0;
		if(CONFIG::CHECK) assert(working_block->Next(instr) == curr_instr);
		tag = EMPTY;
	}
	
	if(CONFIG::DEBUG) std::cerr << "ProcessGroup returns " << *working_block << std::endl;
	return working_block;
}

template <typename CONFIG>
void Builder<CONFIG>::InvalidateExtensions()
{
	mapping.InvalidateExtensions();
}

} // end of namespace cfg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_CFG_CFG_TCC__
