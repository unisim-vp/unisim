/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CONFIG_HH__

#include <inttypes.h>

#include "unisim/component/cxx/processor/arm/cache/types.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cache {

/** Base class for the arm caches configurations 
 * 
 * This class defines the basic parameters of the arm caches. More concretely it defines
 *   a 512 bytes direct-mapped write trough cache with line sizes of 8 bytes.
 * This class is here to be inherited to create new configurations. */
class ARMCacheBase {
public:
	/** enables/disables debug */
	static const bool DEBUG_ENABLE = false;
	
	/** the address type of arm caches (always 32-bits) */
	typedef uint32_t address_t;             // 32-bit effective address
	
	/** the line lenght in bytes
	 * The line lenght in bytes. Four different possibilities are possible:
	 *  - 0b1000 0x8 -> 8 bytes
	 *  - 0b10000 0x10-> 16 bytes
	 *  - 0b100000 0x20-> 32 bytes
	 *  - 0b1000000 0x40-> 64 bytes
	 */
	static const uint32_t LINELEN = 0x8; // 8 bytes
	/** cache size multiplier
	 * This field is used to know the cache size, two different values are possible:
	 *  - 2 (for 0.5, 1, 2, 4, 8, 16, 32 and 64 KB)
	 *  - 3 (for 0.75, 1.5, 3, 6, 12, 24, 48 and 96KB)
	 * Note that the CACHE_SIZE and ASSOCIATIVITY fields have already the multiplier 
	 *    applied. This field is just for information.
	 */
	static const uint32_t MULTIPLIER = 2;
	/** cache associativity
	 *  This field defines the cache associativity. The following are valid values (X-ways):
	 *    1 (direct-mapped), 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192 
	 */
	static const uint32_t ASSOCIATIVITY = 1;
	/** cache size in bytes
	 * Defines the cache size in KB. The following are valid values (in KB):
	 *    0.5, 0.75, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96
	 * and in bytes:
	 *    512, 768, 1024, 2048, 3072, 4096, 6144, 8192, 12288, 16384, 24576, 32768, 49152, 65536, 98304
	 */
	static const uint32_t SIZE = 512;
	/** the number of sets
	 * The number of sets in the cache.
	 */
	static const uint32_t NSETS = 64; // 0b1000000
	
	/** the cache allocation policy
	 * We need to set one allocation policy: Read or Write
	 * If Read allocate new lines will be allocated only on read misses, never for write misses
	 * If Write allocate new lines will be allocated for read and write misses
	 */
	static const AllocationPolicy ALLOCATION_POLICY = READ_ALLOCATE;
	//AllocationPolicy::READ_ALLOCATE;
	
	/** the cache write policy
	 * Two possibilities:
	 * - WriteThrough
	 * - WriteBack
	 */
	static const WritePolicy WRITE_POLICY = WRITE_THROUGH;
	
	/** the cache replacement policy
	 * Two possibilites:
	 * - Random replacement (RANDOM_REPLACEMENT)
	 * - Round-robin replacement (ROUNDROBIN_REPLACEMENT)
	 */
	static const ReplacementPolicy REPLACEMENT_POLICY = RANDOM_REPLACEMENT;
};

/** 
 * ARM cache of 512 bytes direct-mapped write through with line size of 8 bytes
 */
class ARMCache512bDMWT8bls_Config : //
	public ARMCacheBase {
	// nothing needs to be redefined, it corresponds to the default cache configuration
};

/** 
 * ARM cache of 512 bytes direct-mapped write through with line size of 8 bytes
 *   with debug mode activated
 */
class ARMCache512bDMWT8bls_DebugConfig :
	public ARMCacheBase {
public:
	static const bool DEBUG_ENABLE = true;
};

class ARMCache512bDMWB8bls_Config : //
	public ARMCacheBase {
public:
	static const AllocationPolicy ALLOCATION_POLICY = WRITE_ALLOCATE;
	static const WritePolicy WRITE_POLICY = WRITE_BACK;
};

