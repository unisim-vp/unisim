/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_TCC

#include <unisim/component/cxx/processor/tesla/stats.hh>
#include <unisim/component/cxx/processor/tesla/register.hh>
#include <iomanip>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


using namespace std;

template<class CONFIG>
void OperationStats<CONFIG>::SetRegNum(int16_t reg, DataType dt, Operand n)
{
	switch(dt) {
	case DT_U32:
	case DT_S32:
	case DT_F32:
		regnum[n] = reg;
		break;
	case DT_U16:
	case DT_S16:
		regnum[n] = reg / 2;
		break;
	default:
		regnum[n] = reg;	// TODO: count multiple registers for DT_U64/DT_U128
		break;
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::RegRead(VectorRegister<CONFIG> const * regs, DataType dt)
{
	if(CONFIG::STAT_SCALAR_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			scalarRegInputs += regs[0].CheckScalar16(false);
			scalarRegInputsCaught += regs[0].IsScalar16(false);
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			scalarRegInputs += regs[0].CheckScalar();
			scalarRegInputsCaught += regs[0].IsScalar();
		}
	}
	if(CONFIG::STAT_STRIDED_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			stridedRegInputs += regs[0].CheckStrided16(false);
			stridedRegInputsCaught += regs[0].IsStrided16(false);
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			stridedRegInputs += regs[0].CheckStrided();
			stridedRegInputsCaught += regs[0].IsStrided();
		}
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::RegWrite(VectorRegister<CONFIG> const * regs, DataType dt)
{
	if(CONFIG::STAT_SCALAR_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			scalarRegOutputs += regs[0].CheckScalar16(false);
			scalarRegOutputsCaught += regs[0].IsScalar16(false);
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			scalarRegOutputs += regs[0].CheckScalar();
			scalarRegOutputsCaught += regs[0].IsScalar();
		}
	}
	if(CONFIG::STAT_STRIDED_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			stridedRegOutputs += regs[0].CheckStrided16(false);
			stridedRegOutputsCaught += regs[0].IsStrided16(false);
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			stridedRegOutputs += regs[0].CheckStrided();
			stridedRegOutputsCaught += regs[0].IsStrided();
		}
	}
}

// Format:
// address,name,count,scalarcount,integer,fp,flow,memory
template<class CONFIG>
void OperationStats<CONFIG>::DumpCSV(std::ostream & os) const
{
	bool outputregs = (regnum[OpDest] != -1);
	int inputregs = 0;
	// Count unique registers only
	if(regnum[OpSrc1] != -1) { ++inputregs; }
	if(regnum[OpSrc2] != -1 && regnum[OpSrc2] != regnum[OpSrc1]) { ++inputregs; }
	if(regnum[OpSrc3] != -1 && regnum[OpSrc3] != regnum[OpSrc2]
		&& regnum[OpSrc3] != regnum[OpSrc2]) { ++inputregs; }
	
	os << "\"" << name << "\","
	   << count << ","
	   << scalarcount << ","
	   << integer << ","
	   << fp << ","
	   << flow << ","
	   << memory << ","
	   << shared << ","
	   << constant << ","
	   << inputregs << ","
	   << outputregs;
	if(CONFIG::STAT_SCALAR_REG) {
		os << "," << scalarRegInputs
		   << "," << scalarRegOutputs;
		os << "," << scalarRegInputsCaught
		   << "," << scalarRegOutputsCaught;
	}
	if(CONFIG::STAT_STRIDED_REG) {
		os << "," << stridedRegInputs
		   << "," << stridedRegOutputs;
		os << "," << stridedRegInputsCaught
		   << "," << stridedRegOutputsCaught;
	}
	os << endl;
}

template<class CONFIG>
void Stats<CONFIG>::DumpCSV(std::ostream & os) const
{
	os << "\"Address\","
	   << "\"Name\","
	   << "\"Executed\","
	   << "\"Exec. scalar\","
	   << "\"Integer\","
	   << "\"FP32\","
	   << "\"Flow\","
	   << "\"Memory\","
	   << "\"Shared\","
	   << "\"Constant\","
	   << "\"Input regs\","
	   << "\"Output regs\"";
	if(CONFIG::STAT_SCALAR_REG) {
		os << ",\"Scalar inputs\""
		   << ",\"Scalar outputs\"";
		os << ",\"Scalar inputs caught\""
		   << ",\"Scalar outputs caught\"";
	}
	if(CONFIG::STAT_STRIDED_REG) {
		os << ",\"Strided inputs\""
		   << ",\"Strided outputs\"";
		os << ",\"Strided inputs caught\""
		   << ",\"Strided outputs caught\"";
	}
	os << endl;

	typedef typename stats_map::const_iterator it_t;
	for(it_t it = stats.begin(); it != stats.end(); ++it)
	{
		os << it->first << ",";
		it->second.DumpCSV(os);
	}
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
