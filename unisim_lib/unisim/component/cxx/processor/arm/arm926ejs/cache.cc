/*
 *  Copyright (c) 2009,
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
#include "unisim/component/cxx/processor/arm/arm926ejs/cache.hh"

// #define ARM926EJS_CACHE_DEBUG

#ifdef ARM926EJS_CACHE_DEBUG
#include <iostream>
using namespace std;
#endif

#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

	Cache::Cache() :
	m_is_ok(false),
	m_size(0),
	m_round_robin_replacement_policy(false)
	{
		for (uint32_t set = 0; set < m_sets_; set++)
		{
			for (uint32_t way = 0; way < m_associativity_; way++)
			{
				m_tag[set][way] = 0;
				memset(m_data[set][way], 0, m_line_size_);
				m_valid[set][way] = 0;
				m_dirty[set][way] = 0;
			}
			m_replacement_history[set] = 0;
		}
	}

	Cache::~Cache()
	{
		// nothing to do
	}

	bool 
	Cache::
	SetSize(uint32_t size)
	{
		m_is_ok = false;
		// test that the size parameter is a valid value
		if ( size > 0 )
		{
			// ARM926EJS caches can only be between 4KB (0x01000) and 128KB (0x020000)
			//   and must be 2^x = size
#ifdef ARM926EJS_CACHE_DEBUG
			cerr << "Cache size = 0x" << hex << size << dec
					<< "(" << (unsigned int)size << ")" << endl;
#endif
			if ( (size & ~(uint32_t)0x03f000) == 0 )
			{
				uint32_t i = size;
				uint32_t count = 0;
#ifdef ARM926EJS_CACHE_DEBUG
				cerr << "Cache size is in range" << endl;
#endif
				while ( i != 0 )
				{
					if ( i & (uint32_t)0x01 ) count++;
					i = i >> 1;
				}
				if ( count == 1 )
				{
					m_size = size;
					m_is_ok = true;
				}
			}
			// compute the masks
			if (m_is_ok)
			{
				m_set_mask = 1;
				m_tag_shift = m_set_shift_ + 1;
				for(uint32_t t_nsets = m_size / m_associativity_ / m_line_size_;
					t_nsets != 0;
					t_nsets = t_nsets >> 1)
				{
					m_set_mask = (m_set_mask << 1) + 1;
					m_tag_shift++;
				}
				m_set_mask = m_set_mask << m_set_shift_;
				m_tag_mask = ~(m_set_mask | m_index_mask_);
			}
		}

		return m_is_ok;
	}

	bool
	Cache::
	SetRRReplacementPolicy(bool round_robin)
	{
		bool prev = m_round_robin_replacement_policy;
		m_round_robin_replacement_policy = round_robin;
		return prev;
	}

	bool 
	Cache::IsOK()
	const
	{
		return m_is_ok;
	}

	uint32_t
	Cache::
	GetTag(uint32_t addr)
	const
	{
		return (addr & m_tag_mask) >> m_tag_shift;
	}

	void
	Cache::
	SetTag(uint32_t set, uint32_t way, uint32_t tag)
	{
		m_tag[set][way] = tag;
	}
	
	uint32_t
	Cache::
	GetSet(uint32_t addr)
	const
	{
		return (addr & m_set_mask) >> m_set_shift_;
	}
	
	uint32_t
	Cache::
	GetIndex(uint32_t addr)
	const
	{
		return addr & m_index_mask_;
	}
	
	bool
	Cache::
	HasTag(uint32_t tag, uint32_t set)
	const
	{
		uint32_t current_way = 0;
		
		while (current_way < m_associativity_)
		{
			if (m_tag[set][current_way] == tag)
				return true;
			current_way++;
		}
		return false;
	}
	
	bool
	Cache::
	GetWay(uint32_t tag, uint32_t set, uint32_t *way)
	const
	{
		bool found = false;
		uint32_t current_way = 0;
		
		while (current_way < m_associativity_)
		{
			found = (m_tag[set][current_way] == tag);
			if (found)
			{
				*way = current_way;
				return found;
			}
			current_way++;
		}
		return found;
	}

	uint32_t
	Cache::
	GetNewWay(uint32_t set)
	{
		// apply the arm926ejs replacement policy
		if ( m_round_robin_replacement_policy )
		{
			// current replacement policy is round robin
			m_replacement_history[set] = (m_replacement_history[set] + 1) %
					m_associativity_;
		}
		else
		{
			// current replacement policy is random
			m_replacement_history[set] = random() % m_associativity_;
		}
		return m_replacement_history[set];
	}

	uint32_t
	Cache::
	GetBaseAddress(uint32_t set, uint32_t way)
	const
	{
		uint32_t tag = m_tag[set][way];
		uint32_t addr = 0;
		addr = (tag << m_tag_shift) |
				(set << m_set_shift_);
		return addr;
	}

	uint32_t
	Cache::
	GetBaseAddressFromAddress(uint32_t addr)
	const
	{
#ifdef ARM926EJS_CACHE_DEBUG
		cerr << "GettingAddressFromAddress" << endl
				<< " - addr       = 0x" << hex << (unsigned long int)addr << endl
				<< " - m_set_mask = 0x" << (unsigned long int)m_set_mask << dec
				<< " (shift = " << (unsigned int)m_set_shift_ << ")" << endl
				<< " - m_tag_mask = 0x" << hex << (unsigned long int)m_tag_mask << dec
				<< " (shift = " << (unsigned int)m_tag_shift << ")" << endl
				<< " - new_addr   = 0x" << hex
				<< (unsigned long int)(addr & (m_set_mask | m_tag_mask)) << dec
				<< endl;
#endif
		addr = addr & (m_set_mask | m_tag_mask);
		return addr;
	}

	uint32_t
	Cache::
	GetData(uint32_t set, uint32_t way, uint8_t **data)
	{
		*data = m_data[set][way];
		return m_line_size_;
	}
	
	uint32_t
	Cache::
	GetData(uint32_t set, uint32_t way, uint32_t index, uint32_t size, uint8_t **data)
	{
		uint32_t max_size = m_line_size_ - index;
		*data = &(m_data[set][way][index]);
		return (max_size < size) ? max_size : size;
	}
	
	uint32_t
	Cache::
	GetDataCopy(uint32_t set, uint32_t way, uint8_t *data)
	const
	{
		memcpy(data, m_data[set][way], m_line_size_);
		return m_line_size_;
	}
	
	uint32_t
	Cache::
	GetDataCopy(uint32_t set, uint32_t way, uint32_t index, uint32_t size, uint8_t *data)
	const
	{
		uint32_t max_size = m_line_size_ - index;
		size = (max_size < size) ? max_size : size;
		memcpy(data, &(m_data[set][way][index]), size);
		return size;
	}
	
	uint32_t
	Cache::
	SetData(uint32_t set, uint32_t way, uint8_t *data)
	{
		memcpy(m_data[set][way], data, m_line_size_);
		return m_line_size_;
	}

	uint32_t
	Cache::
	SetData(uint32_t set, uint32_t way, uint32_t index, uint32_t size, uint8_t *data)
	{
		uint32_t max_size = m_line_size_ - index;
		size = (max_size < size) ? max_size : size;
		memcpy(&(m_data[set][way][index]), data, size);
		return size;
	}
	
	uint8_t
	Cache::
	GetValid(uint32_t set, uint32_t way)
	const
	{
		return m_valid[set][way];
	}

	void
	Cache::
	SetValid(uint32_t set, uint32_t way, uint8_t valid)
	{
		m_valid[set][way] = valid;
	}
	
	uint8_t
	Cache::
	GetDirty(uint32_t set, uint32_t way)
	const
	{
		return m_dirty[set][way];
	}
	
	void
	Cache::
	SetDirty(uint32_t set, uint32_t way, uint8_t dirty)
	{
		m_dirty[set][way] = dirty;
	}

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