class ARMCache512bDMWB8bls_DebugConfig :
	public ARMCache512bDMWB8bls_Config {
public:
	static const bool DEBUG_ENABLE = true;
};

class ARMCache32KBDMWT32bls_Config :
	public ARMCacheBase {
public:
	static const uint32_t LINELEN = 0x20; // 32 bytes
	static const uint32_t MULTIPLIER = 2;
	static const uint32_t ASSOCIATIVITY = 1;
	static const uint32_t SIZE = 32*1024;
	static const uint32_t NSETS = 1024; // 0b1000000
	static const AllocationPolicy ALLOCATION_POLICY = READ_ALLOCATE;
	static const ReplacementPolicy REPLACEMENT_POLICY = RANDOM_REPLACEMENT;
};

class ARMCache32KBDMWT32bls_DebugConfig :
	public ARMCache32KBDMWT32bls_Config {
public:
	static const bool DEBUG_ENABLE = true;
};

class ARMCache32KBDMWB32bls_Config :
	public ARMCache32KBDMWT32bls_Config {
public:
	static const AllocationPolicy ALLOCATION_POLICY = WRITE_ALLOCATE;
	static const WritePolicy WRITE_POLICY = WRITE_BACK;
};

class ARMCache32KBDMWB32bls_DebugConfig :
	public ARMCache32KBDMWB32bls_Config {
public:
	static const bool DEBUG_ENABLE = true;
};

class ARMCache64KBDMWT32bls_Config :
	public ARMCacheBase {
public:
	static const uint32_t LINELEN = 0x20; // 32 bytes
	static const uint32_t MULTIPLIER = 2;
	static const uint32_t ASSOCIATIVITY = 1;
	static const uint32_t SIZE = 64*1024;
	static const uint32_t NSETS = 2048; // 0b1000000
	static const AllocationPolicy ALLOCATION_POLICY = READ_ALLOCATE;
	static const ReplacementPolicy REPLACEMENT_POLICY = RANDOM_REPLACEMENT;
};

class ARMCache64KBDMWT32bls_DebugConfig :
	public ARMCache64KBDMWT32bls_Config {
public:
	static const bool DEBUG_ENABLE = true;
};

class ARMCache64KBDMWB32bls_Config :
	public ARMCache64KBDMWT32bls_Config {
public:
	static const AllocationPolicy ALLOCATION_POLICY = WRITE_ALLOCATE;
	static const WritePolicy WRITE_POLICY = WRITE_BACK;
};

class ARMCache64KBDMWB32bls_DebugConfig :
	public ARMCache64KBDMWB32bls_Config {
public:
	static const bool DEBUG_ENABLE = true;
};

class ARMCache96KBDMWT32bls_Config :
	public ARMCacheBase {
public:
	static const uint32_t LINELEN = 0x20; // 32 bytes
	static const uint32_t MULTIPLIER = 3;
	static const uint32_t ASSOCIATIVITY = 1;
	static const uint32_t SIZE = 96*1024;
	static const uint32_t NSETS = 3072; // 0b1000000
	static const AllocationPolicy ALLOCATION_POLICY = READ_ALLOCATE;
	static const ReplacementPolicy REPLACEMENT_POLICY = RANDOM_REPLACEMENT;
};

class ARMCache96KBDMWT32bls_DebugConfig :
	public ARMCache96KBDMWT32bls_Config {
public:
	static const bool DEBUG_ENABLE = true;
};

class ARMCache96KBDMWB32bls_Config :
	public ARMCache96KBDMWT32bls_Config {
public:
	static const AllocationPolicy ALLOCATION_POLICY = WRITE_ALLOCATE;
	static const WritePolicy WRITE_POLICY = WRITE_BACK;
};

class ARMCache96KBDMWB32bls_DebugConfig :
	public ARMCache96KBDMWB32bls_Config {
public:
	static const bool DEBUG_ENABLE = true;
};

} // end of namespace cache
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CONFIG_HH__
