/*
 *  Copyright (c) 2010-2021,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef MEMTRACE_HH_
#define MEMTRACE_HH_

#include <unisim/util/cache/cache.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/kernel/kernel.hh>

struct MemTrace;

struct StorageAttr
{
  StorageAttr() : memtrace(0), mat(), mtp() {}
  StorageAttr(MemTrace* _memtrace, unisim::util::debug::MemoryAccessType _mat, unisim::util::debug::MemoryType _mtp)
    : memtrace(_memtrace), mat(_mat), mtp(_mtp)
  {}
  MemTrace* memtrace;
  unisim::util::debug::MemoryAccessType mat;
  unisim::util::debug::MemoryType mtp;
  
  char mode() const { return mat == unisim::util::debug::MAT_WRITE ? 'w' : mtp == unisim::util::debug::MT_INSN ? 'f' : 'r'; }
};

struct AccessLog
{
  AccessLog(std::ostream& _sink) : sink(_sink) {}

  typedef uint32_t addr_t;

  void ReportAccess(StorageAttr const& storage_attr, addr_t addr, addr_t size)
  {
    uint32_t const granularity = 4;
    for (uint32_t cur = addr & -granularity, end = addr + size; cur < end; cur += granularity)
      sink << storage_attr.mode() << ",0x" << std::hex << cur << ',' << std::dec << granularity << std::endl;
  }
  void ReportInsn(addr_t addr)
  {
    sink << "i,0x" << std::hex << addr << std::endl;
  }

  std::ostream& sink;
};

struct MSSConfig
{
  typedef StorageAttr STORAGE_ATTR;
  typedef uint32_t PHYSICAL_ADDRESS;
  typedef uint32_t ADDRESS;
};

struct MemorySubSystem : public unisim::util::cache::MemorySubSystem<MSSConfig,MemorySubSystem>
{
  typedef unisim::util::cache::MemorySubSystem<MSSConfig,MemorySubSystem> BaseMSS;
  
  MemorySubSystem() : BaseMSS() {}
  
  bool IsStorageCacheable(StorageAttr storage_attr) const { return true; }

  // Tightly Coupled Memories (none)
  typedef unisim::util::cache::LocalMemorySet<MSSConfig> DATA_LOCAL_MEMORIES;
  typedef unisim::util::cache::LocalMemorySet<MSSConfig> INSTRUCTION_LOCAL_MEMORIES;

  // Cache memories
  struct L1D_CONFIG
  {
    static const unsigned int SIZE                                      = 4096;
    static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY;
    static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
    static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
    static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
    static const unsigned int ASSOCIATIVITY                             = 4;
    static const unsigned int BLOCK_SIZE                                = 32;
    static const unsigned int BLOCKS_PER_LINE                           = 1;
    
    //    typedef CACHE_CPU CPU;
    struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
    struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
    struct SET_STATUS : unisim::util::cache::CacheSetStatus { unisim::util::cache::LRU_ReplacementPolicy<ASSOCIATIVITY> lru; };
  };
  
  struct L1D : public unisim::util::cache::Cache<MSSConfig, L1D_CONFIG, L1D>
  {
    static const char *GetCacheName() { return "L1D"; }
    bool IsEnabled() const { return true; }

    static bool IsStaticWriteAllocate() { return true; }

    bool ChooseLineToEvict(unisim::util::cache::CacheAccess<MSSConfig, L1D>& access)
    {
      access.way = access.set->Status().lru.Select();
      // Choosen line [access.set, access.way] is:
      // 1. written to old memory location if its valid and dirty 
      // 2. read from new memory location
      // access.storage_attr.memtrace
      return true;
    }

    void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<MSSConfig, L1D>& access)
    {
      access.set->Status().lru.UpdateOnAccess(access.way);
      // Hit line [access.set, access.way] is sent to processor
      // access.storage_attr.memtrace
    }
  } l1d;
  L1D* GetCache(const L1D* ) ALWAYS_INLINE { return &l1d; }
  
  // Cache memories
  struct I1D_CONFIG
  {
    static const unsigned int SIZE                                      = 4096;
    static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY;
    static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
    static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
    static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
    static const unsigned int ASSOCIATIVITY                             = 4;
    static const unsigned int BLOCK_SIZE                                = 32;
    static const unsigned int BLOCKS_PER_LINE                           = 1;
    
    //    typedef CACHE_CPU CPU;
    struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
    struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
    struct SET_STATUS : unisim::util::cache::CacheSetStatus { unisim::util::cache::LRU_ReplacementPolicy<ASSOCIATIVITY> lru; };
  };
  
  struct I1D : public unisim::util::cache::Cache<MSSConfig, I1D_CONFIG, I1D>
  {
    static const char *GetCacheName() { return "I1D"; }
    bool IsEnabled() const { return true; }

    static bool IsStaticWriteAllocate() { return true; }

    bool ChooseLineToEvict(unisim::util::cache::CacheAccess<MSSConfig, I1D>& access)
    {
      access.way = access.set->Status().lru.Select();
      // Choosen line [access.set, access.way] is:
      // 1. written to old memory location if its valid and dirty 
      // 2. read from new memory location
      // access.storage_attr.memtrace
      return true;
    }

    void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<MSSConfig, I1D>& access)
    {
      access.set->Status().lru.UpdateOnAccess(access.way);
      // Hit line [access.set, access.way] is sent to processor
      // access.storage_attr.memtrace
    }
  } i1d;
  I1D* GetCache(const I1D* ) { return &i1d; }
  
  typedef unisim::util::cache::CacheHierarchy<MSSConfig, L1D> DATA_CACHE_HIERARCHY;

  typedef unisim::util::cache::CacheHierarchy<MSSConfig, I1D> INSTRUCTION_CACHE_HIERARCHY;
  // typedef MSSConfig::STORAGE_ATTR STORAGE_ATTR;
  
  bool DataBusWrite(uint32_t addr, void const* buffer, unsigned int size, StorageAttr const& storage_attr);

  bool DataBusRead(uint32_t addr, void* buffer, unsigned int size, StorageAttr const& storage_attr, bool rwitm);

  bool InstructionBusRead(uint32_t addr, void* buffer, unsigned int size, StorageAttr const& storage_attr);
};

struct MemTrace
  : unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReporting<uint32_t>>
  , unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint32_t>>
  , unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReportingControl>
{
  MemTrace(char const* name, unisim::kernel::Object* parent, std::ostream& wcache_log, std::ostream& ncache_log)
    : unisim::kernel::Object(name, parent)
    , unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReporting<uint32_t>>(name, parent)
    , unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
    , unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint32_t>>(name, parent)
    , unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
    , memory_access_reporting_export("memory-access-reporting-export", this)
    , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
    , memory_access_reporting_import("memory-access-reporting-import", this)
    , memory_access_reporting_control_import("memory-access-reporting-control-import", this)
    , requires_memory_access_reporting(false)
    , requires_fetch_instruction_reporting(false)
    , requires_commit_instruction_reporting(false)
    , mss()
    , nclog_sink(ncache_log)
    , wclog_sink(wcache_log)
  {}

  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReporting<uint32_t>> memory_access_reporting_export;
  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  
  unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint32_t>> memory_access_reporting_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_import;

  bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
  bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
  bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported

  MemorySubSystem mss;
  AccessLog nclog_sink;
  AccessLog wclog_sink;
  
  // unisim::service::interfaces::MemoryAccessReportingControl
  void Setup(unisim::service::interfaces::MemoryAccessReportingControl*)
  {
    memory_access_reporting_control_import.RequireSetup();
    memory_access_reporting_control_import->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_MEM_ACCESS, true);
    memory_access_reporting_control_import->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_FETCH_INSN, true);
    memory_access_reporting_control_import->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_COMMIT_INSN, true);
  }

  void RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report )
  {
    switch (type) {
    case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
    case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
    case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
    default: throw 0;
    }
  }

  // unisim::service::interfaces::MemoryAccessReporting<uint32_t>
  void Setup(unisim::service::interfaces::MemoryAccessReporting<uint32_t>*) { memory_access_reporting_import.RequireSetup(); }

  bool ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp, uint32_t addr, uint32_t size ) override
  {
    StorageAttr storage_attr(this, mat, mtp);
    nclog_sink.ReportAccess(storage_attr, addr, size);
    uint8_t buffer[size];
    if (mat == unisim::util::debug::MAT_WRITE)
      mss.DataStore(addr, addr, &buffer[0], size, storage_attr);
    else if (mtp == unisim::util::debug::MT_DATA)
      mss.DataLoad(addr, addr, &buffer[0], size, storage_attr);
    else
      mss.InstructionFetch(addr, addr, &buffer[0], size, storage_attr);
    
    if (requires_memory_access_reporting and memory_access_reporting_import)
      return memory_access_reporting_import->ReportMemoryAccess(mat, mtp, addr, size);
    
    return true;
  }

  void ReportFetchInstruction(uint32_t insn_addr) override
  {
    nclog_sink.ReportInsn(insn_addr);
    wclog_sink.ReportInsn(insn_addr);
    if (requires_fetch_instruction_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportFetchInstruction(insn_addr);
  }

  void ReportCommitInstruction(uint32_t insn_addr, unsigned insn_size) override
  {
    if (requires_commit_instruction_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportCommitInstruction(insn_addr, insn_size);
  }
};

#endif /* MEMTRACE_HH_ */
