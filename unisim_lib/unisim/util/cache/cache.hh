/*
 *  Copyright (c) 2008-2017,
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

#ifndef __UNISIM_UTIL_CACHE_CACHE_HH__
#define __UNISIM_UTIL_CACHE_CACHE_HH__

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(HAVE_CXXABI)
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <iostream>
#include <typeinfo>
#include <inttypes.h>
#include <string.h>
#include <stdexcept>
#include <cstdlib>
#include <cassert>

namespace unisim {
namespace util {
namespace cache {

//                                                                    SECTORED ASSOCIATIVE CACHE                                                  
//          INDEX
//            
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #0  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #1  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #2  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #3  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// DECLARATIONS //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////// CacheType /////////////////////////////

enum CacheType
{
	NULL_CACHE         = 0, // null cache
	DATA_CACHE         = 1, // data cache
	INSTRUCTION_CACHE  = 2, // instruction cache
	UNIFIED_CACHE      = 3  // unified cache (both data and instruction)
};

inline std::ostream& operator << (std::ostream& os, const CacheType& ct);

///////////////////////// CacheWritingPolicyFlags /////////////////////////////

enum CacheWritingPolicyFlags
{
	CACHE_WRITE_BACK     = 1, // Flag write back/write through
	CACHE_WRITE_ALLOCATE = 2  // Flag write allocate/no-write allocate
};

////////////////////////////// CacheWritingPolicy /////////////////////////////

enum CacheWritingPolicy
{
	CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY = 0,                                       // Write-through and No-Write allocate policy
	CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY    = CACHE_WRITE_BACK,                        // Write-back and Write allocate policy
	CACHE_WRITE_THROUGH_AND_WRITE_ALLOCATE_POLICY    = CACHE_WRITE_ALLOCATE,                    // Write-through and No-Write allocate policy
	CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY       = CACHE_WRITE_BACK | CACHE_WRITE_ALLOCATE  // Write-back and Write allocate policy
};

inline std::ostream& operator << (std::ostream& os, const CacheWritingPolicy& cwp);

/////////////////////////// Forward declarations //////////////////////////////

template <typename TYPES, typename CACHE> struct CacheAccess;
struct CacheBlockStatus;
struct CacheLineStatus;
struct CacheSetStatus;
template <typename TYPES, typename CACHE> struct CacheBlock;
template <typename TYPES, typename CACHE> struct CacheLine;
template <typename TYPES, typename CACHE> struct CacheSet;
template <typename TYPES, typename CACHE> struct Cache;
template <typename TYPES> struct NullCache;
template <typename TYPES> struct NullCache2;
template <typename TYPES> struct NullCache3;
template <typename TYPES> struct NullCache4;
template <typename TYPES, typename _L1CACHE, typename _L2CACHE, typename _L3CACHE, typename _L4CACHE> struct CacheHierarchy;
template <typename TYPES, typename MSS> struct MemorySubSystem;

/////////////////////////////// CacheAccess<> /////////////////////////////////

template <typename TYPES, typename CACHE>
struct CacheAccess
{
	///////////////////////////////// input ///////////////////////////////////
	typename TYPES::ADDRESS addr;              // address
	typename TYPES::STORAGE_ATTR storage_attr; // storage attribute
	bool rwitm;                                // read with intent to modify
	
	////////////////////////////////////////////////////////////// input ////////////////// output //////////
	typename TYPES::ADDRESS line_base_addr;            //            X              Lookup
	typename TYPES::ADDRESS block_base_addr;           //            X              Lookup
	unsigned int index;                                //            X              Lookup/ChooseLineToEvict
	unsigned int way;                                  //            X              Lookup
	unsigned int sector;                               //            X              Lookup
	unsigned int offset;                               //            X              Lookup
	unsigned int size_to_block_boundary;               //            X              Lookup
	CacheSet<TYPES, CACHE> *set;                       //            X              Lookup
	CacheLine<TYPES, CACHE> *line;                     //            X              Lookup
	CacheLine<TYPES, CACHE> *line_to_write_back_evict; // WriteBackLine/EvictLine     X
	CacheBlock<TYPES, CACHE> *block;                   //       FillBlock           Lookup
};

/////////////////////////// CacheBlockStatus<> //////////////////////////////////

struct CacheBlockStatus
{
	bool valid;
	bool dirty;
	
	CacheBlockStatus() : valid(false), dirty(false) {}
	inline bool IsValid() const ALWAYS_INLINE { return valid; }
	inline bool IsDirty() const ALWAYS_INLINE { return dirty; }
	inline void SetValid() ALWAYS_INLINE { valid = true; }
	inline void SetDirty() ALWAYS_INLINE { dirty = true; }
	inline void Invalidate() ALWAYS_INLINE { valid = dirty = false; }
};

/////////////////////////// CacheLineStatus<> ///////////////////////////////////

struct CacheLineStatus
{
	inline void Invalidate() ALWAYS_INLINE {}
};

//////////////////////////// CacheSetStatus<> ///////////////////////////////////

struct CacheSetStatus
{
	inline void Invalidate() ALWAYS_INLINE {}
};

/////////////////////////////// CacheBlock<> //////////////////////////////////

template <typename TYPES, typename CACHE>
struct CacheBlock
{
public:
	CacheBlock();
	~CacheBlock();
	inline uint8_t& operator [] (unsigned int offset) ALWAYS_INLINE;
	static inline void DecodeAddress(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& base_addr, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	inline typename TYPES::ADDRESS GetBaseAddr() const ALWAYS_INLINE;
	inline void Zero() ALWAYS_INLINE;
	inline void Write(const void *buffer, unsigned int offset, unsigned int size) ALWAYS_INLINE;
	inline void Read(void *buffer, unsigned int offset, unsigned int size) ALWAYS_INLINE;
	
	inline bool IsValid() const ALWAYS_INLINE;
	inline bool IsDirty() const ALWAYS_INLINE;
	inline void SetValid() ALWAYS_INLINE;
	inline void SetDirty() ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;
	template <typename BLOCK_STATUS> BLOCK_STATUS& Status();

	void SetSector(unsigned int sector);
	inline unsigned int GetSector() const;
protected:
	friend struct CacheLine<TYPES, CACHE>;
	inline void SetBaseAddr(typename TYPES::ADDRESS base_addr);
private:
	typename TYPES::ADDRESS base_addr;
	CacheBlockStatus *status;
	unsigned int sector;
	uint8_t *storage;
};

/////////////////////////////// CacheLine<> ///////////////////////////////////

template <typename TYPES, typename CACHE>
struct CacheLine
{
public:
	CacheLine();
	~CacheLine();

	inline void SetWay(unsigned int way) ALWAYS_INLINE;
	inline void SetBaseAddr(typename TYPES::ADDRESS addr) ALWAYS_INLINE;
	inline typename TYPES::ADDRESS GetBaseAddr() const ALWAYS_INLINE;
	static inline void DecodeAddress(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	inline CacheBlock<TYPES, CACHE>& operator [] (unsigned int sector) ALWAYS_INLINE;

	inline unsigned int GetWay() const ALWAYS_INLINE;
	inline bool IsValid() const ALWAYS_INLINE;
	inline void SetValid() ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;
	template <typename LINE_STATUS> LINE_STATUS& Status();
protected:
private:
	friend struct CacheSet<TYPES, CACHE>;
	
	bool valid;
	CacheLine<TYPES, CACHE> *next;
	CacheLine<TYPES, CACHE> *prev;
	typename TYPES::ADDRESS base_addr;
	unsigned int way;
	CacheLineStatus *status;
	CacheBlock<TYPES, CACHE> *blocks;
};

//////////////////////////////// CacheSet<> ///////////////////////////////////

template <typename TYPES, typename CACHE>
struct CacheSet
{
public:
	CacheSet();
	~CacheSet();

	static inline void DecodeAddress(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	inline CacheLine<TYPES, CACHE>& operator [] (unsigned int way) ALWAYS_INLINE;

	inline void InvalidateLineByWay(unsigned int way) ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;

	void SetIndex(unsigned int index);
	inline unsigned int GetIndex() const ALWAYS_INLINE;
	
	template <typename SET_STATUS> SET_STATUS& Status();
	
	inline CacheLine<TYPES, CACHE> *AssociativeSearch(typename TYPES::ADDRESS line_base_addr) ALWAYS_INLINE;

protected:
private:
	CacheSetStatus *status;
	CacheLine<TYPES, CACHE> *mrs_line;
	CacheLine<TYPES, CACHE> *lrs_line;
	CacheLine<TYPES, CACHE> *lines;
	unsigned int index;
};
/////////////////////////////////// Cache<> ///////////////////////////////////

template <typename TYPES, typename CACHE>
struct Cache
{
	Cache();
	~Cache();

	static inline void DecodeAddress(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& index, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	inline CacheSet<TYPES, CACHE>& operator [] (unsigned int index) ALWAYS_INLINE;

	inline void InvalidateSet(unsigned int index) ALWAYS_INLINE;
	inline void InvalidateLineBySetAndWay(unsigned int index, unsigned int way) ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;
	inline CacheSet<TYPES, CACHE> *LookupSet(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	inline CacheLine<TYPES, CACHE> *LookupLine(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	inline CacheBlock<TYPES, CACHE> *LookupBlock(typename TYPES::ADDRESS addr, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	inline void Lookup(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	
	static inline bool IsNullCache() ALWAYS_INLINE;
	static inline bool IsDataCacheOnly() ALWAYS_INLINE;
	static inline bool IsDataCache() ALWAYS_INLINE;
	static inline bool IsInstructionCacheOnly() ALWAYS_INLINE;
	static inline bool IsInstructionCache() ALWAYS_INLINE;
	static inline bool IsUnifiedCache() ALWAYS_INLINE;
	static inline bool IsWriteBackCache() ALWAYS_INLINE;
	static inline bool IsWriteThroughCache() ALWAYS_INLINE;
	static inline bool IsWriteAllocate() ALWAYS_INLINE;
	static inline bool IsNotWriteAllocate() ALWAYS_INLINE;
	
	static inline unsigned int NumSets() ALWAYS_INLINE;
	static inline unsigned int NumWays() ALWAYS_INLINE;
	static inline unsigned int NumSectors() ALWAYS_INLINE;
protected:
	static const char *GetName();
private:
	unsigned int num_accesses;
	unsigned int num_misses;
	CacheSet<TYPES, CACHE> *sets;

	template <typename, typename MSS> friend struct MemorySubSystem;
	static inline const char *__MSS_GetName__() ALWAYS_INLINE;
	static std::string GenName();
};

////////////////////////////// NullCache<> ///////////////////////////////////

template <typename TYPES>
struct NullCache : Cache<TYPES, NullCache<TYPES> >
{
	static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
	static const CacheType TYPE = NULL_CACHE;
	static const unsigned int ASSOCIATIVITY = 1;
	static const unsigned int BLOCKS_PER_LINE = 1;
	static const unsigned int BLOCK_SIZE = 1;
	static const unsigned int SIZE = 1;
	
	static const char *GetName() { return "NullCache"; }
};

////////////////////////////// NullCache2<> ///////////////////////////////////

template <typename TYPES>
struct NullCache2 : Cache<TYPES, NullCache2<TYPES> >
{
	static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
	static const CacheType TYPE = NULL_CACHE;
	static const unsigned int ASSOCIATIVITY = 1;
	static const unsigned int BLOCKS_PER_LINE = 1;
	static const unsigned int BLOCK_SIZE = 1;
	static const unsigned int SIZE = 1;
	
	static const char *GetName() { return "NullCache2"; }
};

////////////////////////////// NullCache3<> ///////////////////////////////////

template <typename TYPES>
struct NullCache3 : Cache<TYPES, NullCache3<TYPES> >
{
	static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
	static const CacheType TYPE = NULL_CACHE;
	static const unsigned int ASSOCIATIVITY = 1;
	static const unsigned int BLOCKS_PER_LINE = 1;
	static const unsigned int BLOCK_SIZE = 1;
	static const unsigned int SIZE = 1;
	
	static const char *GetName() { return "NullCache3"; }
};

////////////////////////////// NullCache4<> ///////////////////////////////////

template <typename TYPES>
struct NullCache4 : Cache<TYPES, NullCache4<TYPES> >
{
	static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
	static const CacheType TYPE = NULL_CACHE;
	static const unsigned int ASSOCIATIVITY = 1;
	static const unsigned int BLOCKS_PER_LINE = 1;
	static const unsigned int BLOCK_SIZE = 1;
	static const unsigned int SIZE = 1;
	
	static const char *GetName() { return "NullCache4"; }
};

////////////////////////////// CacheHierarchy<> /////////////////////////////////

template <typename TYPES, typename _L1CACHE, typename _L2CACHE = NullCache2<TYPES>, typename _L3CACHE = NullCache3<TYPES>, typename _L4CACHE = NullCache4<TYPES> >
struct CacheHierarchy
{
	typedef _L1CACHE L1CACHE;
	typedef _L2CACHE L2CACHE;
	typedef _L3CACHE L3CACHE;
	typedef _L4CACHE L4CACHE;
	
	template <typename _CACHE, bool dummy = true>
	struct NextLevel
	{
		typedef NullCache<TYPES> CACHE;
	};
	
	template <bool dummy>
	struct NextLevel<_L1CACHE, dummy>
	{
		typedef _L2CACHE CACHE;
	};

	template <bool dummy>
	struct NextLevel<_L2CACHE, dummy>
	{
		typedef _L3CACHE CACHE;
	};

	template <bool dummy>
	struct NextLevel<_L3CACHE, dummy>
	{
		typedef _L4CACHE CACHE;
	};
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

///////////////////////// PLRU_ReplacementPolicy<> ////////////////////////////

/* associativity up to 65 */
template <unsigned ASSOCIATIVITY>
struct PLRU_ReplacementPolicy
{
	inline PLRU_ReplacementPolicy() : plru_bits(0) {}
	
