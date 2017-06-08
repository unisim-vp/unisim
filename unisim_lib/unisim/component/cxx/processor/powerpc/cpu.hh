/*
 *  Copyright (c) 2007-2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__

#include <unisim/util/reg/core/register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <iosfwd>

#include <unisim/util/backtrace/backtrace.hh>

#ifdef powerpc
#undef powerpc
#endif

#define IF_COND_TRAIT(CLASS_NAME) template <bool, bool __IF_COND_TRAIT_DUMMY__ = true> struct CLASS_NAME {}; template <bool __IF_COND_TRAIT_DUMMY__> struct CLASS_NAME<true, __IF_COND_TRAIT_DUMMY__>
#define ELSE_COND_TRAIT(CLASS_NAME) template <bool __IF_COND_TRAIT_DUMMY__> struct CLASS_NAME<false, __IF_COND_TRAIT_DUMMY__>
#define COND_TRAIT(COND, CLASS_NAME) CLASS_NAME<COND>
	
#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;

using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

enum Model
{
	// Freescale models
	// G1 processors
	MPC601 = 0,
	// G2 processors
	MPC603E,
	MPC604E,
	// G3 processors
	MPC740,
	MPC745,
	MPC750,
	MPC755,
	// G4 processors
	MPC7400,
	MPC7410,
	MPC7441,
	MPC7445,
	MPC7447,
	MPC7447A,
	MPC7448,
	MPC7450,
	MPC7451,
	MPC7455,
	MPC7457,
	// IBM models
	PPC405,
	PPC440,
	// Freescale models
	E200Z425BN3,
	E200Z710N3
};

inline std::ostream& operator << (std::ostream& os, const Model& model)
{
	switch(model)
	{
		case MPC601:      os << "MPC601"; break;
		case MPC603E:     os << "MPC603E"; break;
		case MPC604E:     os << "MPC604E"; break;
		case MPC740:      os << "MPC740"; break;
		case MPC745:      os << "MPC745"; break;
		case MPC750:      os << "MPC750"; break;
		case MPC755:      os << "MPC755"; break;
		case MPC7400:     os << "MPC7400"; break;
		case MPC7410:     os << "MPC7410"; break;
		case MPC7441:     os << "MPC7441"; break;
		case MPC7445:     os << "MPC7445"; break;
		case MPC7447:     os << "MPC7447"; break;
		case MPC7447A:    os << "MPC7447A"; break;
		case MPC7448:     os << "MPC7448"; break;
		case MPC7450:     os << "MPC7450"; break;
		case MPC7451:     os << "MPC7451"; break;
		case MPC7455:     os << "MPC7455"; break;
		case MPC7457:     os << "MPC7457"; break;
		case PPC405:      os << "PPC405"; break;
		case PPC440:      os << "PPC440"; break;
		case E200Z425BN3: os << "e200z425Bn3"; break;
		case E200Z710N3:  os << "e200z710n3"; break;
		default:          os << "unknown"; break;
	}
	return os;
}

using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::RegisterFile;

////////////////////////////////// LOG2 ///////////////////////////////////////

template <unsigned int N>
struct LOG2
{
    static const unsigned int VALUE = LOG2<N / 2>::VALUE + 1;
};

template <>
struct LOG2<1>
{
    static const unsigned int VALUE = 0;
};

 /////////////////////////////// CEIL_LOG2 ////////////////////////////////////

template <unsigned int N>
struct CEIL_LOG2
{
    static const unsigned int VALUE = (N > (1 << LOG2<N>::VALUE)) ? LOG2<N>::VALUE + 1 : LOG2<N>::VALUE;
};

//////////////////////////////////// Field<> //////////////////////////////////

template <typename FIELD, int OFFSET1, int OFFSET2 = -1>
struct Field : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1>
{
	typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1> Super;
};

//////////////////////////////// Register /////////////////////////////////

template <typename REGISTER, unsigned int _SIZE, typename REGISTER_BASE = unisim::util::reg::core::NullRegisterBase>
class Register : public unisim::util::reg::core::Register<REGISTER, _SIZE, unisim::util::reg::core::HW_RW, REGISTER_BASE>
{
public:
	typedef unisim::util::reg::core::Register<REGISTER, _SIZE, unisim::util::reg::core::HW_RW, REGISTER_BASE> Super;
	
	Register() : Super() {}
	Register(typename Super::TYPE value) : Super(value) {}
	
	using Super::operator =;
};

///////// Common User land Special Purpose Register Representation ////////

// Integer Exception Register
struct XER : Register<XER, 32>
{
	typedef Register<XER, 32> Super;
	
	struct SO         : Field<SO        ,0> {};      // Summary Overflow
	struct OV         : Field<OV        ,1> {};      // Overflow
	struct CA         : Field<CA        ,2> {};      // Carry
	struct BYTE_COUNT : Field<BYTE_COUNT,25, 31> {}; // Byte count
	
	struct _0_3       : Field<_0_3, 0, 3> {}; // XER[0-3]
	
	typedef FieldSet<SO, OV, CA, BYTE_COUNT> ALL;
	
	XER() : Super() { Init(); }
	XER(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
	
	virtual void Reset() { this->Initialize(0x00000000); }
private:
	void Init()
	{
		this->SetName("xer");
		this->SetDescription("Fixed-Point Exception Register");
		SO::SetName("so");
		OV::SetName("ov");
		CA::SetName("ca");
		BYTE_COUNT::SetName("byte_count");
		SO::SetDescription("Summary Overflow");
		OV::SetDescription("Overflow");
		CA::SetDescription("Carry");
		BYTE_COUNT::SetDescription("Byte count");
	}
};

// Link Register
struct LR : Register<LR, 32>
{
	typedef Register<LR, 32> Super;
	
	struct ALL : Field<ALL, 0, 31> {};
	
	LR() : Super() { Init(); }
	LR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
	
	virtual void Reset() { /* unaffected */ }
private:
	void Init()
	{
		this->SetName("lr");
		this->SetDescription("Link Register");
	}
};

// Count Register
struct CTR : Register<CTR, 32>
{
	typedef Register<CTR, 32> Super;
	
	struct ALL : Field<ALL, 0, 31> {};
	
	CTR() : Super() { Init(); }
	CTR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
	
	virtual void Reset() { /* unaffected */ }
private:
	void Init()
	{
		this->SetName("ctr");
		this->SetDescription("Count Register");
	}
};

// Condition Register
struct CR : Register<CR, 32>
{
public:
	typedef Register<CR, 32> Super;
	
	struct CR0 : Field<CR0,0,3>
	{
		struct LT : Field<LT,0> {};
		struct GT : Field<GT,1> {};
		struct EQ : Field<EQ,2> {};
		struct SO : Field<SO,3> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR1 : Field<CR1,4,7>
	{
		struct LT : Field<LT,4> {};
		struct GT : Field<GT,5> {};
		struct EQ : Field<EQ,6> {};
		struct SO : Field<SO,7> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR2 : Field<CR2,8,11>
	{
		struct LT : Field<LT,8> {};
		struct GT : Field<GT,9> {};
		struct EQ : Field<EQ,10> {};
		struct SO : Field<SO,11> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR3 : Field<CR3,12,15>
	{
		struct LT : Field<LT,12> {};
		struct GT : Field<GT,13> {};
		struct EQ : Field<EQ,14> {};
		struct SO : Field<SO,15> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR4 : Field<CR4,16,19>
	{
		struct LT : Field<LT,16> {};
		struct GT : Field<GT,17> {};
		struct EQ : Field<EQ,18> {};
		struct SO : Field<SO,19> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR5 : Field<CR5,20,23>
	{
		struct LT : Field<LT,20> {};
		struct GT : Field<GT,21> {};
		struct EQ : Field<EQ,22> {};
		struct SO : Field<SO,23> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR6 : Field<CR6,24,27>
	{
		struct LT : Field<LT,24> {};
		struct GT : Field<GT,25> {};
		struct EQ : Field<EQ,26> {};
		struct SO : Field<SO,27> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR7 : Field<CR7,28,31>
	{
		struct LT : Field<LT,28> {};
		struct GT : Field<GT,29> {};
		struct EQ : Field<EQ,30> {};
		struct SO : Field<SO,31> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	
	typedef FieldSet< CR0::LT, CR0::GT, CR0::EQ, CR0::SO
					, CR1::LT, CR1::GT, CR1::EQ, CR1::SO
					, CR2::LT, CR2::GT, CR2::EQ, CR2::SO
					, CR3::LT, CR3::GT, CR3::EQ, CR3::SO
					, CR4::LT, CR4::GT, CR4::EQ, CR4::SO
					, CR5::LT, CR5::GT, CR5::EQ, CR5::SO
					, CR6::LT, CR6::GT, CR6::EQ, CR6::SO
					, CR7::LT, CR7::GT, CR7::EQ, CR7::SO> ALL;
	
	CR() : Super() { Init(); }
	CR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
private:
	void Init()
	{
		this->SetName("cr");
		this->SetDescription("Condition Register");
		
		CR0::LT::SetName("cr0[lt]"); CR0::GT::SetName("cr0[gt]"); CR0::EQ::SetName("cr0[eq]"); CR0::SO::SetName("cr0[so]");
		CR1::LT::SetName("cr1[lt]"); CR1::GT::SetName("cr1[gt]"); CR1::EQ::SetName("cr1[eq]"); CR1::SO::SetName("cr1[so]");
		CR2::LT::SetName("cr2[lt]"); CR2::GT::SetName("cr2[gt]"); CR2::EQ::SetName("cr2[eq]"); CR2::SO::SetName("cr2[so]");
		CR3::LT::SetName("cr3[lt]"); CR3::GT::SetName("cr3[gt]"); CR3::EQ::SetName("cr3[eq]"); CR3::SO::SetName("cr3[so]");
		CR4::LT::SetName("cr4[lt]"); CR4::GT::SetName("cr4[gt]"); CR4::EQ::SetName("cr4[eq]"); CR4::SO::SetName("cr4[so]");
		CR5::LT::SetName("cr5[lt]"); CR5::GT::SetName("cr5[gt]"); CR5::EQ::SetName("cr5[eq]"); CR5::SO::SetName("cr5[so]");
		CR6::LT::SetName("cr6[lt]"); CR6::GT::SetName("cr6[gt]"); CR6::EQ::SetName("cr6[eq]"); CR6::SO::SetName("cr6[so]");
		CR7::LT::SetName("cr7[lt]"); CR7::GT::SetName("cr7[gt]"); CR7::EQ::SetName("cr7[eq]"); CR7::SO::SetName("cr7[so]");
		
		CR0::LT::SetDescription("Less Than"); CR0::GT::SetDescription("Greater Than"); CR0::EQ::SetDescription("Equal"); CR0::SO::SetDescription("Summary Overflow");
		CR1::LT::SetDescription("Less Than"); CR1::GT::SetDescription("Greater Than"); CR1::EQ::SetDescription("Equal"); CR1::SO::SetDescription("Summary Overflow");
		CR2::LT::SetDescription("Less Than"); CR2::GT::SetDescription("Greater Than"); CR2::EQ::SetDescription("Equal"); CR2::SO::SetDescription("Summary Overflow");
		CR3::LT::SetDescription("Less Than"); CR3::GT::SetDescription("Greater Than"); CR3::EQ::SetDescription("Equal"); CR3::SO::SetDescription("Summary Overflow");
		CR4::LT::SetDescription("Less Than"); CR4::GT::SetDescription("Greater Than"); CR4::EQ::SetDescription("Equal"); CR4::SO::SetDescription("Summary Overflow");
		CR5::LT::SetDescription("Less Than"); CR5::GT::SetDescription("Greater Than"); CR5::EQ::SetDescription("Equal"); CR5::SO::SetDescription("Summary Overflow");
		CR6::LT::SetDescription("Less Than"); CR6::GT::SetDescription("Greater Than"); CR6::EQ::SetDescription("Equal"); CR6::SO::SetDescription("Summary Overflow");
		CR7::LT::SetDescription("Less Than"); CR7::GT::SetDescription("Greater Than"); CR7::EQ::SetDescription("Equal"); CR7::SO::SetDescription("Summary Overflow");
	}
};

///////////////////////////// TypeForByteSize<> ///////////////////////////////

template <unsigned int BYTE_SIZE> struct TypeForByteSize {};
template <> struct TypeForByteSize<1> { typedef uint8_t TYPE; };
template <> struct TypeForByteSize<2> { typedef uint16_t TYPE; };
template <> struct TypeForByteSize<3> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<4> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<5> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<6> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<7> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<8> { typedef uint64_t TYPE; };

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> struct TypeForBitSize
{
	static const unsigned int BYTE_SIZE = (SIZE + 7) / 8;
	typedef typename TypeForByteSize<BYTE_SIZE>::TYPE TYPE;
	static const TYPE MASK = (SIZE == (8 * BYTE_SIZE)) ? (~TYPE(0)) : ((TYPE(1) << SIZE) - 1);
};

////////////////////////////// PoolAllocator<> ////////////////////////////////

template <typename OBJECT>
class PoolAllocator
{
public:
	PoolAllocator();
	virtual ~PoolAllocator();
	
	OBJECT *Allocate();
	void Free(OBJECT *object);
private:
	std::stack<OBJECT *, std::vector<OBJECT *> > free_list;
	std::vector<OBJECT *> objects;
};

template <typename TYPES, typename CONFIG>
class CPU
	: public unisim::kernel::service::Client<typename unisim::service::interfaces::SymbolTableLookup<typename TYPES::ADDRESS> >
	, public unisim::kernel::service::Client<typename unisim::service::interfaces::DebugControl<typename TYPES::ADDRESS> >
	, public unisim::kernel::service::Client<typename unisim::service::interfaces::MemoryAccessReporting<typename TYPES::ADDRESS> >
	, public unisim::kernel::service::Client<typename unisim::service::interfaces::TrapReporting>
	, public unisim::kernel::service::Service<typename unisim::service::interfaces::MemoryAccessReportingControl>
	, public unisim::kernel::service::Service<typename unisim::service::interfaces::Registers>
	, public unisim::kernel::service::Service<typename unisim::service::interfaces::Synchronizable>
{
public:
	typedef CPU<TYPES, CONFIG> ThisCPU;
	
	/////////////////////////// service imports ///////////////////////////////

	unisim::kernel::service::ServiceImport<typename unisim::service::interfaces::SymbolTableLookup<typename TYPES::ADDRESS> > symbol_table_lookup_import;
	unisim::kernel::service::ServiceImport<typename unisim::service::interfaces::DebugControl<typename TYPES::ADDRESS> > debug_control_import;
	unisim::kernel::service::ServiceImport<typename unisim::service::interfaces::MemoryAccessReporting<typename TYPES::ADDRESS> > memory_access_reporting_import;
	unisim::kernel::service::ServiceImport<typename unisim::service::interfaces::TrapReporting> trap_reporting_import;
	
	/////////////////////////// service exports ///////////////////////////////

	unisim::kernel::service::ServiceExport<typename unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
	unisim::kernel::service::ServiceExport<typename unisim::service::interfaces::Registers> registers_export;
	unisim::kernel::service::ServiceExport<typename unisim::service::interfaces::Synchronizable> synchronizable_export;

	////////////////////////////// constructor ////////////////////////////////

	CPU(const char *name, unisim::kernel::service::Object *parent = 0);

	/////////////////////////////// destructor ////////////////////////////////

	virtual ~CPU();
	
	////////////////////////////// setup hooks ////////////////////////////////

	virtual bool BeginSetup();
	virtual bool EndSetup();
	
	//////////////////////////////// Reset ////////////////////////////////////

	void Reset();
	
	//////////////////////////// Logger streams ///////////////////////////////
	
	std::ostream& GetDebugInfoStream() { return logger.DebugInfoStream(); }
	std::ostream& GetDebugWarningStream() { return logger.DebugWarningStream(); }
	std::ostream& GetDebugErrorStream() { return logger.DebugErrorStream(); }
	
	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

	//////////////// unisim::service::interface::DebugControl /////////////////
	
	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report);
	
	/////////////// unisim::service::interface::Synchronizable ////////////////
	
	virtual void Synchronize();

	///////////// External Device Control Register access methods /////////////
		
	virtual void DCRRead(unsigned int dcrn, uint32_t& value) { value = 0; }
	virtual void DCRWrite(unsigned int dcrn, uint32_t value) {}

	///////////// Interface with .isa behavioral description files ////////////
	
	// see also ThrowException<> for interrupt
	
	inline uint32_t GetGPR(unsigned int n) const ALWAYS_INLINE { return gpr[n].template Get<typename GPR::ALL>(); }
	inline void SetGPR(unsigned int n, uint32_t value) ALWAYS_INLINE { gpr[n].template Set<typename GPR::ALL>(value); }
	inline uint32_t GetLR() const ALWAYS_INLINE { return lr.template Get<typename LR::ALL>(); }
	inline void SetLR(uint32_t value) ALWAYS_INLINE { lr.template Set<typename LR::ALL>(value); }
	inline uint32_t GetCTR() const ALWAYS_INLINE { return ctr.template Get<typename CTR::ALL>(); }
	inline void SetCTR(uint32_t value) ALWAYS_INLINE { ctr = value; }
	inline XER& GetXER() ALWAYS_INLINE { return xer; }
	inline CR& GetCR() ALWAYS_INLINE { return cr; }
	
	inline typename TYPES::ADDRESS GetCIA() const ALWAYS_INLINE { return cia; }
	inline void SetCIA(uint32_t value) ALWAYS_INLINE { cia = value; }
	inline typename TYPES::ADDRESS GetNIA() const ALWAYS_INLINE { return nia; }
	inline void SetNIA(typename TYPES::ADDRESS value) ALWAYS_INLINE { nia = value; }
	inline void Branch(typename TYPES::ADDRESS value) ALWAYS_INLINE { nia = value; }

	bool MoveFromSPR(unsigned int n, uint32_t& value);
	bool MoveToSPR(unsigned int n, uint32_t value);
	bool MoveFromTBR(unsigned int n, uint32_t& value);
	bool MoveToTBR(unsigned int n, uint32_t value);
	bool MoveFromDCR(unsigned int n, uint32_t& value);
	bool MoveToDCR(unsigned int n, uint32_t value);
	bool MoveFromPMR(unsigned int n, uint32_t& value);
	bool MoveToPMR(unsigned int n, uint32_t value);
	bool MoveFromTMR(unsigned int n, uint32_t& value);
	bool MoveToTMR(unsigned int n, uint32_t value);

	bool Int8Load(unsigned int rd, typename TYPES::ADDRESS ea);
	bool Int16Load(unsigned int rd, typename TYPES::ADDRESS ea);
	bool SInt16Load(unsigned int rd, typename TYPES::ADDRESS ea);
	bool Int32Load(unsigned int rd, typename TYPES::ADDRESS ea);
	bool Int16LoadByteReverse(unsigned int rd, typename TYPES::ADDRESS ea);
	bool Int32LoadByteReverse(unsigned int rd, typename TYPES::ADDRESS ea);
	bool IntLoadMSBFirst(unsigned int rd, typename TYPES::ADDRESS ea, unsigned int size);
	bool Int8Store(unsigned int rs, typename TYPES::ADDRESS ea);
	bool Int16Store(unsigned int rs, typename TYPES::ADDRESS ea);
	bool Int32Store(unsigned int rs, typename TYPES::ADDRESS ea);
	bool Int16StoreByteReverse(unsigned int rs, typename TYPES::ADDRESS ea);
	bool Int32StoreByteReverse(unsigned int rs, typename TYPES::ADDRESS ea);
	bool IntStoreMSBFirst(unsigned int rs, typename TYPES::ADDRESS ea, unsigned int size);
	
	std::string GetObjectFriendlyName(typename TYPES::ADDRESS addr);
	
	///////////////////////////////////////////////////////////////////////////
protected:
	
	///////////////////////////////// InterruptBase ///////////////////////////////////

	class InterruptBase
	{
	public:
		virtual ~InterruptBase() {}
		virtual void ProcessInterrupt(typename CONFIG::CPU *cpu) = 0;
		virtual const char *GetInterruptName() const { return "Unknown Interrupt"; }
		virtual unsigned int GetOffset() const { return 0; }
	};

	/////////////////////////// ExceptionDispatcherBase  //////////////////////////////
	
	class ExceptionDispatcherBase
	{
	public:
		virtual void InstallInterrupt(unsigned int priority, InterruptBase *interrupt) = 0;
	};

	//////////////////////////////// Exception<> ///////////////////////////////////////

	template <typename _INTERRUPT, unsigned int _PRIORITY>
	class Exception
	{
	public:
		typedef _INTERRUPT INTERRUPT;
		static const unsigned int PRIORITY = _PRIORITY;
		
		template <typename T> static T GetMask() { return T(1) << _PRIORITY; }
		
		static void InstallInterrupt(ExceptionDispatcherBase *exception_dispatcher, InterruptBase *interrupt) { exception_dispatcher->InstallInterrupt(_PRIORITY, interrupt); }
		
		static const char *GetName()
		{
			static std::string name;
			if(!name.empty()) return name.c_str();
			
			std::stringstream sstr_name;
			sstr_name << "Exception #" << _PRIORITY;
			name = sstr_name.str();
			
			return name.c_str();
		}
	};
	
	//////////////////////// Interrupt<> ///////////////////////////

	template <typename INTERRUPT, unsigned int _OFFSET>
	class Interrupt : public InterruptBase
	{
	public:
		static const unsigned int OFFSET = _OFFSET;
		
		template <typename T> static T GetMask() { return INTERRUPT::ALL::template GetMask<T>(); }
		
		static void InstallInterrupt(ExceptionDispatcherBase *exception_dispatcher)
		{
			static INTERRUPT interrupt;
			INTERRUPT::ALL::InstallInterrupt(exception_dispatcher, &interrupt);
		}
		
		virtual void ProcessInterrupt(typename CONFIG::CPU *cpu) { cpu->ProcessInterrupt(static_cast<INTERRUPT *>(this)); }

		virtual unsigned int GetOffset() const { return _OFFSET; }
		
		virtual const char *GetInterruptName() const { return INTERRUPT::GetName(); }
	};

	/////////////////////// InterruptWithAddress<> ////////////////////////////

	template <typename INTERRUPT, unsigned int _OFFSET>
	struct InterruptWithAddress : public Interrupt<INTERRUPT, _OFFSET>
	{
		void SetAddress(typename TYPES::ADDRESS _addr) { addr = _addr; has_addr = true; }
		void ClearAddress() { has_addr = false; }
		bool HasAddress() const { return has_addr; }
		typename TYPES::ADDRESS GetAddress() const { return addr; }
		
	private:
		bool has_addr;
		typename TYPES::ADDRESS addr;
	};
	
	

	////////////////////////// NullPrioritizedException ///////////////////////////

	struct NullException
	{
		template <typename T> static T GetMask() { return 0; }
		static void InstallInterrupt(ExceptionDispatcherBase *exception_dispatcher, InterruptBase *interrupt) {}
	};
	
	///////////////////////// ExceptionSet<> ///////////////////////////

