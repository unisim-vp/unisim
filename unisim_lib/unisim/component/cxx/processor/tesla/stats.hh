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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_HH

#include <string>
//#include <unisim/component/cxx/processor/tesla/operation.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
struct OperationStats
{
private:
	std::string name;
	unsigned int count;
	unsigned int scalarcount;
	bool integer;
	bool fp;
	bool flow;
	bool memory;
	bool shared;
	bool constant;
	int16_t outputreg;
	int16_t input1reg;
	int16_t input2reg;
	int16_t input3reg;
	
public:
	OperationStats() :
		count(0), scalarcount(0), integer(false), fp(false), flow(false), memory(false),
		shared(false), constant(false), outputreg(-1), input1reg(-1),
		input2reg(-1), input3reg(-1) {}

	void SetName(char const * insnname) {
		name = std::string(insnname);
	}
	
	void SetInteger() { integer = true;	}
	void SetFP() { fp = true; }
	void SetFlow() { flow = true; }
	void SetMov() { integer = true;	}
	void SetGather() { memory = true;	}
	void SetScatter() { memory = true;	}
	void SetInputConst() { constant = true; }
	void SetInputShared() { shared = true; }
	void SetOutputShared() { shared = true; }
	void SetOutputReg(int16_t reg) { outputreg = reg; }
	void SetInput1Reg(int16_t reg) { input1reg = reg; }
	void SetInput2Reg(int16_t reg) { input2reg = reg; }
	void SetInput3Reg(int16_t reg) { input3reg = reg; }
	
	void ResetStatic() {
		integer = false; fp = false; flow = false;
		memory = false; shared = false; constant = false;
		outputreg = -1; input1reg = -1; input2reg = -1; input3reg = -1;
	}
	
	void ResetDynamic() {
		count = 0; scalarcount = 0;
	}

	void Execute(std::bitset<CONFIG::WARP_SIZE> mask) {
		++count;
		scalarcount += mask.count();
	}
	
	//void Gather(VectorAddress<CONFIG> const & addr, std::bitset<CONFIG::WARP_SIZE> mask);
	//void BranchUniTaken(std::bitset<CONFIG::WARP_SIZE> mask);

	void DumpCSV(std::ostream & os) const;
};

template<class CONFIG>
struct Stats
{
private:
	typedef std::map<typename CONFIG::address_t, OperationStats<CONFIG> > stats_map;	
	stats_map stats;
	
public:
	OperationStats<CONFIG> & operator[] (typename CONFIG::address_t addr) {
		return stats[addr];
	}
	
	void DumpCSV(std::ostream & os) const;
	void Reset() {
		stats.clear();
	}
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