	void PrintPLRU(std::ostream& os) const
	{
		unsigned int i;
		for(i = 0; i < (ASSOCIATIVITY - 1); i++)
		{
			if(i) os << " ";
			os << "B" << i << "=" << ((plru_bits >> i) & 1);
		}
	}
	
	inline void Clear()
	{
		plru_bits = 0;
		//std::cerr << "Clear: plru_bits = 0x" << std::hex << (unsigned long long) plru_bits << std::dec << std::endl;
	}
	
	inline unsigned int Select() const
	{
		unsigned int way;
		unsigned int i;
		unsigned int n;

		//std::cerr << "S: "; PrintPLRU(std::cerr); std::cerr << std::endl;

		for(i = 0, way = 0, n = 0; n < CEIL_LOG2<ASSOCIATIVITY>::VALUE; n++)
		{
			unsigned int bi = (plru_bits >> i) & 1;
			way = (way << 1) | bi;
			i = (i << 1) + 1 + bi;
		}

		//std::cerr << "S: way=" << way << std::endl;
		return way;
	}

	inline void UpdateOnAccess(unsigned int way)
	{
		//std::cerr << "A(" << way << "): "; PrintPLRU(std::cerr); std::cerr << std::endl;
		unsigned int i;
		unsigned int n;

		for(n = CEIL_LOG2<ASSOCIATIVITY>::VALUE, i = 0; n != 0; n--)
		{
			unsigned int bi = (way >> (n - 1)) & 1;
			plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
			i = (i << 1) + 1 + bi;
		}
		//std::cerr << "A(" << way << ") -> "; PrintPLRU(std::cerr); std::cerr << std::endl;
	}
	
private:
	typename TypeForBitSize<ASSOCIATIVITY - 1>::TYPE plru_bits;
};

////////////////////////// LRU_ReplacementPolicy<> ////////////////////////////

/* associativity up to 16 */
template <unsigned int ASSOCIATIVITY>
struct LRU_ReplacementPolicy
{
	typedef typename TypeForBitSize<ASSOCIATIVITY * CEIL_LOG2<ASSOCIATIVITY>::VALUE>::TYPE LRU_BITS_TYPE;
	static const unsigned int S = CEIL_LOG2<ASSOCIATIVITY>::VALUE;
	static const unsigned int L = (ASSOCIATIVITY - 1) * CEIL_LOG2<ASSOCIATIVITY>::VALUE;
	static const LRU_BITS_TYPE M = (LRU_BITS_TYPE(1) << S) - 1;
	
	inline LRU_ReplacementPolicy() : lru_bits(0)
	{
		Clear();
	}
	
	inline void Clear()
	{
		int i;
		for(i = ASSOCIATIVITY; i >= 0; i--)
		{
			lru_bits = (lru_bits << S) | i;
		}
		//std::cerr << "Clear: lru_bits = 0x" << std::hex << (unsigned long long) lru_bits << std::dec << std::endl;
	}
	
	inline unsigned int Select() const
	{
		unsigned int way = (lru_bits >> L) & M;
		//std::cerr << "S: way=" << way << std::endl;
		return way;
	}

	inline void UpdateOnAccess(unsigned int way)
	{
		//std::cerr << "A(" << way << "): lru_bits = 0x" << std::hex << (unsigned long long) lru_bits << std::dec << std::endl;
		if((lru_bits & M) == way) return;
		
		unsigned int i;

		LRU_BITS_TYPE _lru_bits = lru_bits;
		
		for(i = 0; i < ASSOCIATIVITY; i++, _lru_bits >>= S)
		{
			if((_lru_bits & M) == way)
			{
				lru_bits = ((((_lru_bits >> S) << (i * S)) | (lru_bits & ((LRU_BITS_TYPE(1) << (i * S)) - 1))) << S) | way; 
				//std::cerr << "A(" << way << ") -> 0x" << std::hex << (unsigned long long) lru_bits << std::dec << std::endl;
				return;
			}
		}
	}
	
private:
	LRU_BITS_TYPE lru_bits;
};

/////////////////////// RoundRobinReplacementPolicy<> /////////////////////////

/* unlimited associativity */
template <unsigned int ASSOCIATIVITY>
struct RoundRobinReplacementPolicy
{
	inline RoundRobinReplacementPolicy()
		: way(0)
	{
	}
	
	inline void Clear()
	{
		way = 0;
		//std::cerr << "Clear: way = " << way << std::endl;
	}
	
	inline unsigned int Select() const
	{
		//std::cerr << "S(): way = " << way << std::endl;
		return way;
	}

	inline void Update()
	{
		way = (way + 1) % ASSOCIATIVITY;
		//std::cerr << "F(): way = " << way << std::endl;
	}
	
private:
	unsigned int way;
};

///////////////////// PseudoRandomReplacementPolicy<> /////////////////////////

/* associativity up to 128 */
template <unsigned int ASSOCIATIVITY>
struct PseudoRandomReplacementPolicy
{
	inline PseudoRandomReplacementPolicy()
		: y(ASSOCIATIVITY - 1)
	{
	}
	
	inline void Clear()
	{
		y = ASSOCIATIVITY - 1;
	}
	
	inline unsigned int Select() const
	{
		unsigned int way = y / (256 / ASSOCIATIVITY); // Note: zero has slightly less probability to occur
		//std::cerr << "S(): way = " << way << std::endl;
		return way;
	}

	inline void Update()
	{
		// xorshift algorithm for 8-bit. y is in [1..255]
		y ^= (y << 7);
		y ^= (y >> 5);
		y ^= (y << 3);
		//std::cerr << "F(): way = " << way << std::endl;
	}
	
private:
	uint8_t y;
};

////////////////////////////// MemorySubSystem<> ////////////////////////////////

template <typename TYPES, typename MSS>
struct MemorySubSystem
{
	/* Constructor */
	MemorySubSystem();
	
