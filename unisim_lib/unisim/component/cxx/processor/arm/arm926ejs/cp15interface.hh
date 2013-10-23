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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15INTERFACE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15INTERFACE_HH__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

class CP15Interface
{
public:
	virtual ~CP15Interface() {};

	/** Get caches info
	 *
	 */
	virtual void GetCacheInfo(bool &unified, 
			uint32_t &isize, uint32_t &iassoc, uint32_t &ilen,
			uint32_t &dsize, uint32_t &dassoc, uint32_t &dlen) = 0;
	/** Drain write buffer.
	 * Perform a memory barrier by draining the write buffer.
	 */
	virtual void DrainWriteBuffer() = 0;
	/** Invalidate ICache single entry using MVA
	 *
	 * Perform an invalidation of a single entry in the ICache using the
	 *   given address in MVA format.
	 *
	 * @param mva the address to invalidate
	 */
	virtual void InvalidateICacheSingleEntryWithMVA(uint32_t mva) = 0;
	/** Clean DCache single entry using MVA
	 *
	 * Perform a clean of a single entry in the DCache using the given
	 *   address in MVA format.
	 *
	 * @param mva the address to clean
 	 * @param invalidate true if the line needs to be also invalidated
	 */
	virtual void CleanDCacheSingleEntryWithMVA(uint32_t mva, bool invalidate)
		= 0;
	/** Invalidate the caches.
	 * Perform a complete invalidation of the instruction cache and/or the 
	 *   data cache.
	 *
	 * @param insn_cache whether or not the instruction cache should be 
	 *   invalidated
	 * @param data_cache whether or not the data cache should be invalidated
	 */
	virtual void InvalidateCache(bool insn_cache, bool data_insn) = 0;
	/** Invalidate the TLBs.
	 * Perform a complete invalidation of the unified TLB.
	 */
	virtual void InvalidateTLB() = 0;
	/** Test and clean DCache.
	 * Perform a test and clean operation of the DCache.
	 *
	 * @return return true if the complete cache is clean, false otherwise
	 */
	virtual bool TestAndCleanDCache() = 0;
	/** Test, clean and invalidate DCache.
	 * Perform a test and clean operation of the DCache, and invalidate the
	 *   complete cache if it is clean.
	 *
	 * @return return true if the complete cache is clean, false otherwise
	 */
	virtual bool TestCleanAndInvalidateDCache() = 0;
};

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15INTERFACE_HH__

