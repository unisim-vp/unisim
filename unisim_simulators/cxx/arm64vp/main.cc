/*
 *  Copyright (c) 2020-2022,
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include "architecture.hh"
#include "debugger.hh"
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/service/web_terminal/web_terminal.hh>
#include <unisim/service/netstreamer/netstreamer.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/util/host_time/time.hh>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

struct MemoryMapping
{
  static const uint64_t memory_base_addr = 0x40000000;
  static const uint64_t initrd_start     = 0x44000000;
  static const uint64_t gic_base_addr    = 0x08000000;
  static const uint64_t uart_base_addr   = 0x09000000;
  static const uint64_t rtc_base_addr    = 0x09010000;
  static const uint64_t virtio_base_addr = 0x0a000000;
};

struct MMapped : AArch64::Page::Free
{
  MMapped(char const* _filename)
    : filename(_filename), fd(open(_filename,O_RDONLY))
  {
    if (fd < 0)
      {
        std::cerr << "With '" << _filename << "': " << std::flush;
        perror("open error");
      }
  }
  void free (AArch64::Page& page) const override
  {
    munmap((void*)page.data_beg(), page.size());
    munmap((void*)page.udat_beg(), page.size());
    delete this;
  }
  bool get(struct stat& res)
  {
    if (fstat(fd,&res) >= 0)
      return true;
    std::cerr << "With '" << filename << "': " << std::flush;
    perror("fstat error");
    return false;
  }
  bool get(AArch64::Page& page)
  {
    uintptr_t size = page.size();
    uint8_t* data = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (data == (void*)-1) { perror("mmap error"); return false; }
    uint8_t* udat = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (udat == (void*)-1) { perror("mmap error"); return false; }
    page.set_data(data);
    page.set_udat(udat);
    return true;
  }

  void dump_load( std::ostream& sink, char const* reason, AArch64::Page const& page )
  {
    sink << "*** Loading ";
    page.dump_range( sink );
    sink << " with '" << filename << "' ***" << std::endl;
  }

  char const* filename; // For logging purpose
  int fd;

private:
  ~MMapped() { close(fd); }
};

bool
load_snapshot(AArch64& arch, char const* shot_filename)
{
  if (access(shot_filename, R_OK) != 0)
    return false;
  arch.load_snapshot(shot_filename);
  std::cerr << "Starting at " << std::dec << arch.insn_counter << " instructions." << std::endl;
  return true;
}

bool
load_linux(AArch64& arch, char const* img_filename, char const* dt_filename, char const* initrd_filename = 0)
{
  MMapped* linux_image  = new MMapped( img_filename );

  if (linux_image->fd < 0) return false;

  MMapped* device_tree = new MMapped( dt_filename );

  if (device_tree->fd < 0) return false;

  {
    struct stat f_stat;
    if (not linux_image->get(f_stat))
      return 1;
    uint64_t size = f_stat.st_size;
    // Memory map will be rounded up to page boundary. We round up to
    // AArch64 ZVA boundary which should always be less than host page
    // boundary.
    size = (size + 63) & uint64_t(-64);

    struct
    {
      uint8_t buf[8];
      uint64_t read(int fd)
      {
        uint64_t res = 0;
        if (::read(fd, &buf[0], sizeof (buf)) != sizeof (buf)) throw *this;
        for (unsigned idx = sizeof (buf); idx-- > 0;) res = res << 8 | buf[idx];
        return res;
      }
    } le64;
    lseek(linux_image->fd,8,SEEK_SET);
    uint64_t base = MemoryMapping::memory_base_addr + le64.read(linux_image->fd), kernel_size = le64.read(linux_image->fd);
    lseek(linux_image->fd,0,SEEK_SET);
    if (base % 64) { std::cerr << "Bad base: " << std::hex << base << std::endl; return 1; }
    if (size % 64) { std::cerr << "Bad size: " << std::hex << size << std::endl; return 1; }
    AArch64::Page page(base,base+size-1,0,0,linux_image);
    linux_image->dump_load( std::cerr, "linux image", page );
    if (not linux_image->get(page))
      return 1;
    arch.mem_map(std::move(page));
    arch.BranchTo(AArch64::U64(base),arch.B_JMP);
    // If kernel_size is greater than mapped file size, we need to reserve extra space with unitialized data
    uint64_t xbase = base + size, xlast = base + kernel_size - 1;
    if (xbase <= xlast)
      {
        AArch64::Page extra(xbase, xlast);
        std::fill(extra.udat_beg(), extra.udat_end(), -1);
        arch.mem_map(std::move(extra));
      }
    arch.MemDump64(base);
  }

  {
    struct stat f_stat;
    if (not device_tree->get(f_stat))
      return 1;
    uint64_t base = arch.pages.begin()->last + 1, size = f_stat.st_size;
    size = (size + 63) & uint64_t(-64);
    if (base % 64) { std::cerr << "Bad base: " << std::hex << base << std::endl; return 1; }
    // The device tree blob (dtb) must be placed on an 8-byte boundary
    AArch64::Page page(base,base+size-1,0,0,device_tree);
    device_tree->dump_load( std::cerr, "device tree", page );
    if (not device_tree->get(page))
      return 1;
    arch.mem_map(std::move(page));
    /* Primary CPU general-purpose register settings
     * x0 = physical address of device tree blob (dtb) in system RAM.
     * x1 = 0 (reserved for future use)
     * x2 = 0 (reserved for future use)
     * x3 = 0 (reserved for future use)
     */
    arch.SetGSR(0, AArch64::U64(base));
    for (unsigned idx = 1; idx < 4; ++idx)
      arch.SetGSR(idx, AArch64::U64(0/*0xdeadbeefbadfeed0 + idx,-1*/));
  }

  if(initrd_filename)
  {
    MMapped* initrd = new MMapped( initrd_filename );

    if (initrd->fd < 0) return false;

    struct stat f_stat;
    if (not initrd->get(f_stat))
      return 1;
    uint64_t initrd_end = MemoryMapping::initrd_start+f_stat.st_size;
    AArch64::Page page(MemoryMapping::initrd_start,initrd_end-1,0,0,initrd);
    initrd->dump_load( std::cerr, "initrd", page );
    if (not initrd->get(page))
      return 1;
    arch.mem_map(std::move(page));
  }
  
  return true;
}
// Docs & Notes:
//  [AArch64 boot info] https://www.kernel.org/doc/Documentation/arm64/booting.txt
//  [Yocto dtb generation] runqemu qemuparams="-machine dumpdtb=device_tree_from_qemu.dtb"