	/* Data Load */
	bool DataLoad(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Data Store */
	bool DataStore(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Instruction Fetch */
	bool InstructionFetch(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Data Load */
	bool DebugDataLoad(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Data Store */
	bool DebugDataStore(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Instruction Fetch */
	bool DebugInstructionFetch(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
protected:
	///////////////// To be overriden by derived class ////////////////////////

#if 0	
	inline CACHE *GetCache(const CACHE *null) ALWAYS_INLINE;
	inline bool IsCacheVerbose(const CACHE *null) ALWAYS_INLINE;
	inline bool IsCacheEnabled(const CACHE *null) ALWAYS_INLINE;
	inline bool ChooseLineToEvict(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void UpdateReplacementPolicyOnAccess(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void UpdateReplacementPolicyOnFill(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
#endif
	
	inline std::ostream& GetDebugInfoStream() ALWAYS_INLINE;
	
	inline std::ostream& GetDebugWarningStream() ALWAYS_INLINE;

	inline std::ostream& GetDebugErrorStream() ALWAYS_INLINE;

	inline bool IsVerboseDataLoad() const ALWAYS_INLINE;
	
	inline bool IsVerboseDataStore() const ALWAYS_INLINE;
	
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE;
	
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE;
	
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE;
	
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE;

	inline bool IsStorageCacheable(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;
	
	inline bool IsDataWriteAccessWriteThrough(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;

	bool DataBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm);

	bool DataBusWrite(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	bool InstructionBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	bool DebugDataBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	bool DebugDataBusWrite(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	bool DebugInstructionBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	////////////////////////////// Helper functions ///////////////////////////
	
	/* Load data/instruction from Cache CACHE */
	template <typename CACHE_HIERARCHY, typename CACHE>
	inline bool LoadFromCacheHierarchy(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Store data into Cache CACHE. If cache or access is write-through then store also in next level caches, until memory */
	template <typename CACHE_HIERARCHY, typename CACHE>
	inline bool StoreInCacheHierarchy(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Load data/instruction from Cache CACHE or, if missing, in next levels */
	template <typename CACHE_HIERARCHY, typename CACHE>
	bool DebugLoadFromCacheHierarchy(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Store data into Cache CACHE or, if missing, in next levels */
	template <typename CACHE_HIERARCHY, typename CACHE>
	bool DebugStoreInCacheHierarchy(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Fill block from Cache TO with content from Cache FROM. If Cache FROM is disabled, then try with next level caches, until memory */
	template <typename CACHE_HIERARCHY, typename TO, typename FROM>
	bool FillBlock(CacheAccess<TYPES, TO>& to_access);

	/* Write back dirty block from Cache FROM to Cache TO. If Cache TO is disabled, then try with next level caches, until memory */
	template <typename CACHE_HIERARCHY, typename FROM, typename TO, bool INVALIDATE>
	bool WriteBackDirtyBlock(CacheAccess<TYPES, FROM>& from_access, unsigned int from_sector);
	
	/* Write back line from Cache CACHE to first enabled next level cache, or if none are enabled to memory, then optionnally invalidate line in Cache CACHE. */
	template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
	bool WriteBackLine(CacheAccess<TYPES, CACHE>& from_access);

	/* Evict line (write back and invalidate) from Cache CACHE to first enabled next level cache, or if none are enabled to memory */
	template <typename CACHE_HIERARCHY, typename CACHE>
	bool EvictLine(CacheAccess<TYPES, CACHE>& from_access);

	/* Decode address */
	template <typename CACHE>
	static inline void DecodeAddress(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& index, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;

	/* Lookup set by address */
	template <typename CACHE>
	inline CacheSet<TYPES, CACHE> *LookupSet(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	
	/* Lookup line by address */
	template <typename CACHE>
	inline CacheLine<TYPES, CACHE> *LookupLine(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	
	/* Lookup block by address */
	template <typename CACHE>
	inline CacheBlock<TYPES, CACHE> *LookupBlock(typename TYPES::ADDRESS addr, unsigned int& offset, unsigned int& size_to_block_boundary) ALWAYS_INLINE;
	
	/* Lookup by address */
	template <typename CACHE>
	inline void Lookup(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	
	/* Invalidate block by address */
	template <typename CACHE>
	inline void InvalidateBlockByAddress(typename TYPES::ADDRESS addr);

	/* Invalidate line by address */
	template <typename CACHE>
	inline void InvalidateLineByAddress(typename TYPES::ADDRESS addr);
	
	/* Invalidate block by set, way, and sector */
	template <typename CACHE>
	inline void InvalidateBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector);

	/* Invalidate line by set and way */
	template <typename CACHE>
	inline void InvalidateLineBySetAndWay(unsigned int index, unsigned int way);
	
	/* Invalidate set */
	template <typename CACHE>
	inline void InvalidateSet(unsigned int index);
	
	/* Invalidate all */
	template <typename CACHE>
	inline void InvalidateAll();
	
	/* Invalidate blocks by address in caches [CACHE..UNTIL] */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
	inline void GlobalInvalidateBlockByAddress(typename TYPES::ADDRESS addr);

	/* Invalidate lines by address in cache [CACHE..UNTIL] */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
	inline void GlobalInvalidateLineByAddress(typename TYPES::ADDRESS addr);

	/* Write back block by set, way and sector, and optionnally invalidate block */
	template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
	inline bool WriteBackBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector);

	/* Write back block by set and way, and optionnally invalidate line */
	template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
	inline bool WriteBackLineBySetAndWay(unsigned int index, unsigned int way);

	/* Write back blocks by address in caches [CACHE..UNTIL], and optionnaly invalidate blocks */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
	inline bool GlobalWriteBackBlockByAddress(typename TYPES::ADDRESS addr);

	/* Write back lines by address in caches [CACHE..UNTIL], and optionnaly invalidate lines */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
	inline bool GlobalWriteBackLineByAddress(typename TYPES::ADDRESS addr);
private:

	//////////////////// trampoline to overridable methods /////////////////////
	
	template <typename CACHE, bool dummy = true>
	struct __MSS_CACHE_INTERFACE__
	{
		static inline CACHE *GetCache(MSS *mss, const CACHE *null)  ALWAYS_INLINE { return mss->GetCache(null); }
		static inline bool IsCacheVerbose(MSS *mss, const CACHE *null) ALWAYS_INLINE { return mss->IsCacheVerbose(null); }
		static inline bool IsCacheEnabled(MSS *mss, const CACHE *null) ALWAYS_INLINE { return mss->IsCacheEnabled(null); }
		static inline bool IsCacheWriteAllocate(MSS *mss, const CACHE *null) ALWAYS_INLINE { return mss->IsCacheWriteAllocate(null); }
		static inline bool ChooseLineToEvict(MSS *mss, CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE { return mss->ChooseLineToEvict(access); }
		static inline void UpdateReplacementPolicyOnAccess(MSS *mss, CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE { mss->UpdateReplacementPolicyOnAccess(access); }
		static inline void UpdateReplacementPolicyOnFill(MSS *mss, CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE { mss->UpdateReplacementPolicyOnFill(access); }
	};
	
	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache<TYPES>, dummy>
	{
		static inline NullCache<TYPES> *GetCache(MSS *mss, const NullCache<TYPES> *null) ALWAYS_INLINE { return 0; }
		static inline bool IsCacheVerbose(MSS *mss, const NullCache<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheEnabled(MSS *mss, const NullCache<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheWriteAllocate(MSS *mss, const NullCache<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool ChooseLineToEvict(MSS *mss, CacheAccess<TYPES, NullCache<TYPES> >& access) ALWAYS_INLINE { return false; }
		static inline void UpdateReplacementPolicyOnAccess(MSS *mss, CacheAccess<TYPES, NullCache<TYPES> >& access) ALWAYS_INLINE {}
		static inline void UpdateReplacementPolicyOnFill(MSS *mss, CacheAccess<TYPES, NullCache<TYPES> >& access) ALWAYS_INLINE {}
	};

	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache2<TYPES>, dummy>
	{
		static inline NullCache2<TYPES> *GetCache(MSS *mss, const NullCache2<TYPES> *null) ALWAYS_INLINE { return 0; }
		static inline bool IsCacheVerbose(MSS *mss, const NullCache2<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheEnabled(MSS *mss, const NullCache2<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheWriteAllocate(MSS *mss, const NullCache2<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool ChooseLineToEvict(MSS *mss, CacheAccess<TYPES, NullCache2<TYPES> >& access) ALWAYS_INLINE { return false; }
		static inline void UpdateReplacementPolicyOnAccess(MSS *mss, CacheAccess<TYPES, NullCache2<TYPES> >& access) ALWAYS_INLINE {}
		static inline void UpdateReplacementPolicyOnFill(MSS *mss, CacheAccess<TYPES, NullCache2<TYPES> >& access) ALWAYS_INLINE {}
	};

	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache3<TYPES>, dummy>
	{
		static inline NullCache3<TYPES> *GetCache(MSS *mss, const NullCache3<TYPES> *null) ALWAYS_INLINE { return 0; }
		static inline bool IsCacheVerbose(MSS *mss, const NullCache3<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheEnabled(MSS *mss, const NullCache3<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheWriteAllocate(MSS *mss, const NullCache3<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool ChooseLineToEvict(MSS *mss, CacheAccess<TYPES, NullCache3<TYPES> >& access) ALWAYS_INLINE { return false; }
		static inline void UpdateReplacementPolicyOnAccess(MSS *mss, CacheAccess<TYPES, NullCache3<TYPES> >& access) ALWAYS_INLINE {}
		static inline void UpdateReplacementPolicyOnFill(MSS *mss, CacheAccess<TYPES, NullCache3<TYPES> >& access) ALWAYS_INLINE {}
	};

	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache4<TYPES>, dummy>
	{
		static inline NullCache4<TYPES> *GetCache(MSS *mss, const NullCache4<TYPES> *null) ALWAYS_INLINE { return 0; }
		static inline bool IsCacheVerbose(MSS *mss, const NullCache4<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheEnabled(MSS *mss, const NullCache4<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool IsCacheWriteAllocate(MSS *mss, const NullCache4<TYPES> *null) ALWAYS_INLINE { return false; }
		static inline bool ChooseLineToEvict(MSS *mss, CacheAccess<TYPES, NullCache4<TYPES> >& access) ALWAYS_INLINE { return false; }
		static inline void UpdateReplacementPolicyOnAccess(MSS *mss, CacheAccess<TYPES, NullCache4<TYPES> >& access) ALWAYS_INLINE {}
		static inline void UpdateReplacementPolicyOnFill(MSS *mss, CacheAccess<TYPES, NullCache4<TYPES> >& access) ALWAYS_INLINE {}
	};

	template <typename CACHE> inline CACHE *__MSS_GetCache__() ALWAYS_INLINE;
	
	inline std::ostream& __MSS_GetDebugInfoStream__() ALWAYS_INLINE;
	
	inline std::ostream& __MSS_GetDebugWarningStream__() ALWAYS_INLINE;

	inline std::ostream& __MSS_GetDebugErrorStream__() ALWAYS_INLINE;

	template <typename CACHE> inline bool __MSS_IsCacheVerbose__() ALWAYS_INLINE;
	
	inline bool __MSS_IsStorageCacheable__(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;
	
	inline bool __MSS_IsDataWriteAccessWriteThrough__(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;

	template <typename CACHE> inline bool __MSS_IsCacheEnabled__() ALWAYS_INLINE;

	template <typename CACHE> inline bool __MSS_IsCacheWriteAllocate__() ALWAYS_INLINE;
	
	template <typename CACHE> inline bool __MSS_ChooseLineToEvict__(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	
	template <typename CACHE> inline void __MSS_UpdateReplacementPolicyOnAccess__(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;

	template <typename CACHE> inline void __MSS_UpdateReplacementPolicyOnFill__(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataLoad__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataStore__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseInstructionFetch__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataBusRead__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataBusWrite__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseInstructionBusRead__() const ALWAYS_INLINE;
	
	inline bool __MSS_DataBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm) ALWAYS_INLINE;

	inline bool __MSS_DataBusWrite__(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;
	
	inline bool __MSS_InstructionBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;
	
	inline bool __MSS_DebugDataBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;

	inline bool __MSS_DebugDataBusWrite__(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;
	
	inline bool __MSS_DebugInstructionBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;

	///////////////////////////////////////////////////////////////////////////
	
	void Trace(const char *type, typename TYPES::ADDRESS addr, const void *buffer, unsigned int size);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DEFINITIONS /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////// CacheType /////////////////////////////

inline std::ostream& operator << (std::ostream& os, const CacheType& ct)
{
	switch(ct)
	{
		case NULL_CACHE       : os << "null cache"; break;
		case DATA_CACHE       : os << "data cache"; break;
		case INSTRUCTION_CACHE: os << "instruction cache"; break;
		case UNIFIED_CACHE    : os << "unified cache"; break;
		default: os << "?"; break;
	}
	return os;
}

////////////////////////////// CacheWritingPolicy /////////////////////////////

inline std::ostream& operator << (std::ostream& os, const CacheWritingPolicy& cwp)
{
	switch(cwp)
	{
		case CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY: os << "Write-through and No-Write allocate policy"; break;
		case CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY   : os << "Write-back and Write allocate policy"; break;
		case CACHE_WRITE_THROUGH_AND_WRITE_ALLOCATE_POLICY   : os << "Write-through and No-Write allocate policy"; break;
		case CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY      : os << "Write-back and Write allocate policy"; break;
		default: os << "?"; break;
	}
	return os;
}

/////////////////////////////// CacheBlock<> //////////////////////////////////

template <typename TYPES, typename CACHE>
CacheBlock<TYPES, CACHE>::CacheBlock()
	: base_addr(0)
	, status(0)
	, sector(0)
	, storage(0)
{
	status = new typename CACHE::BLOCK_STATUS();
	storage = new uint8_t[CACHE::BLOCK_SIZE];
	memset(storage, 0, CACHE::BLOCK_SIZE);
}

template <typename TYPES, typename CACHE>
CacheBlock<TYPES, CACHE>::~CacheBlock()
{
	delete status;
	delete[] storage;
}

template <typename TYPES, typename CACHE>
inline uint8_t& CacheBlock<TYPES, CACHE>::operator [] (unsigned int offset)
{
	return storage[offset];
}

template <typename TYPES, typename CACHE>
inline void CacheBlock<TYPES, CACHE>::SetBaseAddr(typename TYPES::ADDRESS addr)
{
	base_addr = addr;
}

template <typename TYPES, typename CACHE>
inline typename TYPES::ADDRESS CacheBlock<TYPES, CACHE>::GetBaseAddr() const
{
	return base_addr;
}

template <typename TYPES, typename CACHE>
inline void CacheBlock<TYPES, CACHE>::Zero()
{
	memset(storage, 0, CACHE::BLOCK_SIZE);
}

template <typename TYPES, typename CACHE>
inline void CacheBlock<TYPES, CACHE>::Write(const void *buffer, unsigned int offset, unsigned int size)
{
	assert(CACHE::BLOCK_SIZE >= (offset + size));
	memcpy(storage + offset, buffer, size);
}

template <typename TYPES, typename CACHE>
inline void CacheBlock<TYPES, CACHE>::Read(void *buffer, unsigned int offset, unsigned int size)
{
	assert(CACHE::BLOCK_SIZE >= (offset + size));
	memcpy(buffer, storage + offset, size);
}

template <typename TYPES, typename CACHE>
inline bool CacheBlock<TYPES, CACHE>::IsValid() const
{
	return status->IsValid();
}

template <typename TYPES, typename CACHE>
inline bool CacheBlock<TYPES, CACHE>::IsDirty() const
{
	return status->IsDirty();
}

template <typename TYPES, typename CACHE>
inline void CacheBlock<TYPES, CACHE>::SetValid()
{
	status->SetValid();
}

template <typename TYPES, typename CACHE>
inline void CacheBlock<TYPES, CACHE>::SetDirty()
{
	status->SetDirty();
}

template <typename TYPES, typename CACHE>
inline void CacheBlock<TYPES, CACHE>::Invalidate()
{
	status->Invalidate();
}

template <typename TYPES, typename CACHE>
template <typename BLOCK_STATUS>
inline BLOCK_STATUS& CacheBlock<TYPES, CACHE>::Status()
{
	return *static_cast<BLOCK_STATUS *>(status);
}

template <typename TYPES, typename CACHE>
void CacheBlock<TYPES, CACHE>::SetSector(unsigned int _sector)
{
	sector = _sector;
}

template <typename TYPES, typename CACHE>
inline unsigned int CacheBlock<TYPES, CACHE>::GetSector() const
{
	return sector;
}

/////////////////////////////// CacheLine<> ///////////////////////////////////

template <typename TYPES, typename CACHE>
CacheLine<TYPES, CACHE>::CacheLine()
	: valid(false)
	, next(0)
	, prev(0)
	, base_addr(0)
	, way(0)
	, status(0)
	, blocks(0)
{
	status = new typename CACHE::LINE_STATUS();
	
	blocks = new CacheBlock<TYPES, CACHE>[CACHE::BLOCKS_PER_LINE]();
	
	unsigned int sector;
	for(sector = 0; sector < CACHE::BLOCKS_PER_LINE; sector++)
	{
		blocks[sector].SetSector(sector);
	}
}

template <typename TYPES, typename CACHE>
CacheLine<TYPES, CACHE>::~CacheLine()
{
	delete status;
	delete[] blocks;
}

template <typename TYPES, typename CACHE>
inline typename TYPES::ADDRESS CacheLine<TYPES, CACHE>::GetBaseAddr() const
{
	return base_addr;
}

template <typename TYPES, typename CACHE>
inline void CacheLine<TYPES, CACHE>::SetWay(unsigned int _way)
{
	way = _way;
}

template <typename TYPES, typename CACHE>
inline void CacheLine<TYPES, CACHE>::SetBaseAddr(typename TYPES::ADDRESS base_addr)
{
	this->base_addr = base_addr;
	unsigned int sector;
	for(sector = 0; sector < CACHE::BLOCKS_PER_LINE; sector++)
	{
		blocks[sector].SetBaseAddr(base_addr + (sector * CACHE::BLOCK_SIZE));
	}
}

template <typename TYPES, typename CACHE>
inline CacheBlock<TYPES, CACHE>& CacheLine<TYPES, CACHE>::operator [] (unsigned int sector)
{
	return blocks[sector % CACHE::BLOCKS_PER_LINE];
}

template <typename TYPES, typename CACHE>
inline unsigned int CacheLine<TYPES, CACHE>::GetWay() const
{
	return way;
}

template <typename TYPES, typename CACHE>
inline bool CacheLine<TYPES, CACHE>::IsValid() const
{
	return valid;
}

template <typename TYPES, typename CACHE>
inline void CacheLine<TYPES, CACHE>::SetValid()
{
	valid = true;
}

template <typename TYPES, typename CACHE>
inline void CacheLine<TYPES, CACHE>::Invalidate()
{
	unsigned int sector;

	for(sector = 0; sector < CACHE::BLOCKS_PER_LINE; sector++)
	{
		CacheBlock<TYPES, CACHE>& block = blocks[sector];

		block.Invalidate();
	}
	
	valid = false;
	base_addr = 0;
	status->Invalidate();
}

template <typename TYPES, typename CACHE>
template <typename LINE_STATUS>
inline LINE_STATUS& CacheLine<TYPES, CACHE>::Status()
{
	return *static_cast<LINE_STATUS *>(status);
}

//////////////////////////////// CacheSet<> ///////////////////////////////////

template <typename TYPES, typename CACHE>
CacheSet<TYPES, CACHE>::CacheSet()
	: status(0)
	, mrs_line(0)
	, lrs_line(0)
	, lines(0)
	, index(0)
{
	status = new typename CACHE::SET_STATUS();
	
	lines = new CacheLine<TYPES, CACHE>[CACHE::ASSOCIATIVITY]();
	
	unsigned int way;
	
	for(way = 0; way < CACHE::ASSOCIATIVITY; way++)
	{
		CacheLine<TYPES, CACHE>& line = lines[way];
		line.SetWay(way);
		
		line.prev = (way > 0) ? &lines[way - 1] : 0;
		line.next = (way < (CACHE::ASSOCIATIVITY - 1)) ? &lines[way + 1] : 0;
	}
	
	mrs_line = &lines[0];
	lrs_line = &lines[CACHE::ASSOCIATIVITY - 1];
}

template <typename TYPES, typename CACHE>
CacheSet<TYPES, CACHE>::~CacheSet()
{
	delete status;
	delete[] lines;
}

template <typename TYPES, typename CACHE>
inline CacheLine<TYPES, CACHE>& CacheSet<TYPES, CACHE>::operator [] (unsigned int way)
{
	return lines[way % CACHE::ASSOCIATIVITY];
}

template <typename TYPES, typename CACHE>
inline void CacheSet<TYPES, CACHE>::InvalidateLineByWay(unsigned int way)
{
	CacheLine<TYPES, CACHE>& line = lines[way % CACHE::ASSOCIATIVITY];
	line.Invalidate();
}

template <typename TYPES, typename CACHE>
inline void CacheSet<TYPES, CACHE>::Invalidate()
{
	unsigned int way;

	for(way = 0; way < CACHE::ASSOCIATIVITY; way++)
	{
		CacheLine<TYPES, CACHE>& line = lines[way];
		
		line.Invalidate();

		line.prev = (way > 0) ? &lines[way - 1] : 0;
		line.next = (way < (CACHE::ASSOCIATIVITY - 1)) ? &lines[way + 1] : 0;
	}
	mrs_line = &lines[0];
	lrs_line = &lines[CACHE::ASSOCIATIVITY - 1];
	
	status->Invalidate();
}

template <typename TYPES, typename CACHE>
inline void CacheSet<TYPES, CACHE>::SetIndex(unsigned int _index)
{
	index = _index;
}

template <typename TYPES, typename CACHE>
inline unsigned int CacheSet<TYPES, CACHE>::GetIndex() const
{
	return index;
}

template <typename TYPES, typename CACHE>
template <typename SET_STATUS>
inline SET_STATUS& CacheSet<TYPES, CACHE>::Status()
{
	return *static_cast<SET_STATUS *>(status);
}

template <typename TYPES, typename CACHE>
inline CacheLine<TYPES, CACHE> *CacheSet<TYPES, CACHE>::AssociativeSearch(typename TYPES::ADDRESS line_base_addr)
{
	CacheLine<TYPES, CACHE> *line;
	
	// Associative search from most recently searched to least recently searched
	for(line = mrs_line; line != 0; line = line->next)
	{
		if(likely(line->IsValid() && (line->GetBaseAddr() == line_base_addr)))
		{
			// Hit
			
			// Update MRS/LRS scheme for fast line search
			if(unlikely(line->prev != 0))
			{
				if(line->next)
				{
					line->next->prev = line->prev;
				}
				else
				{
					lrs_line = line->prev;
				}
				line->prev->next = line->next;
				line->prev = 0;
				line->next = mrs_line;
				mrs_line->prev = line;
				mrs_line = line;
			}
			
			return line;
		}
	}
	
	return 0;
}

////////////////////////////////// Cache<> ////////////////////////////////////

template <typename TYPES, typename CACHE>
Cache<TYPES, CACHE>::Cache()
	: num_accesses(0)
	, num_misses(0)
	, sets(0)
{
	sets = new CacheSet<TYPES, CACHE>[CACHE::SIZE / CACHE::BLOCK_SIZE / CACHE::BLOCKS_PER_LINE / CACHE::ASSOCIATIVITY]();
	
	unsigned int index;
	for(index = 0; index < (CACHE::SIZE / CACHE::BLOCK_SIZE / CACHE::BLOCKS_PER_LINE / CACHE::ASSOCIATIVITY); index++)
	{
		sets[index].SetIndex(index);
	}
}

template <typename TYPES, typename CACHE>
Cache<TYPES, CACHE>::~Cache()
{
	delete[] sets;
}

template <typename TYPES, typename CACHE>
inline void Cache<TYPES, CACHE>::DecodeAddress(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& index, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	offset = addr % CACHE::BLOCK_SIZE;
	block_base_addr = addr & ~(CACHE::BLOCK_SIZE - 1);
	size_to_block_boundary = CACHE::BLOCK_SIZE - offset;
	line_base_addr = addr & ~((CACHE::BLOCK_SIZE * CACHE::BLOCKS_PER_LINE) - 1);
	sector = (addr / CACHE::BLOCK_SIZE) % CACHE::BLOCKS_PER_LINE;
	index = (addr % CACHE::SIZE) / CACHE::BLOCK_SIZE / CACHE::BLOCKS_PER_LINE / CACHE::ASSOCIATIVITY;
}

template <typename TYPES, typename CACHE>
inline CacheSet<TYPES, CACHE>& Cache<TYPES, CACHE>::operator [] (unsigned int index)
{
	return sets[index % (CACHE::SIZE / CACHE::BLOCK_SIZE / CACHE::BLOCKS_PER_LINE / CACHE::ASSOCIATIVITY)];
}

template <typename TYPES, typename CACHE>
inline void Cache<TYPES, CACHE>::InvalidateSet(unsigned int index)
{
	CacheSet<TYPES, CACHE>& set = sets[index];

	set.Invalidate();
}

template <typename TYPES, typename CACHE>
inline void Cache<TYPES, CACHE>::InvalidateLineBySetAndWay(unsigned int index, unsigned int way)
{
	CacheSet<TYPES, CACHE>& set = sets[index % (CACHE::SIZE / CACHE::BLOCK_SIZE / CACHE::BLOCKS_PER_LINE / CACHE::ASSOCIATIVITY)];
	
	set.InvalidateLineByWay(way);
}

template <typename TYPES, typename CACHE>
inline void Cache<TYPES, CACHE>::Invalidate()
{
	unsigned int index;
	
	for(index = 0; index < (CACHE::SIZE / CACHE::BLOCK_SIZE / CACHE::BLOCKS_PER_LINE / CACHE::ASSOCIATIVITY); index++)
	{
		CacheSet<TYPES, CACHE>& set = sets[index];
		set.Invalidate();
	}
}

template <typename TYPES, typename CACHE>
inline CacheSet<TYPES, CACHE> *Cache<TYPES, CACHE>::LookupSet(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	unsigned int index;
	
	// Decode the address
	DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	
	CacheSet<TYPES, CACHE> *set = &sets[index];
	
	return set;
}

template <typename TYPES, typename CACHE>
inline CacheLine<TYPES, CACHE> *Cache<TYPES, CACHE>::LookupLine(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	typename TYPES::ADDRESS line_base_addr;
	
	CacheSet<TYPES, CACHE> *set = LookupSet(addr, line_base_addr, block_base_addr, sector, offset, size_to_block_boundary);
	CacheLine<TYPES, CACHE> *line = set->AssociativeSearch(line_base_addr);
	
	return line;
}

template <typename TYPES, typename CACHE>
inline CacheBlock<TYPES, CACHE> *Cache<TYPES, CACHE>::LookupBlock(typename TYPES::ADDRESS addr, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	typename TYPES::ADDRESS block_base_addr;
	unsigned int sector;
	CacheLine<TYPES, CACHE> *line = LookupLine(addr, block_base_addr, sector, offset, size_to_block_boundary);
	
	if(line)
	{
		CacheBlock<TYPES, CACHE> *block = &line->blocks[sector];
		
		return block->IsValid() ? block : 0;
	}
	
	return 0;
}

template <typename TYPES, typename CACHE>
inline void Cache<TYPES, CACHE>::Lookup(CacheAccess<TYPES, CACHE>& access)
{
	typename TYPES::ADDRESS addr = access.addr;
	typename TYPES::ADDRESS line_base_addr;
	typename TYPES::ADDRESS block_base_addr;
	unsigned int index;
	unsigned int sector;
	unsigned int offset;
	unsigned int size_to_block_boundary;
	CacheSet<TYPES, CACHE> *set;
	CacheLine<TYPES, CACHE> *line;
	CacheBlock<TYPES, CACHE> *block;
	
	num_accesses++;
	// Decode the address
	DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	access.line_base_addr = line_base_addr;
	access.block_base_addr = block_base_addr;
	access.index = index;
	access.sector = sector;
	access.offset = offset;
	access.size_to_block_boundary = size_to_block_boundary;
	access.set = set = &sets[index];

	line = set->AssociativeSearch(line_base_addr);
	
	if(line)
	{
		// line hit: block may need a fill if not yet present in the line
		access.line = line;
		access.line_to_write_back_evict = 0;
		access.way = line->GetWay();
		block = &(*line)[sector];
		access.block = block->IsValid() ? block : 0;

		return;
	}

	// line miss
	access.line = 0;
	access.block = 0;
	num_misses++;
}

template <typename TYPES, typename CACHE>
std::string Cache<TYPES, CACHE>::GenName()
{
	std::string generated_cache_typename;
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(HAVE_CXXABI)
	int status = -1;
	char *demangled_cache_typename = abi::__cxa_demangle(typeid(CACHE).name(), NULL, NULL, &status);
	if(status == 0)
	{
		std::string unstripped_demangled_cache_typename(demangled_cache_typename);
		std::size_t pos = unstripped_demangled_cache_typename.find_last_of("::");
		if(pos != std::string::npos)
		{
			generated_cache_typename = unstripped_demangled_cache_typename.substr(pos + 1);
		}
		else
		{
			generated_cache_typename = demangled_cache_typename;
		}
	}
	else
	{
		generated_cache_typename = typeid(CACHE).name();
	}
	std::free(demangled_cache_typename);
#else
	generated_cache_typename = typeid(CACHE).name();
#endif
	return generated_cache_typename;
}

template <typename TYPES, typename CACHE>
const char *Cache<TYPES, CACHE>::GetName()
{
	static std::string cache_typename = GenName();
	
	return cache_typename.c_str();
}

template <typename TYPES, typename CACHE>
inline const char *Cache<TYPES, CACHE>::__MSS_GetName__()
{
	return CACHE::GetName();
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsNullCache()
{
	return CACHE::TYPE == NULL_CACHE;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsDataCacheOnly()
{
	return CACHE::TYPE == DATA_CACHE;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsDataCache()
{
	return CACHE::TYPE & DATA_CACHE;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsInstructionCacheOnly()
{
	return CACHE::TYPE == INSTRUCTION_CACHE;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsInstructionCache()
{
	return CACHE::TYPE & INSTRUCTION_CACHE;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsUnifiedCache()
{
	return CACHE::TYPE == UNIFIED_CACHE;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsWriteBackCache()
{
	return CACHE::WRITING_POLICY & CACHE_WRITE_BACK;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsWriteThroughCache()
{
	return !(CACHE::WRITING_POLICY & CACHE_WRITE_BACK);
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsWriteAllocate()
{
	return CACHE::WRITING_POLICY & CACHE_WRITE_ALLOCATE;
}

template <typename TYPES, typename CACHE>
inline bool Cache<TYPES, CACHE>::IsNotWriteAllocate()
{
	return !(CACHE::WRITING_POLICY & CACHE_WRITE_ALLOCATE);
}

template <typename TYPES, typename CACHE>
inline unsigned int Cache<TYPES, CACHE>::NumSets()
{
	return CACHE::SIZE / CACHE::BLOCK_SIZE / CACHE::BLOCKS_PER_LINE / CACHE::ASSOCIATIVITY;
}

template <typename TYPES, typename CACHE>
inline unsigned int Cache<TYPES, CACHE>::NumWays()
{
	return CACHE::ASSOCIATIVITY;
}

template <typename TYPES, typename CACHE>
inline unsigned int Cache<TYPES, CACHE>::NumSectors()
{
	return CACHE::BLOCKS_PER_LINE;
}

////////////////////////////// MemorySubSystem<> ////////////////////////////////

template <typename TYPES, typename MSS>
MemorySubSystem<TYPES, MSS>::MemorySubSystem()
{
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataLoad(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!LoadFromCacheHierarchy<typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, buffer, size, storage_attr)) return false;
	
	if(unlikely(__MSS_IsVerboseDataLoad__())) Trace("Loading Data", addr, buffer, size);
	
	return true;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataStore(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(unlikely(__MSS_IsVerboseDataStore__())) Trace("Storing Data", addr, buffer, size);

	return StoreInCacheHierarchy<typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::InstructionFetch(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!LoadFromCacheHierarchy<typename MSS::INSTRUCTION_CACHE_HIERARCHY, typename MSS::INSTRUCTION_CACHE_HIERARCHY::L1CACHE>(addr, buffer, size, storage_attr)) return false;
	
	if(unlikely(__MSS_IsVerboseInstructionFetch__())) Trace("Fetching Instruction", addr, buffer, size);
	
	return true;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataLoad(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	bool status = true;
	
	if(size)
	{
		unsigned int load_offset = 0;
		
		do
		{
			unsigned int size_to_block_boundary = MSS::DATA_CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE - (addr % MSS::DATA_CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE);
			unsigned int load_size = (size_to_block_boundary > size) ? size : size_to_block_boundary;
			
			if(!DebugLoadFromCacheHierarchy<typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, (uint8_t *) buffer + load_offset, load_size, storage_attr))
			{
				status = false;
			}
			
			size -= load_size;
			addr += load_size;
			load_offset += load_size;
		}
		while(size);
	}
	
	return status;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataStore(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	bool status = true;
	
	if(size)
	{
		unsigned int store_offset = 0;
		
		do
		{
			unsigned int size_to_block_boundary = MSS::DATA_CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE - (addr % MSS::DATA_CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE);
			unsigned int store_size = (size_to_block_boundary > size) ? size : size_to_block_boundary;
			
			if(unlikely((!DebugStoreInCacheHierarchy<typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, (uint8_t *) buffer + store_offset, store_size, storage_attr))))
			{
				status = false;
			}
			
			size -= store_size;
			addr += store_size;
			store_offset += store_size;
		}
		while(size);
	}
	
	return status;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugInstructionFetch(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	bool status = true;
	
	if(size)
	{
		unsigned int fetch_offset = 0;
		
		do
		{
			unsigned int size_to_block_boundary = MSS::INSTRUCTION_CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE - (addr % MSS::INSTRUCTION_CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE);
			unsigned int fetch_size = (size_to_block_boundary > size) ? size : size_to_block_boundary;
			
			if(unlikely((!DebugLoadFromCacheHierarchy<typename MSS::INSTRUCTION_CACHE_HIERARCHY, typename MSS::INSTRUCTION_CACHE_HIERARCHY::L1CACHE>(addr, (uint8_t *) buffer + fetch_offset, fetch_size, storage_attr))))
			{
				status = false;
			}
			
			size -= fetch_size;
			addr += fetch_size;
			fetch_offset += fetch_size;
		}
		while(size);
	}
	
	return status;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::LoadFromCacheHierarchy(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Load from memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, load from this cache
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) Load from memory
		if(typeid(CACHE_HIERARCHY) == typeid(typename MSS::DATA_CACHE_HIERARCHY))
		{
			if(unlikely(!__MSS_DataBusRead__(addr, buffer, size, storage_attr, false)))
			{
				return false;
			}
		}
		else
		{
			if(unlikely(!__MSS_InstructionBusRead__(addr, buffer, size, storage_attr)))
			{
				return false;
			}
		}
	}
	else if(unlikely(!__MSS_IsCacheEnabled__<CACHE>()))
	{
		// (2) Load from next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!LoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr)) return false;
	}
	else
	{
		// (3) Load from this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		access.storage_attr = storage_attr;
		access.rwitm = false;
		
		CACHE *cache = __MSS_GetCache__<CACHE>();
		cache->Lookup(access);
		if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Lookup at @0x"
			                             << std::hex << access.addr << std::dec << " : "
			                             << "line_base_addr=0x" << std::hex << access.line_base_addr << std::dec << ","
			                             << "index=0x" << std::hex << access.index << std::dec << ","
			                             << "sector=0x" << std::hex << access.sector << std::dec << ","
			                             << "offset=0x" << std::hex << access.offset << std::dec << ","
			                             << "size_to_block_boundary=0x" << std::hex << access.size_to_block_boundary << std::dec
			                             << std::endl;
		}
		
		if(unlikely(!access.line))
		{
			// Line miss
			if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Line miss at @0x" << std::hex << access.addr << std::dec << std::endl;
			}

			if(likely(__MSS_ChooseLineToEvict__(access)))
			{
				if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Line miss: choosen way=" << access.way << std::endl;
				}
				
				if(unlikely((!EvictLine<CACHE_HIERARCHY, CACHE>(access)))) return false;
			}
		}
		else
		{
			if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Line hit: index=" << access.index << ", way=" << access.line->GetWay() << std::endl;
			}
		}

		if(likely(access.line))
		{
			if(unlikely(!access.block))
			{
				// Block miss
				if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Block miss at @0x" << std::hex << access.addr << std::dec << std::endl;
				}
				
				typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
				
				if(unlikely((!FillBlock<CACHE_HIERARCHY, CACHE, NLC>(access)))) return false;
				__MSS_UpdateReplacementPolicyOnFill__(access);
			}
			else
			{
				if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Block hit at @0x" << std::hex << access.addr << std::dec << std::endl;
				}
			}

			assert(size <= access.size_to_block_boundary);
			memcpy(buffer, &(*access.block)[access.offset], size);
			__MSS_UpdateReplacementPolicyOnAccess__(access);
		}
		else
		{
			// Line miss and locked way: Load from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			if(!LoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr)) return false;
		}
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::StoreInCacheHierarchy(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Store to memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, store in this cache
	if(unlikely((CACHE::TYPE == NULL_CACHE) || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) Store to memory
		if(unlikely(!__MSS_DataBusWrite__(addr, buffer, size, storage_attr)))
		{
			return false;
		}
	}
	else if(unlikely(!__MSS_IsCacheEnabled__<CACHE>()))
	{
		// (2) Store in next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!StoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr)) return false;
	}
	else
	{
		// (3) store in this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		access.storage_attr = storage_attr;
		access.rwitm = true;
		
		CACHE *cache = __MSS_GetCache__<CACHE>();
		cache->Lookup(access);
		if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Lookup at @0x"
			                             << std::hex << access.addr << std::dec << " : "
			                             << "line_base_addr=0x" << std::hex << access.line_base_addr << std::dec << ","
			                             << "index=0x" << std::hex << access.index << std::dec << ","
			                             << "sector=0x" << std::hex << access.sector << std::dec << ","
			                             << "offset=0x" << std::hex << access.offset << std::dec << ","
			                             << "size_to_block_boundary=0x" << std::hex << access.size_to_block_boundary << std::dec
			                             << std::endl;
		}
		
		if(unlikely(!access.line))
		{
			// Line miss
			if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Line miss at @0x" << std::hex << access.addr << std::dec << std::endl;
			}

			if(CACHE::IsWriteAllocate() || __MSS_IsCacheWriteAllocate__<CACHE>())
			{
				// write-allocate: evict before fill
				if(likely(__MSS_ChooseLineToEvict__(access)))
				{
					if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
					{
						__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Line miss: choosen way=" << access.way << std::endl;
					}

					if(unlikely((!EvictLine<CACHE_HIERARCHY, CACHE>(access)))) return false;
				}
			}
		}
		else
		{
			if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Line hit: index=" << access.index << ", way=" << access.line->GetWay() << std::endl;
			}
		}
		
		if(likely(access.line))
		{
			if(unlikely(!access.block))
			{
				// Block miss
				if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Block miss at @0x" << std::hex << access.addr << std::dec << std::endl;
				}
				
				if(CACHE::IsWriteAllocate() || __MSS_IsCacheWriteAllocate__<CACHE>())
				{
					// write-allocate: fill if missing block
					typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
					
					if(unlikely((!FillBlock<CACHE_HIERARCHY, CACHE, NLC>(access)))) return false;
					__MSS_UpdateReplacementPolicyOnFill__(access);
				}
			}
			else
			{
				if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << " Block hit at @0x" << std::hex << access.addr << std::dec << std::endl;
				}
			}

			if(CACHE::IsWriteAllocate() || __MSS_IsCacheWriteAllocate__<CACHE>() || access.block)
			{
				// write-allocate or cache hit
				assert(size <= access.size_to_block_boundary);
				memcpy(&(*access.block)[access.offset], buffer, size);
				if(CACHE::IsWriteBackCache())
				{
					// write-back
					access.block->SetDirty();
				}
				__MSS_UpdateReplacementPolicyOnAccess__(access);
			}
			
			if(CACHE::IsWriteThroughCache() || __MSS_IsDataWriteAccessWriteThrough__(storage_attr))
			{
				typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
				
				if(unlikely((!StoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr)))) return false;
			}
		}
		else
		{
			// Line miss and locked way: Store to next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			if(!StoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr)) return false;
		}
	}

	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
bool MemorySubSystem<TYPES, MSS>::DebugLoadFromCacheHierarchy(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Debug Load from memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, debug load from this cache
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) Load from memory
		if(typeid(CACHE_HIERARCHY) == typeid(typename MSS::DATA_CACHE_HIERARCHY))
		{
			return __MSS_DebugDataBusRead__(addr, buffer, size, storage_attr);
		}
		else
		{
			return __MSS_DebugInstructionBusRead__(addr, buffer, size, storage_attr);
		}
	}
	else if(unlikely(!__MSS_IsCacheEnabled__<CACHE>()))
	{
		// (2) Load from next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		return DebugLoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr);
	}
	else
	{
		// (3) Debug Load from this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		
		CACHE *cache = __MSS_GetCache__<CACHE>();
		cache->Lookup(access);

		if(unlikely(!access.block))
		{
			// miss: try debug loading from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			return DebugLoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr);
		}

		assert(size <= access.size_to_block_boundary);
		memcpy(buffer, &(*access.block)[access.offset], size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
bool MemorySubSystem<TYPES, MSS>::DebugStoreInCacheHierarchy(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Debug Store to memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, debug store into this cache
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) debug store to memory
		return __MSS_DebugDataBusWrite__(addr, buffer, size, storage_attr);
	}
	else if(unlikely(!__MSS_IsCacheEnabled__<CACHE>()))
	{
		// (2) Store into next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		return DebugStoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr);
	}
	else
	{
		// (3) Debug Store into this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		
		CACHE *cache = __MSS_GetCache__<CACHE>();
		cache->Lookup(access);

		if(unlikely(!access.block))
		{
			// miss: try debug loading from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			return DebugStoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, buffer, size, storage_attr);
		}

		assert(size <= access.size_to_block_boundary);
		memcpy(&(*access.block)[access.offset], buffer, size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename TO, typename FROM>
inline bool MemorySubSystem<TYPES, MSS>::FillBlock(CacheAccess<TYPES, TO>& to_access)
{
	// 3 cases:
	//     - (1) Fill block from memory
	//     - (2) if cache is disabled, try to fill block from next level cache
	//     - (3) if cache is enabled, fill block from cache
	
	to_access.block = &(*to_access.line)[to_access.sector];
	
	if(unlikely(FROM::IsNullCache()))
	{
		// (1) Block Fill from Memory
		if(unlikely(__MSS_IsCacheVerbose__<TO>()))
		{
			__MSS_GetDebugInfoStream__() << TO::__MSS_GetName__() << ": Filling block from memory at @0x" << std::hex << to_access.block_base_addr << std::dec << std::endl;
		}
		
		if(TO::IsDataCache())
		{
			if(unlikely(!__MSS_DataBusRead__(to_access.block_base_addr, &(*to_access.block)[0], TO::BLOCK_SIZE, to_access.storage_attr, to_access.rwitm)))
			{
				return false;
			}
		}
		else
		{
			if(unlikely(!__MSS_InstructionBusRead__(to_access.block_base_addr, &(*to_access.block)[0], TO::BLOCK_SIZE, to_access.storage_attr)))
			{
				return false;
			}
		}
	}
	else if(unlikely(!__MSS_IsCacheEnabled__<FROM>()))
	{
		// (2) fill block from next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
		
		if(unlikely((!FillBlock<CACHE_HIERARCHY, TO, NLC>(to_access)))) return false;
		__MSS_UpdateReplacementPolicyOnFill__(to_access);
	}
	else
	{
		// (3) cache is enabled, try to fill block from cache
		
		// 2 sub-cases:
		//   - (3.1) if block fill hits then fill block from cache
		//   - (3.2) if block fill misses then try to fill block from next level cache

		if(unlikely(__MSS_IsCacheVerbose__<TO>()))
		{
			__MSS_GetDebugInfoStream__() << TO::__MSS_GetName__() << ": Filling block from " << FROM::__MSS_GetName__() << " at @0x" << std::hex << to_access.block_base_addr << std::dec << std::endl;
		}
		
		CacheAccess<TYPES, FROM> from_access;
		
		from_access.addr = to_access.block_base_addr;
		from_access.storage_attr = to_access.storage_attr;
		from_access.rwitm = to_access.rwitm;

		FROM *from = __MSS_GetCache__<FROM>();
		from->Lookup(from_access);
		if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
		{
			__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Lookup at @0x"
			                             << std::hex << from_access.addr << std::dec << " : "
			                             << "line_base_addr=0x" << std::hex << from_access.line_base_addr << std::dec << ","
			                             << "index=0x" << std::hex << from_access.index << std::dec << ","
			                             << "sector=0x" << std::hex << from_access.sector << std::dec << ","
			                             << "offset=0x" << std::hex << from_access.offset << std::dec << ","
			                             << "size_to_block_boundary=0x" << std::hex << from_access.size_to_block_boundary << std::dec
			                             << std::endl;
		}

		if(unlikely(!from_access.line))
		{
			// Line miss
			if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
			{
				__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Line miss at @0x" << std::hex << from_access.addr << std::dec << std::endl;
			}
			
			if(likely(__MSS_ChooseLineToEvict__(from_access)))
			{
				if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
				{
					__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Line miss: choosen way=" << from_access.way << std::endl;
				}
				
				if(unlikely((!EvictLine<CACHE_HIERARCHY, FROM>(from_access)))) return false;
			}
		}
		else
		{
			if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
			{
				__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Line hit: index=" << from_access.index << ", way=" << from_access.line->GetWay() << std::endl;
			}
		}

		if(likely(from_access.line))
		{
			if(unlikely(!from_access.block))
			{
				// (3.2) fill block from next level cache
				// Block miss
				if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
				{
					__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Block miss at @0x" << std::hex << from_access.addr << std::dec << std::endl;
				}
				
				typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
				
				if(unlikely((!FillBlock<CACHE_HIERARCHY, FROM, NLC>(from_access)))) return false;
				__MSS_UpdateReplacementPolicyOnFill__(from_access);
			}
			else
			{
				// (3.1) fill block from cache
				if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
				{
					__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << " Block hit at @0x" << std::hex << from_access.addr << std::dec << std::endl;
				}
			}

			assert(TO::BLOCK_SIZE <= from_access.size_to_block_boundary);
			memcpy(&(*to_access.block)[0], &(*from_access.block)[from_access.offset], TO::BLOCK_SIZE); // <-- block fill
			__MSS_UpdateReplacementPolicyOnAccess__(from_access);
		}
		else
		{
			// Line miss and locked way: fill from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
			
			if(unlikely((!FillBlock<CACHE_HIERARCHY, TO, NLC>(to_access)))) return false;
			__MSS_UpdateReplacementPolicyOnFill__(to_access);
		}
	}
	
	to_access.line->SetValid();
	to_access.line->SetBaseAddr(to_access.line_base_addr);
	to_access.block->SetValid();
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename FROM, typename TO, bool INVALIDATE>
bool MemorySubSystem<TYPES, MSS>::WriteBackDirtyBlock(CacheAccess<TYPES, FROM>& from_access, unsigned int from_sector)
{
	CacheBlock<TYPES, FROM>& from_dirty_block_to_write_back = (*from_access.line_to_write_back_evict)[from_sector];
	
	// 3 cases:
	//   - (1) write back to memory
	//   - (2) if cache is disabled, try to write back to next level cache
	//   - (3) if cache is enabled, write back to cache
	if(unlikely(TO::IsNullCache()))
	{
		// (1) Write back block to memory
		if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
		{
			__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Writing back dirty block to memory 0x" << std::hex << from_dirty_block_to_write_back.GetBaseAddr() << std::dec << std::endl;
		}
		if(unlikely(!__MSS_DataBusWrite__(from_dirty_block_to_write_back.GetBaseAddr(), &from_dirty_block_to_write_back[0], FROM::BLOCK_SIZE, typename TYPES::STORAGE_ATTR())))
		{
			return false;
		}
	}
	else if(unlikely(!__MSS_IsCacheEnabled__<TO>()))
	{
		// (2) try to write back to next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
		
		if(!WriteBackDirtyBlock<CACHE_HIERARCHY, FROM, NLC, INVALIDATE>(from_access, from_sector)) return false;
	}
	else
	{
		// (3) cache is enabled, try to write back to cache
		
		// 2 sub-cases:
		//   - (3.1) if write back hits then write back to cache
		//   - (3.2) if write back misses then try to write back in next level cache
		CacheAccess<TYPES, TO> to_access;

		to_access.addr = from_dirty_block_to_write_back.GetBaseAddr();
		
		TO *to = __MSS_GetCache__<TO>();
		to->Lookup(to_access);
		if(unlikely(__MSS_IsCacheVerbose__<TO>()))
		{
			__MSS_GetDebugInfoStream__() << TO::__MSS_GetName__() << ": Lookup at @0x"
			                             << std::hex << to_access.addr << std::dec << " : "
			                             << "line_base_addr=0x" << std::hex << to_access.line_base_addr << std::dec << ","
			                             << "index=0x" << std::hex << to_access.index << std::dec << ","
			                             << "sector=0x" << std::hex << to_access.sector << std::dec << ","
			                             << "offset=0x" << std::hex << to_access.offset << std::dec << ","
			                             << "size_to_block_boundary=0x" << std::hex << to_access.size_to_block_boundary << std::dec
			                             << std::endl;
		}

		if(to_access.block)
		{
			// (3.1) write back to cache
			// block hit
			if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
			{
				__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Writing back dirty block to " << TO::__MSS_GetName__() << " at @0x" << std::hex << from_dirty_block_to_write_back.GetBaseAddr() << std::dec << std::endl;
			}
			assert(FROM::BLOCK_SIZE <= to_access.size_to_block_boundary);
			memcpy(&(*to_access.block)[to_access.offset], &from_dirty_block_to_write_back[0], FROM::BLOCK_SIZE); // <-- write back
			to_access.block->SetDirty();
		}
		else
		{
			// (3.2) try to write back in next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
			
			if(!WriteBackDirtyBlock<CACHE_HIERARCHY, FROM, NLC, INVALIDATE>(from_access, from_sector)) return false;
		}
	}
	
	// optionnally invalidate block
	if(INVALIDATE)
	{
		if(unlikely(__MSS_IsCacheVerbose__<FROM>()))
		{
			__MSS_GetDebugInfoStream__() << FROM::__MSS_GetName__() << ": Invalidating block at 0x" << std::hex << from_dirty_block_to_write_back.GetBaseAddr() << std::dec << std::endl;
		}
		from_dirty_block_to_write_back.Invalidate();
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
bool MemorySubSystem<TYPES, MSS>::WriteBackLine(CacheAccess<TYPES, CACHE>& access)
{
	if(access.line_to_write_back_evict->IsValid())
	{
		if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Writing back Line at @0x" << std::hex << access.line_to_write_back_evict->GetBaseAddr() << std::dec << std::endl;
		}
		if(CACHE::IsWriteBackCache())
		{
			// write-back cache
			unsigned int sector;
		
			for(sector = 0; sector < CACHE::BLOCKS_PER_LINE; sector++)
			{
				CacheBlock<TYPES, CACHE>& block_to_evict = (*access.line_to_write_back_evict)[sector];
		
				if(block_to_evict.IsValid() && block_to_evict.IsDirty())
				{
					// write back dirty block
					typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
					
					if(!WriteBackDirtyBlock<CACHE_HIERARCHY, CACHE, NLC, INVALIDATE>(access, sector)) return false;
				}
			}
		}
		
		if(INVALIDATE)
		{
			if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Line at @0x" << std::hex << access.line_to_write_back_evict->GetBaseAddr() << std::dec << std::endl;
			}
			access.line_to_write_back_evict->Invalidate();
		}
	}
	else
	{
		if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Nothing to write back because Line is invalid" << std::endl;
		}
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
bool MemorySubSystem<TYPES, MSS>::EvictLine(CacheAccess<TYPES, CACHE>& access)
{
	if(CACHE::IsDataCache() && CACHE::IsWriteBackCache())
	{
		WriteBackLine<CACHE_HIERARCHY, CACHE, /* invalidate */ true >(access);
	}
	else
	{
		if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Line at @0x" << std::hex << access.line_to_write_back_evict->GetBaseAddr() << std::dec << std::endl;
		}
		access.line_to_write_back_evict->Invalidate();
	}
	
	access.line = access.line_to_write_back_evict;
	access.line_to_write_back_evict = 0;
	
	return true;
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::GetDebugInfoStream()
{
	return std::cout;
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::GetDebugWarningStream()
{
	return std::cerr;
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::GetDebugErrorStream()
{
	return std::cerr;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataLoad() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataStore() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseInstructionFetch() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataBusRead() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataBusWrite() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseInstructionBusRead() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsStorageCacheable(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsDataWriteAccessWriteThrough(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataBusWrite(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::InstructionBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataBusWrite(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugInstructionBusRead(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
void MemorySubSystem<TYPES, MSS>::Trace(const char *type, typename TYPES::ADDRESS addr, const void *buffer, unsigned int size)
{
	unsigned int i;
	__MSS_GetDebugInfoStream__() << "Memory Sub-System: " << type << " [";
	for(i = 0; i < size; i++)
	{
		uint8_t value = ((uint8_t *) buffer)[i];
		uint8_t l = value & 15;
		uint8_t h = value >> 4;
		__MSS_GetDebugInfoStream__() << (i ? " ": "") << "0x" << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10);
	}
	__MSS_GetDebugInfoStream__() << "] (" << size << " bytes) at @0x" << std::hex << addr << std::dec << std::endl;
}

template <typename TYPES, typename MSS>
template <typename CACHE> inline CACHE *MemorySubSystem<TYPES, MSS>::__MSS_GetCache__()
{
	CACHE *cache = __MSS_CACHE_INTERFACE__<CACHE>::GetCache(static_cast<MSS *>(this), (const CACHE *) 0);
	return cache;
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsCacheVerbose__()
{
	return __MSS_CACHE_INTERFACE__<CACHE>::IsCacheVerbose(static_cast<MSS *>(this), (const CACHE *) 0);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsCacheEnabled__()
{
	return __MSS_CACHE_INTERFACE__<CACHE>::IsCacheEnabled(static_cast<MSS *>(this), (const CACHE *) 0);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsCacheWriteAllocate__()
{
	return __MSS_CACHE_INTERFACE__<CACHE>::IsCacheWriteAllocate(static_cast<MSS *>(this), (const CACHE *) 0);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_ChooseLineToEvict__(CacheAccess<TYPES, CACHE>& access)
{
	if(likely(__MSS_CACHE_INTERFACE__<CACHE>::ChooseLineToEvict(static_cast<MSS *>(this), access)))
	{
		access.line_to_write_back_evict = &(*access.set)[access.way];
		return true;
	}
	return false;
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::__MSS_UpdateReplacementPolicyOnAccess__(CacheAccess<TYPES, CACHE>& access)
{
	__MSS_CACHE_INTERFACE__<CACHE>::UpdateReplacementPolicyOnAccess(static_cast<MSS *>(this), access);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::__MSS_UpdateReplacementPolicyOnFill__(CacheAccess<TYPES, CACHE>& access)
{
	__MSS_CACHE_INTERFACE__<CACHE>::UpdateReplacementPolicyOnFill(static_cast<MSS *>(this), access);
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::__MSS_GetDebugInfoStream__()
{
	return static_cast<MSS *>(this)->template GetDebugInfoStream();
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::__MSS_GetDebugWarningStream__()
{
	return static_cast<MSS *>(this)->template GetDebugWarningStream();
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::__MSS_GetDebugErrorStream__()
{
	return static_cast<MSS *>(this)->template GetDebugErrorStream();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsStorageCacheable__(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return static_cast<const MSS *>(this)->IsStorageCacheable(storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsDataWriteAccessWriteThrough__(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return static_cast<const MSS *>(this)->IsDataWriteAccessWriteThrough(storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataLoad__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataLoad();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataStore__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataStore();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseInstructionFetch__() const
{
	return static_cast<const MSS *>(this)->IsVerboseInstructionFetch();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataBusRead__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataBusRead();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataBusWrite__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataBusWrite();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseInstructionBusRead__() const
{
	return static_cast<const MSS *>(this)->IsVerboseInstructionBusRead();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DataBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm)
{
	if(!static_cast<MSS *>(this)->DataBusRead(addr, buffer, size, storage_attr, rwitm)) return false;

	if(unlikely(__MSS_IsVerboseDataBusRead__())) Trace("Reading from Bus Data", addr, buffer, size);

	return true;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DataBusWrite__(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(unlikely(__MSS_IsVerboseDataBusWrite__())) Trace("Writing to Bus Data", addr, buffer, size);

	return static_cast<MSS *>(this)->DataBusWrite(addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_InstructionBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!static_cast<MSS *>(this)->InstructionBusRead(addr, buffer, size, storage_attr)) return false;

	if(unlikely(__MSS_IsVerboseInstructionBusRead__())) Trace("Reading from Bus Instruction", addr, buffer, size);
	
	return true;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DebugDataBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return static_cast<MSS *>(this)->DebugDataBusRead(addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DebugDataBusWrite__(typename TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return static_cast<MSS *>(this)->DebugDataBusWrite(addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DebugInstructionBusRead__(typename TYPES::ADDRESS addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return static_cast<MSS *>(this)->DebugInstructionBusRead(addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::DecodeAddress(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& index, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	if(unlikely(CACHE::IsNullCache()))
	{
		line_base_addr = 0;
		block_base_addr = 0;
		index = 0;
		sector = 0;
		offset = 0;
		size_to_block_boundary = 0;
		return;
	}

	CACHE::DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline CacheSet<TYPES, CACHE> *MemorySubSystem<TYPES, MSS>::LookupSet(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& line_base_addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	if(CACHE::IsNullCache()) return 0;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	return cache->LookupSet(addr, line_base_addr, block_base_addr, sector, offset, size_to_block_boundary);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline CacheLine<TYPES, CACHE> *MemorySubSystem<TYPES, MSS>::LookupLine(typename TYPES::ADDRESS addr, typename TYPES::ADDRESS& block_base_addr, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	if(CACHE::IsNullCache()) return 0;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	return cache->LookupLine(addr, block_base_addr, sector, offset, size_to_block_boundary);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline CacheBlock<TYPES, CACHE> *MemorySubSystem<TYPES, MSS>::LookupBlock(typename TYPES::ADDRESS addr, unsigned int& offset, unsigned int& size_to_block_boundary)
{
	if(CACHE::IsNullCache()) return 0;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	return cache->LookupBlock(addr, offset, size_to_block_boundary);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::Lookup(CacheAccess<TYPES, CACHE>& access)
{
	if(unlikely(CACHE::IsNullCache()))
	{
		access.line_base_addr = 0;
		access.block_base_addr = 0;
		access.index = 0;
		access.way = 0;
		access.sector = 0;
		access.offset = 0;
		access.size_to_block_boundary = 0;
		access.set = 0;
		access.line = 0;
		access.line_to_write_back_evict = 0;
		access.block = 0;
		return;
	}

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	cache->Lookup(access);
	
	if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Lookup at @0x"
		                             << std::hex << access.addr << std::dec << " : "
		                             << "line_base_addr=0x" << std::hex << access.line_base_addr << std::dec << ","
		                             << "index=0x" << std::hex << access.index << std::dec << ","
		                             << "sector=0x" << std::hex << access.sector << std::dec << ","
		                             << "offset=0x" << std::hex << access.offset << std::dec << ","
		                             << "size_to_block_boundary=0x" << std::hex << access.size_to_block_boundary << std::dec
		                             << std::endl;
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateBlockByAddress(typename TYPES::ADDRESS addr)
{
	if(CACHE::IsNullCache()) return;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	typename TYPES::ADDRESS block_base_addr;
	unsigned int offset;
	unsigned int size_to_block_boundary;
	
	CacheBlock<TYPES, CACHE> *block = cache->LookupBlock(addr, offset, size_to_block_boundary);
	
	if(block)
	{
		if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Block at @0x" << std::hex << block->GetBaseAddr() << std::dec << std::endl;
		}
		block->Invalidate();
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateLineByAddress(typename TYPES::ADDRESS addr)
{
	if(CACHE::IsNullCache()) return;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	typename TYPES::ADDRESS block_base_addr;
	unsigned int sector;
	unsigned int offset;
	unsigned int size_to_block_boundary;
	
	CacheLine<TYPES, CACHE> *line = cache->LookupLine(addr, block_base_addr, sector, offset, size_to_block_boundary);
	
	if(line)
	{
		if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Line at @0x" << std::hex << line->GetBaseAddr() << std::dec << std::endl;
		}
		line->Invalidate();
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector)
{
	if(CACHE::IsNullCache()) return;

	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to invalidate a block in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to invalidate a block in a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}
	
	if(sector >= CACHE::NumSectors())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to invalidate a block with an invalid sector 0x" << std::hex << sector << std::dec << std::endl;
	}

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheSet<TYPES, CACHE>& set = (*cache)[index];
	
	CacheLine<TYPES, CACHE>& line = set[way];
	
	CacheBlock<TYPES, CACHE>& block = line[sector];
		
	if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Block in set #0x" << std::hex << set.GetIndex() << ", way #0x" << line.GetWay() << ", sector #0x" << block.GetSector() << std::dec << std::endl;
	}
		
	block.Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateLineBySetAndWay(unsigned int index, unsigned int way)
{
	if(CACHE::IsNullCache()) return;

	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to invalidate a line in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to invalidate a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}
	
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheSet<TYPES, CACHE>& set = (*cache)[index];
	
	CacheLine<TYPES, CACHE>& line = set[way];
	
	if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Line in set #0x" << std::hex << set.GetIndex() << ", way #0x" << line.GetWay() << std::dec << std::endl;
	}
	
	line.Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateSet(unsigned int index)
{
	if(CACHE::IsNullCache()) return;

	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to invalidate a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheSet<TYPES, CACHE>& set = (*cache)[index];

	if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Set #0x" << std::hex << set.GetIndex() << std::dec << std::endl;
	}
	
	set.Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateAll()
{
	if(CACHE::IsNullCache()) return;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating all" << std::endl;
	}

	cache->Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
inline void MemorySubSystem<TYPES, MSS>::GlobalInvalidateBlockByAddress(typename TYPES::ADDRESS addr)
{
	if(CACHE::IsNullCache()) return;

	InvalidateBlockByAddress<CACHE>(addr);

	if(typeid(CACHE) != typeid(UNTIL))
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		GlobalInvalidateBlockByAddress<CACHE_HIERARCHY, NLC, UNTIL>(addr);
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
inline void MemorySubSystem<TYPES, MSS>::GlobalInvalidateLineByAddress(typename TYPES::ADDRESS addr)
{
	if(CACHE::IsNullCache()) return;

	InvalidateLineByAddress<CACHE>(addr);

	if(typeid(CACHE) != typeid(UNTIL))
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		GlobalInvalidateLineByAddress<CACHE_HIERARCHY, NLC, UNTIL>(addr);
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::WriteBackBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector)
{
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a block in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a block in a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}
	
	if(sector >= CACHE::NumSectors())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a block with an invalid sector 0x" << std::hex << sector << std::dec << std::endl;
	}

	CacheSet<TYPES, CACHE>& set = (*cache)[index];
	
	CacheLine<TYPES, CACHE>& line = set[way];

	if(line.IsValid())
	{
		CacheBlock<TYPES, CACHE>& block = line[sector];
		
		if(block.IsValid() && block.IsDirty())
		{
			CacheAccess<TYPES, CACHE> access;
			access.line_to_write_back_evict = &line;
		
			if(!WriteBackDirtyBlock<CACHE_HIERARCHY, CACHE, INVALIDATE>(access, block.GetSector())) return false;
		}
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::WriteBackLineBySetAndWay(unsigned int index, unsigned int way)
{
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a line in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}

	CacheSet<TYPES, CACHE>& set = (*cache)[index];
	
	CacheLine<TYPES, CACHE>& line = set[way];

	if(line.IsValid())
	{
		CacheAccess<TYPES, CACHE> access;
		access.line_to_write_back_evict = &line;
		
		if(!WriteBackLine<CACHE_HIERARCHY, CACHE, INVALIDATE>(access)) return false;
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::GlobalWriteBackBlockByAddress(typename TYPES::ADDRESS addr)
{
	if(CACHE::IsNullCache())
	{
		return true;
	}
	else
	{
		CACHE *cache = __MSS_GetCache__<CACHE>();
		
		typename TYPES::ADDRESS block_base_addr;
		unsigned int sector;
		unsigned int offset;
		unsigned int size_to_block_boundary;
		
		CacheLine<TYPES, CACHE> *line = cache->LookupLine(addr, block_base_addr, sector, offset, size_to_block_boundary);
		
		if(line)
		{
			CacheBlock<TYPES, CACHE>& block = (*line)[sector];
			
			if(likely(__MSS_IsCacheEnabled__<CACHE>()))
			{
				CacheAccess<TYPES, CACHE> access;
				
				access.line_to_write_back_evict = line;
				
				if(block.IsValid() && block.IsDirty())
				{
					// write back dirty block
					typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
					
					if(!WriteBackDirtyBlock<CACHE_HIERARCHY, CACHE, NLC, INVALIDATE>(access, block.GetSector())) return false;
				}
			}
			else if(INVALIDATE)
			{
				if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Block at @0x" << std::hex << block.GetBaseAddr() << std::dec << std::endl;
				}
				block.Invalidate();
			}
		}
	}

	if(typeid(CACHE) != typeid(UNTIL))
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!GlobalWriteBackBlockByAddress<CACHE_HIERARCHY, NLC, UNTIL, INVALIDATE>(addr)) return false;
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::GlobalWriteBackLineByAddress(typename TYPES::ADDRESS addr)
{
	if(CACHE::IsNullCache())
	{
		return true;
	}
	else
	{
		CACHE *cache = __MSS_GetCache__<CACHE>();
		
		typename TYPES::ADDRESS block_base_addr;
		unsigned int sector;
		unsigned int offset;
		unsigned int size_to_block_boundary;
		
		CacheLine<TYPES, CACHE> *line = cache->LookupLine(addr, block_base_addr, sector, offset, size_to_block_boundary);
		
		if(line)
		{
			if(likely(__MSS_IsCacheEnabled__<CACHE>()))
			{
				CacheAccess<TYPES, CACHE> access;
				
				access.line_to_write_back_evict = line;
				
				if(!WriteBackLine<CACHE_HIERARCHY, CACHE, INVALIDATE>(access)) return false;
			}
			else if(INVALIDATE)
			{
				if(unlikely(__MSS_IsCacheVerbose__<CACHE>()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetName__() << ": Invalidating Line at @0x" << std::hex << line->GetBaseAddr() << std::dec << std::endl;
				}
				line->Invalidate();
			}
		}
	}

	if(typeid(CACHE) != typeid(UNTIL))
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!GlobalWriteBackLineByAddress<CACHE_HIERARCHY, NLC, UNTIL, INVALIDATE>(addr)) return false;
	}
	
	return true;
}

} // end of namespace cache
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_CACHE_CACHE_HH__