	template < typename FE0                 , typename FE1  = NullException, typename FE2  = NullException, typename FE3  = NullException
	         , typename FE4  = NullException, typename FE5  = NullException, typename FE6  = NullException, typename FE7  = NullException
	         , typename FE8  = NullException, typename FE9  = NullException, typename FE10 = NullException, typename FE11 = NullException
	         , typename FE12 = NullException, typename FE13 = NullException, typename FE14 = NullException, typename FE15 = NullException
	         , typename FE16 = NullException, typename FE17 = NullException, typename FE18 = NullException, typename FE19 = NullException
	         , typename FE20 = NullException, typename FE21 = NullException, typename FE22 = NullException, typename FE23 = NullException
	         , typename FE24 = NullException, typename FE25 = NullException, typename FE26 = NullException, typename FE27 = NullException
	         , typename FE28 = NullException, typename FE29 = NullException, typename FE30 = NullException, typename FE31 = NullException
	         , typename FE32 = NullException, typename FE33 = NullException, typename FE34 = NullException, typename FE35 = NullException
	         , typename FE36 = NullException, typename FE37 = NullException, typename FE38 = NullException, typename FE39 = NullException
	         , typename FE40 = NullException, typename FE41 = NullException, typename FE42 = NullException, typename FE43 = NullException
	         , typename FE44 = NullException, typename FE45 = NullException, typename FE46 = NullException, typename FE47 = NullException
	         , typename FE48 = NullException, typename FE49 = NullException, typename FE50 = NullException, typename FE51 = NullException
	         , typename FE52 = NullException, typename FE53 = NullException, typename FE54 = NullException, typename FE55 = NullException
	         , typename FE56 = NullException, typename FE57 = NullException, typename FE58 = NullException, typename FE59 = NullException
	         , typename FE60 = NullException, typename FE61 = NullException, typename FE62 = NullException, typename FE63 = NullException >
	class ExceptionSet
	{
	public:
		template <typename T> static T GetMask()
		{
			return  FE0::template GetMask<T>() |  FE1::template GetMask<T>() |  FE2::template GetMask<T>() |  FE3::template GetMask<T>()
			     |  FE4::template GetMask<T>() |  FE5::template GetMask<T>() |  FE6::template GetMask<T>() |  FE7::template GetMask<T>()
			     |  FE8::template GetMask<T>() |  FE9::template GetMask<T>() | FE10::template GetMask<T>() | FE11::template GetMask<T>()
			     | FE12::template GetMask<T>() | FE13::template GetMask<T>() | FE14::template GetMask<T>() | FE15::template GetMask<T>()
			     | FE16::template GetMask<T>() | FE17::template GetMask<T>() | FE18::template GetMask<T>() | FE19::template GetMask<T>()
			     | FE20::template GetMask<T>() | FE21::template GetMask<T>() | FE22::template GetMask<T>() | FE23::template GetMask<T>()
			     | FE24::template GetMask<T>() | FE25::template GetMask<T>() | FE26::template GetMask<T>() | FE27::template GetMask<T>()
			     | FE28::template GetMask<T>() | FE29::template GetMask<T>() | FE30::template GetMask<T>() | FE31::template GetMask<T>()
			     | FE32::template GetMask<T>() | FE33::template GetMask<T>() | FE34::template GetMask<T>() | FE35::template GetMask<T>()
			     | FE36::template GetMask<T>() | FE37::template GetMask<T>() | FE38::template GetMask<T>() | FE39::template GetMask<T>()
			     | FE40::template GetMask<T>() | FE41::template GetMask<T>() | FE42::template GetMask<T>() | FE43::template GetMask<T>()
			     | FE44::template GetMask<T>() | FE45::template GetMask<T>() | FE46::template GetMask<T>() | FE47::template GetMask<T>()
			     | FE48::template GetMask<T>() | FE49::template GetMask<T>() | FE50::template GetMask<T>() | FE51::template GetMask<T>()
			     | FE52::template GetMask<T>() | FE53::template GetMask<T>() | FE54::template GetMask<T>() | FE55::template GetMask<T>()
			     | FE56::template GetMask<T>() | FE57::template GetMask<T>() | FE58::template GetMask<T>() | FE59::template GetMask<T>()
			     | FE60::template GetMask<T>() | FE61::template GetMask<T>() | FE62::template GetMask<T>() | FE63::template GetMask<T>();
		}
		
		static void InstallInterrupt(ExceptionDispatcherBase *exception_dispatcher, InterruptBase *interrupt)
		{
			if(typeid(FE0) == typeid(NullException)) return; FE0::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE1) == typeid(NullException)) return; FE1::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE2) == typeid(NullException)) return; FE2::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE3) == typeid(NullException)) return; FE3::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE4) == typeid(NullException)) return; FE4::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE5) == typeid(NullException)) return; FE5::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE6) == typeid(NullException)) return; FE6::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE7) == typeid(NullException)) return; FE7::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE8) == typeid(NullException)) return; FE8::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE9) == typeid(NullException)) return; FE9::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE10) == typeid(NullException)) return; FE10::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE11) == typeid(NullException)) return; FE11::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE12) == typeid(NullException)) return; FE12::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE13) == typeid(NullException)) return; FE13::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE14) == typeid(NullException)) return; FE14::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE15) == typeid(NullException)) return; FE15::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE16) == typeid(NullException)) return; FE16::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE17) == typeid(NullException)) return; FE17::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE18) == typeid(NullException)) return; FE18::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE19) == typeid(NullException)) return; FE19::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE20) == typeid(NullException)) return; FE20::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE21) == typeid(NullException)) return; FE21::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE22) == typeid(NullException)) return; FE22::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE23) == typeid(NullException)) return; FE23::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE24) == typeid(NullException)) return; FE24::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE25) == typeid(NullException)) return; FE25::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE26) == typeid(NullException)) return; FE26::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE27) == typeid(NullException)) return; FE27::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE28) == typeid(NullException)) return; FE28::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE29) == typeid(NullException)) return; FE29::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE30) == typeid(NullException)) return; FE30::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE31) == typeid(NullException)) return; FE31::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE32) == typeid(NullException)) return; FE32::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE33) == typeid(NullException)) return; FE33::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE34) == typeid(NullException)) return; FE34::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE35) == typeid(NullException)) return; FE35::InstallInterrupt(exception_dispatcher, interrupt);                                                                                            
			if(typeid(FE36) == typeid(NullException)) return; FE36::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE37) == typeid(NullException)) return; FE37::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE38) == typeid(NullException)) return; FE38::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE39) == typeid(NullException)) return; FE39::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE40) == typeid(NullException)) return; FE40::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE41) == typeid(NullException)) return; FE41::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE42) == typeid(NullException)) return; FE42::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE43) == typeid(NullException)) return; FE43::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE44) == typeid(NullException)) return; FE44::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE45) == typeid(NullException)) return; FE45::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE46) == typeid(NullException)) return; FE46::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE47) == typeid(NullException)) return; FE47::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE48) == typeid(NullException)) return; FE48::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE49) == typeid(NullException)) return; FE49::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE50) == typeid(NullException)) return; FE50::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE51) == typeid(NullException)) return; FE51::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE52) == typeid(NullException)) return; FE52::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE53) == typeid(NullException)) return; FE53::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE54) == typeid(NullException)) return; FE54::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE55) == typeid(NullException)) return; FE55::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE56) == typeid(NullException)) return; FE56::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE57) == typeid(NullException)) return; FE57::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE58) == typeid(NullException)) return; FE58::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE59) == typeid(NullException)) return; FE59::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE60) == typeid(NullException)) return; FE60::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE61) == typeid(NullException)) return; FE61::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE62) == typeid(NullException)) return; FE62::InstallInterrupt(exception_dispatcher, interrupt);
			if(typeid(FE63) == typeid(NullException)) return; FE63::InstallInterrupt(exception_dispatcher, interrupt);
		}
	};

	///////////////////////////// ExceptionDispatcher<> ///////////////////////////

	template <unsigned int NUM_EXCEPTIONS>
	class ExceptionDispatcher : public ExceptionDispatcherBase
	{
	public:
		ExceptionDispatcher(typename CONFIG::CPU *cpu);
		~ExceptionDispatcher();
		virtual void InstallInterrupt(unsigned int priority, InterruptBase *interrupt);
		template <typename EXCEPTION> typename EXCEPTION::INTERRUPT& ThrowException();
		template <typename INTERRUPT> void AckInterrupt();
		template <typename INTERRUPT> void EnableInterrupt();
		template <typename INTERRUPT> void DisableInterrupt();
		template <typename EXCEPTION> bool RecognizedException() const;
		inline void ProcessExceptions() ALWAYS_INLINE;
	private:
		typedef typename TypeForBitSize<NUM_EXCEPTIONS>::TYPE TYPE;
		
		typename CONFIG::CPU *cpu;
		TYPE exc_flags;                            // Exception flags (bits are ordered according to exception priority): 1=thrown 0=not thrown
		TYPE exc_mask;                             // Exception mask (bits are ordered according to exception priority): 1=enabled, 0=disabled
		InterruptBase *interrupts[NUM_EXCEPTIONS]; // Installed interrupts by priority
	};
	
public:
	template <typename INTERRUPT> void InstallInterrupt() { INTERRUPT::InstallInterrupt(&exception_dispatcher); }
	template <typename EXCEPTION> typename EXCEPTION::INTERRUPT& ThrowException() { return exception_dispatcher.template ThrowException<EXCEPTION>(); }
	template <typename INTERRUPT> void AckInterrupt() { exception_dispatcher.template AckInterrupt<INTERRUPT>(); }
	template <typename INTERRUPT> void EnableInterrupt() { exception_dispatcher.template EnableInterrupt<INTERRUPT>(); }
	template <typename INTERRUPT> void DisableInterrupt() { exception_dispatcher.template DisableInterrupt<INTERRUPT>(); }
	template <typename EXCEPTION> bool RecognizedException() const { return exception_dispatcher.template RecognizedException<EXCEPTION>(); }
	inline void ProcessExceptions() ALWAYS_INLINE { exception_dispatcher.ProcessExceptions(); }
protected:
	
	////////////////////// System Level Register Modeling /////////////////////
	
	enum SLR_Space_Type
	{
		SLR_SPR_SPACE = 0,
		SLR_TBR_SPACE = 1,
		SLR_DCR_SPACE = 2,
		SLR_PMR_SPACE = 3,
		SLR_TMR_SPACE = 4
	};
	
	enum SLR_Access_Type
	{
		SLR_NO_ACCESS = unisim::util::reg::core::HW_RW,
		SLR_RO        = unisim::util::reg::core::SW_R,
		SLR_WO        = unisim::util::reg::core::SW_W,
		SLR_RW        = unisim::util::reg::core::SW_RW
	};
	
	enum SLR_Privilege_Type
	{
		SLR_NON_PRIVILEGED = 0,
		SLR_PRIVILEGED     = 1
	};

	struct SLRBase
	{
		virtual bool IsValid() const { return false; }
		virtual unsigned int GetRegNum() const { return 0; }
		virtual SLR_Space_Type GetSpace() const { return SLR_SPR_SPACE; }
		virtual bool IsPrivileged() const { return false; }
		virtual bool IsReadOnly() const { return false; }
		virtual bool IsWriteOnly() const { return false; }
		virtual bool CheckMoveToLegality() { return false; }
		virtual bool CheckMoveFromLegality() { return false; }
		virtual bool MoveTo(uint32_t value) { return false; }
		virtual bool MoveFrom(uint32_t& value) { return false; }
		virtual void Reset() {}
		virtual void AddRegisterInterface() {}
	};
	
	typedef SLRBase SPRBase;
	typedef SLRBase TBRBase;
	typedef SLRBase DCRBase;
	typedef SLRBase PMRBase;
	typedef SLRBase TMRBase;

	template <SLR_Space_Type _SLR_SPACE>
	struct InvalidSLR : SLRBase
	{
		InvalidSLR(ThisCPU *_cpu) : cpu(_cpu) {}
		
		virtual bool CheckMoveToLegality()
		{
			// Illegal Instruction
			if(cpu->verbose_move_to_slr)
			{
				cpu->GetDebugWarningStream() << "Move to invalid system level register is an illegal operation" << std::endl;
			}
			cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
			return false;
		}
		
		virtual bool CheckMoveFromLegality()
		{
			// Illegal Instruction
			if(cpu->verbose_move_from_slr)
			{
				cpu->GetDebugWarningStream() << "Move from invalid system level register is an illegal operation" << std::endl;
			}
			cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
			return false;
		}
	private:
		ThisCPU *cpu;
	};
	
	struct ExternalDCR : DCRBase
	{
		ThisCPU *cpu;
		unsigned int n;
		
		ExternalDCR(ThisCPU *_cpu) : cpu(_cpu), n(0) {}
		
		void Attach(unsigned int _n) { n = _n; }
		virtual bool IsValid() const { return true; }
		virtual bool CheckMoveFromLegality() { return true; }
		virtual bool CheckMoveToLegality() { return true; }
		virtual bool MoveTo(uint32_t value) { cpu->DCRWrite(n, value); return true; }
		virtual bool MoveFrom(uint32_t& value) { cpu->DCRRead(n, value); return true; }
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, SLR_Access_Type _SLR_ACCESS = SLR_RW, SLR_Privilege_Type _SLR_PRIVILEGE = SLR_NON_PRIVILEGED>
	struct SLR : unisim::util::reg::core::Register<SLR_REGISTER, 32, unisim::util::reg::core::Access(_SLR_ACCESS), SLRBase>
	{
		typedef unisim::util::reg::core::Register<SLR_REGISTER, 32, unisim::util::reg::core::Access(_SLR_ACCESS), SLRBase> Super;
		static const SLR_Space_Type SLR_SPACE = _SLR_SPACE;
		static const unsigned int SLR_NUM = _SLR_NUM;
		static const SLR_Access_Type SLR_ACCESS = _SLR_ACCESS;
		static const SLR_Privilege_Type SLR_PRIVILEGE = _SLR_PRIVILEGE;
		
		SLR(typename CONFIG::CPU *_cpu) : Super(), cpu(_cpu) { Init(); }
		SLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_value), cpu(_cpu) { Init(); }
		virtual bool IsValid() const { return true; }
		virtual unsigned int GetRegNum() const { return _SLR_NUM; }
		virtual SLR_Space_Type GetSpace() const { return _SLR_SPACE; }
		virtual bool IsPrivileged() const { return SLR_PRIVILEGE == SLR_PRIVILEGED; }
		virtual bool IsReadOnly() const { return SLR_ACCESS == SLR_RO; }
		virtual bool IsWriteOnly() const { return SLR_ACCESS == SLR_WO; }

		virtual bool CheckMoveToLegality()
		{
			if(cpu->GetMSR().template Get<typename CONFIG::CPU::MSR::PR>() && (SLR_PRIVILEGE == SLR_PRIVILEGED))
			{
				// Privilege Violation
				if(cpu->verbose_move_to_slr)
				{
					cpu->GetDebugWarningStream() << "Move to " << this->GetName() << " is a privileged operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::PrivilegeViolation>();
				return false;
			}

			if(SLR_ACCESS == SLR_RO)
			{
				// Illegal Instruction
				if(cpu->verbose_move_to_slr)
				{
					cpu->GetDebugWarningStream() << "Move to " << this->GetName() << " is an illegal operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}
			
			return true;
		}
		
		virtual bool CheckMoveFromLegality()
		{
			if(cpu->GetMSR().template Get<typename CONFIG::CPU::MSR::PR>() && (SLR_PRIVILEGE == SLR_PRIVILEGED))
			{
				// Privilege Violation
				if(cpu->verbose_move_from_slr)
				{
					cpu->GetDebugWarningStream() << "Move from " << this->GetName() << " is a privileged operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::PrivilegeViolation>();
				return false;
			}
			
			if(SLR_ACCESS == SLR_WO)
			{
				// Illegal Instruction
				if(cpu->verbose_move_from_slr)
				{
					cpu->GetDebugWarningStream() << "Move from " << this->GetName() << " is an illegal operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}

			return true;
		}
		
		virtual bool MoveTo(uint32_t value)
		{
			unisim::util::reg::core::WarningStatus ws = this->Write(value);
			if(ws)
			{
				cpu->GetDebugWarningStream() << "While moving 0x" << std::hex << value << std::dec << " to " << this->GetName() << ", " << ws << "; See below " << this->GetName() << " content after move operation: " << std::endl;
				this->LongPrettyPrint(cpu->GetDebugWarningStream());
				cpu->GetDebugWarningStream() << std::endl;
			}
			else if(cpu->verbose_move_to_slr)
			{
				cpu->GetDebugWarningStream() << "Moving 0x" << std::hex << value << std::dec << " to " << this->GetName() << std::endl;
			}

			
			return true;
		}
		
		virtual bool MoveFrom(uint32_t& value)
		{
			value = 0;
			unisim::util::reg::core::WarningStatus ws = this->Read(value);
			if(ws)
			{
				cpu->GetDebugWarningStream() << ws << std::endl;
				cpu->GetDebugWarningStream() << "While moving 0x" << std::hex << value << std::dec << " from " << this->GetName() << ", " << ws << "; See below " << this->GetName() << " content after move operation: " << std::endl;
				this->LongPrettyPrint(cpu->GetDebugWarningStream());
				cpu->GetDebugWarningStream() << std::endl;
			}
			else if(cpu->verbose_move_to_slr)
			{
				cpu->GetDebugWarningStream() << "Moving 0x" << std::hex << value << std::dec << " from " << this->GetName() << std::endl;
			}
			
			return true;
		}

		typename CONFIG::CPU *GetCPU() const { return cpu; }
		
		virtual void AddRegisterInterface()
		{
			cpu->AddRegisterInterface(this->CreateRegisterInterface());
		}

		using Super::operator =;
		
	protected:
		typename CONFIG::CPU *cpu;
	private:
		
		void Init()
		{
			cpu->RegisterSLR(_SLR_NUM, this);
		}
	};
	
	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct PrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_PRIVILEGED> Super;
		
		PrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct ReadOnlyPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_PRIVILEGED> Super;

		ReadOnlyPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct NonPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_NON_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_NON_PRIVILEGED> Super;

		NonPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct ReadOnlyNonPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_NON_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_NON_PRIVILEGED> Super;

		ReadOnlyNonPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _USLR_NUM, SLR_Access_Type _USLR_ACCESS = SLR_RW>
	struct USLR : SLRBase
	{
		USLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : cpu(_cpu), slr(_slr) {}
		virtual bool IsValid() const { return true; }
		virtual unsigned int GetRegNum() const { return _USLR_NUM; }
		virtual SLR_Space_Type GetSpace() const { return _SLR_SPACE; }
		virtual bool IsPrivileged() const { return false; }
		virtual bool IsReadOnly() const { return _USLR_ACCESS == SLR_RO; }
		virtual bool IsWriteOnly() const { return _USLR_ACCESS == SLR_WO; }
		virtual bool CheckMoveToLegality()
		{
			if(_USLR_ACCESS == SLR_RO)
			{
				// Illegal Instruction
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}

			return true;
		}
		virtual bool CheckMoveFromLegality()
		{
			if(_USLR_ACCESS == SLR_WO)
			{
				// Illegal Instruction
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}
			
			return true;
		}
		virtual bool MoveTo(uint32_t value) { return slr->MoveTo(value); }
		virtual bool MoveFrom(uint32_t& value) { return slr->MoveFrom(value); }
	private:
		typename CONFIG::CPU *cpu;
		SLR_REGISTER *slr;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _USLR_NUM>
	struct ReadOnlyUSLR : USLR<SLR_REGISTER, _SLR_SPACE, _USLR_NUM, SLR_RO>
	{
		typedef USLR<SLR_REGISTER, _SLR_SPACE, _USLR_NUM, SLR_RO> Super;
		
		ReadOnlyUSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
	};

	//////////////////// Special Purpose Register Modeling ////////////////////

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct PrivilegedSPR : PrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef PrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;
		
		PrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct ReadOnlyPrivilegedSPR : ReadOnlyPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		ReadOnlyPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct NonPrivilegedSPR : NonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef NonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		NonPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct ReadOnlyNonPrivilegedSPR : ReadOnlyNonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		ReadOnlyNonPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _USPR_NUM>
	struct USPR : USLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM>
	{
		typedef USLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM> Super;
		
		USPR(SPR_REGISTER *_spr) : Super(_spr) {}
	};

	template <typename SPR_REGISTER, unsigned int _USPR_NUM>
	struct ReadOnlyUSPR : ReadOnlyUSLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM>
	{
		typedef ReadOnlyUSLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM> Super;
		
		ReadOnlyUSPR(SPR_REGISTER *_spr) : Super(_spr) {}
	};

	/////////////////////// Time Base Register Modeling ///////////////////////

	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct PrivilegedTBR : PrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef PrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;
		
		PrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct ReadOnlyPrivilegedTBR : ReadOnlyPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;

		ReadOnlyPrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct NonPrivilegedTBR : NonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef NonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;

		NonPrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct ReadOnlyNonPrivilegedTBR : ReadOnlyNonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;

		ReadOnlyNonPrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TBR_REGISTER, unsigned int _UTBR_NUM>
	struct UTBR : USLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM>
	{
		typedef USLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM> Super;
		
		UTBR(TBR_REGISTER *_spr) : Super(_spr) {}
	};

	template <typename TBR_REGISTER, unsigned int _UTBR_NUM>
	struct ReadOnlyUTBR : ReadOnlyUSLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM>
	{
		typedef ReadOnlyUSLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM> Super;
		
		ReadOnlyUTBR(TBR_REGISTER *_spr) : Super(_spr) {}
	};

	////////////////////// Device Control Register Modeling ///////////////////

	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct PrivilegedDCR : PrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef PrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;
		
		PrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct ReadOnlyPrivilegedDCR : ReadOnlyPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;

		ReadOnlyPrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct NonPrivilegedDCR : NonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef NonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;

		NonPrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct ReadOnlyNonPrivilegedDCR : ReadOnlyNonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;

		ReadOnlyNonPrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename DCR_REGISTER, unsigned int _UDCR_NUM>
	struct UDCR : USLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM>
	{
		typedef USLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM> Super;
		
		UDCR(DCR_REGISTER *_spr) : Super(_spr) {}
	};

	template <typename DCR_REGISTER, unsigned int _UDCR_NUM>
	struct ReadOnlyUDCR : ReadOnlyUSLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM>
	{
		typedef ReadOnlyUSLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM> Super;
		
		ReadOnlyUDCR(DCR_REGISTER *_spr) : Super(_spr) {}
	};

	////////////////// Performance Monitor Register Modeling //////////////////

	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct PrivilegedPMR : PrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef PrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;
		
		PrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct ReadOnlyPrivilegedPMR : ReadOnlyPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;

		ReadOnlyPrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct NonPrivilegedPMR : NonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef NonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;

		NonPrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct ReadOnlyNonPrivilegedPMR : ReadOnlyNonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;

		ReadOnlyNonPrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename PMR_REGISTER, unsigned int _UPMR_NUM>
	struct UPMR : USLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM>
	{
		typedef USLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM> Super;
		
		UPMR(typename CONFIG::CPU *_cpu, PMR_REGISTER *_pmr) : Super(_cpu, _pmr) {}
	};

	template <typename PMR_REGISTER, unsigned int _UPMR_NUM>
	struct ReadOnlyUPMR : ReadOnlyUSLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM>
	{
		typedef ReadOnlyUSLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM> Super;
		
		ReadOnlyUPMR(typename CONFIG::CPU *_cpu, PMR_REGISTER *_pmr) : Super(_cpu, _pmr) {}
	};

	//////////////////// Thread Management Register Modeling //////////////////

	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct PrivilegedTMR : PrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef PrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;
		
		PrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct ReadOnlyPrivilegedTMR : ReadOnlyPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;

		ReadOnlyPrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct NonPrivilegedTMR : NonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef NonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;

		NonPrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct ReadOnlyNonPrivilegedTMR : ReadOnlyNonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;

		ReadOnlyNonPrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename TMR_REGISTER, unsigned int _UTMR_NUM>
	struct UTMR : USLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM>
	{
		typedef USLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM> Super;
		
		UTMR(typename CONFIG::CPU *_cpu, TMR_REGISTER *_tmr) : Super(_cpu, _tmr) {}
	};

	template <typename TMR_REGISTER, unsigned int _UTMR_NUM>
	struct ReadOnlyUTMR : ReadOnlyUSLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM>
	{
		typedef ReadOnlyUSLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM> Super;
		
		ReadOnlyUTMR(typename CONFIG::CPU *_cpu, TMR_REGISTER *_tmr) : Super(_cpu, _tmr) {}
	};

	void RegisterSLR(unsigned int n, SLRBase *slr);
	
	virtual SPRBase& GetSPR(unsigned int n);
	virtual TBRBase& GetTBR(unsigned int n);
	virtual DCRBase& GetDCR(unsigned int n);
	virtual PMRBase& GetPMR(unsigned int n);
	virtual TMRBase& GetTMR(unsigned int n);
	
public:
	/////////////////////////// Machine State Register ////////////////////////

	// Machine State Register
	struct MSR : unisim::util::reg::core::Register<MSR, 32>
	{
		typedef unisim::util::reg::core::Register<MSR, 32> Super;
		
		struct SPV : Field<SPV,6>  {}; // SP/Embedded FP/Vector available
		struct WE  : Field<WE ,13> {}; // Wait state (Power management) enable
		struct CE  : Field<CE ,14> {}; // Critical Interrupt Enable
		struct EE  : Field<EE ,15> {}; // External Interrupt Enable
		struct PR  : Field<PR ,17> {}; // Problem State
		struct FP  : Field<FP ,18> {}; // Floating-Point Available
		struct ME  : Field<ME ,19> {}; // Machine Check Enable
		struct FE0 : Field<FE0,20> {}; // Floating-point exception mode 0 (not used)
		struct DE  : Field<DE ,22> {}; // Debug Interrupt Enable
		struct FE1 : Field<FE1,23> {}; // Floating-point exception mode 1 (not used)
		struct IS  : Field<IS ,26> {}; // Instruction Address Space
		struct DS  : Field<DS ,27> {}; // Data Address Space
		struct PMM : Field<PMM,29> {}; // PMM Performance monitor mark bit
		struct RI  : Field<RI ,30> {}; // Recoverable Interrupt
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<SPV, WE, CE, EE, PR, FP, ME, FE0, DE, FE1, IS, DS, PMM, RI> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CE, EE, PR, ME, DE, PMM, RI> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MSR(typename CONFIG::CPU *_cpu) : Super(), cpu(_cpu) { Init(); }
		MSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_value), cpu(_cpu) { Init(); }
		using Super::operator =;
	protected:
		typename CONFIG::CPU *cpu;
	private:
		
		void Init()
		{
			     this->SetName("msr");      this->SetDescription("Machine State Register");
			SPV::SetName("spv"); SPV::SetDescription("SP/Embedded FP/Vector available");
			WE ::SetName("we");  WE ::SetDescription("Wait state (Power management) enable");
			CE ::SetName("ce");  CE ::SetDescription("Critical Interrupt Enable");
			EE ::SetName("ee");  EE ::SetDescription("External Interrupt Enable");
			PR ::SetName("pr");  PR ::SetDescription("Problem State");
			FP ::SetName("fp");  FP ::SetDescription("Floating-Point Available");
			ME ::SetName("me");  ME ::SetDescription("Machine Check Enable");
			FE0::SetName("fe0"); FE0::SetDescription("Floating-point exception mode 0 (not used)");
			DE ::SetName("de");  DE ::SetDescription("Debug Interrupt Enable");
			FE1::SetName("fe1"); FE1::SetDescription("Floating-point exception mode 1 (not used)");
			IS ::SetName("is");  IS ::SetDescription("Instruction Address Space");
			DS ::SetName("ds");  DS ::SetDescription("Data Address Space");
			PMM::SetName("pmm"); PMM::SetDescription("PMM Performance monitor mark bit");
			RI ::SetName("ri");  RI ::SetDescription("Recoverable Interrupt");
			
			cpu->AddRegisterInterface(this->CreateRegisterInterface());
		}
	};
protected:
	////////////////////////// Special Purpose Registers //////////////////////
	
	// Save/Restore Register 0
	struct SRR0 : PrivilegedSPR<SRR0, 26>
	{
		typedef PrivilegedSPR<SRR0, 26> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("srr0"); this->SetDescription("Save/Restore Register 0"); }
	};
	
	// Save/Restore Register 1
	struct SRR1 : PrivilegedSPR<SRR1, 27>
	{
		typedef PrivilegedSPR<SRR1, 27> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("srr1"); this->SetDescription("Save/Restore Register 1"); }
	};

	// Process ID Register
	struct PID0 : PrivilegedSPR<PID0, 48>
	{
	public:
		typedef PrivilegedSPR<PID0, 48> Super;
		
		struct Process_ID : Field<Process_ID, 24, 31> {}; // Process ID
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<Process_ID> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<Process_ID> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PID0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PID0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("pid0"); this->SetDescription("Process ID Register"); Process_ID::SetName("process_id"); }
	};
	
	// Critical Save/Restore Register 0
	struct CSRR0 : PrivilegedSPR<CSRR0, 58>
	{
		typedef PrivilegedSPR<CSRR0, 58> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		CSRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		CSRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("csrr0"); this->SetDescription("Critical Save/Restore Register 0"); }
	};

	// Critical Save/Restore Register 1
	struct CSRR1 : PrivilegedSPR<CSRR1, 59>
	{
		typedef PrivilegedSPR<CSRR1, 59> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		CSRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		CSRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;

		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("csrr1"); this->SetDescription("Critical Save/Restore Register 1"); }
	};

	// Data Exception Address Register
	struct DEAR : PrivilegedSPR<DEAR, 61>
	{
		typedef PrivilegedSPR<DEAR, 61> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DEAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DEAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("dear"); this->SetDescription("Data Exception Address Register");
		}
	};

