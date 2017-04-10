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

template <typename MSS_TYPES>
struct MyMemorySubSystem : MemorySubSystem<MSS_TYPES, MyMemorySubSystem<MSS_TYPES> >
{
	typedef MemorySubSystem<MSS_TYPES, MyMemorySubSystem> Super;
	
	struct L1I : Cache<MSS_TYPES, L1I>
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

	struct L1D : Cache<MSS_TYPES, L1D>
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
	
	struct L2U : Cache<MSS_TYPES, L2U>
	{
		typedef Cache<MSS_TYPES, L2U> Super;
		
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
	
	typedef CacheHierarchy<MSS_TYPES, L1D, L2U> DATA_CACHE_HIERARCHY;
	typedef CacheHierarchy<MSS_TYPES, L1I, L2U> INSTRUCTION_CACHE_HIERARCHY;
	
	MyMemorySubSystem();
	
	template <typename CACHE, bool dummy = true>
	struct INTERFACE
	{
		static CACHE *GetCache(MyMemorySubSystem<MSS_TYPES>* mss) { return 0; }
		static bool IsCacheVerbose(const MyMemorySubSystem<MSS_TYPES>* mss) { return false; }
		static bool IsCacheEnabled(MyMemorySubSystem<MSS_TYPES>* mss) { return true; }
		static void ChooseLineToEvict(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, CACHE>& access) { access.way = 0; }
		static void UpdateReplacementPolicyOnAccess(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, CACHE>& access) {}
		static void UpdateReplacementPolicyOnFill(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, CACHE>& access) {}
	};
	
	template <bool dummy>
	struct INTERFACE<L1I, dummy>
	{
		static L1I *GetCache(MyMemorySubSystem<MSS_TYPES>* mss) { return &mss->l1i; }
		static bool IsCacheVerbose(const MyMemorySubSystem<MSS_TYPES>* mss) { return true; }
		static bool IsCacheEnabled(MyMemorySubSystem<MSS_TYPES>* mss) { return true; }
		static void ChooseLineToEvict(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L1I>& access) { access.way = access.set->template Status<typename L1I::SET_STATUS>().lru.Select(); }
		static void UpdateReplacementPolicyOnAccess(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L1I>& access) { access.set->template Status<typename L1I::SET_STATUS>().lru.UpdateOnAccess(access.way); }
		static void UpdateReplacementPolicyOnFill(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L1I>& access) {}
	};

	template <bool dummy>
	struct INTERFACE<L1D, dummy>
	{
		static L1D *GetCache(MyMemorySubSystem<MSS_TYPES>* mss) { return &mss->l1d; }
		static bool IsCacheVerbose(const MyMemorySubSystem<MSS_TYPES>* mss) { return true; }
		static bool IsCacheEnabled(MyMemorySubSystem<MSS_TYPES>* mss) { return true; }
		static void ChooseLineToEvict(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L1D>& access) { access.way = access.set->template Status<typename L1D::SET_STATUS>().lru.Select(); }
		static void UpdateReplacementPolicyOnAccess(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L1D>& access) { access.set->template Status<typename L1D::SET_STATUS>().lru.UpdateOnAccess(access.way); }
		static void UpdateReplacementPolicyOnFill(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L1D>& access) {}
	};

	template <bool dummy>
	struct INTERFACE<L2U, dummy>
	{
		static L2U *GetCache(MyMemorySubSystem<MSS_TYPES>* mss) { return &mss->l2u; }
		static bool IsCacheVerbose(const MyMemorySubSystem<MSS_TYPES>* mss) { return true; }
		static bool IsCacheEnabled(MyMemorySubSystem<MSS_TYPES>* mss) { return true; }
		static void ChooseLineToEvict(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L2U>& access) { access.way = access.set->template Status<typename L2U::SET_STATUS>().lru.Select(); }
		static void UpdateReplacementPolicyOnAccess(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L2U>& access) { access.set->template Status<typename L2U::SET_STATUS>().lru.UpdateOnAccess(access.way); }
		static void UpdateReplacementPolicyOnFill(MyMemorySubSystem<MSS_TYPES>* mss, CacheAccess<MSS_TYPES, L2U>& access) {}
	};
	
	template <typename CACHE> CACHE *GetCache() { return INTERFACE<CACHE>::GetCache(this); }
	template <typename CACHE> bool IsCacheVerbose() const { return INTERFACE<CACHE>::IsCacheVerbose(this); }
	template <typename CACHE> bool IsCacheEnabled() { return INTERFACE<CACHE>::IsCacheEnabled(this); }
	template <typename CACHE> void ChooseLineToEvict(CacheAccess<MSS_TYPES, CACHE>& access) { return INTERFACE<CACHE>::ChooseLineToEvict(this, access); }
	template <typename CACHE> void UpdateReplacementPolicyOnAccess(CacheAccess<MSS_TYPES, CACHE>& access) { return INTERFACE<CACHE>::UpdateReplacementPolicyOnAccess(this, access); }
	template <typename CACHE> void UpdateReplacementPolicyOnFill(CacheAccess<MSS_TYPES, CACHE>& access) { return INTERFACE<CACHE>::UpdateReplacementPolicyOnFill(this, access); }

	inline bool IsVerboseDataLoad() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataStore() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE { return true; }
	bool IsStorageCacheable(typename MSS_TYPES::STORAGE_ATTR storage_attr) const { return true; }
	bool IsDataWriteAccessWriteThrough(typename MSS_TYPES::STORAGE_ATTR storage_attr) const { return false; }
	
