#include <unisim/util/cache/cache.hh>
#include <inttypes.h>
#include <iostream>
#include <map>

namespace sandbox {

using unisim::util::cache::CacheSetStatus;
using unisim::util::cache::CacheLineStatus;
using unisim::util::cache::CacheBlockStatus;
using unisim::util::cache::MemorySubSystem;
using unisim::util::cache::Cache;
using unisim::util::cache::CacheType;
using unisim::util::cache::DATA_CACHE;
using unisim::util::cache::INSTRUCTION_CACHE;
using unisim::util::cache::UNIFIED_CACHE;
using unisim::util::cache::CacheWritingPolicy;
using unisim::util::cache::CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CACHE_WRITE_THROUGH_AND_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CacheHierarchy;
using unisim::util::cache::CacheAccess;
using unisim::util::cache::PLRU_ReplacementPolicy;
using unisim::util::cache::LRU_ReplacementPolicy;
using unisim::util::cache::RoundRobinReplacementPolicy;
using unisim::util::cache::PseudoRandomReplacementPolicy;
	
struct MEMORY_SUB_SYSTEM_TYPES
{
	typedef uint32_t ADDRESS;
	typedef uint32_t STORAGE_ATTR;
};

struct MyMemorySubSystem : MemorySubSystem<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem>
{
	typedef MemorySubSystem<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem> Super;
	
	struct L1I : Cache<MEMORY_SUB_SYSTEM_TYPES, L1I>
	{
		static const uint32_t SIZE                     = 32768;
		static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const CacheType TYPE                    = INSTRUCTION_CACHE;
		static const unsigned int ASSOCIATIVITY        = 8;
		static const unsigned int BLOCK_SIZE           = 32;
		static const unsigned int BLOCKS_PER_LINE      = 1;
		
		struct SET_STATUS : CacheSetStatus
		{
			LRU_ReplacementPolicy<ASSOCIATIVITY> lru;
		};
		
		struct LINE_STATUS : CacheLineStatus {};
		
		struct BLOCK_STATUS : CacheBlockStatus {};
	};

	struct L1D : Cache<MEMORY_SUB_SYSTEM_TYPES, L1D>
	{
		static const uint32_t SIZE                     = 32768;
		static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const CacheType TYPE                    = DATA_CACHE;
		static const unsigned int ASSOCIATIVITY        = 8;
		static const unsigned int BLOCK_SIZE           = 32;
		static const unsigned int BLOCKS_PER_LINE      = 1;
		
		struct SET_STATUS : CacheSetStatus
		{
			LRU_ReplacementPolicy<ASSOCIATIVITY> lru;
		};
		
		struct LINE_STATUS : CacheLineStatus {};
		
		struct BLOCK_STATUS : CacheBlockStatus {};
	};
	
	struct L2U : Cache<MEMORY_SUB_SYSTEM_TYPES, L2U>
	{
		typedef Cache<MEMORY_SUB_SYSTEM_TYPES, L2U> Super;
		
		static const uint32_t SIZE                     = 131072;
		static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const CacheType TYPE                    = UNIFIED_CACHE;
		static const unsigned int ASSOCIATIVITY        = 2;
		static const unsigned int BLOCK_SIZE           = 32;
		static const unsigned int BLOCKS_PER_LINE      = 2;
		
		struct SET_STATUS : CacheSetStatus
		{
			LRU_ReplacementPolicy<ASSOCIATIVITY> lru;
		};
		
		struct LINE_STATUS : CacheLineStatus {};
		
		struct BLOCK_STATUS : CacheBlockStatus {};
	};

	L1I l1i;
	L1D l1d;
	L2U l2u;
	
	typedef CacheHierarchy<MEMORY_SUB_SYSTEM_TYPES, L1D, L2U> DATA_CACHE_HIERARCHY;
	typedef CacheHierarchy<MEMORY_SUB_SYSTEM_TYPES, L1I, L2U> INSTRUCTION_CACHE_HIERARCHY;
	
	MyMemorySubSystem(std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream);
	
	template <typename CACHE> CACHE *GetCache() { return 0; }
	template <typename CACHE> bool IsCacheVerbose() const { return true; }
	inline bool IsVerboseDataLoad() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataStore() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE { return true; }
	bool IsStorageCacheable(MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr) const { return true; }
	bool IsDataWriteAccessWriteThrough(MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr) const { return false; }
	template <typename CACHE> bool IsCacheEnabled() { return true; }
	template <typename CACHE> void ChooseLineToEvict(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, CACHE>& access) { access.way = 0; }
	template <typename CACHE> void UpdateReplacementPolicyOnAccess(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, CACHE>& access) {}
	template <typename CACHE> void UpdateReplacementPolicyOnFill(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, CACHE>& access) {}
	
	bool DataBusRead(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr, void *buffer, unsigned int size, MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr, bool rwitm);
	bool DataBusWrite(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr, const void *buffer, unsigned int size, MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr);
	bool InstructionBusRead(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr, void *buffer, unsigned int size, MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr);
	