AArch64* core_instance(AArch64* new_arch)
{
  static AArch64* instance = 0;
  if (new_arch) instance = new_arch;
  return instance;
}

void signal_handler(int signum)
{
  struct Bad {};
  switch (signum)
    {
    default: throw Bad();
    case SIGUSR1:
      core_instance(0)->suspend = true;
      break;
    case SIGUSR2:
    case SIGINT:
      core_instance(0)->terminate = true;
      break;
    }
}

void simdefault(unisim::kernel::Simulator* sim)
{
  sim->SetVariable("http-server.http-port", 12360);
  sim->SetVariable("web-terminal.verbose", false);
  sim->SetVariable("netstreamer.tcp-port", 1234);
  sim->SetVariable("netstreamer.filter-null-character", true);
  sim->SetVariable("netstreamer.verbose", false);

  sim->SetVariable("debugger.gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_aarch64.xml");
  sim->SetVariable("debugger.debug-hub.parse-dwarf", false);
  sim->SetVariable("debugger.debug-hub.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/aarch64_eabi_dwarf_register_number_mapping.xml");
  sim->SetVariable("debugger.debug-hub.architecture[0]", "aarch64");


  new unisim::kernel::config::json::JSONConfigFileHelper(sim);
}

struct StreamSpy
  : public unisim::kernel::Service<unisim::service::interfaces::CharIO>
  , public unisim::kernel::Client<unisim::service::interfaces::CharIO>
{
  StreamSpy(char const* name, unisim::kernel::Object* parent, AArch64& _arch)
    : unisim::kernel::Object(name, parent)
    , unisim::kernel::Service<unisim::service::interfaces::CharIO>(name, parent)
    , unisim::kernel::Client<unisim::service::interfaces::CharIO>(name, parent)
    , outgoing()
    , incoming()
    , expect()
    , step()
    , char_io_import("char-io-import", this)
    , char_io_export("char-io-export", this)
    , arch(_arch)
  {
    advance(0, "qemuarm64 login: ", &StreamSpy::step1);
  }

  void step1()
  {
    advance("root\r", "root@qemuarm64:~#", &StreamSpy::step2);
  }

  void step2()
  {
    advance("su - demo\r", "qemuarm64:~$ ", &StreamSpy::step3);
  }
  
  void step3()
  {
    advance(0,0,0);
    arch.suspend = true;
    arch.silence( &AArch64::handle_suspend );
    arch.notify( 0, &AArch64::handle_suspend );
  }
  
  typedef void (StreamSpy::*step_t)();
  
  void advance(char const* _out, char const* _in, step_t _step)
  {
    if (outgoing and *outgoing) { struct Bad {}; raise( Bad() ); }
    outgoing = _out;
    incoming = _in;
    expect = _in;
    step = _step;
  }

  void Setup(unisim::service::interfaces::CharIO*) override { char_io_import.RequireSetup(); }
  
  virtual void ResetCharIO() override { char_io_import->ResetCharIO(); }
  virtual bool GetChar(char& ch) override
  {
    if (not outgoing or not *outgoing) return char_io_import->GetChar(ch);
    ch = *outgoing++;
    return true;
  }
  virtual void PutChar(char ch) override
  {
    char_io_import->PutChar(ch);
    if (not ch or !expect) return;
    if (incoming and *expect != ch) { expect = incoming; return; }
    if (*++expect) return;
    (this->*step)();
  }
  virtual void FlushChars() override { char_io_import->FlushChars(); }

  char const* outgoing;
  char const* incoming;
  char const* expect;
  step_t step;
  unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO> char_io_import;
  unisim::kernel::ServiceExport<unisim::service::interfaces::CharIO> char_io_export;

  AArch64& arch;
};

