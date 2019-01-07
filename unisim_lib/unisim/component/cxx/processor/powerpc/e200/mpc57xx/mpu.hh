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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_MPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_MPU_HH__

#include <inttypes.h>
#include <iostream>

#include <unisim/util/cache/cache.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

struct MPU_ENTRY
{
	MPU_ENTRY() : mas0(0), mas1(0), mas2(0), mas3(0) {}
	
	uint32_t mas0;
	uint32_t mas1;
	uint32_t mas2;
	uint32_t mas3;
};

template <typename TYPES, typename CONFIG>
struct MPU
	: unisim::kernel::service::Object
	, unisim::util::cache::AccessController<TYPES, MPU<TYPES, CONFIG> >
{
	typedef typename CONFIG::CPU CPU;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	typedef typename CPU::MAS1 MAS1;
	typedef typename CPU::MAS2 MAS2;
	typedef typename CPU::MAS3 MAS3;
	typedef typename CPU::PID0 PID0;
	typedef typename CPU::MSR MSR;
	static const unsigned int NUM_INST_MPU_ENTRIES = CONFIG::NUM_INST_MPU_ENTRIES;
	static const unsigned int NUM_DATA_MPU_ENTRIES = CONFIG::NUM_DATA_MPU_ENTRIES;
	static const unsigned int NUM_SHARED_MPU_ENTRIES = CONFIG::NUM_SHARED_MPU_ENTRIES;
	
	MPU(CPU *cpu, unsigned int sel);
	void WriteEntry();
	void ReadEntry();
	template <bool force> void Invalidate();
	MPU_ENTRY *Lookup(ADDRESS addr, bool exec, bool write);
	template <bool DEBUG> inline bool ControlAccess(unisim::util::cache::AccessControl<TYPES>& access_control) ALWAYS_INLINE;
	void DumpEntry(std::ostream& os, MPU_ENTRY *mpu_entry);
	void Dump(std::ostream& os);
protected:
	struct MAS0 : CPU::MAS0
	{
		typedef typename CPU::MAS0 Super;
		
		MAS0(CPU *_cpu) : Super(_cpu) {}
		MAS0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->Initialize(Super::VLE::template GetMask<uint32_t>() | Super::W::template GetMask<uint32_t>()); // VLE and W sticked to 1
		}
		
		using Super::operator =;
	};
	
	struct MMUCFG : CPU::MMUCFG
	{
		typedef typename CPU::MMUCFG Super;
		
		MMUCFG(CPU *_cpu) : Super(_cpu) {}
		MMUCFG(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->template Set<typename CPU::MMUCFG::RASIZE>(32);
			this->template Set<typename CPU::MMUCFG::PIDSIZE>(PID0::Process_ID::GetBitWidth() - 1);
			this->template Set<typename CPU::MMUCFG::NMPUS>(1);
			this->template Set<typename CPU::MMUCFG::NTLBS>(0);
			this->template Set<typename CPU::MMUCFG::MAVN>(3);
		}
	};
	
	struct MPU0CFG : CPU::MPU0CFG
	{
		typedef typename CPU::MPU0CFG Super;
		
		MPU0CFG(CPU *_cpu) : Super(_cpu) {}
		MPU0CFG(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->template Set<typename CPU::MPU0CFG::FASSOC>(1);
			this->template Set<typename CPU::MPU0CFG::MINSIZE>(0);   // Smallest region size is 1 byte; Note: Due to the use of access address matching, the effective smallest region size is 8 bytes
			this->template Set<typename CPU::MPU0CFG::MAXSIZE>(0xb); // Largest region size is 4 GB
			this->template Set<typename CPU::MPU0CFG::IPROT>(1);
			this->template Set<typename CPU::MPU0CFG::UAMSKA>(1);
			this->template Set<typename CPU::MPU0CFG::SHENTRY>(0x2);
			this->template Set<typename CPU::MPU0CFG::DENTRY>(0x4);
			this->template Set<typename CPU::MPU0CFG::IENTRY>(0x2);
		}
	};
	
	struct MPU0CSR0 : CPU::MPU0CSR0
	{
		typedef typename CPU::MPU0CSR0 Super;
		
		MPU0CSR0(CPU *_cpu, MPU *_mpu) : Super(_cpu), mpu(_mpu) {}
		MPU0CSR0(CPU *_cpu, MPU *_mpu, uint32_t _value) : Super(_cpu, _value), mpu(_mpu) {}
		
		void Effect()
		{
			if(this->template Get<typename CPU::MPU0CSR0::MPUFI>())
			{
				mpu->Invalidate</* force */ false>();
				this->template Set<typename CPU::MPU0CSR0::MPUFI>(0);
			}
		}
		
		MPU0CSR0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	private:
		MPU *mpu;
	};