	void InvalidateLineByAddress(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr);
	void CleanBySetAndWay(unsigned int index, unsigned int way);
	void CleanByAddress(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr);
	
	std::map<MEMORY_SUB_SYSTEM_TYPES::ADDRESS, uint8_t> memory;
};

template <> MyMemorySubSystem::L1I *MyMemorySubSystem::GetCache<MyMemorySubSystem::L1I>()
{
	return &l1i;
}

template <> MyMemorySubSystem::L1D *MyMemorySubSystem::GetCache<MyMemorySubSystem::L1D>()
{
	return &l1d;
}

template <> MyMemorySubSystem::L2U *MyMemorySubSystem::GetCache<MyMemorySubSystem::L2U>()
{
	return &l2u;
}

template <> void MyMemorySubSystem::ChooseLineToEvict(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem::L1I>& access)
{
	access.way = access.set->Status<L1I::SET_STATUS>().lru.Select();
}

template <> void MyMemorySubSystem::ChooseLineToEvict(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem::L1D>& access)
{
	access.way = access.set->Status<L1D::SET_STATUS>().lru.Select();
}

template <> void MyMemorySubSystem::ChooseLineToEvict(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem::L2U>& access)
{
	access.way = access.set->Status<L2U::SET_STATUS>().lru.Select();
}

template <> void MyMemorySubSystem::UpdateReplacementPolicyOnAccess(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem::L1I>& access)
{
	access.set->Status<L1I::SET_STATUS>().lru.UpdateOnAccess(access.way);
}

template <> void MyMemorySubSystem::UpdateReplacementPolicyOnAccess(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem::L1D>& access)
{
	access.set->Status<L1D::SET_STATUS>().lru.UpdateOnAccess(access.way);
}

template <> void MyMemorySubSystem::UpdateReplacementPolicyOnAccess(CacheAccess<MEMORY_SUB_SYSTEM_TYPES, MyMemorySubSystem::L2U>& access)
{
	access.set->Status<L2U::SET_STATUS>().lru.UpdateOnAccess(access.way);
}

MyMemorySubSystem::MyMemorySubSystem(std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream)
	: Super(debug_info_stream, debug_warning_stream, debug_error_stream)
	, l1i()
	, l1d()
	, l2u()
{
}


bool MyMemorySubSystem::DataBusRead(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr, void *buffer, unsigned int size, MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr, bool rwitm)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[i];
	}
	return true;
}

bool MyMemorySubSystem::DataBusWrite(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr, const void *buffer, unsigned int size, MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		memory[i] = ((uint8_t *) buffer)[i];
	}
	return true;
}

bool MyMemorySubSystem::InstructionBusRead(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr, void *buffer, unsigned int size, MEMORY_SUB_SYSTEM_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[i];
	}
	return true;
}

void MyMemorySubSystem::InvalidateLineByAddress(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr)
{
	Super::GlobalInvalidateLineByAddress<DATA_CACHE_HIERARCHY, L1D, L2U>(addr);
}

void MyMemorySubSystem::CleanBySetAndWay(unsigned int index, unsigned int way)
{
	WriteBackLineBySetAndWay<DATA_CACHE_HIERARCHY, L1D, true>(index, way);
}

void MyMemorySubSystem::CleanByAddress(MEMORY_SUB_SYSTEM_TYPES::ADDRESS addr)
{
	GlobalWriteBackBlockByAddress<DATA_CACHE_HIERARCHY, L1D, L2U, true>(addr);
}

} // end of namespace sandbox

int main()
{
	uint8_t buf[4] = { 0xab, 0xcd, 0xef, 0x10 };
	
	sandbox::MyMemorySubSystem mss(std::cout, std::cerr, std::cerr);
	
	mss.DataStore(0x1234, buf, 4, 0);
	mss.DataLoad(0x1234, buf, 4, 0);
	mss.DataLoad(0x1260, buf, 4, 0);
	mss.DataLoad(0x1280, buf, 4, 0);
	mss.DataLoad(0x12a0, buf, 4, 0);
	//mss.DataLoad(0x1234, buf, 4, 0);
	//mss.InstructionFetch(0x1234, buf, 4, 0);
	//mss.CleanBySetAndWay(0x12, 0x0);
	mss.CleanByAddress(0x1234);
	
	//mss.WriteBackCacheLine<sandbox::MyMemorySubSystem::DATA_CACHE_HIERARCHY, sandbox::MyMemorySubSystem::L1D, sandbox::MyMemorySubSystem::L2U>(4, 1);
	//mss.WriteBackCacheSet<sandbox::MyMemorySubSystem::DATA_CACHE_HIERARCHY, sandbox::MyMemorySubSystem::L1D, sandbox::MyMemorySubSystem::Memory, false>(0x12);
	
	//sandbox::MyMemorySubSystem::L2U *l2u = mss.GetCache<sandbox::MyMemorySubSystem::L2U>();
	//l2u->InvalidateLine(0x12, 0);
	return 0;
}