	bool DataBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr, bool rwitm);
	bool DataBusWrite(typename MSS_TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr);
	bool InstructionBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr);
	
	void InvalidateLineByAddress(typename MSS_TYPES::ADDRESS addr);
	void CleanBySetAndWay(unsigned int index, unsigned int way);
	void CleanByAddress(typename MSS_TYPES::ADDRESS addr);
	
	std::map<typename MSS_TYPES::ADDRESS, uint8_t> memory;
};

#if 0
template <typename MSS_TYPES>
template <bool dummy>
int *MyMemorySubSystem<MSS_TYPES>::INTERFACE<int, dummy>::GetCache(MyMemorySubSystem<MSS_TYPES> *mss)
{
	return &l1i;
}

template <typename MSS_TYPES>
template <bool dummy>
typename MyMemorySubSystem<MSS_TYPES>::L1D *MyMemorySubSystem<MSS_TYPES>::INTERFACE<dummy>::GetCache<MyMemorySubSystem<MSS_TYPES>::L1D>()
{
	return &l1d;
}

template <typename MSS_TYPES>
template <bool dummy>
typename MyMemorySubSystem<MSS_TYPES>::L2U *MyMemorySubSystem<MSS_TYPES>::INTERFACE<dummy>::GetCache<MyMemorySubSystem<MSS_TYPES>::L2U>()
{
	return &l2u;
}

template <typename MSS_TYPES>
template <bool dummy>
void MyMemorySubSystem<MSS_TYPES>::INTERFACE<MyMemorySubSystem<MSS_TYPES>::L1I, dummy>::ChooseLineToEvict(CacheAccess<MSS_TYPES, MyMemorySubSystem<MSS_TYPES>::L1I>& access)
{
	access.way = access.set->Status<L1I::SET_STATUS>().lru.Select();
}

template <typename MSS_TYPES>
template <bool dummy>
void MyMemorySubSystem<MSS_TYPES>::INTERFACE<dummy>::ChooseLineToEvict(CacheAccess<MSS_TYPES, MyMemorySubSystem<MSS_TYPES>::L1D>& access)
{
	access.way = access.set->Status<L1D::SET_STATUS>().lru.Select();
}

template <typename MSS_TYPES>
template <bool dummy>
void MyMemorySubSystem<MSS_TYPES>::INTERFACE<dummy>::ChooseLineToEvict(CacheAccess<MSS_TYPES, MyMemorySubSystem<MSS_TYPES>::L2U>& access)
{
	access.way = access.set->Status<L2U::SET_STATUS>().lru.Select();
}

template <typename MSS_TYPES>
template <bool dummy>
void MyMemorySubSystem<MSS_TYPES>::INTERFACE<dummy>::UpdateReplacementPolicyOnAccess(CacheAccess<MSS_TYPES, MyMemorySubSystem<MSS_TYPES>::L1I>& access)
{
	access.set->Status<L1I::SET_STATUS>().lru.UpdateOnAccess(access.way);
}

template <typename MSS_TYPES>
template <bool dummy>
void MyMemorySubSystem<MSS_TYPES>::INTERFACE<dummy>::UpdateReplacementPolicyOnAccess(CacheAccess<MSS_TYPES, MyMemorySubSystem<MSS_TYPES>::L1D>& access)
{
	access.set->Status<L1D::SET_STATUS>().lru.UpdateOnAccess(access.way);
}

template <typename MSS_TYPES>
template <bool dummy>
void MyMemorySubSystem<MSS_TYPES>::INTERFACE<dummy>::UpdateReplacementPolicyOnAccess(CacheAccess<MSS_TYPES, MyMemorySubSystem<MSS_TYPES>::L2U>& access)
{
	access.set->Status<L2U::SET_STATUS>().lru.UpdateOnAccess(access.way);
}
#endif


template <typename MSS_TYPES>
MyMemorySubSystem<MSS_TYPES>::MyMemorySubSystem()
	: Super()
	, l1i()
	, l1d()
	, l2u()
{
}


template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::DataBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr, bool rwitm)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[i];
	}
	return true;
}

template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::DataBusWrite(typename MSS_TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		memory[i] = ((uint8_t *) buffer)[i];
	}
	return true;
}

template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::InstructionBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[i];
	}
	return true;
}

template <typename MSS_TYPES>
void MyMemorySubSystem<MSS_TYPES>::InvalidateLineByAddress(typename MSS_TYPES::ADDRESS addr)
{
	Super::template GlobalInvalidateLineByAddress<DATA_CACHE_HIERARCHY, L1D, L2U>(addr);
}

template <typename MSS_TYPES>
void MyMemorySubSystem<MSS_TYPES>::CleanBySetAndWay(unsigned int index, unsigned int way)
{
	Super::template WriteBackLineBySetAndWay<DATA_CACHE_HIERARCHY, L1D, true>(index, way);
}

template <typename MSS_TYPES>
void MyMemorySubSystem<MSS_TYPES>::CleanByAddress(typename MSS_TYPES::ADDRESS addr)
{
	Super::template GlobalWriteBackBlockByAddress<DATA_CACHE_HIERARCHY, L1D, L2U, true>(addr);
}

} // end of namespace sandbox

int main()
{
	uint8_t buf[4] = { 0xab, 0xcd, 0xef, 0x10 };
	
	sandbox::MyMemorySubSystem<sandbox::MEMORY_SUB_SYSTEM_TYPES> mss;
	
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