int
main(int argc, char *argv[])
{
  char const* disk_filename = "root.fs";

  struct Simulator : unisim::kernel::Simulator
  {
    bool enable_linux_os;
    unisim::kernel::variable::Parameter<bool> param_enable_linux_os;

    Simulator(int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::Simulator *simulator) = 0)
      : unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
      , enable_linux_os(false)
      , param_enable_linux_os("enable-linux-os", 0, enable_linux_os, "enable Linux OS emulation")
    {
    }

    virtual void Stop(unisim::kernel::Object *object, int exit_status, bool asynchronous = false) override
    {
      AArch64 *arch = core_instance(0);
      if (arch)
        {
          arch->terminate = true;
          unisim::kernel::Simulator::Kill();
        }
      else
        {
          unisim::kernel::Simulator::Kill();
          exit(0);
        }
    }
  };

  Simulator simulator(argc, argv, &simdefault);
  unisim::kernel::logger::console::Printer printer;
  // unisim::service::http_server::HttpServer http_server("http-server");
  // unisim::service::web_terminal::WebTerminal web_terminal("web-terminal");

  AArch64 arch("arm64vp");

  std::unique_ptr<unisim::service::netstreamer::NetStreamer> netstreamer;
  std::unique_ptr<StreamSpy> stream_spy;

  if (not simulator.enable_linux_os)
    {
      netstreamer = std::make_unique<unisim::service::netstreamer::NetStreamer>("netstreamer");
      stream_spy = std::make_unique<StreamSpy>("stream-spy", (unisim::kernel::Object*) 0, arch);
      arch.uart.char_io_import >> stream_spy->char_io_export;
      stream_spy->char_io_import >> netstreamer->char_io_export;
      // arch.uart.char_io_import >> netstreamer->char_io_export;
    }

  // *http_server.http_server_import[0] >> web_terminal.http_server_export;

  std::ifstream linux_elf;

  std::unique_ptr<unisim::service::os::linux_os::ArmLinux64> linux_os; 
  if (simulator.enable_linux_os)
  {
    linux_os = std::make_unique<unisim::service::os::linux_os::ArmLinux64>("linux-os");

    // ARCH <-> LinuxOS connections
    arch.linux_os_import >> linux_os->linux_os_export_;
    linux_os->memory_import_ >> arch.memory_export;
    linux_os->memory_injection_import_ >> arch.memory_injection_export;
    linux_os->registers_import_ >> arch.registers_export;
  }
  else
  {
    linux_elf.open("linux.elf");
  }

  std::unique_ptr<Debugger> dbg = std::make_unique<Debugger>( "debugger", arch, linux_elf);
  
  switch (simulator.Setup())
    {
    case simulator.ST_ERROR:         return 1;
    case simulator.ST_OK_DONT_START: return 0;
    default: break;
    }

  if (not simulator.enable_linux_os)
  {
    /* Start asynchronous reception loop */
    // host_term.Start();

    arch.map_gic(MemoryMapping::gic_base_addr);
    arch.map_uart(MemoryMapping::uart_base_addr);
    arch.map_rtc(MemoryMapping::rtc_base_addr);

    for (unsigned int idx = 0; idx < 32; ++idx)
      {
        uint64_t base_addr = MemoryMapping::virtio_base_addr + 0x200*idx;
        unsigned irq = 32 + 16 + idx;
        switch (idx)
          {
          default: arch.map_virtio_placeholder(idx, base_addr); break;
      // case 30: arch.map_virtio_console(base_addr, irq); break;
          case 31: arch.map_virtio_disk(disk_filename, base_addr, irq); break;
          }
      }

    // Loading image
    if (not load_snapshot(arch, "uvp.shot") and not load_linux(arch, "Image", "device_tree.dtb", "initrd.img"))
      return 1;

    signal(SIGUSR1, signal_handler);
  }

  signal(SIGUSR2, signal_handler);

  if (not dbg->enable_inline_debugger)
    signal(SIGINT, signal_handler);

  core_instance(&arch);
  arch.silence( &AArch64::handle_suspend );
  arch.notify( 0, &AArch64::handle_suspend );
  
  // // ffffffc010eb5198 0x20000
  // AArch64::Page const& logbuf = arch.modify_page(0xeb5198);
  // uint8_t const* logbytes = logbuf.data_abs(0xeb5198);
  // std::cerr << "logbuf: " << (void*)logbytes << std::endl;

  std::cerr << "\n*** Run ***" << std::endl;

  double time_start = unisim::util::host_time::GetHostTime();
  try
    {
      uint64_t suspend_at;
      if (char const* arg = getenv("SUSPEND_AT")) suspend_at = strtoull(arg,0,0);
      else                                        suspend_at = uint64_t(-1);
      //else                                        suspend_at = arch.insn_counter + 0x1000000000ull;
      arch.run( suspend_at );
      arch.show_exec_stats(std::cerr);
      arch.write_tfpstats();
    }
  catch (...)
    {
      arch.show_exec_stats(std::cerr);
      std::ofstream tail("tail");
      for (unsigned idx = 1; idx <= arch.histsize; ++idx)
        if (arch.last_insn(idx).op)
          {
            AArch64::InstructionInfo const& last_insn = arch.last_insn(idx);
            unisim::util::debug::Statement<uint64_t> const* stmt = dbg->debug_hub.FindStatement(
              last_insn.addr,
              /* any filename */ 0,
              unisim::service::interfaces::StatementLookup<uint64_t>::OPT_FIND_EXACT_STMT
            );
            if (stmt)
              {
                char const* source_filename = stmt->GetSourceFilename();
                if (source_filename)
                  {
                    tail << "-> ";
                    unsigned lineno = stmt->GetLineNo();
                    char const* source_dirname = stmt->GetSourceDirname();
                    if (source_dirname)
                      {
                        tail << source_dirname << "/";
                      }
                    tail << source_filename << "#" << std::dec << lineno << ":" << std::endl;
                  }
              }

            tail << last_insn << std::endl;
          }
      throw;
    }
  double time_stop = unisim::util::host_time::GetHostTime();
  double spent_time = time_stop - time_start;
  double run_time = (double) arch.insn_counter / (arch.get_freq() * arch.get_ipc());

  std::cerr << "Run time: " << run_time << " seconds" << std::endl;
  std::cerr << "Host simulation speed: " << ((double) arch.insn_counter / spent_time / 1e6) << " MIPS" << std::endl;
  std::cerr << "Time dilation: " << spent_time / run_time << " times slower than target machine" << std::endl;
  return 0;
}

void AArch64::breakdance() {}

