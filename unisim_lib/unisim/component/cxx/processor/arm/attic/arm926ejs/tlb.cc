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


#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "unisim/component/cxx/processor/arm/arm926ejs/tlb.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"

// #define TLB_DEBUG

#ifdef TLB_DEBUG
#include <iostream>
using namespace std;
#endif

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using std::endl;

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

TLB::
TLB(const char *name, unisim::kernel::service::Object *parent) 
	: unisim::kernel::service::Object(name, parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::CachePowerEstimator>(name,  parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::PowerMode>(name,  parent)
	, power_estimator_import("power-estimator-import", this)
	, power_mode_import("power-mode-import", this)
	, read_accesses(0)
	, write_accesses(0)
	, read_hits(0)
	, logger(*this)
	, rand(1, -1, rand.Max, rand.Min)
	, stat_read_accesses("read-accesses", this,
			read_accesses,
			"Number of read accesses to the TLB.")
	, stat_write_accesses("write-accesses", this,
			write_accesses,
			"Number of write accesses to the TLB.")
	, form_accesses("accesses", this, "+",
			&stat_read_accesses, &stat_write_accesses, 0,
			"Number of accesses to the TLB.")
	, stat_read_hits("read-hits", this,
			read_hits,
			"Number of read hit accesses to the TLB.")
	, form_hit_rate("read-hit-rate", this, "/",
			&stat_read_hits, &stat_read_accesses, 0,
			"TLB hit rate.")
{
	for (uint32_t set = 0; set < m_sets_; set++)
	{
		for (uint32_t way = 0; way < m_associativity_; way++)
		{
			m_tag[set][way] = 0;
			m_data[set][way] = 0;
			m_valid[set][way] = false;
		}
		m_last_accessed_way[set] = 0;
		m_replacement_history[set] = 0;
	}
	stat_read_accesses.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	stat_write_accesses.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	form_accesses.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	stat_read_hits.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	form_hit_rate.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
}

TLB::
~TLB()
{
	// nothing to do
}

bool
TLB::
BeginSetup()
{
	return true;
}

/** Invalidate all the entries of the tlb */
void
TLB::
Invalidate()
{
	memset(m_valid, 0, sizeof(uint8_t) * m_sets_ * m_associativity_);
}

/** Get the tag corresponding to the given address
 *
 * @param addr the address to consider
 * @return the tag for the given address
 */
uint32_t 
TLB::
GetTag(uint32_t addr) const
{
	return (addr & m_tag_mask_) >> m_tag_shift_;
}

/** Get the set corresponding to the given address
 *
 * @param addr the address to consider
 * @return the set for the given address
 */
uint32_t 
TLB::
GetSet(uint32_t addr) const
{
	return (addr & m_set_mask_) >> m_set_shift_;
}

/** Get the way for the corresponding couple tag and set.
 *
 * @param tag the tag to search for
 * @param set the set to look for the tag in
 * @param way the way in which the tag was found
 * @return true if found, false otherwise
 */
bool 
TLB::
GetWay(uint32_t tag, uint32_t set, uint32_t *way)
{
	bool found = false;
	uint32_t current_way = 0;

	current_way = m_last_accessed_way[set];
	if ( (m_tag[set][current_way] == tag) && m_valid[set][current_way] )
	{
		*way = current_way;
		return true;
	}

	current_way = 0;
	while (current_way < m_associativity_)
	{
		found = (m_tag[set][current_way] == tag) && m_valid[set][current_way];
		if ( found )
		{
			*way = current_way;
			m_last_accessed_way[set] = current_way;
			return true;
		}
		current_way++;
	}
	*way = 0;
	return false;
}

/** Get a new way where a new entry can be placed.
 *
 * @param set the set to which the entry should be placed
 */
uint32_t 
TLB::
GetNewWay(uint32_t set) 
{
	return rand.Generate(m_associativity_/2) +
		(m_associativity_/2);
}

/** Get the valid bit of the given set and way.
 *
 * @param set the set to consider
 * @param way the way to consider
 * @return different than 0 if found, 0 otherwise
 */
uint32_t 
TLB::
GetValid(uint32_t set, uint32_t way) const
{
	return m_valid[set][way];
}

/** Get the data contained in the given set and way.
 *
 * @param set the set to consider
 * @param way the way to consider
 * @return the data in the given set and way
 */
uint32_t 
TLB::
GetData(uint32_t set, uint32_t way) const
{
	return m_data[set][way];
}

/** Create a new entry.
 *
 * @param tag the tag to set
 * @param set the set to use
 * @param way the way to use
 * @param data the data to set
 * @param valid the value of the valid field
 */
void 
TLB::
SetEntry(uint32_t tag,
		uint32_t set,
		uint32_t way,
		uint32_t data,
		uint32_t valid)
{
	m_tag[set][way] = tag;
	m_data[set][way] = data;
	m_valid[set][way] = valid;
}

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