public:
	// Exception Syndrome Register
	struct ESR : PrivilegedSPR<ESR, 62>
	{
		typedef PrivilegedSPR<ESR, 62> Super;
		
		struct PIL   : Field<PIL  ,4>  {}; // Illegal Instruction exception
		struct PPR   : Field<PPR  ,5>  {}; // Privileged Instruction exception
		struct PTR   : Field<PTR  ,6>  {}; // Trap exception
		struct FP    : Field<FP   ,7>  {}; // Floating-point operation
		struct ST    : Field<ST   ,8>  {}; // Store operation
		struct BO    : Field<BO   ,14> {}; // Byte Ordering exception/Mismatched Instruction Storage exception
		struct SPV   : Field<SPV  ,24> {}; // EFPU APU Operation
		struct VLEMI : Field<VLEMI,26> {}; // VLE Mode Instruction
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<PIL, PPR, PTR, FP, ST, BO, SPV, VLEMI> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<PIL, PPR, PTR, FP, ST, BO, SPV, VLEMI> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;

		ESR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ESR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;

		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			       this->SetName("esr");          this->SetDescription("Exception Syndrome Register");
			PIL  ::SetName("pil");   PIL  ::SetDescription("Illegal Instruction exception");
			PPR  ::SetName("ppr");   PPR  ::SetDescription("Privileged Instruction exception");
			PTR  ::SetName("ptr");   PTR  ::SetDescription("Trap exception");
			FP   ::SetName("fp");    FP   ::SetDescription("Floating-point operation");
			ST   ::SetName("st");    ST   ::SetDescription("Store operation");
			BO   ::SetName("bo");    BO   ::SetDescription("Byte Ordering exception/Mismatched Instruction Storage exception");
			SPV  ::SetName("spv");   SPV  ::SetDescription("EFPU APU Operation");
			VLEMI::SetName("vlemi"); VLEMI::SetDescription("VLE Mode Instruction");
		}
	};
