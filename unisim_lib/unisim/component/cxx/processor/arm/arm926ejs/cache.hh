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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CACHE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CACHE_HH__

#include <inttypes.h>

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

class Cache
{
public:
	/** Constructor */
	Cache();
	/** Destructor */
	~Cache();

	/** Set the size of the cache.
	 * Sets the cache size to the indicated size.
	 * @param size the selected size (this value can be 4KB, 8KB, 16KB, 32KB, 64KB, 128KB)
	 * @return true on success, false on error (not valid size)
	 */
	bool SetSize(uint32_t size);
	/** Set the current replacement policy.
	 * Sets the cache active replacement policy (round-robin/random).
	 *
	 * @param round_robin Sets the replacement policy to round-robin if true, to random otherwise.
	 * @return the previous replacement policy (true = round-robin, false = random)
	 */
	bool SetRRReplacementPolicy(bool round_robin);
	/** Check if the cache has been correctly initialized.
	 * @return true on cache correctly initialized, false otherwise
	 */
	bool IsOK() const;
	uint32_t GetTag(uint32_t addr) const;
	void SetTag(uint32_t index, uint32_t way, uint32_t tag);
	uint32_t GetSet(uint32_t addr) const;
	uint32_t GetIndex(uint32_t addr) const;
	bool HasTag(uint32_t tag, uint32_t set) const;
	bool GetWay(uint32_t tag, uint32_t set, uint32_t *way) const;
	/** Get a way where to place a new line.
	 * The current replacement policy will be selected.
	 *
	 * @param set the set where the line should be placed
	 * @return the selected way
	 */
	uint32_t GetNewWay(uint32_t set);
	/** Get the base address from a line in the cache.
	 *
	 * @param set the set the line is located in
	 * @param way the say the line is located in
	 * @return the requested line base address
	 */
	uint32_t GetBaseAddress(uint32_t set, uint32_t way) const;
	/** Get the line base address from a given address.
	 *
	 * @param addr the address to get obtain the line base address from
	 * @return the line base address from the given address
	 */
	uint32_t GetBaseAddressFromAddress(uint32_t addr) const;
	uint32_t GetData(uint32_t set, uint32_t way, uint8_t **data);
	uint32_t GetData(uint32_t set, uint32_t way, uint32_t index, uint32_t size,
			uint8_t **data);
	uint32_t GetDataCopy(uint32_t set, uint32_t way, uint8_t *data) const;
	uint32_t GetDataCopy(uint32_t set, uint32_t way, uint32_t index,
			uint32_t size, uint8_t *data) const;
	uint32_t SetData(uint32_t set, uint32_t way, uint8_t *data);
	uint32_t SetData(uint32_t set, uint32_t way, uint32_t index, uint32_t size,
			uint8_t *data);
	uint8_t GetValid(uint32_t set, uint32_t way) const;
	void SetValid(uint32_t set, uint32_t way, uint8_t valid);
	uint8_t GetDirty(uint32_t set, uint32_t way) const;
	void SetDirty(uint32_t set, uint32_t way, uint8_t dirty);

private:
	static const uint32_t m_sets_ = 1024;
	static const uint32_t m_associativity_ = 4;
	static const uint32_t m_line_size_ = 32;
	static const uint32_t m_set_shift_ = 5;
	static const uint32_t m_index_mask_ = 0x01fULL;

	bool m_is_ok;
	uint32_t m_size;
	/** Indicates the current replacement policy (true = round-robin, false = random) */
	bool m_round_robin_replacement_policy;
	uint32_t m_tag_mask;
	uint32_t m_tag_shift;
	uint32_t m_set_mask;
	uint32_t m_tag[m_sets_][m_associativity_];
	uint8_t m_data[m_sets_][m_associativity_][m_line_size_];
	uint8_t m_valid[m_sets_][m_associativity_];
	uint8_t m_dirty[m_sets_][m_associativity_];
	uint32_t m_replacement_history[m_sets_];

public:
	static const uint32_t LINE_SIZE = m_line_size_;
};

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CACHE_HH__