private:
	bool Match(MPU_ENTRY *mpu_entry, ADDRESS addr, bool exec, bool write);
	
	CPU *cpu;
	unsigned int sel;
	MAS0 mas0;
	MAS1 mas1;
	MAS2 mas2;
	MAS3 mas3;
	MPU0CFG mpu0cfg;
	MPU0CSR0 mpu0csr0;
	MMUCFG mmucfg;
	MPU_ENTRY *mru_inst_mpu_entry;
	MPU_ENTRY *mru_data_mpu_entry;
	MPU_ENTRY *mru_shd_mpu_entry;
	MPU_ENTRY hole_mpu_entry;
	
	MPU_ENTRY inst_mpu_entries[NUM_INST_MPU_ENTRIES];
	MPU_ENTRY data_mpu_entries[NUM_DATA_MPU_ENTRIES];
	MPU_ENTRY shd_mpu_entries[NUM_SHARED_MPU_ENTRIES];
};

template <typename TYPES, typename CONFIG>
MPU<TYPES, CONFIG>::MPU(CPU *_cpu, unsigned int _sel)
	: unisim::kernel::service::Object("MPU", _cpu)
	, cpu(_cpu)
	, sel(_sel)
	, mas0(_cpu)
	, mas1(_cpu)
	, mas2(_cpu)
	, mas3(_cpu)
	, mpu0cfg(_cpu)
	, mpu0csr0(_cpu, this)
	, mmucfg(_cpu)
	, mru_inst_mpu_entry(0)
	, mru_data_mpu_entry(0)
	, mru_shd_mpu_entry(0)
	, hole_mpu_entry()
{
	Invalidate</* force */ true>();
	MAS0::VALID::Set(hole_mpu_entry.mas0, 1U); // Valid
	MAS0::I::Set(hole_mpu_entry.mas0, 0U); // *NOT* cache inhibited
	MAS0::G::Set(hole_mpu_entry.mas0, 0U); // *NOT* guarded
	MAS2::UPPER_BOUND::Set(hole_mpu_entry.mas2, ~ADDRESS(0));
	MAS3::LOWER_BOUND::Set(hole_mpu_entry.mas3, 0U);
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::WriteEntry()
{
	unsigned int sel = mas0.template Get<typename MAS0::SEL>();
	
	if(sel == mas0.template Get<typename MAS0::SEL>())
	{
		unsigned int esel = mas0.template Get<typename MAS0::ESEL>();
		unsigned int inst = mas0.template Get<typename MAS0::INST>();
		unsigned int shd = mas0.template Get<typename MAS0::SHD>();
		
		MPU_ENTRY *mpu_entry = 0;
		
		if(shd)
		{
			// shared entry
			mpu_entry = (esel < NUM_SHARED_MPU_ENTRIES) ? &shd_mpu_entries[esel] : 0;
		}
		else if(inst)
		{
			// pure instruction entry
			mpu_entry = (esel < NUM_INST_MPU_ENTRIES) ? &inst_mpu_entries[esel] : 0;
		}
		else
		{
			// pure data entry
			mpu_entry = (esel < NUM_DATA_MPU_ENTRIES) ? &data_mpu_entries[esel] : 0;
		}
		
		if(mpu_entry && !MAS0::RO::Get(mpu_entry->mas0))
		{
			mpu_entry->mas0 = MAS0::IPROT::Get(mpu_entry->mas0) ? ((mpu_entry->mas0 & MAS0::VALID::template GetMask<uint32_t>()) | mas0) : mas0;
			mpu_entry->mas1 = mas1;
			mpu_entry->mas2 = mas2;
			mpu_entry->mas3 = mas3;
		}
	}
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::ReadEntry()
{
	MPU_ENTRY *mpu_entry = 0;
	
	if(sel == mas0.template Get<typename MAS0::SEL>())
	{
		unsigned int esel = mas0.template Get<typename MAS0::ESEL>();
		unsigned int inst = mas0.template Get<typename MAS0::INST>();
		unsigned int shd = mas0.template Get<typename MAS0::SHD>();
		
		if(shd)
		{
			// shared entry
			mpu_entry = (esel < NUM_SHARED_MPU_ENTRIES) ? &shd_mpu_entries[esel] : 0;
		}
		else if(inst)
		{
			// pure instruction entry
			mpu_entry = (esel < NUM_INST_MPU_ENTRIES) ? &inst_mpu_entries[esel] : 0;
		}
		else
		{
			// pure data entry
			mpu_entry = (esel < NUM_DATA_MPU_ENTRIES) ? &data_mpu_entries[esel] : 0;
		}
	}
	
	mas0 = (mpu_entry ? mpu_entry->mas0 : 0);
	mas1 = (mpu_entry ? mpu_entry->mas1 : 0);
	mas2 = (mpu_entry ? mpu_entry->mas2 : 0);
	mas3 = (mpu_entry ? mpu_entry->mas3 : 0);
}

template <typename TYPES, typename CONFIG>
template <bool force>
void MPU<TYPES, CONFIG>::Invalidate()
{
	unsigned int esel;
	
	for(esel = 0; esel < NUM_INST_MPU_ENTRIES; esel++)
	{
		MPU_ENTRY *mpu_entry = &inst_mpu_entries[esel];
		
		if(force || !MAS0::IPROT::Get(mpu_entry->mas0))
		{
			mpu_entry->mas0 = 0;
			mpu_entry->mas1 = 0;
			mpu_entry->mas2 = 0;
			mpu_entry->mas3 = 0;
			MAS0::SEL::Set(mpu_entry->mas0, sel);
			MAS0::ESEL::Set(mpu_entry->mas0, esel);
			MAS0::INST::Set(mpu_entry->mas0, 1U);
		}
	}
	
	for(esel = 0; esel < NUM_DATA_MPU_ENTRIES; esel++)
	{
		MPU_ENTRY *mpu_entry = &data_mpu_entries[esel];
		if(force || !MAS0::IPROT::Get(mpu_entry->mas0))
		{
			mpu_entry->mas0 = 0;
			mpu_entry->mas1 = 0;
			mpu_entry->mas2 = 0;
			mpu_entry->mas3 = 0;
			MAS0::SEL::Set(mpu_entry->mas0, sel);
			MAS0::ESEL::Set(mpu_entry->mas0, esel);
		}
	}
	
	for(esel = 0; esel < NUM_SHARED_MPU_ENTRIES; esel++)
	{
		MPU_ENTRY *mpu_entry = &shd_mpu_entries[esel];
		if(force || !MAS0::IPROT::Get(mpu_entry->mas0))
		{
			mpu_entry->mas0 = 0;
			mpu_entry->mas1 = 0;
			mpu_entry->mas2 = 0;
			mpu_entry->mas3 = 0;
			MAS0::SEL::Set(mpu_entry->mas0, sel);
			MAS0::ESEL::Set(mpu_entry->mas0, esel);
			MAS0::SHD::Set(mpu_entry->mas0, 1U);
		}
	}

	mru_inst_mpu_entry = &inst_mpu_entries[0];
	mru_data_mpu_entry = &data_mpu_entries[0];
	mru_shd_mpu_entry = &shd_mpu_entries[0];
}

template <typename TYPES, typename CONFIG>
bool MPU<TYPES, CONFIG>::Match(MPU_ENTRY *mpu_entry, ADDRESS addr, bool exec, bool write)
{
	MSR& msr = cpu->msr;
	PID0& pid0 = cpu->pid0;
	unsigned int pid = pid0.template Get<typename PID0::Process_ID>();

	if(MAS0::VALID::Get(mpu_entry->mas0))
	{
		// valid MPU entry
		unsigned int pr = msr.template Get<typename MSR::PR>();
		
		if(( exec && ((pr && (mpu0csr0.template Get<typename MPU0CSR0::BYPUX>()            || MAS0::UX_UR::Get(mpu_entry->mas0))) || (!pr && (mpu0csr0.template Get<typename MPU0CSR0::BYPSX>()    || MAS0::SX_SR::Get(mpu_entry->mas0))))) ||
		   (!exec && ((pr && ((write && (mpu0csr0.template Get<typename MPU0CSR0::BYPUW>() || MAS0::UW::Get(mpu_entry->mas0)))    || (!write && (mpu0csr0.template Get<typename MPU0CSR0::BYPUR>() || MAS0::UX_UR::Get(mpu_entry->mas0))))) ||
		             (!pr && ((write && (mpu0csr0.template Get<typename MPU0CSR0::BYPSW>() || MAS0::SW::Get(mpu_entry->mas0)))    || (!write && (mpu0csr0.template Get<typename MPU0CSR0::BYPSR>() || MAS0::SX_SR::Get(mpu_entry->mas0))))))))
		{
			// Access right match
			unsigned int tid = MAS1::TID::Get(mpu_entry->mas1);
			unsigned int tidmsk = MAS1::TIDMSK::Get(mpu_entry->mas1);
			
			if(!tid || (((tid ^ pid) & ~tidmsk) == 0) || (!pr && mpu0csr0.template Get<typename MPU0CSR0::TIDCTL>()))
			{
				// TID match
				
				// Note: Due to the use of access address matching, the effective smallest region size is 8 bytes
				struct B29_31 : Field<void, 29, 31> {};
				
				ADDRESS upper_bound = MAS2::UPPER_BOUND::Get(mpu_entry->mas2);
				ADDRESS lower_bound = MAS3::LOWER_BOUND::Get(mpu_entry->mas3);
				
				ADDRESS addr_mask = ~((int32_t) 0x80000000 >> MAS0::UAMSK::Get(mpu_entry->mas0));
				B29_31::Set(addr_mask, 0U);
				
				ADDRESS masked_addr = addr & addr_mask;
				ADDRESS masked_upper_bound = upper_bound & addr_mask;
				ADDRESS masked_lower_bound = lower_bound & addr_mask;
				
				if((masked_addr >= masked_lower_bound) && (masked_addr <= masked_upper_bound))
				{
					// Address match
					return true;
				}
			}
		}
	}
	
	return false;
}

#if 0
template <typename CONFIG>
template <bool exec, bool write>
MPU_ENTRY *MPU<CONFIG>::CheckPermissions(ADDRESS addr)
{
	if(mpu0csr0.template Get<typename MPU0CSR0::MPUEN>())
	{
		unsigned int esel;

		if(exec)
		{
			// instruction
			if(Match(mru_inst_mpu_entry, addr, exec, write)) return mru_inst_mpu_entry;
			
			for(esel = 0; esel < NUM_INST_MPU_ENTRIES; esel++)
			{
				MPU_ENTRY *mpu_entry = &inst_mpu_entries[esel];
				
				if(Match(mpu_entry, addr, exec, write))
				{
					mru_inst_mpu_entry = mpu_entry;
					return mpu_entry;
				}
			}
		}
		else
		{
			// data
			if(Match(mru_data_mpu_entry, addr, exec, write)) return mru_data_mpu_entry;
			
			for(esel = 0; esel < NUM_DATA_MPU_ENTRIES; esel++)
			{
				MPU_ENTRY *mpu_entry = &data_mpu_entries[esel];
				
				if(Match(mpu_entry, addr, exec, write))
				{
					mru_data_mpu_entry = mpu_entry;
					return mpu_entry;
				}
			}
		}
		
		// shared
		if(Match(mru_shd_mpu_entry, addr, exec, write)) return mru_shd_mpu_entry;

		for(esel = 0; esel < NUM_SHARED_MPU_ENTRIES; esel++)
		{
			MPU_ENTRY *mpu_entry = &shd_mpu_entries[esel];
			
			if(Match(mpu_entry, addr, exec, write))
			{
				mru_shd_mpu_entry = mpu_entry;
				return mpu_entry;
			}
		}
	}
	else
	{
		return &hole_mpu_entry;
	}

	return 0;
}
#endif

template <typename TYPES, typename CONFIG>
MPU_ENTRY *MPU<TYPES, CONFIG>::Lookup(ADDRESS addr, bool exec, bool write)
{
	if(mpu0csr0.template Get<typename MPU0CSR0::MPUEN>())
	{
		unsigned int esel;

		if(exec)
		{
			// instruction
			if(Match(mru_inst_mpu_entry, addr, exec, write)) return mru_inst_mpu_entry;
			
			for(esel = 0; esel < NUM_INST_MPU_ENTRIES; esel++)
			{
				MPU_ENTRY *mpu_entry = &inst_mpu_entries[esel];
				
				if(Match(mpu_entry, addr, exec, write))
				{
					mru_inst_mpu_entry = mpu_entry;
					return mpu_entry;
				}
			}
		}
		else
		{
			// data
			if(Match(mru_data_mpu_entry, addr, exec, write)) return mru_data_mpu_entry;
			
			for(esel = 0; esel < NUM_DATA_MPU_ENTRIES; esel++)
			{
				MPU_ENTRY *mpu_entry = &data_mpu_entries[esel];
				
				if(Match(mpu_entry, addr, exec, write))
				{
					mru_data_mpu_entry = mpu_entry;
					return mpu_entry;
				}
			}
		}
		
		// shared
		if(Match(mru_shd_mpu_entry, addr, exec, write)) return mru_shd_mpu_entry;

		for(esel = 0; esel < NUM_SHARED_MPU_ENTRIES; esel++)
		{
			MPU_ENTRY *mpu_entry = &shd_mpu_entries[esel];
			
			if(Match(mpu_entry, addr, exec, write))
			{
				mru_shd_mpu_entry = mpu_entry;
				return mpu_entry;
			}
		}
	}
	else
	{
		return &hole_mpu_entry;
	}

	return 0;
}

template <typename TYPES, typename CONFIG>
template <bool DEBUG>
inline bool MPU<TYPES, CONFIG>::ControlAccess(unisim::util::cache::AccessControl<TYPES>& access_control)
{
	MPU_ENTRY *mpu_entry = Lookup(access_control.addr, access_control.mem_access_type == unisim::util::cache::MAT_EXEC, access_control.mem_access_type == unisim::util::cache::MAT_WRITE);
	
	if(unlikely(!mpu_entry))
	{
		if(DEBUG)
		{
			mpu_entry = &hole_mpu_entry;
		}
		else
		{
			switch(access_control.mem_access_type)
			{
				case unisim::util::cache::MAT_WRITE:
				case unisim::util::cache::MAT_READ:
					cpu->template ThrowException<typename CPU::DataStorageInterrupt::AccessControl>().SetAddress(access_control.addr);
					break;
				case unisim::util::cache::MAT_EXEC:
					cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::AccessControl>();
					break;
			}
			return false;
		}
	}
	
	access_control.size_to_protection_boundary = MAS2::UPPER_BOUND::Get(mpu_entry->mas2) - access_control.addr + 1;
	access_control.phys_addr = access_control.addr;
	access_control.storage_attr = STORAGE_ATTR((MAS0::I::Get(mpu_entry->mas0) ? TYPES::SA_I : 0) | (MAS0::G::Get(mpu_entry->mas0) ? TYPES::SA_G : 0));
	
	return true;
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::DumpEntry(std::ostream& os, MPU_ENTRY *mpu_entry)
{
	std::ios::fmtflags f(os.flags());
	os << "[valid=" << std::dec << MAS0::VALID::Get(mpu_entry->mas0) << ",";
	os << "iprot=" << std::dec << MAS0::IPROT::Get(mpu_entry->mas0) << ",";
	os << "sel=" << std::dec << MAS0::SEL::Get(mpu_entry->mas0) << ",";
	os << "ro=" << std::dec << MAS0::RO::Get(mpu_entry->mas0) << ",";
	os << "debug=" << std::dec << MAS0::DEBUG::Get(mpu_entry->mas0) << ",";
	os << "inst=" << std::dec << MAS0::INST::Get(mpu_entry->mas0) << ",";
	os << "shd=" << std::dec << MAS0::SHD::Get(mpu_entry->mas0) << ",";
	os << "esel=" << std::dec << MAS0::ESEL::Get(mpu_entry->mas0) << ",";
	os << "uamsk=" << std::dec << MAS0::UAMSK::Get(mpu_entry->mas0) << ",";
	os << "uw=" << std::dec << MAS0::UW::Get(mpu_entry->mas0) << ",";
	os << "sw=" << std::dec << MAS0::SW::Get(mpu_entry->mas0) << ",";
	os << "ux_ur=" << std::dec << MAS0::UX_UR::Get(mpu_entry->mas0) << ",";
	os << "sx_sr=" << std::dec << MAS0::SX_SR::Get(mpu_entry->mas0) << ",";
	os << "iovr=" << std::dec << MAS0::IOVR::Get(mpu_entry->mas0) << ",";
	os << "govr=" << std::dec << MAS0::GOVR::Get(mpu_entry->mas0) << ",";
	os << "i=" << std::dec << MAS0::I::Get(mpu_entry->mas0) << ",";
	os << "g=" << std::dec << MAS0::G::Get(mpu_entry->mas0) << ",";
	os << "tid=0x" << std::hex << MAS1::TID::Get(mpu_entry->mas1) << ",";
	os << "tidmsk=0x" << std::hex << MAS1::TIDMSK::Get(mpu_entry->mas1) << ",";
	os << "upper_bound=0x" << std::hex << MAS2::UPPER_BOUND::Get(mpu_entry->mas2) << ",";
	os << "lower_bound=0x" << std::hex << MAS3::LOWER_BOUND::Get(mpu_entry->mas3) << "]";
	os.flags(f);
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::Dump(std::ostream& os)
{
	unsigned int esel;
	
	for(esel = 0; esel < NUM_INST_MPU_ENTRIES; esel++)
	{
		MPU_ENTRY *mpu_entry = &inst_mpu_entries[esel];
		
		DumpEntry(os, mpu_entry); os << std::endl;
	}
	
	for(esel = 0; esel < NUM_DATA_MPU_ENTRIES; esel++)
	{
		MPU_ENTRY *mpu_entry = &data_mpu_entries[esel];
		
		DumpEntry(os, mpu_entry); os << std::endl;
	}
	
	for(esel = 0; esel < NUM_SHARED_MPU_ENTRIES; esel++)
	{
		MPU_ENTRY *mpu_entry = &shd_mpu_entries[esel];

		DumpEntry(os, mpu_entry); os << std::endl;
	}
}

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_MPU_HH__