protected:
	
	// Interrupt Vector Prefix Register
	struct IVPR : PrivilegedSPR<IVPR, 63>
	{
		typedef PrivilegedSPR<IVPR, 63> Super;
		
		struct Vector_Base : Field<Vector_Base, 0, 23> {}; // Vector Base
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<Vector_Base> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<Vector_Base> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IVPR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IVPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			             this->SetName("ivpr");                     this->SetDescription("Interrupt Vector Prefix Register");
			Vector_Base::SetName("vector_base"); Vector_Base::SetDescription("Vector Base");
		}
	};
	
	// User SPR General 0
	struct USPRG0 : NonPrivilegedSPR<USPRG0, 256>
	{
		typedef NonPrivilegedSPR<USPRG0, 256> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		USPRG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		USPRG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("usprg0"); this->SetDescription("User SPR General 0"); }
	};
	
	// SPR General 0
	struct SPRG0 : PrivilegedSPR<SPRG0, 272>
	{
		typedef PrivilegedSPR<SPRG0, 272> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("sprg0"); this->SetDescription("SPR General 0"); }
	};

	// SPR General 1
	struct SPRG1 : PrivilegedSPR<SPRG1, 273>
	{
		typedef PrivilegedSPR<SPRG1, 273> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("sprg1"); this->SetDescription("SPR General 1"); }
	};

	// SPR General 2
	struct SPRG2 : PrivilegedSPR<SPRG2, 274>
	{
		typedef PrivilegedSPR<SPRG2, 274> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("sprg2"); this->SetDescription("SPR General 2"); }
	};

	// SPR General 3
	struct SPRG3 : PrivilegedSPR<SPRG3, 275>
	{
		typedef PrivilegedSPR<SPRG3, 275> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("sprg3"); this->SetDescription("SPR General 3"); }
	};

	// Processor ID Register
	struct PIR : PrivilegedSPR<PIR, 286>
	{
		typedef PrivilegedSPR<PIR, 286> Super;
		
		struct ID_0_23  : Field<ID_0_23 , 0, 23>  {}; // bits 0:23
		struct ID_24_31 : Field<ID_24_31, 24, 31> {}; // bits 24:31
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<ID_0_23, ID_24_31> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ID_0_23, ID_24_31> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PIR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("pir");
			this->SetDescription("Processor ID Register");
		}
	};
	
	// Processor Version Register
	struct PVR : NonPrivilegedSPR<PVR, 287>
	{
		typedef NonPrivilegedSPR<PVR, 287> Super;
		
		struct MANID     : Field<MANID    , 0, 3>   {};
		struct Type      : Field<Type     , 6, 11>  {};
		struct Version   : Field<Version  , 12, 15> {};
		struct MBG_Use   : Field<MBG_Use  , 16, 19> {};
		struct Minor_Rev : Field<Minor_Rev, 20, 23> {};
		struct Major_Rev : Field<Major_Rev, 24, 27> {};
		struct MBG_ID    : Field<MBG_ID   , 28, 31> {};

		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<MANID, Type, Version, MBG_Use, Minor_Rev, MBG_ID> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<MANID, Type, Version, MBG_Use, Minor_Rev, MBG_ID> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PVR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PVR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("pvr");
			MANID    ::SetName("MANID");
			Type     ::SetName("Type");
			Version  ::SetName("Version");
			MBG_Use  ::SetName("MBG_Use");
			Minor_Rev::SetName("Minor_Rev");
			Major_Rev::SetName("Major_Rev");
			MBG_ID   ::SetName("MBG_ID");
		}
	};

	// Debug Status Register
	struct DBSR : PrivilegedSPR<DBSR, 304>
	{
		typedef PrivilegedSPR<DBSR, 304> Super;
		
		struct IDE      : Field<IDE     , 0>      {}; // Imprecise Debug Event
		struct UDE      : Field<UDE     , 1>      {}; // Unconditional Debug Event
		struct MRR      : Field<MRR     , 2, 3>   {}; // Most Recent Reset
		struct ICMP     : Field<ICMP    , 4>      {}; // Instruction Complete Debug Event
		struct BRT      : Field<BRT     , 5>      {}; // Branch Taken Debug Event
		struct IRPT     : Field<IRPT    , 6>      {}; // Interrupt Taken Debug Event
		struct TRAP     : Field<TRAP    , 7>      {}; // Trap Taken Debug Event
		struct IAC1     : Field<IAC1    , 8>      {}; // Instruction Address Compare 1 Debug Event
		struct IAC2     : Field<IAC2    , 9>      {}; // Instruction Address Compare 2 Debug Event
		struct IAC3     : Field<IAC3    , 10>     {}; // Instruction Address Compare 3 Debug Event
		struct IAC4_8   : Field<IAC4_8  , 11>     {}; // Instruction Address Compare 4-8 Debug Event
		struct DAC1R    : Field<DAC1R   , 12>     {}; // Data Address Compare 1 Read Debug Event
		struct DAC1W    : Field<DAC1W   , 13>     {}; // Data Address Compare 1 Write Debug Event
		struct DAC2R    : Field<DAC2R   , 14>     {}; // Data Address Compare 2 Read Debug Event
		struct DAC2W    : Field<DAC2W   , 15>     {}; // Data Address Compare 2 Write Debug Event
		struct RET      : Field<RET     , 16>     {}; // Return Debug Event
		struct DEVT1    : Field<DEVT1   , 21>     {}; // External Debug Event 1 Debug Event
		struct DEVT2    : Field<DEVT2   , 22>     {}; // External Debug Event 2 Debug Event
		struct PMI      : Field<PMI     , 23>     {}; // Performance Monitor Interrupt Debug Event
		struct MPU      : Field<MPU     , 24>     {}; // Memory Protection Unit Debug Event
		struct CIRPT    : Field<CIRPT   , 25>     {}; // Critical Interrupt Taken Debug Event
		struct CRET     : Field<CRET    , 26>     {}; // Critical Return Debug Event
		struct DNI      : Field<DNI     , 27>     {}; // Debug Notify Interrupt (se_dni)
		struct DAC_OFST : Field<DAC_OFST, 28, 30> {}; // Data Address Compare Offset
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< IDE, UDE, MRR, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4_8, DAC1R, DAC1W
		                                                  , DAC2R, DAC2W, RET, DEVT1, DEVT2, PMI, MPU, CIRPT, CRET, DNI, DAC_OFST> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< IDE, UDE, MRR, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4_8, DAC1R, DAC1W
		                                                  , DAC2R, DAC2W, RET, DEVT1, DEVT2, PMI, MPU, CIRPT, CRET, DNI, DAC_OFST> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBSR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual bool MoveTo(uint32_t value) { this->template Set<ALL>(this->template Get<ALL>() & ~value); return true; } // W1C
	private:
		void Init()
		{
			          this->SetName("dbsr");               this->SetDescription("Debug Status Register");
			IDE     ::SetName("ide");      IDE     ::SetDescription("Imprecise Debug Event");
			UDE     ::SetName("ude");      UDE     ::SetDescription("Unconditional Debug Event");
			MRR     ::SetName("mrr");      MRR     ::SetDescription("Most Recent Reset");
			ICMP    ::SetName("icmp");     ICMP    ::SetDescription("Instruction Complete Debug Event");
			BRT     ::SetName("brt");      BRT     ::SetDescription("Branch Taken Debug Event");
			IRPT    ::SetName("irpt");     IRPT    ::SetDescription("Interrupt Taken Debug Event");
			TRAP    ::SetName("trap");     TRAP    ::SetDescription("Trap Taken Debug Event");
			IAC1    ::SetName("iac1");     IAC1    ::SetDescription("Instruction Address Compare 1 Debug Event");
			IAC2    ::SetName("iac2");     IAC2    ::SetDescription("Instruction Address Compare 2 Debug Event");
			IAC3    ::SetName("iac3");     IAC3    ::SetDescription("Instruction Address Compare 3 Debug Event");
			IAC4_8  ::SetName("iac4_8");   IAC4_8  ::SetDescription("Instruction Address Compare 4-8 Debug Event");
			DAC1R   ::SetName("dac1r");    DAC1R   ::SetDescription("Data Address Compare 1 Read Debug Event");
			DAC1W   ::SetName("dac1w");    DAC1W   ::SetDescription("Data Address Compare 1 Write Debug Event");
			DAC2R   ::SetName("dac2r");    DAC2R   ::SetDescription("Data Address Compare 2 Read Debug Event");
			DAC2W   ::SetName("dac2w");    DAC2W   ::SetDescription("Data Address Compare 2 Write Debug Event");
			RET     ::SetName("ret");      RET     ::SetDescription("Return Debug Event");
			DEVT1   ::SetName("devt1");    DEVT1   ::SetDescription("External Debug Event 1 Debug Event");
			DEVT2   ::SetName("devt2");    DEVT2   ::SetDescription("External Debug Event 2 Debug Event");
			PMI     ::SetName("pmi");      PMI     ::SetDescription("Performance Monitor Interrupt Debug Event");
			MPU     ::SetName("mpu");      MPU     ::SetDescription("Memory Protection Unit Debug Event");
			CIRPT   ::SetName("cirpt");    CIRPT   ::SetDescription("Critical Interrupt Taken Debug Event");
			CRET    ::SetName("cret");     CRET    ::SetDescription("Critical Return Debug Event");
			DNI     ::SetName("dni");      DNI     ::SetDescription("Debug Notify Interrupt (se_dni)");
			DAC_OFST::SetName("dac_ofst"); DAC_OFST::SetDescription("Data Address Compare Offset");
		}
	};
	
	// Debug Control Register 0
	struct DBCR0 : PrivilegedSPR<DBCR0, 308>
	{
		typedef PrivilegedSPR<DBCR0, 308> Super;
		
		struct EDM   : Field<EDM  , 0>      {}; // External Debug Mode
		struct IDM   : Field<IDM  , 1>      {}; // Internal Debug Mode
		struct RST   : Field<RST  , 2, 3>   {}; // Reset Control
		struct ICMP  : Field<ICMP , 4>      {}; // Instruction Complete Debug Event Enable
		struct BRT   : Field<BRT  , 5>      {}; // Branch Taken Debug Event Enable
		struct IRPT  : Field<IRPT , 6>      {}; // Interrupt Taken Debug Event Enable
		struct TRAP  : Field<TRAP , 7>      {}; // Trap Taken Debug Event Enable
		struct IAC1  : Field<IAC1 , 8>      {}; // Instruction Address Compare 1 Debug Event Enable
		struct IAC2  : Field<IAC2 , 9>      {}; // Instruction Address Compare 2 Debug Event Enable
		struct IAC3  : Field<IAC3 , 10>     {}; // Instruction Address Compare 3 Debug Event Enable
		struct IAC4  : Field<IAC4 , 11>     {}; // Instruction Address Compare 4 Debug Event Enable
		struct DAC1  : Field<DAC1 , 12, 13> {}; // Data Address Compare 1 Debug Event Enable
		struct DAC2  : Field<DAC2 , 14, 15> {}; // Data Address Compare 2 Debug Event Enable
		struct RET   : Field<RET  , 16>     {}; // Return Debug Event Enable
		struct IAC5  : Field<IAC5 , 17>     {}; // Instruction Address Compare 5 Debug Event Enable
		struct IAC6  : Field<IAC6 , 18>     {}; // Instruction Address Compare 6 Debug Event Enable
		struct IAC7  : Field<IAC7 , 19>     {}; // Instruction Address Compare 7 Debug Event Enable
		struct IAC8  : Field<IAC8 , 20>     {}; // Instruction Address Compare 8 Debug Event Enable
		struct DEVT1 : Field<DEVT1, 21>     {}; // External Debug Event 1 Enable
		struct DEVT2 : Field<DEVT2, 22>     {}; // External Debug Event 2 Enable
		struct DCNT1 : Field<DCNT1, 23>     {}; // Debug Counter 1 Debug Event Enable
		struct DCNT2 : Field<DCNT2, 24>     {}; // Debug Counter 2 Debug Event Enable
		struct CIRPT : Field<CIRPT, 25>     {}; // Critical Interrupt Taken Debug Event Enable
		struct CRET  : Field<CRET , 26>     {}; // Critical Return Debug Event Enable
		struct FT    : Field<FT   , 31>     {}; // Freeze Timers on Debug Event
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< EDM, IDM, RST, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4, DAC1, DAC2
		                                                  , RET, IAC5, IAC6, IAC7, IAC8, DEVT1, DEVT2, DCNT1, DCNT2, CIRPT, CRET, FT> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< EDM, IDM, RST, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4, DAC1, DAC2, RET
		                                                  , IAC5, IAC6, IAC7, IAC8, DEVT1, DEVT2, DCNT1, DCNT2, CIRPT, CRET, FT> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			       this->SetName("dbcr0");        this->SetDescription("Debug Control Register 0");
			EDM  ::SetName("edm");   EDM  ::SetDescription("External Debug Mode");
			IDM  ::SetName("idm");   IDM  ::SetDescription("Internal Debug Mode");
			RST  ::SetName("rst");   RST  ::SetDescription("Reset Control");
			ICMP ::SetName("icmp");  ICMP ::SetDescription("Instruction Complete Debug Event Enable");
			BRT  ::SetName("brt");   BRT  ::SetDescription("Branch Taken Debug Event Enable");
			IRPT ::SetName("irpt");  IRPT ::SetDescription("Interrupt Taken Debug Event Enable");
			TRAP ::SetName("trap");  TRAP ::SetDescription("Trap Taken Debug Event Enable");
			IAC1 ::SetName("iac1");  IAC1 ::SetDescription("Instruction Address Compare 1 Debug Event Enable");
			IAC2 ::SetName("iac2");  IAC2 ::SetDescription("Instruction Address Compare 2 Debug Event Enable");
			IAC3 ::SetName("iac3");  IAC3 ::SetDescription("Instruction Address Compare 3 Debug Event Enable");
			IAC4 ::SetName("iac4");  IAC4 ::SetDescription("Instruction Address Compare 4 Debug Event Enable");
			DAC1 ::SetName("dac1");  DAC1 ::SetDescription("Data Address Compare 1 Debug Event Enable");
			DAC2 ::SetName("dac2");  DAC2 ::SetDescription("Data Address Compare 2 Debug Event Enable");
			RET  ::SetName("ret");   RET  ::SetDescription("Return Debug Event Enable");
			IAC5 ::SetName("iac5");  IAC5 ::SetDescription("Instruction Address Compare 5 Debug Event Enable");
			IAC6 ::SetName("iac6");  IAC6 ::SetDescription("Instruction Address Compare 6 Debug Event Enable");
			IAC7 ::SetName("iac7");  IAC7 ::SetDescription("Instruction Address Compare 7 Debug Event Enable");
			IAC8 ::SetName("iac8");  IAC8 ::SetDescription("Instruction Address Compare 8 Debug Event Enable");
			DEVT1::SetName("devt1"); DEVT1::SetDescription("External Debug Event 1 Enable");
			DEVT2::SetName("devt2"); DEVT2::SetDescription("External Debug Event 2 Enable");
			DCNT1::SetName("dcnt1"); DCNT1::SetDescription("Debug Counter 1 Debug Event Enable");
			DCNT2::SetName("dcnt2"); DCNT2::SetDescription("Debug Counter 2 Debug Event Enable");
			CIRPT::SetName("cirpt"); CIRPT::SetDescription("Critical Interrupt Taken Debug Event Enable");
			CRET ::SetName("cret");  CRET ::SetDescription("Critical Return Debug Event Enable");
			FT   ::SetName("ft");    FT   ::SetDescription("Freeze Timers on Debug Event");
		}
	};
	
	// Debug Control Register 1
	struct DBCR1 : PrivilegedSPR<DBCR1, 309>
	{
		typedef PrivilegedSPR<DBCR1, 309> Super;
		
		struct IAC1US : Field<IAC1US, 0, 1>   {}; // Instruction Address Compare 1 User/Supervisor Mode
		struct IAC1ER : Field<IAC1ER, 2, 3>   {}; // Instruction Address Compare 1 Effective/Real Mode
		struct IAC2US : Field<IAC2US, 4, 5>   {}; // Instruction Address Compare 2 User/Supervisor Mode
		struct IAC2ER : Field<IAC2ER, 6, 7>   {}; // Instruction Address Compare 2 Effective/Real Mode
		struct IAC12M : Field<IAC12M, 8, 9>   {}; // Instruction Address Compare 1/2 Mode
		struct IAC3US : Field<IAC3US, 16, 7>  {}; // Instruction Address Compare 3 User/Supervisor Mode
		struct IAC3ER : Field<IAC3ER, 18, 19> {}; // Instruction Address Compare 3 Effective/Real Mode
		struct IAC4US : Field<IAC4US, 20, 21> {}; // Instruction Address Compare 4 User/Supervisor Mode
		struct IAC4ER : Field<IAC4ER, 22, 23> {}; // Instruction Address Compare 4 Effective/Real Mode
		struct IAC34M : Field<IAC34M, 24, 25> {}; // Instruction Address Compare 3/4 Mode
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IAC1US, IAC1ER, IAC2US, IAC2ER, IAC12M, IAC3US, IAC3ER, IAC4US, IAC4ER, IAC34M> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IAC1US, IAC1ER, IAC2US, IAC2ER, IAC12M, IAC3US, IAC3ER, IAC4US, IAC4ER, IAC34M> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("dbcr1");          this->SetDescription("Debug Control Register 1");
			IAC1US::SetName("iac1us"); IAC1US::SetDescription("Instruction Address Compare 1 User/Supervisor Mode");
			IAC1ER::SetName("iac1er"); IAC1ER::SetDescription("Instruction Address Compare 1 Effective/Real Mode");
			IAC2US::SetName("iac2us"); IAC2US::SetDescription("Instruction Address Compare 2 User/Supervisor Mode");
			IAC2ER::SetName("iac2er"); IAC2ER::SetDescription("Instruction Address Compare 2 Effective/Real Mode");
			IAC12M::SetName("iac12m"); IAC12M::SetDescription("Instruction Address Compare 1/2 Mode");
			IAC3US::SetName("iac3us"); IAC3US::SetDescription("Instruction Address Compare 3 User/Supervisor Mode");
			IAC3ER::SetName("iac3er"); IAC3ER::SetDescription("Instruction Address Compare 3 Effective/Real Mode");
			IAC4US::SetName("iac4us"); IAC4US::SetDescription("Instruction Address Compare 4 User/Supervisor Mode");
			IAC4ER::SetName("iac4er"); IAC4ER::SetDescription("Instruction Address Compare 4 Effective/Real Mode");
			IAC34M::SetName("iac34m"); IAC34M::SetDescription("Instruction Address Compare 3/4 Mode");
		}
	};
	
	// Debug Control Register 2
	struct DBCR2 : PrivilegedSPR<DBCR2, 310>
	{
		typedef PrivilegedSPR<DBCR2, 310> Super;
		
		struct DAC1US  : Field<DAC1US , 0, 1>   {}; // Data Address Compare 1 User/Supervisor Mode
		struct DAC1ER  : Field<DAC1ER , 2, 3>   {}; // Data Address Compare 1 Effective/Read Mode
		struct DAC2US  : Field<DAC2US , 4, 5>   {}; // Data Address Compare 2 User/Supervisor Mode
		struct DAC2ER  : Field<DAC2ER , 6, 7>   {}; // Data Address Compare 2 Effective/Read Mode
		struct DAC12M  : Field<DAC12M , 8, 9>   {}; // Data Address Compare 1/2 Mode
		struct DAC1LNK : Field<DAC1LNK, 10>     {}; // Data Address Compare 1 Linked
		struct DAC2LNK : Field<DAC2LNK, 11>     {}; // Data Address Compare 2 Linked
		struct DVC1M   : Field<DVC1M  , 12, 13> {}; // Data Value Compare 1 Mode
		struct DVC2M   : Field<DVC2M  , 14, 15> {}; // Data Value Compare 2 Mode
		struct DVC1BE  : Field<DVC1BE , 16, 23> {}; // Data Value Compare 1 Byte Enables
		struct DVC2BE  : Field<DVC2BE , 24, 31> {}; // Data Value Compare 2 Byte Enables
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DAC1US, DAC1ER, DAC2US, DAC2ER, DAC12M, DAC1LNK, DAC2LNK, DVC1M, DVC2M, DVC1BE, DVC2BE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DAC1US, DAC1ER, DAC2US, DAC2ER, DAC12M, DAC1LNK, DAC2LNK, DVC1M, DVC2M, DVC1BE, DVC2BE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("dbcr2");            this->SetDescription("Debug Control Register 2");
			DAC1US ::SetName("dac1us");  DAC1US ::SetDescription("Data Address Compare 1 User/Supervisor Mode");
			DAC1ER ::SetName("dac1er");  DAC1ER ::SetDescription("Data Address Compare 1 Effective/Read Mode");
			DAC2US ::SetName("dac2us");  DAC2US ::SetDescription("Data Address Compare 2 User/Supervisor Mode");
			DAC2ER ::SetName("dac2er");  DAC2ER ::SetDescription("Data Address Compare 2 Effective/Read Mode");
			DAC12M ::SetName("dac12m");  DAC12M ::SetDescription("Data Address Compare 1/2 Mode");
			DAC1LNK::SetName("dac1lnk"); DAC1LNK::SetDescription("Data Address Compare 1 Linked");
			DAC2LNK::SetName("dac2lnk"); DAC2LNK::SetDescription("Data Address Compare 2 Linked");
			DVC1M  ::SetName("dvc1m");   DVC1M  ::SetDescription("Data Value Compare 1 Mode");
			DVC2M  ::SetName("dvc2m");   DVC2M  ::SetDescription("Data Value Compare 2 Mode");
			DVC1BE ::SetName("dvc1be");  DVC1BE ::SetDescription("Data Value Compare 1 Byte Enables");
			DVC2BE ::SetName("dvc2be");  DVC2BE ::SetDescription("Data Value Compare 2 Byte Enables");
		}
	};
	
	// Instruction Address Compare 1
	struct IAC1 : PrivilegedSPR<IAC1, 312>
	{
		typedef PrivilegedSPR<IAC1, 312> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac1"); this->SetDescription("Instruction Address Compare 1"); }
	};

	// Instruction Address Compare 2
	struct IAC2 : PrivilegedSPR<IAC2, 313>
	{
		typedef PrivilegedSPR<IAC2, 313> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac2"); this->SetDescription("Instruction Address Compare 2"); }
	};

	// Instruction Address Compare 3
	struct IAC3 : PrivilegedSPR<IAC3, 314>
	{
		typedef PrivilegedSPR<IAC3, 314> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac3"); this->SetDescription("Instruction Address Compare 3"); }
	};

	// Instruction Address Compare 4
	struct IAC4 : PrivilegedSPR<IAC4, 315>
	{
		typedef PrivilegedSPR<IAC4, 315> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC4(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC4(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac4"); this->SetDescription("Instruction Address Compare 4"); }
	};

	// Data Address Compare 1
	struct DAC1 : PrivilegedSPR<DAC1, 316>
	{
		typedef PrivilegedSPR<DAC1, 316> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DAC1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("dac1"); this->SetDescription("Data Address Compare 1"); }
	};

	// Data Address Compare 2
	struct DAC2 : PrivilegedSPR<DAC2, 317>
	{
		typedef PrivilegedSPR<DAC2, 317> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DAC2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("dac2"); this->SetDescription("Data Address Compare 2"); }
	};

	// Data Value Compare 1
	struct DVC1 : PrivilegedSPR<DVC1, 318>
	{
		typedef PrivilegedSPR<DVC1, 318> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("dvc1"); this->SetDescription("Data Value Compare 1"); }
	};

	// Data Value Compare 2
	struct DVC2 : PrivilegedSPR<DVC2, 319>
	{
		typedef PrivilegedSPR<DVC2, 319> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("dvc2"); this->SetDescription("Data Value Compare 2"); }
	};
	
	// Thread ID
	struct TIR : ReadOnlyPrivilegedSPR<TIR, 446>
	{
		typedef ReadOnlyPrivilegedSPR<TIR, 446> Super;
		
		struct ALL : Field<ALL, 0, 31> {};

		TIR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		TIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("tir"); this->SetDescription("Thread ID"); }
	};

	// Signal Processing Extension/Embedded Floating-point Status and Control Register
	struct SPEFSCR : NonPrivilegedSPR<SPEFSCR, 512>
	{
		typedef NonPrivilegedSPR<SPEFSCR, 512> Super;
		
		struct SOVH  : Field<SOVH,  0>  {}; // Summary Integer overflow high
		struct OVH   : Field<OVH,   1>  {}; // Embedded Integer overflow high
		struct FGH   : Field<FGH,   2>  {}; // Embedded floating-point guard bit high
		struct FXH   : Field<FXH,   3>  {}; // Embedded floating-point inexact bit high
		struct FINVH : Field<FINVH, 4>  {}; // Embedded Floating-point Invalid Operation / Input error
		struct FDBZH : Field<FDBZH, 5>  {}; // Embedded Floating-point Divide by Zero
		struct FUNFH : Field<FUNFH, 6>  {}; // Embedded Floating-point Underflow
		struct FOVFH : Field<FOVFH, 7>  {}; // Embedded Floating-point Overflow
		struct FINXS : Field<FINXS, 10> {}; // Embedded Floating-point Inexact Sticky Flag
		struct FINVS : Field<FINVS, 11> {}; // Enmedded Floating-point Invalid Operation Sticky Flag
		struct FDBZS : Field<FDBZS, 12> {}; // Embedded Floating-point Divide by Zero Sticky Flag
		struct FUNFS : Field<FUNFS, 13> {}; // Embedded Floating-point Underflow Sticky Flag
		struct FOVFS : Field<FOVFS, 14> {}; // Embedded Floating-point Overflow Sticky Flag
		struct MODE  : Field<MODE , 15> {}; // Embedded Floating-point Operating Mode
		struct FG    : Field<FG   , 18> {}; // Embedded Floating-point Guard bit
		struct FX    : Field<FX   , 19> {}; // Embedded Floating-point Sticky bit
		struct FINV  : Field<FINV , 20> {}; // Embedded Floating-point Invalid Operation / Input error
		struct FDBZ  : Field<FDBZ , 21> {}; // Embedded Floating-point Divide by Zero
		struct FUNF  : Field<FUNF , 22> {}; // Embedded Floating-point Underflow
		struct FOVF  : Field<FOVF , 23> {}; // Embedded Floating-point Overflow
		struct FINXE : Field<FINXE, 25> {}; // Embedded Floating-point Inexact Exception Enable
		struct FINVE : Field<FINVE, 26> {}; // Embedded Floating-point Invalid Operation / Input Error Exception Enable
		struct FDBZE : Field<FDBZE, 27> {}; // Embedded Floating-point Divide by Zero Exception Enable
		struct FUNFE : Field<FUNFE, 28> {}; // Embedded Floating-point Underflow Exception Enable
		struct FOVFE : Field<FOVFE, 29> {}; // Embedded Floating-point Overflow Exception Enable
		struct FRMC  : Field<FRMC , 30> {}; // Embedded Floating-point Rounding Mode Control
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<FINXS, FINVS, FDBZS, FUNFS, FOVFS, MODE, FG, FX, FINV, FDBZ, FUNF, FOVF, FINXE, FINVE, FDBZE, FUNFE, FOVFE, FRMC> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<FINXS, FINVS, FDBZS, FUNFS, FOVFS, MODE, FG, FX, FINV, FDBZ, FUNF, FOVF, FINXE, FINVE, FDBZE, FUNFE, FOVFE, FRMC> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		SPEFSCR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPEFSCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			       this->SetName("spefscr");      this->SetDescription("Signal Processing Extension/Embedded Floating-point Status and Control Register");
			FINXS::SetName("finxs"); FINXS::SetDescription("Embedded Floating-point Inexact Sticky Flag");
			FINVS::SetName("finvs"); FINVS::SetDescription("Enmedded Floating-point Invalid Operation Sticky Flag");
			FDBZS::SetName("fdbzs"); FDBZS::SetDescription("Embedded Floating-point Divide by Zero Sticky Flag");
			FUNFS::SetName("funfs"); FUNFS::SetDescription("Embedded Floating-point Underflow Sticky Flag");
			FOVFS::SetName("fovfs"); FOVFS::SetDescription("Embedded Floating-point Overflow Sticky Flag");
			MODE ::SetName("mode");  MODE ::SetDescription("Embedded Floating-point Operating Mode");
			FG   ::SetName("fg");    FG   ::SetDescription("Embedded Floating-point Guard bit");
			FX   ::SetName("fx");    FX   ::SetDescription("Embedded Floating-point Sticky bit");
			FINV ::SetName("finv");  FINV ::SetDescription("Embedded Floating-point Invalid Operation / Input error");
			FDBZ ::SetName("fdbz");  FDBZ ::SetDescription("Embedded Floating-point Divide by Zero");
			FUNF ::SetName("funf");  FUNF ::SetDescription("Embedded Floating-point Underflow");
			FOVF ::SetName("fovf");  FOVF ::SetDescription("Embedded Floating-point Overflow");
			FINXE::SetName("finxe"); FINXE::SetDescription("Embedded Floating-point Inexact Exception Enable");
			FINVE::SetName("finve"); FINVE::SetDescription("Embedded Floating-point Invalid Operation / Input Error Exception Enable");
			FDBZE::SetName("fdbze"); FDBZE::SetDescription("Embedded Floating-point Divide by Zero Exception Enable");
			FUNFE::SetName("funfe"); FUNFE::SetDescription("Embedded Floating-point Underflow Exception Enable");
			FOVFE::SetName("fovfe"); FOVFE::SetDescription("Embedded Floating-point Overflow Exception Enable");
			FRMC ::SetName("frmc");  FRMC ::SetDescription("Embedded Floating-point Rounding Mode Control");
		}
	};
	
	enum
	{
		CARCH_HARVARD          = 0,
		CARCH_UNIFIED          = 1,
		CARCH_INSTRUCTION_ONLY = 2
	};
	
	enum
	{
		CREPL_LRU                = 0,
		CREPL_PLRU               = 1,
		CREPL_PSEUDO_ROUND_ROBIN = 2,
		CREPL_FIFO               = 3
	};
	
	// L1 Cache Configuration Register 0
	struct L1CFG0 : NonPrivilegedSPR<L1CFG0, 515>
	{
		typedef NonPrivilegedSPR<L1CFG0, 515> Super;
	
		struct CARCH   : Field<CARCH  , 0, 1>   {}; // Cache Architecture: 00=Harvard, 01=Unified, 10=Instruction only (L1CFG1 contains configuration information), 11=Reserved
		struct CWPA    : Field<CWPA   , 2>      {}; // Cache Way Partitioning Available
		struct DCFAHA  : Field<DCFAHA , 3>      {}; // Data Cache Flush All by Hardware Available
		struct DCFISWA : Field<DCFISWA, 4>      {}; // Data Cache Flush/Invalidate by Set and Way Available
		struct DCBSIZE : Field<DCBSIZE, 7, 8>   {}; // Data Cache Block Size: 00=32 bytes, 01=64 bytes, 10=128 bytes, 11=Reserved
		struct DCREPL  : Field<DCREPL , 9, 10>  {}; // Data Cache Replacement Policy: 00=True LRU, 01=Pseudo LRU, 10=Pseudo round robin, 11=FIFO
		struct DCLA    : Field<DCLA   , 11>     {}; // Data Cache Lockup APU Available
		struct DCECA   : Field<DCECA  , 12>     {}; // Data Cache Error Checking Available
		struct DCNWAY  : Field<DCNWAY , 13, 20> {}; // Data Cache Number of Ways (minus 1)
		struct DCSIZE  : Field<DCSIZE , 21, 31> {}; // Data Cache Size (in KB)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CARCH, CWPA, DCFAHA, DCFISWA, DCBSIZE, DCREPL, DCLA, DCECA, DCNWAY, DCSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CARCH, CWPA, DCFAHA, DCFISWA, DCBSIZE, DCREPL, DCLA, DCECA, DCNWAY, DCSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("l1cfg0");           this->SetDescription("L1 Cache Configuration Register 0");
			CARCH  ::SetName("carch");   CARCH  ::SetDescription("Cache Architecture");
			CWPA   ::SetName("cwpa");    CWPA   ::SetDescription("Cache Way Partitioning Available");
			DCFAHA ::SetName("dcfaha");  DCFAHA ::SetDescription("Data Cache Flush All by Hardware Available");
			DCFISWA::SetName("dcfiswa"); DCFISWA::SetDescription("Data Cache Flush/Invalidate by Set and Way Available");
			DCBSIZE::SetName("dcbsize"); DCBSIZE::SetDescription("Data Cache Block Size");
			DCREPL ::SetName("dcrepl");  DCREPL ::SetDescription("Data Cache Replacement Policy");
			DCLA   ::SetName("dcla");    DCLA   ::SetDescription("Data Cache Lockup APU Available");
			DCECA  ::SetName("dceca");   DCECA  ::SetDescription("Data Cache Error Checking Available");
			DCNWAY ::SetName("dcnway");  DCNWAY ::SetDescription("Data Cache Number of Ways");
			DCSIZE ::SetName("dcsize");  DCSIZE ::SetDescription("Data Cache Size");
		}
	};

	// L1 Cache Configuration Register 1
	struct L1CFG1 : NonPrivilegedSPR<L1CFG1, 516>
	{
		typedef NonPrivilegedSPR<L1CFG1, 516> Super;
		
		struct ICFISWA : Field<ICFISWA, 4>      {}; // Instruction Cache Flush/Invalidate by Set and Way Available
		struct ICBSIZE : Field<ICBSIZE, 7, 8>   {}; // Instruction Cache Block Size
		struct ICREPL  : Field<ICREPL , 9, 10>  {}; // Instruction Cache Replacement Policy
		struct ICLA    : Field<ICLA   , 11>     {}; // Instruction Cache Locking APU Available
		struct ICECA   : Field<ICECA  , 12>     {}; // Instruction Cache Error Checking Available
		struct ICNWAY  : Field<ICNWAY , 13, 20> {}; // Instruction Cache Number of Ways (minus 1)
		struct ICSIZE  : Field<ICSIZE , 21, 31> {}; // Instruction Cache Size (in KB)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<ICFISWA, ICBSIZE, ICREPL, ICLA, ICECA, ICNWAY, ICSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ICFISWA, ICBSIZE, ICREPL, ICLA, ICECA, ICNWAY, ICSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CFG1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CFG1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("l1cfg1");           this->SetDescription("L1 Cache Configuration Register 1");
			ICFISWA::SetName("icfiswa"); ICFISWA::SetDescription("Instruction Cache Flush/Invalidate by Set and Way Available");
			ICBSIZE::SetName("icbsize"); ICBSIZE::SetDescription("Instruction Cache Block Size");
			ICREPL ::SetName("icrepl");  ICREPL ::SetDescription("Instruction Cache Replacement Policy");
			ICLA   ::SetName("icla");    ICLA   ::SetDescription("Instruction Cache Locking APU Available");
			ICECA  ::SetName("iceca");   ICECA  ::SetDescription("Instruction Cache Error Checking Available");
			ICNWAY ::SetName("icnway");  ICNWAY ::SetDescription("Instruction Cache Number of Ways");
			ICSIZE ::SetName("icsize");  ICSIZE ::SetDescription("Instruction Cache Size");
		}
	};
	
	// Nexus 3 Process ID register
	struct NPIDR : NonPrivilegedSPR<NPIDR, 517>
	{
		typedef NonPrivilegedSPR<NPIDR, 517> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		NPIDR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		NPIDR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("npidr"); this->SetDescription("Nexus 3 Process ID register"); }
	};

	// Debug Control Register 3
	struct DBCR3 : PrivilegedSPR<DBCR3, 561>
	{
		typedef PrivilegedSPR<DBCR3, 561> Super;
		
		struct DEVT1C1 : Field<DEVT1C1, 0>  {}; // External Debug Event 1 Count 1 Enable
		struct DEVT2C1 : Field<DEVT2C1, 1>  {}; // External Debug Event 2 Count 1 Enable
		struct ICMPC1  : Field<ICMPC1 , 2>  {}; // Instruction Complete Debug Event Count 1 Enable
		struct IAC1C1  : Field<IAC1C1 , 3>  {}; // Instruction Address Compare 1 Count 1 Enable
		struct IAC2C1  : Field<IAC2C1 , 4>  {}; // Instruction Address Compare 2 Count 1 Enable
		struct IAC3C1  : Field<IAC3C1 , 5>  {}; // Instruction Address Compare 3 Count 1 Enable
		struct IAC4C1  : Field<IAC4C1 , 6>  {}; // Instruction Address Compare 4 Count 1 Enable
		struct DAC1RC1 : Field<DAC1RC1, 7>  {}; // Data Address Compare 1 Read Debug Event Count 1 Enable
		struct DAC1WC1 : Field<DAC1WC1, 8>  {}; // Data Address Compare 1 Write Debug Event Count 1 Enable
		struct DAC2RC1 : Field<DAC2RC1, 9>  {}; // Data Address Compare 2 Read Debug Event Count 1 Enable
		struct DAC2WC1 : Field<DAC2WC1, 10> {}; // Data Address Compare 2 Write Debug Event Count 1 Enable
		struct IRPTC1  : Field<IRPTC1 , 11> {}; // Interrupt Taken Debug Event Count 1 Enable
		struct RETC1   : Field<RETC1  , 12> {}; // Return Debug Event Count 1 Enable
		struct DEVT1C2 : Field<DEVT1C2, 13> {}; // External Debug Event 1 Count 2 Enable
		struct DEVT2C2 : Field<DEVT2C2, 14> {}; // External Debug Event 2 Count 2 Enable
		struct ICMPC2  : Field<ICMPC2 , 15> {}; // Instruction Complete Debug Event Count 2 Enable
		struct IAC1C2  : Field<IAC1C2 , 16> {}; // Instruction Address Compare 1 Debug Event Count 2 Enable
		struct IAC2C2  : Field<IAC2C2 , 17> {}; // Instruction Address Compare 2 Debug Event Count 2 Enable
		struct IAC3C2  : Field<IAC3C2 , 18> {}; // Instruction Address Compare 3 Debug Event Count 2 Enable
		struct IAC4C2  : Field<IAC4C2 , 19> {}; // Instruction Address Compare 4 Debug Event Count 2 Enable
		struct DAC1RC2 : Field<DAC1RC2, 20> {}; // Data Address Compare 1 Read Debug Event Count 2 Enable
		struct DAC1WC2 : Field<DAC1WC2, 21> {}; // Data Address Compare 1 Write Debug Event Count 2 Enable
		struct DAC2RC2 : Field<DAC2RC2, 22> {}; // Data Address Compare 2 Read Debug Event Count 2 Enable
		struct DAC2WC2 : Field<DAC2WC2, 23> {}; // Data Address Compare 2 Write Debug Event Count 2 Enable
		struct DEVT1T1 : Field<DEVT1T1, 24> {}; // External Debug Event 1 Trigger Counter 1 Enable
		struct DEVT2T1 : Field<DEVT2T1, 25> {}; // External Debug Event 2 Trigger Counter 1 Enable
		struct IAC1T1  : Field<IAC1T1 , 26> {}; // Instruction Address Compare 1 Trigger Counter 1 Enable
		struct IAC3T1  : Field<IAC3T1 , 27> {}; // Instruction Address Compare 3 Trigger Counter 1 Enable
		struct DAC1RT1 : Field<DAC1RT1, 28> {}; // Data Address Compare 1 Read Trigger Counter 1 Enable
		struct DAC1WT1 : Field<DAC1WT1, 29> {}; // Data Address Compare 1 Write Trigger Counter 1 Enable
		struct CNT2T1  : Field<CNT2T1 , 30> {}; // Debug Counter 2 Trigger Counter 1 Enable
		struct _CONFIG : Field<_CONFIG , 31> {}; // Debug Counter Configuration
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< DEVT1C1, DEVT2C1, ICMPC1, IAC1C1, IAC2C1, IAC3C1
		                                                  , IAC4C1, DAC1RC1, DAC1WC1, DAC2RC1, DAC2WC1, IRPTC1
		                                                  , RETC1, DEVT1C2, DEVT2C2, ICMPC2, IAC1C2, IAC2C2
		                                                  , IAC3C2, IAC4C2, DAC1RC2, DAC1WC2, DAC2RC2, DAC2WC2
		                                                  , DEVT1T1, DEVT2T1, IAC1T1, IAC3T1, DAC1RT1, DAC1WT1
		                                                  , CNT2T1, _CONFIG > ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< DEVT1C1, DEVT2C1, ICMPC1, IAC1C1, IAC2C1, IAC3C1
		                                                  , IAC4C1, DAC1RC1, DAC1WC1, DAC2RC1, DAC2WC1, IRPTC1
		                                                  , RETC1, DEVT1C2, DEVT2C2, ICMPC2, IAC1C2, IAC2C2
		                                                  , IAC3C2, IAC4C2, DAC1RC2, DAC1WC2, DAC2RC2, DAC2WC2
		                                                  , DEVT1T1, DEVT2T1, IAC1T1, IAC3T1, DAC1RT1, DAC1WT1
		                                                  , CNT2T1, _CONFIG > ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("dbcr3");            this->SetDescription("Debug Control Register 3");
			DEVT1C1::SetName("devt1c1"); DEVT1C1::SetDescription("External Debug Event 1 Count 1 Enable");
			DEVT2C1::SetName("devt2c1"); DEVT2C1::SetDescription("External Debug Event 2 Count 1 Enable");
			ICMPC1 ::SetName("icmpc1");  ICMPC1 ::SetDescription("Instruction Complete Debug Event Count 1 Enable");
			IAC1C1 ::SetName("iac1c1");  IAC1C1 ::SetDescription("Instruction Address Compare 1 Count 1 Enable");
			IAC2C1 ::SetName("iac2c1");  IAC2C1 ::SetDescription("Instruction Address Compare 2 Count 1 Enable");
			IAC3C1 ::SetName("iac3c1");  IAC3C1 ::SetDescription("Instruction Address Compare 3 Count 1 Enable");
			IAC4C1 ::SetName("iac4c1");  IAC4C1 ::SetDescription("Instruction Address Compare 4 Count 1 Enable");
			DAC1RC1::SetName("dac1rc1"); DAC1RC1::SetDescription("Data Address Compare 1 Read Debug Event Count 1 Enable");
			DAC1WC1::SetName("dac1wc1"); DAC1WC1::SetDescription("Data Address Compare 1 Write Debug Event Count 1 Enable");
			DAC2RC1::SetName("dac2rc1"); DAC2RC1::SetDescription("Data Address Compare 2 Read Debug Event Count 1 Enable");
			DAC2WC1::SetName("dac2wc1"); DAC2WC1::SetDescription("Data Address Compare 2 Write Debug Event Count 1 Enable");
			IRPTC1 ::SetName("irptc1");  IRPTC1 ::SetDescription("Interrupt Taken Debug Event Count 1 Enable");
			RETC1  ::SetName("retc1");   RETC1  ::SetDescription("Return Debug Event Count 1 Enable");
			DEVT1C2::SetName("devt1c2"); DEVT1C2::SetDescription("External Debug Event 1 Count 2 Enable");
			DEVT2C2::SetName("devt2c2"); DEVT2C2::SetDescription("External Debug Event 2 Count 2 Enable");
			ICMPC2 ::SetName("icmpc2");  ICMPC2 ::SetDescription("Instruction Complete Debug Event Count 2 Enable");
			IAC1C2 ::SetName("iac1c2");  IAC1C2 ::SetDescription("Instruction Address Compare 1 Debug Event Count 2 Enable");
			IAC2C2 ::SetName("iac2c2");  IAC2C2 ::SetDescription("Instruction Address Compare 2 Debug Event Count 2 Enable");
			IAC3C2 ::SetName("iac3c2");  IAC3C2 ::SetDescription("Instruction Address Compare 3 Debug Event Count 2 Enable");
			IAC4C2 ::SetName("iac4c2");  IAC4C2 ::SetDescription("Instruction Address Compare 4 Debug Event Count 2 Enable");
			DAC1RC2::SetName("dac1rc2"); DAC1RC2::SetDescription("Data Address Compare 1 Read Debug Event Count 2 Enable");
			DAC1WC2::SetName("dac1wc2"); DAC1WC2::SetDescription("Data Address Compare 1 Write Debug Event Count 2 Enable");
			DAC2RC2::SetName("dac2rc2"); DAC2RC2::SetDescription("Data Address Compare 2 Read Debug Event Count 2 Enable");
			DAC2WC2::SetName("dac2wc2"); DAC2WC2::SetDescription("Data Address Compare 2 Write Debug Event Count 2 Enable");
			DEVT1T1::SetName("devt1t1"); DEVT1T1::SetDescription("External Debug Event 1 Trigger Counter 1 Enable");
			DEVT2T1::SetName("devt2t1"); DEVT2T1::SetDescription("External Debug Event 2 Trigger Counter 1 Enable");
			IAC1T1 ::SetName("iac1t1");  IAC1T1 ::SetDescription("Instruction Address Compare 1 Trigger Counter 1 Enable");
			IAC3T1 ::SetName("iac3t1");  IAC3T1 ::SetDescription("Instruction Address Compare 3 Trigger Counter 1 Enable");
			DAC1RT1::SetName("dac1rt1"); DAC1RT1::SetDescription("Data Address Compare 1 Read Trigger Counter 1 Enable");
			DAC1WT1::SetName("dac1wt1"); DAC1WT1::SetDescription("Data Address Compare 1 Write Trigger Counter 1 Enable");
			CNT2T1 ::SetName("cnt2t1");  CNT2T1 ::SetDescription("Debug Counter 2 Trigger Counter 1 Enable");
			_CONFIG ::SetName("config"); _CONFIG ::SetDescription("Debug Counter Configuration");
		}
	};
	
	// Debug Counter Register
	struct DBCNT : PrivilegedSPR<DBCNT, 562>
	{
		typedef PrivilegedSPR<DBCNT, 562> Super;
		
		struct CNT1 : Field<CNT1, 0, 15>  {};
		struct CNT2 : Field<CNT2, 16, 31> {};
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CNT1, CNT2> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CNT1, CNT2> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCNT(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCNT(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("dbcnt"); this->SetDescription("Debug Counter Register");
			CNT1::SetName("cnt1"); CNT2::SetName("cnt2");
		}
	};

	// Debug Control Register 4
	struct DBCR4 : PrivilegedSPR<DBCR4, 563>
	{
		typedef PrivilegedSPR<DBCR4, 563> Super;
		
		struct DVC1C   : Field<DVC1C  , 1>      {}; // Data Value Compare 1 Control
		struct DVC2C   : Field<DVC2C  , 3>      {}; // Data Value Compare 2 Control
		struct DAC1XMH : Field<DAC1XMH, 13>     {}; // Data Address Compare 1 Extended Mask Control High
		struct DAC2XMH : Field<DAC2XMH, 15>     {}; // Data Address Compare 2 Extended Mask Control High 
		struct DAC1XM  : Field<DAC1XM , 16, 19> {}; // Data Address Compare 1 Extended Mask Control
		struct DAC2XM  : Field<DAC2XM , 20, 23> {}; // Data Address Compare 2 Extended Mask Control
		struct DAC1CFG : Field<DAC1CFG, 20, 23> {}; // Data Address Compare 1 Configuration 
		struct DAC2CFG : Field<DAC2CFG, 28, 31> {}; // Data Address Compare 2 Configuration 
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DVC1C, DVC2C, DAC1XMH, DAC2XMH, DAC1XM, DAC2XM, DAC1CFG, DAC2CFG> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DVC1C, DVC2C, DAC1XMH, DAC2XMH, DAC1XM, DAC2XM, DAC1CFG, DAC2CFG> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;

		DBCR4(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR4(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("dbcr4");            this->SetDescription("Debug Control Register 4");
			DVC1C  ::SetName("dvc1c");   DVC1C  ::SetDescription("Data Value Compare 1 Control");
			DVC2C  ::SetName("dvc2c");   DVC2C  ::SetDescription("Data Value Compare 2 Control");
			DAC1XMH::SetName("dac1xmh"); DAC1XMH::SetDescription("Data Address Compare 1 Extended Mask Control High");
			DAC2XMH::SetName("dac2xmh"); DAC2XMH::SetDescription("Data Address Compare 2 Extended Mask Control High");
			DAC1XM ::SetName("dac1xm");  DAC1XM ::SetDescription("Data Address Compare 1 Extended Mask Control");
			DAC2XM ::SetName("dac2xm");  DAC2XM ::SetDescription("Data Address Compare 2 Extended Mask Control");
			DAC1CFG::SetName("dac1cfg"); DAC1CFG::SetDescription("Data Address Compare 1 Configuration ");
			DAC2CFG::SetName("dac2cfg"); DAC2CFG::SetDescription("Data Address Compare 2 Configuration ");
		}
	};

	// Debug Control Register 5
	struct DBCR5 : PrivilegedSPR<DBCR5, 564>
	{
		typedef PrivilegedSPR<DBCR5, 564> Super;
		
		struct IAC5US : Field<IAC5US, 0, 1>   {}; // Instruction Address Compare 5 User/Supervisor Mode
		struct IAC5ER : Field<IAC5ER, 2, 3>   {}; // Instruction Address Compare 5 Effective/Real Mode
		struct IAC6US : Field<IAC6US, 4, 5>   {}; // Instruction Address Compare 6 User/Supervisor Mode
		struct IAC6ER : Field<IAC6ER, 6, 7>   {}; // Instruction Address Compare 6 Effective/Real Mode
		struct IAC56M : Field<IAC56M, 8, 9>   {}; // Instruction Address Compare 5/6 Mode
		struct IAC7US : Field<IAC7US, 16, 17> {}; // Instruction Address Compare 7 User/Supervisor Mode
		struct IAC7ER : Field<IAC7ER, 18, 19> {}; // Instruction Address Compare 7 Effective/Real Mode
		struct IAC8US : Field<IAC7ER, 20, 21> {}; // Instruction Address Compare 8 User/Supervisor Mode
		struct IAC8ER : Field<IAC8US, 22, 23> {}; // Instruction Address Compare 8 Effective/Real Mode
		struct IAC78M : Field<IAC78M, 24, 25> {}; // Instruction Address Compare 7/8 Mode
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IAC5US, IAC5ER, IAC6US, IAC6ER, IAC56M, IAC7US, IAC7ER, IAC7ER, IAC8US, IAC78M> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IAC5US, IAC5ER, IAC6US, IAC6ER, IAC56M, IAC7US, IAC7ER, IAC7ER, IAC8US, IAC78M> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR5(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR5(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("dbcr5");          this->SetDescription("Debug Control Register 5");
			IAC5US::SetName("iac5us"); IAC5US::SetDescription("Instruction Address Compare 5 User/Supervisor Mode");
			IAC5ER::SetName("iac5er"); IAC5ER::SetDescription("Instruction Address Compare 5 Effective/Real Mode");
			IAC6US::SetName("iac6us"); IAC6US::SetDescription("Instruction Address Compare 6 User/Supervisor Mode");
			IAC6ER::SetName("iac6er"); IAC6ER::SetDescription("Instruction Address Compare 6 Effective/Real Mode");
			IAC56M::SetName("iac56m"); IAC56M::SetDescription("Instruction Address Compare 5/6 Mode");
			IAC7US::SetName("iac7us"); IAC7US::SetDescription("Instruction Address Compare 7 User/Supervisor Mode");
			IAC7ER::SetName("iac7er"); IAC7ER::SetDescription("Instruction Address Compare 7 Effective/Real Mode");
			IAC8US::SetName("iac8us"); IAC8US::SetDescription("Instruction Address Compare 8 User/Supervisor Mode");
			IAC8ER::SetName("iac8er"); IAC7ER::SetDescription("Instruction Address Compare 8 Effective/Real Mode");
			IAC78M::SetName("iac78m"); IAC78M::SetDescription("Instruction Address Compare 7/8 Mode");
		}
	};

	// Instruction Address Compare 5
	struct IAC5 : PrivilegedSPR<IAC5, 565>
	{
		typedef PrivilegedSPR<IAC5, 565> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC5(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC5(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac5"); this->SetDescription("Instruction Address Compare 5"); }
	};

	// Instruction Address Compare 6
	struct IAC6 : PrivilegedSPR<IAC6, 566>
	{
		typedef PrivilegedSPR<IAC6, 566> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC6(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC6(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac6"); this->SetDescription("Instruction Address Compare 6"); }
	};

	// Instruction Address Compare 7
	struct IAC7 : PrivilegedSPR<IAC7, 567>
	{
		typedef PrivilegedSPR<IAC7, 567> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC7(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC7(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac7"); this->SetDescription("Instruction Address Compare 7"); }
	};

	// Instruction Address Compare 8
	struct IAC8 : PrivilegedSPR<IAC8, 568>
	{
		typedef PrivilegedSPR<IAC8, 568> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC8(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC8(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("iac8"); this->SetDescription("Instruction Address Compare 8"); }
	};

	// Machine Check Save/Restore Register 0
	struct MCSRR0 : PrivilegedSPR<MCSRR0, 570>
	{
		typedef PrivilegedSPR<MCSRR0, 570> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		MCSRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCSRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("mcsrr0"); this->SetDescription("Machine Check Save/Restore Register 0"); }
	};
	
	// Machine Check Save/Restore Register 1
	struct MCSRR1 : PrivilegedSPR<MCSRR1, 571>
	{
		typedef PrivilegedSPR<MCSRR1, 571> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		MCSRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCSRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("mcsrr1"); this->SetDescription("Machine Check Save/Restore Register 1"); }
	};
	
	// Machine Check Syndrome Register
	struct MCSR : PrivilegedSPR<MCSR, 572>
	{
		typedef PrivilegedSPR<MCSR, 572> Super;
		
		struct MCP         : Field<MCP        , 0>  {}; // Machine check input pin
		struct IC_DPERR    : Field<IC_DPERR   , 1>  {}; // Instruction Cache data array parity error
		struct DC_DPERR    : Field<DC_DPERR   , 3>  {}; // Data Cache data array parity error
		struct EXCP_ERR    : Field<EXCP_ERR   , 4>  {}; // ISI or Bus Error on first instruction fetch for an exception handler
		struct IC_TPERR    : Field<IC_TPERR   , 5>  {}; // Instruction Cache Tag parity error
		struct DC_TPERR    : Field<DC_TPERR   , 6>  {}; // Data Cache Tag parity error
		struct IC_LKERR    : Field<IC_LKERR   , 7>  {}; // Instruction Cache Lock error
		struct DC_LKERR    : Field<DC_LKERR   , 8>  {}; // Data Cache Lock error
		struct NMI         : Field<NMI        , 11> {}; // NMI input pin
		struct MAV         : Field<MAV        , 12> {}; // MCAR Address Valid
		struct MEA         : Field<MEA        , 13> {}; // MCAR holds Effective Address
		struct U           : Field<U          , 14> {}; // User
		struct IF          : Field<IF         , 15> {}; // Instruction fecth Error Report
		struct LD          : Field<LD         , 16> {}; // Load type instruction Error Report
		struct ST          : Field<ST         , 17> {}; // Store type instruction Error Report
		struct G           : Field<G          , 18> {}; // Guarded instruction Error Report
		struct STACK_ERR   : Field<STACK_ERR  , 21> {}; // Stack Access Limit Check Error
		struct IMEM_PERR   : Field<IMEM_PERR  , 22> {}; // Instruction Mem (IMEM) Parity Error
		struct DMEM_RDPERR : Field<DMEM_RDPERR, 23> {}; // Data Mem (DMEM) Parity Error
		struct DMEM_WRPERR : Field<DMEM_WRPERR, 24> {}; // Data Mem (DMEM) Write Parity Error
		struct BUS_IRERR   : Field<BUS_IRERR  , 27> {}; // Read bus error on Instruction recovery linefill
		struct BUS_DRERR   : Field<BUS_DRERR  , 28> {}; // Read bus error on data load or linefill
		struct BUS_WRERR   : Field<BUS_WRERR  , 29> {}; // Write bus error on store to bus or DMEM/imprecise write error
		struct BUS_WRDSI   : Field<BUS_WRDSI  , 30> {}; // Write bus error on buffered store to bus with DSI signaled. Set concurrently with BUS_WRERR for this case
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< MCP, IC_DPERR, DC_DPERR, EXCP_ERR, IC_TPERR
		                                                  , DC_TPERR, IC_LKERR, DC_LKERR, NMI, MAV, MEA
		                                                  , U, IF, LD, ST, G, STACK_ERR, IMEM_PERR
		                                                  , DMEM_RDPERR, DMEM_WRPERR, BUS_IRERR, BUS_DRERR
		                                                  , BUS_WRERR, BUS_WRDSI > ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< MCP, IC_DPERR, DC_DPERR, EXCP_ERR, IC_TPERR
		                                                  , DC_TPERR, IC_LKERR, DC_LKERR, NMI, MAV, MEA
		                                                  , U, IF, LD, ST, G, STACK_ERR, IMEM_PERR
		                                                  , DMEM_RDPERR, DMEM_WRPERR, BUS_IRERR, BUS_DRERR
		                                                  , BUS_WRERR, BUS_WRDSI > ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MCSR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
		
		virtual bool MoveTo(uint32_t value) { this->template Set<ALL>(this->template Get<ALL>() & ~value); return true; } // W1C
	
	private:
		void Init()
		{
			             this->SetName("mcsr");                     this->SetDescription("Machine Check Syndrome Register");
			MCP        ::SetName("mcp");         MCP        ::SetDescription("Machine check input pin");
			IC_DPERR   ::SetName("ic_dperr");    IC_DPERR   ::SetDescription("Instruction Cache data array parity error");
			DC_DPERR   ::SetName("dc_dperr");    DC_DPERR   ::SetDescription("Data Cache data array parity error");
			EXCP_ERR   ::SetName("excp_err");    EXCP_ERR   ::SetDescription("ISI or Bus Error on first instruction fetch for an exception handler");
			IC_TPERR   ::SetName("ic_tperr");    IC_TPERR   ::SetDescription("Instruction Cache Tag parity error");
			DC_TPERR   ::SetName("dc_tperr");    DC_TPERR   ::SetDescription("Data Cache Tag parity error");
			IC_LKERR   ::SetName("ic_lkerr");    IC_LKERR   ::SetDescription("Instruction Cache Lock error");
			DC_LKERR   ::SetName("dc_lkerr");    DC_LKERR   ::SetDescription("Data Cache Lock error");
			NMI        ::SetName("nmi");         NMI        ::SetDescription("NMI input pin");
			MAV        ::SetName("mav");         MAV        ::SetDescription("MCAR Address Valid");
			MEA        ::SetName("mea");         MEA        ::SetDescription("MCAR holds Effective Address");
			U          ::SetName("u");           U          ::SetDescription("User");
			IF         ::SetName("if");          IF         ::SetDescription("Instruction fecth Error Report");
			LD         ::SetName("ld");          LD         ::SetDescription("Load type instruction Error Report");
			ST         ::SetName("st");          ST         ::SetDescription("Store type instruction Error Report");
			G          ::SetName("g");           G          ::SetDescription("Guarded instruction Error Report");
			STACK_ERR  ::SetName("stack_err");   STACK_ERR  ::SetDescription("Stack Access Limit Check Error");
			IMEM_PERR  ::SetName("imem_perr");   IMEM_PERR  ::SetDescription("Instruction Mem (IMEM) Parity Error");
			DMEM_RDPERR::SetName("dmem_rdperr"); DMEM_RDPERR::SetDescription("Data Mem (DMEM) Parity Error");
			DMEM_WRPERR::SetName("dmem_wrperr"); DMEM_WRPERR::SetDescription("Data Mem (DMEM) Write Parity Error");
			BUS_IRERR  ::SetName("bus_irerr");   BUS_IRERR  ::SetDescription("Read bus error on Instruction recovery linefill");
			BUS_DRERR  ::SetName("bus_drerr");   BUS_DRERR  ::SetDescription("Read bus error on data load or linefill");
			BUS_WRERR  ::SetName("bus_wrerr");   BUS_WRERR  ::SetDescription("Write bus error on store to bus or DMEM/imprecise write error");
			BUS_WRDSI  ::SetName("bus_wrdsi");   BUS_WRDSI  ::SetDescription("Write bus error on buffered store to bus with DSI signaled. Set concurrently with BUS_WRERR for this case");
		}
	};

	// Machine Check Address Register
	struct MCAR : PrivilegedSPR<MCAR, 573>
	{
		typedef PrivilegedSPR<MCAR, 573> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		MCAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("mcar"); this->SetDescription("Machine Check Address Register"); }
	};
	
	// Debug Save/Restore Register 0
	struct DSRR0 : PrivilegedSPR<DSRR0, 574>
	{
		typedef PrivilegedSPR<DSRR0, 574> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DSRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DSRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("dsrr0"); this->SetDescription("Debug Save/Restore Register 0"); }
	};
	
	// Debug Save/Restore Register 1
	struct DSRR1 : PrivilegedSPR<DSRR1, 575>
	{
		typedef PrivilegedSPR<DSRR1, 575> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DSRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DSRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("dsrr1"); this->SetDescription("Debug Save/Restore Register 1"); }
	};
	
	// Debug Data Acquisition Messaging Register
	struct DDAM : NonPrivilegedSPR<DDAM, 576>
	{
		typedef NonPrivilegedSPR<DDAM, 576> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DDAM(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DDAM(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("ddam"); this->SetDescription("Debug Data Acquisition Messaging Register"); }
	};

	// Data Address Compare 3
	struct DAC3 : PrivilegedSPR<DAC3, 592>
	{
		typedef PrivilegedSPR<DAC3, 592> Super;
		
		struct ALL : Field<ALL, 0, 31> {};

		DAC3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("dac3"); this->SetDescription("Data Address Compare 3"); }
	};

	// Data Address Compare 4
	struct DAC4 : PrivilegedSPR<DAC4, 593>
	{
		typedef PrivilegedSPR<DAC4, 593> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DAC4(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC4(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("dac4"); this->SetDescription("Data Address Compare 4"); }
	};

	// Debug Control Register 7
	struct DBCR7 : PrivilegedSPR<DBCR7, 596>
	{
		typedef PrivilegedSPR<DBCR7, 596> Super;
		
		struct DAC3XMH : Field<DAC3XMH, 13>     {}; // Data Address Compare 3 Extended Mask Control High
		struct DAC4XMH : Field<DAC4XMH, 15>     {}; // Data Address Compare 4 Extended Mask Control High
		struct DAC3XM  : Field<DAC3XM , 16, 19> {}; // Data Address Compare 3 Extended Mask Control
		struct DAC4XM  : Field<DAC4XM , 20, 23> {}; // Data Address Compare 4 Extended Mask Control
		struct DAC3CFG : Field<DAC3CFG, 24, 27> {}; // Data Address Compare 3 Configuration
		struct DAC4CFG : Field<DAC4CFG, 28, 31> {}; // Data Address Compare 4 Configuration
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DAC3XMH, DAC4XMH, DAC3XM, DAC4XM, DAC3CFG, DAC4CFG> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DAC3XMH, DAC4XMH, DAC3XM, DAC4XM, DAC3CFG, DAC4CFG> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR7(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR7(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("dbcr7");            this->SetDescription("Debug Control Register 7");
			DAC3XMH::SetName("dac3xmh"); DAC3XMH::SetDescription("Data Address Compare 3 Extended Mask Control High");
			DAC4XMH::SetName("dac4xmh"); DAC4XMH::SetDescription("Data Address Compare 4 Extended Mask Control High");
			DAC3XM ::SetName("dac3xm");  DAC3XM ::SetDescription("Data Address Compare 3 Extended Mask Control");
			DAC4XM ::SetName("dac4xm");  DAC4XM ::SetDescription("Data Address Compare 4 Extended Mask Control");
			DAC3CFG::SetName("dac3cfg"); DAC3CFG::SetDescription("Data Address Compare 3 Configuration");
			DAC4CFG::SetName("dac4cfg"); DAC4CFG::SetDescription("Data Address Compare 4 Configuration");
		}
	};

	// Debug Control Register 8
	struct DBCR8 : PrivilegedSPR<DBCR8, 597>
	{
		typedef PrivilegedSPR<DBCR8, 597> Super;
		
		struct DAC3US  : Field<DAC3US , 0, 1> {}; // Data Address Compare 3 User/Supervisor Mode
		struct DAC3ER  : Field<DAC3ER , 2, 3> {}; // Data Address Compare 3 Effective/Real Mode
		struct DAC4US  : Field<DAC4US , 4, 5> {}; // Data Address Compare 4 User/Supervisor Mode
		struct DAC4ER  : Field<DAC4ER , 6, 7> {}; // Data Address Compare 4 Effective/Real Mode
		struct DAC34M  : Field<DAC34M , 8, 9> {}; // Data Address Compare 3/4 Mode
		struct DAC3LNK : Field<DAC3LNK, 10>   {}; // Data Address Compare 3 Linked
		struct DAC4LNK : Field<DAC4LNK, 11>   {}; // Data Address Compare 4 Linked
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DAC3US, DAC3ER, DAC4US, DAC4ER, DAC34M, DAC3LNK, DAC4LNK> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DAC3US, DAC3ER, DAC4US, DAC4ER, DAC34M, DAC3LNK, DAC4LNK> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR8(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR8(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("dbcr8");            this->SetDescription("Debug Control Register 8");
			DAC3US ::SetName("dac3us");  DAC3US ::SetDescription("Data Address Compare 3 User/Supervisor Mode");
			DAC3ER ::SetName("dac3er");  DAC3ER ::SetDescription("Data Address Compare 3 Effective/Real Mode");
			DAC4US ::SetName("dac4us");  DAC4US ::SetDescription("Data Address Compare 4 User/Supervisor Mode");
			DAC4ER ::SetName("dac4er");  DAC4ER ::SetDescription("Data Address Compare 4 Effective/Real Mode");
			DAC34M ::SetName("dac34m");  DAC34M ::SetDescription("Data Address Compare 3/4 Mode");
			DAC3LNK::SetName("dac3lnk"); DAC3LNK::SetDescription("Data Address Compare 3 Linked");
			DAC4LNK::SetName("dac4lnk"); DAC4LNK::SetDescription("Data Address Compare 4 Linked");
		}
	};

	// Debug Data Effective Address Register
	struct DDEAR : PrivilegedSPR<DDEAR, 600>
	{
		typedef PrivilegedSPR<DDEAR, 600> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DDEAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DDEAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("ddear"); this->SetDescription("Debug Data Effective Address Register"); }
	};
	
	// Data Value Compare 1 Upper
	struct DVC1U : PrivilegedSPR<DVC1U, 601>
	{
		typedef PrivilegedSPR<DVC1U, 601> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC1U(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC1U(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("dvc1u"); this->SetDescription("Data Value Compare 1 Upper"); }
	};

	// Data Value Compare 2 Upper
	struct DVC2U : PrivilegedSPR<DVC2U, 602>
	{
		typedef PrivilegedSPR<DVC2U, 602> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC2U(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC2U(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("dvc2u"); this->SetDescription("Data Value Compare 2 Upper"); }
	};

	// Debug Control Register 6
	struct DBCR6 : PrivilegedSPR<DBCR6, 603>
	{
		typedef PrivilegedSPR<DBCR6, 603> Super;
		
		struct IAC1XM : Field<IAC1XM, 0, 3>   {}; // Instruction Address Compare 1 Extended Mask Control
		struct IAC2XM : Field<IAC2XM, 4, 7>   {}; // Instruction Address Compare 2 Extended Mask Control
		struct IAC3XM : Field<IAC3XM, 8, 11>  {}; // Instruction Address Compare 3 Extended Mask Control
		struct IAC4XM : Field<IAC4XM, 12, 15> {}; // Instruction Address Compare 4 Extended Mask Control
		struct IAC5XM : Field<IAC5XM, 16, 19> {}; // Instruction Address Compare 5 Extended Mask Control
		struct IAC6XM : Field<IAC6XM, 20, 23> {}; // Instruction Address Compare 6 Extended Mask Control
		struct IAC7XM : Field<IAC7XM, 24, 27> {}; // Instruction Address Compare 7 Extended Mask Control
		struct IAC8XM : Field<IAC8XM, 28, 31> {}; // Instruction Address Compare 8 Extended Mask Control
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IAC1XM, IAC2XM, IAC3XM, IAC4XM, IAC5XM, IAC6XM, IAC7XM, IAC8XM> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IAC1XM, IAC2XM, IAC3XM, IAC4XM, IAC5XM, IAC6XM, IAC7XM, IAC8XM> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR6(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR6(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("dbcr6");          this->SetDescription("Debug Control Register 6");
			IAC1XM::SetName("iac1xm"); IAC1XM::SetDescription("Instruction Address Compare 1 Extended Mask Control");
			IAC2XM::SetName("iac2xm"); IAC2XM::SetDescription("Instruction Address Compare 2 Extended Mask Control");
			IAC3XM::SetName("iac3xm"); IAC3XM::SetDescription("Instruction Address Compare 3 Extended Mask Control");
			IAC4XM::SetName("iac4xm"); IAC4XM::SetDescription("Instruction Address Compare 4 Extended Mask Control");
			IAC5XM::SetName("iac5xm"); IAC5XM::SetDescription("Instruction Address Compare 5 Extended Mask Control");
			IAC6XM::SetName("iac6xm"); IAC6XM::SetDescription("Instruction Address Compare 6 Extended Mask Control");
			IAC7XM::SetName("iac7xm"); IAC7XM::SetDescription("Instruction Address Compare 7 Extended Mask Control");
			IAC8XM::SetName("iac8xm"); IAC8XM::SetDescription("Instruction Address Compare 8 Extended Mask Control");
		}
	};

	// L1 Cache Control and Status Register 2
	struct L1CSR2 : PrivilegedSPR<L1CSR2, 606>
	{
		typedef PrivilegedSPR<L1CSR2, 606> Super;
		
		struct STGC : Field<STGC, 2, 3> {}; // Store Gather Control
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<STGC> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<STGC> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CSR2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CSR2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			      this->SetName("l1csr2");       this->SetDescription("L1 Cache Control and Status Register 2");
			STGC::SetName("stgc");   STGC::SetDescription("Store Gather Control");
		}
	};
	
	// MPU Assist Register 0
	struct MAS0 : PrivilegedSPR<MAS0, 624>
	{
		typedef PrivilegedSPR<MAS0, 624> Super;
		
		struct VALID : Field<VALID, 0>      {}; // MPU Entry Valid
		struct IPROT : Field<IPROT, 1>      {}; // Invalidation Protect
		struct SEL   : Field<SEL  , 2, 3>   {}; // Selects MPU for access
		struct RO    : Field<RO   , 5>      {}; // Read-Only
		struct DEBUG : Field<DEBUG, 6>      {}; // Debug Control for Entry
		struct INST  : Field<INST , 7>      {}; // Instruction Entry
		struct SHD   : Field<SHD  , 8>      {}; // Shared Entry Select
		struct ESEL  : Field<ESEL , 12, 15> {}; // Entry select for MPU
		struct UAMSK : Field<UAMSK, 17, 19> {}; // Upper Address Mask Control
		struct UW    : Field<UW   , 20>     {}; // User Mode Write Permission
		struct SW    : Field<SW   , 21>     {}; // Supervisor Mode Write / Read Permission
		struct UX_UR : Field<UX_UR, 22>     {}; // User Mode Execute / Read Permission
		struct SX_SR : Field<SX_SR, 23>     {}; // Supervisor Mode Execute / Read Permission
		struct IOVR  : Field<IOVR , 24>     {}; // Cache-Inihibit attribute Override
		struct GOVR  : Field<GOVR , 25>     {}; // G attribute Override
		struct I     : Field<I    , 28>     {}; // Cache Inhibited
		struct G     : Field<G    , 30>     {}; // Guarded
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<VALID, IPROT, SEL, RO, DEBUG, INST, SHD, ESEL, UAMSK, UW, SW, UX_UR, SX_SR, IOVR, GOVR, I, G> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<VALID, IPROT, SEL, RO, DEBUG, INST, SHD, ESEL, UAMSK, UW, SW, UX_UR, SX_SR, IOVR, GOVR, I, G> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset()
		{
			this->template Set<RO>(0);
		}
	private:
		void Init()
		{
			       this->SetName("mas0");         this->SetDescription("MPU Assist Register 0");
			VALID::SetName("valid"); VALID::SetDescription("MPU Entry Valid");
			IPROT::SetName("iprot"); IPROT::SetDescription("Invalidation Protect");
			SEL  ::SetName("sel");   SEL  ::SetDescription("Selects MPU for access");
			RO   ::SetName("ro");    RO   ::SetDescription("Read-Only");
			DEBUG::SetName("debug"); DEBUG::SetDescription("Debug Control for Entry");
			INST ::SetName("inst");  INST ::SetDescription("Instruction Entry");
			SHD  ::SetName("shd");   SHD  ::SetDescription("Shared Entry Select");
			ESEL ::SetName("esel");  ESEL ::SetDescription("Entry select for MPU");
			UAMSK::SetName("uamsk"); UAMSK::SetDescription("Upper Address Mask Control");
			UW   ::SetName("uw");    UW   ::SetDescription("User Mode Write Permission");
			SW   ::SetName("sw");    SW   ::SetDescription("Supervisor Mode Write / Read Permission");
			UX_UR::SetName("ux_ur"); UX_UR::SetDescription("User Mode Execute / Read Permission");
			SX_SR::SetName("sx_sr"); SX_SR::SetDescription("Supervisor Mode Execute / Read Permission");
			IOVR ::SetName("iovr");  IOVR ::SetDescription("Cache-Inihibit attribute Override");
			GOVR ::SetName("govr");  GOVR ::SetDescription("G attribute Override");
			I    ::SetName("i");     I    ::SetDescription("Cache Inhibited");
			G    ::SetName("g");     G    ::SetDescription("Guarded");
		}
	};
	
	// MPU Assist Register 1
	struct MAS1 : PrivilegedSPR<MAS1, 625>
	{
		typedef PrivilegedSPR<MAS1, 625> Super;
		
		struct TID    : Field<TID   , 8, 15>  {}; // Region ID bits
		struct TIDMSK : Field<TIDMSK, 24, 31> {}; // Region ID mask
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<TID, TIDMSK> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<TID, TIDMSK> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			        this->SetName("mas1");           this->SetDescription("MPU Assist Register 1");
			TID   ::SetName("tid");    TID   ::SetDescription("Region ID bits");
			TIDMSK::SetName("tidmsk"); TIDMSK::SetDescription("Region ID mask");
		}
	};
	
	// MPU Assist Register 2
	struct MAS2 : PrivilegedSPR<MAS2, 626>
	{
		typedef PrivilegedSPR<MAS2, 626> Super;
		
		struct UPPER_BOUND : Field<UPPER_BOUND, 0, 31> {}; // Upper bound of address range covered by entry
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<UPPER_BOUND> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<UPPER_BOUND> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			             this->SetName("mas2");                     this->SetDescription("MPU Assist Register 2");
			UPPER_BOUND::SetName("upper_bound"); UPPER_BOUND::SetDescription("Upper bound of address range covered by entry");
		}
	};
	
	// MPU Assist Register 3
	struct MAS3 : PrivilegedSPR<MAS3, 627>
	{
		typedef PrivilegedSPR<MAS3, 627> Super;
		
		struct LOWER_BOUND : Field<LOWER_BOUND, 0, 31> {}; // Lower bound of address range covered by entry
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<LOWER_BOUND> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<LOWER_BOUND> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			             this->SetName("mas3");                     this->SetDescription("MPU Assist Register 3");
			LOWER_BOUND::SetName("lower_bound"); LOWER_BOUND::SetDescription("Lower bound of address range covered by entry");
		}
	};
	
	// MAS4 ????

	// External Debug Resource Allocation Control Register 0
	struct EDBRAC0 : ReadOnlyPrivilegedSPR<EDBRAC0, 638>
	{
		typedef ReadOnlyPrivilegedSPR<EDBRAC0, 638> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		EDBRAC0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		EDBRAC0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("edbrac0"); this->SetDescription("External Debug Resource Allocation Control Register 0"); }
	};
	
	// MPU0 Configuration Register
	struct MPU0CFG : ReadOnlyPrivilegedSPR<MPU0CFG, 692>
	{
		typedef ReadOnlyPrivilegedSPR<MPU0CFG, 692> Super;
		
		struct FASSOC  : Field<FASSOC , 0>      {}; // Fully Associative
		struct MINSIZE : Field<MINSIZE, 8, 11>  {}; // Minimum Region Size
		struct MAXSIZE : Field<MAXSIZE, 12, 15> {}; // Maximum Region Size
		struct IPROT   : Field<IPROT  , 16>     {}; // Invalidate Protect Capability
		struct UAMSKA  : Field<UAMSKA , 18>     {}; // Upper Address Masking Availability
		struct SHENTRY : Field<SHENTRY, 23, 25> {}; // Number of Shared (configurable for I or D) Entries
		struct DENTRY  : Field<DENTRY , 26, 28> {}; // Number of Data Entries
		struct IENTRY  : Field<IENTRY , 29, 31> {}; // Number of Instruction Entries
		
		template <Model _MODEL, bool dummy = true> struct IMPLEMENTATION {};
		template <bool dummy> struct IMPLEMENTATION<E200Z710N3, dummy>
		{
			typedef FieldSet<FASSOC, MINSIZE, MAXSIZE, IPROT, UAMSKA, SHENTRY, DENTRY, IENTRY> ALL;
		};
		template <bool dummy> struct IMPLEMENTATION<E200Z425BN3, dummy>
		{
			typedef FieldSet<FASSOC, MINSIZE, MAXSIZE, IPROT, UAMSKA, SHENTRY, DENTRY, IENTRY> ALL;
		};

		typedef typename IMPLEMENTATION<CONFIG::MODEL>::ALL ALL;
		
		MPU0CFG(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MPU0CFG(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("mpu0cfg");          this->SetDescription("MPU0 Configuration Register");
			FASSOC ::SetName("fassoc");  FASSOC ::SetDescription("Fully Associative");
			MINSIZE::SetName("minsize"); MINSIZE::SetDescription("Minimum Region Size");
			MAXSIZE::SetName("maxsize"); MAXSIZE::SetDescription("Maximum Region Size");
			IPROT  ::SetName("iprot");   IPROT  ::SetDescription("Invalidate Protect Capability");
			UAMSKA ::SetName("uamska");  UAMSKA ::SetDescription("Upper Address Masking Availability");
			SHENTRY::SetName("shentry"); SHENTRY::SetDescription("Number of Shared (configurable for I or D) Entries");
			DENTRY ::SetName("dentry");  DENTRY ::SetDescription("Number of Data Entries");
			IENTRY ::SetName("ientry");  IENTRY ::SetDescription("Number of Instruction Entries");
		}
	};

	// DMEM Configuration Register 0
	struct DMEMCFG0 : ReadOnlyPrivilegedSPR<DMEMCFG0, 694>
	{
		typedef ReadOnlyPrivilegedSPR<DMEMCFG0, 694> Super;
		
		struct DMEM_BASE_ADDR : Field<DMEM_BASE_ADDR, 0, 19>  {}; // DMEM BASE ADDRESS (CPU Port)
		struct DECUA          : Field<DECUA         , 21>     {}; // DMEM Error Correction Update Available
		struct DECA           : Field<DECA          , 22>     {}; // DMEM Error Correction Available
		struct DMSIZE         : Field<DMSIZE        , 24, 29> {}; // DMEM Size
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DMEM_BASE_ADDR, DECUA, DECA, DMSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DMEM_BASE_ADDR, DECUA, DECA, DMSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DMEMCFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DMEMCFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("dmemcfg0");                       this->SetDescription("DMEM Configuration Register 0");
			DMEM_BASE_ADDR::SetName("dmem_base_addr"); DMEM_BASE_ADDR::SetDescription("DMEM BASE ADDRESS (CPU Port)");
			DECUA         ::SetName("decua");          DECUA         ::SetDescription("DMEM Error Correction Update Available");
			DECA          ::SetName("deca");           DECA          ::SetDescription("DMEM Error Correction Available");
			DMSIZE        ::SetName("dmsize");         DMSIZE        ::SetDescription("DMEM Size");
		}
	};

	// IMEM Configuration Register 0
	struct IMEMCFG0 : PrivilegedSPR<IMEMCFG0, 695>
	{
		typedef PrivilegedSPR<IMEMCFG0, 695> Super;
		
		struct IMEM_BASE_ADDR : Field<IMEM_BASE_ADDR, 0, 19>  {}; // IMEM BASE ADDRESS (CPU Port)
		struct IECUA          : Field<IECUA         , 21>     {}; // IMEM Error Correction Update Available
		struct IECA           : Field<IECA          , 22>     {}; // IMEM Error Correction Available
		struct IMSIZE         : Field<IMSIZE        , 24, 29> {}; // IMEM Size
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IMEM_BASE_ADDR, IECUA, IECA, IMSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IMEM_BASE_ADDR, IECUA, IECA, IMSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IMEMCFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IMEMCFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("imemcfg0");                       this->SetDescription("IMEM Configuration Register 0");
			IMEM_BASE_ADDR::SetName("imem_base_addr"); IMEM_BASE_ADDR::SetDescription("IMEM BASE ADDRESS (CPU Port)");
			IECUA         ::SetName("iecua");          IECUA         ::SetDescription("IMEM Error Correction Update Available");
			IECA          ::SetName("ieca");           IECA          ::SetDescription("IMEM Error Correction Available");
			IMSIZE        ::SetName("imsize");         IMSIZE        ::SetDescription("IMEM Size");
		}
	};

	// L1 Flush and Invalidate Control Register 1
	struct L1FINV1 : PrivilegedSPR<L1FINV1, 959>
	{
		typedef PrivilegedSPR<L1FINV1, 959> Super;
		
		struct CWAY : Field<CWAY, 7>      {}; // Cache Way
		struct CSET : Field<CSET, 21, 26> {}; // Cache Set
		struct CCMD : Field<CCMD, 29, 31> {}; // Cache Command
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;

		L1FINV1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1FINV1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			      this->SetName("l1finv1");    this->SetDescription("L1 Flush and Invalidate Control Register 1");
			CWAY::SetName("cway"); CWAY::SetDescription("Cache Way");
			CSET::SetName("cset"); CSET::SetDescription("Cache Set");
			CCMD::SetName("ccmd"); CCMD::SetDescription("Cache Command");
		}
	};
	
	// Debug Event Register
	struct DEVENT : NonPrivilegedSPR<DEVENT, 975>
	{
		typedef NonPrivilegedSPR<DEVENT, 975> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DEVENT(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DEVENT(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("devent"); this->SetDescription("Debug Event Register"); }
	};
	
	// System Information
	struct SIR : ReadOnlyPrivilegedSPR<SIR, 992>
	{
		typedef ReadOnlyPrivilegedSPR<SIR, 992> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SIR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("sir"); this->SetDescription("System Information"); }
	};

	// Hardware Implementation Dependent Register 0
	struct HID0 : PrivilegedSPR<HID0, 1008>
	{
		typedef PrivilegedSPR<HID0, 1008> Super;
		
		struct EMCP    : Field<EMCP   , 0>  {}; // Enable machine check pin (p_mcp_b)
		struct ICR     : Field<ICR    , 14> {}; // Input Inputs Clear Reservation
		struct NHR     : Field<NHR    , 15> {}; // Not hardware reset
		struct DCLREE  : Field<DCLREE , 19> {}; // Debug Interrupt Clears MSR[EE]
		struct DCLRCE  : Field<DCLRCE , 20> {}; // Debug Interrupt Clears MSR[CE]
		struct CICLRDE : Field<CICLRDE, 21> {}; // Critical Interrupt Clears MSR[DE]
		struct MCCLRDE : Field<MCCLRDE, 22> {}; // Machine Check Interrupt Clears MSR[DE]
		struct NOPTI   : Field<NOPTI  , 31> {}; // No-op Touch Instructions
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<EMCP, ICR, NHR, DCLREE, DCLRCE, CICLRDE, MCCLRDE, NOPTI> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<EMCP, ICR, NHR, DCLREE, DCLRCE, CICLRDE, MCCLRDE, NOPTI> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		HID0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		HID0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{      
			   this->SetName("hid0");       this->SetDescription("Hardware Implementation Dependent Register 0");
			EMCP   ::SetName("emcp");    EMCP   ::SetDescription("Enable machine check pin (p_mcp_b)");
			ICR    ::SetName("icr");     ICR    ::SetDescription("Input Inputs Clear Reservation");
			NHR    ::SetName("nhr");     NHR    ::SetDescription("Not hardware reset");
			DCLREE ::SetName("dclree");  DCLREE ::SetDescription("Debug Interrupt Clears MSR[EE]");
			DCLRCE ::SetName("dclrce");  DCLRCE ::SetDescription("Debug Interrupt Clears MSR[CE]");
			CICLRDE::SetName("ciclrde"); CICLRDE::SetDescription("Critical Interrupt Clears MSR[DE]");
			MCCLRDE::SetName("mcclrde"); MCCLRDE::SetDescription("Machine Check Interrupt Clears MSR[DE]");
			NOPTI  ::SetName("nopti");   NOPTI  ::SetDescription("No-op Touch Instructions");
		}
	};

	// Hardware Implementation Dependent Register 1
	struct HID1 : PrivilegedSPR<HID1, 1009>
	{
		typedef PrivilegedSPR<HID1, 1009> Super;
		
		struct HP_NOR : Field<HP_NOR, 22> {}; // High priority elevation for normal and external interrupts
		struct HP_NMI : Field<HP_NMI, 23> {}; // High priority elevation for NMI and critical interrupts
		struct ATS    : Field<ATS   , 24> {}; // Atomic status
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<HP_NOR, HP_NMI, ATS> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<HP_NOR, HP_NMI, ATS> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		HID1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		HID1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("hid1");           this->SetDescription("Hardware Implementation Dependent Register 1");
			HP_NOR::SetName("hp_nor"); HP_NOR::SetDescription("High priority elevation for normal and external interrupts");
			HP_NMI::SetName("hp_nmi"); HP_NMI::SetDescription("High priority elevation for NMI and critical interrupts");
			ATS   ::SetName("ats");    ATS   ::SetDescription("Atomic status");
		}
	};

	// L1 Cache Control and Status Register 0
	struct L1CSR0 : PrivilegedSPR<L1CSR0, 1010>
	{
		typedef PrivilegedSPR<L1CSR0, 1010> Super;
		
		struct WID     : Field<WID    , 0, 1>   {}; // Way Instruction Disable
		struct WDD     : Field<WDD    , 4, 5>   {}; // Way Data Disable
		struct DCWA    : Field<DCWA   , 12>     {}; // Data Cache Write Allocation Policy
		struct DCECE   : Field<DCECE  , 15>     {}; // Data Cache Error Checking Enable
		struct DCEI    : Field<DCEI   , 16>     {}; // Data Cache Error Injection
		struct DCLOC   : Field<DCLOC  , 17, 18> {}; // Data Cache Lockout Control
		struct DCEA    : Field<DCEA   , 25, 26> {}; // Data Cache Error Action
		struct DCLOINV : Field<DCLOINV, 27>     {}; // Data Cache Lockout Indicator Invalidate
		struct DCABT   : Field<DCABT  , 29>     {}; // Data Cache Operation Aborted
		struct DCINV   : Field<DCINV  , 30>     {}; // Data Cache Invalidate
		struct DCE     : Field<DCE    , 31>     {}; // Data Cache Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<WID, WDD, DCWA, DCECE, DCEI, DCLOC, DCEA, DCLOINV, DCABT, DCINV, DCE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<WID> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CSR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CSR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			         this->SetName("l1csr0");           this->SetDescription("L1 Cache Control and Status Register 0");
			WID    ::SetName("wid");     WID    ::SetDescription("Way Instruction Disable");
			WDD    ::SetName("wdd");     WDD    ::SetDescription("Way Data Disable");
			DCWA   ::SetName("dcwa");    DCWA   ::SetDescription("Data Cache Write Allocation Policy");
			DCECE  ::SetName("dcece");   DCECE  ::SetDescription("Data Cache Error Checking Enable");
			DCEI   ::SetName("dcei");    DCEI   ::SetDescription("Data Cache Error Injection");
			DCLOC  ::SetName("dcloc");   DCLOC  ::SetDescription("Data Cache Lockout Control");
			DCEA   ::SetName("dcea");    DCEA   ::SetDescription("Data Cache Error Action");
			DCLOINV::SetName("dcloinv"); DCLOINV::SetDescription("Data Cache Lockout Indicator Invalidate");
			DCABT  ::SetName("dcabt");   DCABT  ::SetDescription("Data Cache Operation Aborted");
			DCINV  ::SetName("dcinv");   DCINV  ::SetDescription("Data Cache Invalidate");
			DCE    ::SetName("dce");     DCE    ::SetDescription("Data Cache Enable");
		}
	};

	// L1 Cache Control and Status Register 1
	struct L1CSR1 : PrivilegedSPR<L1CSR1, 1011>
	{
		typedef PrivilegedSPR<L1CSR1, 1011> Super;
		
		struct ICECE   : Field<ICECE  , 15>     {}; // Instruction Cache Error Checking Enable
		struct ICEI    : Field<ICEI   , 16>     {}; // Instruction Cache Error Injection Enable
		struct ICLOC   : Field<ICLOC  , 17, 18> {}; // Instruction Cache Lockout Control
		struct ICEA    : Field<ICEA   , 25, 26> {}; // Instruction Cache Error Action
		struct ICLOINV : Field<ICLOINV, 27>     {}; // Instruction Cache Lockout Indicator Invalidate
		struct ICABT   : Field<ICABT  , 29>     {}; // Instruction Cache Operation Aborted
		struct ICINV   : Field<ICINV  , 30>     {}; // Instruction Cache Invalidate
		struct ICE     : Field<ICE    , 31>     {}; // Instruction Cache Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<ICECE, ICEI, ICLOC, ICEA, ICLOINV, ICABT, ICINV, ICE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ICECE, ICEI, ICLOC, ICEA, ICLOINV, ICABT, ICINV, ICE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CSR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CSR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			         this->SetName("l1csr1");           this->SetDescription("L1 Cache Control and Status Register 1");
			ICECE  ::SetName("icece");   ICECE  ::SetDescription("Instruction Cache Error Checking Enable");
			ICEI   ::SetName("icei");    ICEI   ::SetDescription("Instruction Cache Error Injection Enable");
			ICLOC  ::SetName("icloc");   ICLOC  ::SetDescription("Instruction Cache Lockout Control");
			ICEA   ::SetName("icea");    ICEA   ::SetDescription("Instruction Cache Error Action");
			ICLOINV::SetName("icloinv"); ICLOINV::SetDescription("Instruction Cache Lockout Indicator Invalidate");
			ICABT  ::SetName("icabt");   ICABT  ::SetDescription("Instruction Cache Operation Aborted");
			ICINV  ::SetName("icinv");   ICINV  ::SetDescription("Instruction Cache Invalidate");
			ICE    ::SetName("ice");     ICE    ::SetDescription("Instruction Cache Enable");
		}
	};

	// Branch Unit Control and Status Register
	struct BUCSR : PrivilegedSPR<BUCSR, 1013>
	{
		typedef PrivilegedSPR<BUCSR, 1013> Super;
		
		struct BBFI   : Field<BBFI  , 22>     {}; // Branch target buffer flash Invalidate
		struct BALLOC : Field<BALLOC, 26, 27> {}; // Branch Target Buffer Allocation Control
		struct BPRED  : Field<BPRED , 29, 30> {}; // Branch Prediction Control (Static)
		struct BPEN   : Field<BPEN  , 31>     {}; // Branch target buffer prediction enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<BBFI, BALLOC, BPRED, BPEN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<BBFI, BALLOC, BPRED, BPEN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		BUCSR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		BUCSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			        this->SetName("bucsr");          this->SetDescription("Branch Unit Control and Status Register");
			BBFI  ::SetName("bbfi");   BBFI  ::SetDescription("Branch target buffer flash Invalidate");
			BALLOC::SetName("balloc"); BALLOC::SetDescription("Branch Target Buffer Allocation Control");
			BPRED ::SetName("bpred");  BPRED ::SetDescription("Branch Prediction Control (Static)");
			BPEN  ::SetName("bpen");   BPEN  ::SetDescription("Branch target buffer prediction enable");
		}
	};

	// MPU0 Control and Status Register 0
	struct MPU0CSR0 : PrivilegedSPR<MPU0CSR0, 1014>
	{
		typedef PrivilegedSPR<MPU0CSR0, 1014> Super;
		
		struct BYPSR  : Field<BYPSR , 16> {}; // Bypass MPU protection for Supervisor Read accesses
		struct BYPSW  : Field<BYPSW , 17> {}; // Bypass MPU protection for Supervisor Write accesses
		struct BYPSX  : Field<BYPSX , 18> {}; // Bypass MPU protection for Supervisor Instruction accesses
		struct BYPUR  : Field<BYPUR , 19> {}; // Bypass MPU protection for User Read accesses
		struct BYPUW  : Field<BYPUW , 20> {}; // Bypass MPU protection for User Write accesses
		struct BYPUX  : Field<BYPUX , 21> {}; // Bypass MPU protection for User Instruction accesses
		struct DRDEND : Field<DRDEND, 24> {}; // Data Read Debug Enable
		struct DWDEN  : Field<DWDEN , 25> {}; // Data Write Debug Enable
		struct IDEN   : Field<IDEN  , 26> {}; // Instruction Debug Enable
		struct TIDCTL : Field<TIDCTL, 28> {}; // TID usage control
		struct MPUFI  : Field<MPUFI , 30> {}; // MPU flash invalidate
		struct MPUEN  : Field<MPUEN , 31> {}; // MPU Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<BYPSR, BYPSW, BYPSX, BYPUR, BYPUW, BYPUX, DRDEND, DWDEN, IDEN, TIDCTL, MPUFI, MPUEN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<BYPSR, BYPSW, BYPSX, BYPUR, BYPUW, BYPUX, DRDEND, DWDEN, IDEN, TIDCTL, MPUFI, MPUEN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MPU0CSR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MPU0CSR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			        this->SetName("mpu0csr0");       this->SetDescription("MPU0 Control and Status Register 0");
			BYPSR ::SetName("bypsr");  BYPSR ::SetDescription("Bypass MPU protection for Supervisor Read accesses");
			BYPSW ::SetName("bypsw");  BYPSW ::SetDescription("Bypass MPU protection for Supervisor Write accesses");
			BYPSX ::SetName("bypsx");  BYPSX ::SetDescription("Bypass MPU protection for Supervisor Instruction accesses");
			BYPUR ::SetName("bypur");  BYPUR ::SetDescription("Bypass MPU protection for User Read accesses");
			BYPUW ::SetName("bypuw");  BYPUW ::SetDescription("Bypass MPU protection for User Write accesses");
			BYPUX ::SetName("bypux");  BYPUX ::SetDescription("Bypass MPU protection for User Instruction accesses");
			DRDEND::SetName("drdend"); DRDEND::SetDescription("Data Read Debug Enable");
			DWDEN ::SetName("dwden");  DWDEN ::SetDescription("Data Write Debug Enable");
			IDEN  ::SetName("iden");   IDEN  ::SetDescription("Instruction Debug Enable");
			TIDCTL::SetName("tidctl"); TIDCTL::SetDescription("TID usage control");
			MPUFI ::SetName("mpufi");  MPUFI ::SetDescription("MPU flash invalidate");
			MPUEN ::SetName("mpuen");  MPUEN ::SetDescription("MPU Enable");
		}
	};

	// MMU/MPU Configuration Register
	struct MMUCFG : ReadOnlyPrivilegedSPR<MMUCFG, 1015>
	{
		typedef ReadOnlyPrivilegedSPR<MMUCFG, 1015> Super;
		
		struct RASIZE  : Field<RASIZE , 8, 14>  {}; // Number of Bits of Real Address supported
		struct PIDSIZE : Field<PIDSIZE, 21, 25> {}; // PID Register Size
		struct NMPUS   : Field<NMPUS  , 26, 27> {}; // Number of MPUs
		struct NTLBS   : Field<NTLBS  , 28, 29> {}; // Number of TLBs
		struct MAVN    : Field<MAVN   , 30, 31> {}; // MMU Architecture Version Number
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<RASIZE, PIDSIZE, NMPUS, NTLBS, MAVN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<RASIZE, PIDSIZE, NMPUS, NTLBS, MAVN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MMUCFG(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MMUCFG(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("mmucfg");           this->SetDescription("MMU/MPU Configuration Register");
			RASIZE ::SetName("rasize");  RASIZE ::SetDescription("Number of Bits of Real Address supported");
			PIDSIZE::SetName("pidsize"); PIDSIZE::SetDescription("PID Register Size");
			NMPUS  ::SetName("nmpus");   NMPUS  ::SetDescription("Number of MPUs");
			NTLBS  ::SetName("ntlbs");   NTLBS  ::SetDescription("Number of TLBs");
			MAVN   ::SetName("mavn");    MAVN   ::SetDescription("MMU Architecture Version Number");
		}
	};

	// L1 Flush and Invalidate Control Register 0
	struct L1FINV0 : PrivilegedSPR<L1FINV0, 1016>
	{
		typedef PrivilegedSPR<L1FINV0, 1016> Super;
		
		struct CWAY : Field<CWAY, 7>      {}; // Cache Way
		struct CSET : Field<CSET, 21, 26> {}; // Cache Set
		struct CCMD : Field<CCMD, 29, 31> {}; // Cache Command
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1FINV0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1FINV0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			      this->SetName("l1finv0");    this->SetDescription("L1 Flush and Invalidate Control Register 0");
			CWAY::SetName("cway"); CWAY::SetDescription("CWAY");
			CSET::SetName("cset"); CSET::SetDescription("CSET");
			CCMD::SetName("ccmd"); CCMD::SetDescription("CCMD");
		}
	};

	// System Version Register
	struct SVR : ReadOnlyPrivilegedSPR<SVR, 1023>
	{
		typedef ReadOnlyPrivilegedSPR<SVR, 1023> Super;
		
		struct System_Version : Field<System_Version, 0, 31> {};
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<System_Version> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<System_Version> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		SVR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SVR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("svr");
			this->SetDescription("System Version Register");
			System_Version::SetName("system_version");
		}
	};
	
	////////////////////////// Device Control Registers ///////////////////////

	// DMEM Control Register 0
	struct DMEMCTL0 : PrivilegedDCR<DMEMCTL0, 496>
	{
		typedef PrivilegedDCR<DMEMCTL0, 496> Super;
		
		struct DMEM_BASE_ADDR : Field<DMEM_BASE_ADDR, 0, 15>  {}; // DMEM BASE ADDRESS Field (CPU Port)
		struct DBYPCB         : Field<DBYPCB        , 19>     {}; // DMEM Bypass Cache Bypass CPU accesses
		struct DBYPAT         : Field<DBYPAT        , 20>     {}; // DMEM Bypass Atomic CPU accesses
		struct DBYPDEC        : Field<DBYPDEC       , 21>     {}; // DMEM Bypass Decorated CPU accesses
		struct DECUE          : Field<DECUE         , 22>     {}; // DMEM Error Correction Update Enable
		struct DBAPD          : Field<DBAPD         , 24>     {}; // DMEM Base Address Port Disable
		struct DPEIE          : Field<DPEIE         , 25>     {}; // DMEM Processor Error Injection Enable
		struct DICWE          : Field<DICWE         , 26>     {}; // DMEM Imprecise CPU Write Enable
		struct DSWCE          : Field<DSWCE         , 27, 28> {}; // DMEM Slave port Write Check/Correct Enable
		struct DDAUEC         : Field<DDAUEC        , 29>     {}; // DMEM Disable Address Use in Error Check
		struct DCPECE         : Field<DCPECE        , 30>     {}; // DMEM CPU Port ECC Enabled (CPU Port)
		struct DSECE          : Field<DSECE         , 31>     {}; // DMEM Slave port Error Checking Enable (Slave port)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DMEM_BASE_ADDR, DBYPCB, DBYPAT, DBYPDEC, DECUE, DBAPD, DPEIE, DICWE, DSWCE, DDAUEC, DCPECE, DSECE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DMEM_BASE_ADDR, DBYPCB, DBYPAT, DBYPDEC, DECUE, DBAPD, DPEIE, DICWE, DSWCE, DDAUEC, DCPECE, DSECE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DMEMCTL0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DMEMCTL0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("dmemctl0");                       this->SetDescription("DMEM Control Register 0");
			DMEM_BASE_ADDR::SetName("dmem_base_addr"); DMEM_BASE_ADDR::SetDescription("DMEM BASE ADDRESS Field (CPU Port)");
			DBYPCB        ::SetName("dbypcb");         DBYPCB        ::SetDescription("DMEM Bypass Cache Bypass CPU accesses");
			DBYPAT        ::SetName("dbypat");         DBYPAT        ::SetDescription("DMEM Bypass Atomic CPU accesses");
			DBYPDEC       ::SetName("dbypdec");        DBYPDEC       ::SetDescription("DMEM Bypass Decorated CPU accesses");
			DECUE         ::SetName("decue");          DECUE         ::SetDescription("DMEM Error Correction Update Enable");
			DBAPD         ::SetName("dbapd");          DBAPD         ::SetDescription("DMEM Base Address Port Disable");
			DPEIE         ::SetName("dpeie");          DPEIE         ::SetDescription("DMEM Processor Error Injection Enable");
			DICWE         ::SetName("dicwe");          DICWE         ::SetDescription("DMEM Imprecise CPU Write Enable");
			DSWCE         ::SetName("dswce");          DSWCE         ::SetDescription("DMEM Slave port Write Check/Correct Enable");
			DDAUEC        ::SetName("ddauec");         DDAUEC        ::SetDescription("DMEM Disable Address Use in Error Check");
			DCPECE        ::SetName("dcpece");         DCPECE        ::SetDescription("DMEM CPU Port ECC Enabled (CPU Port)");
			DSECE         ::SetName("dsece");          DSECE         ::SetDescription("DMEM Slave port Error Checking Enable (Slave port)");
		}
	};
	
	// DMEM Control Register 1
	struct DMEMCTL1 : PrivilegedDCR<DMEMCTL1, 498>
	{
		typedef PrivilegedDCR<DMEMCTL1, 498> Super;
		
		struct DSWAC3 : Field<DSWAC3, 0, 1>   {}; // DMEM Supervisor Write Access Control for Quadrant 3
		struct DSWAC2 : Field<DSWAC2, 2, 3>   {}; // DMEM Supervisor Write Access Control for Quadrant 2
		struct DSWAC1 : Field<DSWAC1, 4, 5>   {}; // DMEM Supervisor Write Access Control for Quadrant 1
		struct DSWAC0 : Field<DSWAC0, 6, 7>   {}; // DMEM Supervisor Write Access Control for Quadrant 0
		struct DSRAC3 : Field<DSRAC3, 8, 9>   {}; // DMEM Supervisor Read Access Control for Quadrant 3
		struct DSRAC2 : Field<DSRAC2, 10, 11> {}; // DMEM Supervisor Read Access Control for Quadrant 2
		struct DSRAC1 : Field<DSRAC1, 12, 13> {}; // DMEM Supervisor Read Access Control for Quadrant 1
		struct DSRAC0 : Field<DSRAC0, 14, 15> {}; // DMEM Supervisor Read Access Control for Quadrant 0
		struct DUWAC3 : Field<DUWAC3, 16, 17> {}; // DMEM User Write Access Control for Quadrant 3
		struct DUWAC2 : Field<DUWAC2, 18, 19> {}; // DMEM User Write Access Control for Quadrant 2
		struct DUWAC1 : Field<DUWAC1, 20, 21> {}; // DMEM User Write Access Control for Quadrant 1
		struct DUWAC0 : Field<DUWAC0, 22, 23> {}; // DMEM User Write Access Control for Quadrant 0
		struct DURAC3 : Field<DURAC3, 24, 25> {}; // DMEM User Read Access Control for Quadrant 3
		struct DURAC2 : Field<DURAC2, 26, 27> {}; // DMEM User Read Access Control for Quadrant 2
		struct DURAC1 : Field<DURAC1, 28, 29> {}; // DMEM User Read Access Control for Quadrant 1
		struct DURAC0 : Field<DURAC0, 30, 31> {}; // DMEM User Read Access Control for Quadrant 0
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DSWAC3, DSWAC2, DSWAC1, DSWAC0, DSRAC3, DSRAC2, DSRAC1, DSRAC0, DUWAC3, DUWAC2, DUWAC1, DUWAC0, DURAC3, DURAC2, DURAC1, DURAC0> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DSWAC3, DSWAC2, DSWAC1, DSWAC0, DSRAC3, DSRAC2, DSRAC1, DSRAC0, DUWAC3, DUWAC2, DUWAC1, DUWAC0, DURAC3, DURAC2, DURAC1, DURAC0> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DMEMCTL1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DMEMCTL1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x0); }
		
	private:
		void Init()
		{
			        this->SetName("dmemctl1");       this->SetDescription("DMEM Control Register 1");
			DSWAC3::SetName("dswac3"); DSWAC3::SetDescription("DMEM Supervisor Write Access Control for Quadrant 3");
			DSWAC2::SetName("dswac2"); DSWAC2::SetDescription("DMEM Supervisor Write Access Control for Quadrant 2");
			DSWAC1::SetName("dswac1"); DSWAC1::SetDescription("DMEM Supervisor Write Access Control for Quadrant 1");
			DSWAC0::SetName("dswac0"); DSWAC0::SetDescription("DMEM Supervisor Write Access Control for Quadrant 0");
			DSRAC3::SetName("dsrac3"); DSRAC3::SetDescription("DMEM Supervisor Read Access Control for Quadrant 3");
			DSRAC2::SetName("dsrac2"); DSRAC2::SetDescription("DMEM Supervisor Read Access Control for Quadrant 2");
			DSRAC1::SetName("dsrac1"); DSRAC1::SetDescription("DMEM Supervisor Read Access Control for Quadrant 1");
			DSRAC0::SetName("dsrac0"); DSRAC0::SetDescription("DMEM Supervisor Read Access Control for Quadrant 0");
			DUWAC3::SetName("duwac3"); DUWAC3::SetDescription("DMEM User Write Access Control for Quadrant 3");
			DUWAC2::SetName("duwac2"); DUWAC2::SetDescription("DMEM User Write Access Control for Quadrant 2");
			DUWAC1::SetName("duwac1"); DUWAC1::SetDescription("DMEM User Write Access Control for Quadrant 1");
			DUWAC0::SetName("duwac0"); DUWAC0::SetDescription("DMEM User Write Access Control for Quadrant 0");
			DURAC3::SetName("durac3"); DURAC3::SetDescription("DMEM User Read Access Control for Quadrant 3");
			DURAC2::SetName("durac2"); DURAC2::SetDescription("DMEM User Read Access Control for Quadrant 2");
			DURAC1::SetName("durac1"); DURAC1::SetDescription("DMEM User Read Access Control for Quadrant 1");
			DURAC0::SetName("durac0"); DURAC0::SetDescription("DMEM User Read Access Control for Quadrant 0");
		}
	};
	
	// IMEM Control Register 0
	struct IMEMCTL0 : PrivilegedDCR<IMEMCTL0, 497>
	{
		typedef PrivilegedDCR<IMEMCTL0, 497> Super;
		
		struct IMEM_BASE_ADDR : Field<IMEM_BASE_ADDR, 0, 17>  {}; // IMEM BASE ADDRESS Field (CPU Port)
		struct IECUE          : Field<IECUE         , 22>     {}; // IMEM Error Correction Update Enable
		struct IBAPD          : Field<IBAPD         , 24>     {}; // IMEM Base Address Port Disable
		struct ISWCE          : Field<ISWCE         , 27, 28> {}; // IMEM Slave port Write Check/Correct Enable
		struct IDAUEC         : Field<IDAUEC        , 29>     {}; // IMEM Disable Address Use in Error Check
		struct ICPECE         : Field<ICPECE        , 30>     {}; // IMEM CPU Port ECC Enable (CPU Port)
		struct ISECE          : Field<ISECE         , 31>     {}; // IMEM Slave port Error Checking Enable (Slave port)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IMEM_BASE_ADDR, IECUE, IBAPD, ISWCE, IDAUEC, ICPECE, ISECE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IMEM_BASE_ADDR, IECUE, IBAPD, ISWCE, IDAUEC, ICPECE, ISECE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IMEMCTL0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IMEMCTL0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("imemctl0");                       this->SetDescription("IMEM Control Register 0");
			IMEM_BASE_ADDR::SetName("imem_base_addr"); IMEM_BASE_ADDR::SetDescription("IMEM BASE ADDRESS Field (CPU Port)");
			IECUE         ::SetName("iecue");          IECUE         ::SetDescription("IMEM Error Correction Update Enable");
			IBAPD         ::SetName("ibapd");          IBAPD         ::SetDescription("IMEM Base Address Port Disable");
			ISWCE         ::SetName("iswce");          ISWCE         ::SetDescription("IMEM Slave port Write Check/Correct Enable");
			IDAUEC        ::SetName("idauec");         IDAUEC        ::SetDescription("IMEM Disable Address Use in Error Check");
			ICPECE        ::SetName("icpece");         ICPECE        ::SetDescription("IMEM CPU Port ECC Enable (CPU Port)");
			ISECE         ::SetName("isece");          ISECE         ::SetDescription("IMEM Slave port Error Checking Enable (Slave port)");
		}
	};
	
	// IMEM Control Register 1
	struct IMEMCTL1 : PrivilegedDCR<IMEMCTL1, 499>
	{
		typedef PrivilegedDCR<IMEMCTL1, 499> Super;
		
		struct ISXAC3 : Field<ISXAC3, 8, 9>   {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 3
		struct ISXAC2 : Field<ISXAC2, 10, 11> {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 2
		struct ISXAC1 : Field<ISXAC1, 12, 13> {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 1
		struct ISXAC0 : Field<ISXAC0, 14, 15> {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 0
		struct IUXAC3 : Field<IUXAC3, 24, 25> {}; // IMEM User Instruction Fetch Access Control for Quadrant 3
		struct IUXAC2 : Field<IUXAC2, 26, 27> {}; // IMEM User Instruction Fetch Access Control for Quadrant 2
		struct IUXAC1 : Field<IUXAC1, 28, 29> {}; // IMEM User Instruction Fetch Access Control for Quadrant 1
		struct IUXAC0 : Field<IUXAC0, 30, 31> {}; // IMEM User Instruction Fetch Access Control for Quadrant 0
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<ISXAC3, ISXAC2, ISXAC1, ISXAC0, IUXAC3, IUXAC2, IUXAC1, IUXAC0> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ISXAC3, ISXAC2, ISXAC1, ISXAC0, IUXAC3, IUXAC2, IUXAC1, IUXAC0> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IMEMCTL1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IMEMCTL1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x0); }

	private:
		void Init()
		{
			        this->SetName("imemctl1");       this->SetDescription("IMEM Control Register 1");
			ISXAC3::SetName("isxac3"); ISXAC3::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 3");
			ISXAC2::SetName("isxac2"); ISXAC2::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 2");
			ISXAC1::SetName("isxac1"); ISXAC1::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 1");
			ISXAC0::SetName("isxac0"); ISXAC0::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 0");
			IUXAC3::SetName("iuxac3"); IUXAC3::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 3");
			IUXAC2::SetName("iuxac2"); IUXAC2::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 2");
			IUXAC1::SetName("iuxac1"); IUXAC1::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 1");
			IUXAC0::SetName("iuxac0"); IUXAC0::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 0");
		}
	};
	
	// End-to-End ECC Control Register 0
	struct E2ECTL0 : PrivilegedDCR<E2ECTL0, 510>
	{
		typedef PrivilegedDCR<E2ECTL0, 510> Super;
		
		struct IDAC   : Field<IDAC  , 28> {}; // Instruction Disable Address Checking
		struct IECCEN : Field<IECCEN, 29> {}; // Instruction ECC Enable Field
		struct DDAC   : Field<DDAC  , 30> {}; // Data Disable Address Checking
		struct DECCEN : Field<DECCEN, 31> {}; // Data ECC Enable Field
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IDAC, IECCEN, DDAC, DECCEN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IDAC, IECCEN, DDAC, DECCEN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		E2ECTL0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		E2ECTL0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("e2ectl0");        this->SetDescription("End-to-End ECC Control Register 0");
			IDAC  ::SetName("idac");   IDAC  ::SetDescription("Instruction Disable Address Checking");
			IECCEN::SetName("ieccen"); IECCEN::SetDescription("Instruction ECC Enable Field");
			DDAC  ::SetName("ddac");   DDAC  ::SetDescription("Data Disable Address Checking");
			DECCEN::SetName("deccen"); DECCEN::SetDescription("Data ECC Enable Field");
		}
	};
	
	// End-to-End ECC Error Control/Status Register 0
	struct E2EECSR0 : PrivilegedDCR<E2EECSR0, 511>
	{
		typedef PrivilegedDCR<E2EECSR0, 511> Super;
		
		struct RCHKBIT         : Field<RCHKBIT        , 4, 11>  {}; // Read Checkbits
		struct WRC             : Field<WRC            , 16, 17> {}; // Write Control
		struct INVC            : Field<INVC           , 18, 19> {}; // Invert Control
		struct WCHKBIT_CHKINVT : Field<WCHKBIT_CHKINVT, 24, 31> {}; // Write Checkbits / Checkbit Invert Mask
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<RCHKBIT, WRC, INVC, WCHKBIT_CHKINVT> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<RCHKBIT, WRC, INVC, WCHKBIT_CHKINVT> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		E2EECSR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		E2EECSR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			           this->SetName("e2eecsr0");                          this->SetDescription("End-to-End ECC Error Control/Status Register 0");
			RCHKBIT        ::SetName("rchkbit");                RCHKBIT        ::SetDescription("Read Checkbits");
			WRC            ::SetName("wrc");                    WRC            ::SetDescription("Write Control");
			INVC           ::SetName("invc");                   INVC           ::SetDescription("Invert Control");
			WCHKBIT_CHKINVT::SetName("wchkbit_chkinvt");        WCHKBIT_CHKINVT::SetDescription("Write Checkbits / Checkbit Invert Mask");
			WCHKBIT_CHKINVT::SetDisplayName("WCHKBIT/CHKINVT"); 
		}
	};

	// Nexus 3 DCRs are voluntary missing
	
	/////////////////////// Performance Monitor Registers /////////////////////
	
	//  Performance Monitor Counter registers 0-3
	template <unsigned int PMC_NUM>
	struct PMC : PrivilegedPMR<PMC<PMC_NUM>, 16 + PMC_NUM>
	{
		typedef PrivilegedPMR<PMC<PMC_NUM>, 16 + PMC_NUM> Super;
		
		struct OV            : Field<OV           , 0> {};      // Overflow
		struct Counter_Value : Field<Counter_Value, 1, 31> {};  // Indicates the number of occurrences of the specified event
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<OV, Counter_Value> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<OV, Counter_Value> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PMC(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PMC(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			std::stringstream name_sstr;
			name_sstr << "pmc" << PMC_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Performance Monitor Counter register" << PMC_NUM;
			
			               this->SetName(name_sstr.str());                         this->SetDescription(desc_sstr.str());
			OV           ::SetName("ov");            OV           ::SetDescription("Overflow");
			Counter_Value::SetName("counter_value"); Counter_Value::SetDescription("Indicates the number of occurrences of the specified event");
		}
	};
	
	typedef PMC<0> PMC0;
	typedef PMC<1> PMC1;
	typedef PMC<2> PMC2;
	typedef PMC<3> PMC3;

	// Performance Monitor Global Control Register 0
	struct PMGC0 : PrivilegedPMR<PMGC0, 400>
	{
		typedef PrivilegedPMR<PMGC0, 400> Super;
		
		struct FAC   : Field<FAC  , 0>  {}; // Freeze All Counters
		struct PMIE  : Field<PMIE , 1>  {}; // Performance monitor interrupt enable
		struct FCECE : Field<FCECE, 2>  {}; // Freeze Counters on Enabled Condition or Event
		struct TBSEL : Field<TBSEL, 19> {}; // Time Base Selector
		struct TBEE  : Field<TBEE , 23> {}; // Time base transition Event Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<FAC, PMIE, FCECE, TBSEL, TBEE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<FAC, PMIE, FCECE, TBSEL, TBEE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PMGC0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PMGC0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			       this->SetName("pmgc0");        this->SetDescription("Performance Monitor Global Control Register 0");
			FAC  ::SetName("fac");   FAC  ::SetDescription("Freeze All Counters");
			PMIE ::SetName("pmie");  PMIE ::SetDescription("Performance monitor interrupt enable");
			FCECE::SetName("fcece"); FCECE::SetDescription("Freeze Counters on Enabled Condition or Event");
			TBSEL::SetName("tbsel"); TBSEL::SetDescription("Time Base Selector");
			TBEE ::SetName("tbee");  TBEE ::SetDescription("Time base transition Event Enable");
		}
	};
	
	// Performance monitor local control A(0-3)
	template <unsigned int PMLCA_NUM>
	struct PMLCa : PrivilegedPMR<PMLCa<PMLCA_NUM>, 144 + PMLCA_NUM>
	{
		typedef PrivilegedPMR<PMLCa<PMLCA_NUM>, 144 + PMLCA_NUM> Super;
		
		struct FC    : Field<FC   , 0>     {}; // Freeze Counter
		struct FCS   : Field<FCS  , 1>     {}; // Freeze Counter in Supervisor state
		struct FCU   : Field<FCU  , 2>     {}; // Freeze Counter in User state
		struct FCM1  : Field<FCM1 , 3>     {}; // Freeze Counter while Mark is set
		struct FCM0  : Field<FCM0 , 4>     {}; // Freeze Counter while Mark is cleared
		struct CE    : Field<CE   , 5>     {}; // Condition Enable
		struct EVENT : Field<EVENT, 8,15>  {}; // Event selector
		struct PMP   : Field<PMP  , 17,19> {}; // Performance Monitor Watchpoint Periodicity Select
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<FC, FCS, FCU, FCM1, FCM0, CE, EVENT, PMP> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<FC, FCS, FCU, FCM1, FCM0, CE, EVENT, PMP> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PMLCa(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PMLCa(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			std::stringstream name_sstr;
			name_sstr << "pmlca" << PMLCA_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Performance monitor local control A" << PMLCA_NUM;
			
			this->SetName(name_sstr.str()); this->SetDescription(desc_sstr.str());
			
			FC   ::SetName("fc");     FC   ::SetDescription("Freeze Counter");
			FCS  ::SetName("fcs");    FCS  ::SetDescription("Freeze Counter in Supervisor state");
			FCU  ::SetName("fcu");    FCU  ::SetDescription("Freeze Counter in User state");
			FCM1 ::SetName("fcm1");   FCM1 ::SetDescription("Freeze Counter while Mark is set"); 
			FCM0 ::SetName("fcm0");   FCM0 ::SetDescription("Freeze Counter while Mark is cleared"); 
			CE   ::SetName("ce");     CE   ::SetDescription("Condition Enable");
			EVENT::SetName("event");  EVENT::SetDescription("Event selector");  
			PMP  ::SetName("pmp");    PMP  ::SetDescription("Performance Monitor Watchpoint Periodicity Select");
		}
	};
	
	typedef PMLCa<0> PMLCa0;
	typedef PMLCa<1> PMLCa1;
	typedef PMLCa<2> PMLCa2;
	typedef PMLCa<3> PMLCa3;
	
	// Performance monitor local control B(0-3)
	template <unsigned int PMLCB_NUM>
	struct PMLCb : PrivilegedPMR<PMLCb<PMLCB_NUM>, 272 + PMLCB_NUM>
	{
		typedef PrivilegedPMR<PMLCb<PMLCB_NUM>, 272 + PMLCB_NUM> Super;
		
		struct TRIGONCTL  : Field<TRIGONCTL , 1,3>   {}; // Trigger-on Control Class - Class of Trigger-on source
		struct TRIGOFFCTL : Field<TRIGOFFCTL, 5,7>   {}; // Trigger-off Control Class - Class of Trigger-off source
		struct TRIGONSEL  : Field<TRIGONSEL , 9,12>  {}; // Trigger-on Source Select - Source Select based on setting of TRIGONCTL
		struct TRIGOFFSEL : Field<TRIGOFFSEL, 14,17> {}; // Trigger-off Source Select - Source Select based on setting of TRIGOFFCTL
		struct TRIGGERED  : Field<TRIGGERED , 18>    {}; // Triggered
		struct THRESHMUL  : Field<THRESHMUL , 21,23> {}; // Threshold multiple
		struct THRESHOLD  : Field<THRESHOLD , 26,31> {}; // Threshold
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<TRIGONCTL, TRIGOFFCTL, TRIGONSEL, TRIGOFFSEL, TRIGGERED, THRESHMUL, THRESHOLD> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<TRIGONCTL, TRIGOFFCTL, TRIGONSEL, TRIGOFFSEL, TRIGGERED, THRESHMUL, THRESHOLD> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PMLCb(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PMLCb(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			std::stringstream name_sstr;
			name_sstr << "pmlcb" << PMLCB_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Performance monitor local control B" << PMLCB_NUM;
			
			this->SetName(name_sstr.str()); this->SetDescription(desc_sstr.str());
			
			TRIGONCTL ::SetName("TRIGONCTL");  TRIGONCTL ::SetDescription("Trigger-on Control Class - Class of Trigger-on source");
			TRIGOFFCTL::SetName("TRIGOFFCTL"); TRIGOFFCTL::SetDescription("Trigger-off Control Class - Class of Trigger-off source");  
			TRIGONSEL ::SetName("TRIGONSEL");  TRIGONSEL ::SetDescription("Trigger-on Source Select - Source Select based on setting of TRIGONCTL"); 
			TRIGOFFSEL::SetName("TRIGOFFSEL"); TRIGOFFSEL::SetDescription("Trigger-off Source Select - Source Select based on setting of TRIGOFFCTL");  
			TRIGGERED ::SetName("TRIGGERED");  TRIGGERED ::SetDescription("Triggered"); 
			THRESHMUL ::SetName("THRESHMUL");  THRESHMUL ::SetDescription("Threshold multiple"); 
			THRESHOLD ::SetName("THRESHOLD");  THRESHOLD ::SetDescription("Threshold"); 
		}
	};
	
	typedef PMLCb<0> PMLCb0;
	typedef PMLCb<1> PMLCb1;
	typedef PMLCb<2> PMLCb2;
	typedef PMLCb<3> PMLCb3;

	// User Performance Monitor Counter registers 0-3
	template <unsigned int PMC_NUM>
	struct UPMC : ReadOnlyUPMR<PMC<PMC_NUM>, 0 + PMC_NUM>
	{
		typedef ReadOnlyUPMR<PMC<PMC_NUM>, 0 + PMC_NUM> Super;
		
		UPMC(typename CONFIG::CPU *_cpu, PMC<PMC_NUM> *pmc) : Super(_cpu, pmc) {}
	};
	
	typedef UPMC<0> UPMC0;
	typedef UPMC<1> UPMC1;
	typedef UPMC<2> UPMC2;
	typedef UPMC<3> UPMC3;
	
	// User Performance Monitor Global Control Register 0
	struct UPMGC0 : ReadOnlyUPMR<PMGC0, 384>
	{
		typedef ReadOnlyUPMR<PMGC0, 384> Super;
		
		UPMGC0(typename CONFIG::CPU *_cpu, PMGC0 *pmgc0) : Super(_cpu, pmgc0) {}
	};

	//  User performance monitor local control A(0-3)
	template <unsigned int PMLCA_NUM>
	struct UPMLCa : ReadOnlyUPMR<PMLCa<PMLCA_NUM>, 128 + PMLCA_NUM>
	{
		typedef ReadOnlyUPMR<PMLCa<PMLCA_NUM>, 128 + PMLCA_NUM> Super;
		
		UPMLCa(typename CONFIG::CPU *_cpu, PMLCa<PMLCA_NUM> *pmlca) : Super(_cpu, pmlca) {}
	};
	
	typedef UPMLCa<0> UPMLCa0;
	typedef UPMLCa<1> UPMLCa1;
	typedef UPMLCa<2> UPMLCa2;
	typedef UPMLCa<3> UPMLCa3;

	//  User performance monitor local control B(0-3)
	template <unsigned int PMLCB_NUM>
	struct UPMLCb : ReadOnlyUPMR<PMLCb<PMLCB_NUM>, 256 + PMLCB_NUM>
	{
		typedef ReadOnlyUPMR<PMLCb<PMLCB_NUM>, 256 + PMLCB_NUM> Super;
		
		UPMLCb(typename CONFIG::CPU *_cpu, PMLCb<PMLCB_NUM> *pmlca) : Super(_cpu, pmlca) {}
	};
	
	typedef UPMLCb<0> UPMLCb0;
	typedef UPMLCb<1> UPMLCb1;
	typedef UPMLCb<2> UPMLCb2;
	typedef UPMLCb<3> UPMLCb3;

	//////////////////////// Thread Management Registers //////////////////////
	
	// Thread Management Configuration Register 0
	struct TMCFG0 : ReadOnlyPrivilegedTMR<TMCFG0, 16>
	{
		typedef ReadOnlyPrivilegedTMR<TMCFG0, 16> Super;
		
		struct NPRIBITS : Field<NPRIBITS, 10,15> {}; // Number of bits of thread priority implemented
		struct NATHRD   : Field<NATHRD  , 18,23> {}; // Number of active threads implemented
		struct NTHRD    : Field<NTHRD   , 26,31> {}; // Number of threads implemented
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<NPRIBITS, NATHRD, NTHRD> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<NPRIBITS, NATHRD, NTHRD> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		TMCFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		TMCFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			          this->SetName("tmcfg0");             this->SetDescription("Thread Management Configuration Register 0");
			NPRIBITS::SetName("npribits"); NPRIBITS::SetDescription("Number of bits of thread priority implemented");
			NATHRD  ::SetName("nathrd");   NATHRD  ::SetDescription("Number of active threads implemented");
			NTHRD   ::SetName("nthrd");    NTHRD   ::SetDescription("Number of threads implemented");
		}
	};

	
	/////////////////////// General Purpose Register //////////////////////////
	
	struct GPR : unisim::util::reg::core::Register<GPR, 32>
	{
		typedef unisim::util::reg::core::Register<GPR, 32> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		GPR() : Super() {}
		GPR(uint32_t _value) : Super(_value) {}
		void Init(unsigned int reg_num)
		{
			std::stringstream sstr;
			sstr << "r" << reg_num;
			this->SetName(sstr.str());
			this->SetDescription("General Purpose Register");
		}
		
		using Super::operator =;
		
	};
	
	///////////////////////////// Logger //////////////////////////////////////
	
	unisim::kernel::logger::Logger logger;
	
	/////////// unisim::service::interfaces::MemoryAccessReporting<> //////////
	
    bool requires_memory_access_reporting;        // indicates if the memory accesses require to be reported
    bool requires_finished_instruction_reporting; // indicates if the finished instructions require to be reported
	
	inline void MonitorLoad(typename TYPES::ADDRESS ea, unsigned int size);
	inline void MonitorStore(typename TYPES::ADDRESS ea, unsigned int size);
	
	////////////////////////// Run-time parameters ////////////////////////////

	uint64_t instruction_counter;
	unisim::kernel::service::Parameter<uint64_t> param_instruction_counter;
	
	uint64_t trap_on_instruction_counter;
	unisim::kernel::service::Parameter<uint64_t> param_trap_on_instruction_counter;
	
	uint64_t max_inst;
	unisim::kernel::service::Parameter<uint64_t> param_max_inst;
	
	typename TYPES::ADDRESS halt_on_addr;
	std::string halt_on;
	unisim::kernel::service::Parameter<std::string> param_halt_on;
	
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> param_verbose_setup;

	bool verbose_exception;
	unisim::kernel::service::Parameter<bool> param_verbose_exception;

	bool verbose_interrupt;
	unisim::kernel::service::Parameter<bool> param_verbose_interrupt;
	
	bool verbose_move_to_slr;
	unisim::kernel::service::Parameter<bool> param_verbose_move_to_slr;

	bool verbose_move_from_slr;
	unisim::kernel::service::Parameter<bool> param_verbose_move_from_slr;

private:
	/////////////////////////// Registers Registry ////////////////////////////

	std::map<std::string, unisim::service::interfaces::Register *> registers_registry;       // Every CPU register interfaces

	void AddRegisterInterface(unisim::service::interfaces::Register *reg_if);

	///////////////////////// Exception dispatcher ////////////////////////////
	
	ExceptionDispatcher<CONFIG::NUM_EXCEPTIONS> exception_dispatcher;
	
	//////////////////////// System Level Registers ///////////////////////////
	
	InvalidSLR<SLR_SPR_SPACE> invalid_spr;
	InvalidSLR<SLR_TBR_SPACE> invalid_tbr;
	InvalidSLR<SLR_PMR_SPACE> invalid_pmr;
	InvalidSLR<SLR_TMR_SPACE> invalid_tmr;
	ExternalDCR external_dcr;
	SLRBase *slr[SLR_TMR_SPACE - SLR_SPR_SPACE + 1][1024];
	
protected:
	//////////////////////////// Reset Address ////////////////////////////////
	
	typename TYPES::ADDRESS reset_addr;
	unisim::kernel::service::Parameter<typename TYPES::ADDRESS> param_reset_addr;
	
	/////////////////////////// Program Counter ///////////////////////////////
	
	typename TYPES::ADDRESS cia;
	typename TYPES::ADDRESS nia;
	
	////////////////////// General Purpose Registers //////////////////////////
	
	GPR gpr[32];

	////////////////////// Special Purpose Registers //////////////////////////
	
	XER xer;
	LR lr;
	CTR ctr;
	
	////////////////////////// Condition Register  ////////////////////////////
	
	CR cr;
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__
