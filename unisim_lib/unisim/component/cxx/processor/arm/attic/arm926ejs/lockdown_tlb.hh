/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_LOCKDOWN_TLB_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_LOCKDOWN_TLB_HH__

#include <inttypes.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/cache_power_estimator.hh"
#include "unisim/service/interfaces/power_mode.hh"
#include "unisim/util/random/random.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

class LockdownTLB
	: public unisim::kernel::service::Client<
	  	unisim::service::interfaces::CachePowerEstimator>
	, public unisim::kernel::service::Client<
	  	unisim::service::interfaces::PowerMode>
{
public:
	/** Constructor 
	 *
	 * @param name the name of this UNISIM Object
	 * @param parent the parent UNISIM Object
	 */
	LockdownTLB(const char *name, unisim::kernel::service::Object *parent = 0);
	/** Destructor */
	~LockdownTLB();

	/** Setup
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool BeginSetup();

	/** Cache power estimator service import. */
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::CachePowerEstimator> 
		power_estimator_import;
	/** Power mode service import. */
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::PowerMode> 
		power_mode_import;

	/** Invalidate all the entries of the tlb */
	void Invalidate();

	/** Get the way of the given tag
	 *
	 * @param tag the tag we are looking for 
	 * @param will be set to the way of the tag if found
	 * @return true if found, false otherwise
	 */
	bool GetWay(uint32_t tag, uint32_t *way) const;
	/** Get the valid bit of the given way
	 *
	 * @param the way to check
	 * @return different than 0 if valid, 0 otherwise
	 */
	uint32_t GetValid(uint32_t way) const;
	/** Get the data contained in one entry
	 * 
	 * @param the way to use
	 * @return the data contained in the way
	 */
	uint32_t GetData(uint32_t way) const;

	/** Cache read access counter. */
	uint64_t read_accesses;
	/** Cache write access counter. */
	uint64_t write_accesses;
	/** Cache read hit access counter. */
	uint64_t read_hits;

private:
	/** Unisim logging services. */
	unisim::kernel::logger::Logger logger;
	
	static const uint32_t m_associativity_ = 8;

	unisim::util::random::Random rand;

	uint32_t m_tag_mask;
	uint32_t m_tag_shift;
	uint32_t m_tag[m_associativity_];
	uint32_t m_data[m_associativity_];
	uint8_t m_valid[m_associativity_];
	uint32_t m_replacement_history;

	/** UNISIM Statistic of the number of read accesses to the 
	 * cache.
	 */
	unisim::kernel::service::Statistic<uint64_t> stat_read_accesses;
	/** UNISIM Statistic of the number of write accesses to the 
	 * cache.
	 */
	unisim::kernel::service::Statistic<uint64_t> stat_write_accesses;
	/** UNISIM Formula of the number of accesses to the cache.
	 */
	unisim::kernel::service::Formula<uint64_t> form_accesses;
	/** UNISIM Statistic of the number of read hits to the cache.
	 */
	unisim::kernel::service::Statistic<uint64_t> stat_read_hits;
	/** UNISIM Formula for the hit rate of the cache.
	 */
	unisim::kernel::service::Formula<double> form_hit_rate;
};

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_LOCKDOWN_TLB_HH__