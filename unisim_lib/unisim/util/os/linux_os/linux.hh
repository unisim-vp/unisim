/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_UTIL_OS_LINUX_LINUX_HH__
#define __UNISIM_UTIL_OS_LINUX_LINUX_HH__

#include <unisim/util/blob/blob.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <queue>
// #include <iostream>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
  // see http://mathieuturcotte.ca/textes/windows-gettimeofday
  struct timeval {
    int32_t  tv_sec;    /* seconds */
    int32_t tv_usec;    /* microseconds */
  };
  struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
  };
  int gettimeofday(struct timeval* p, struct timezone* tz);
#endif

  template <typename T> struct AsSigned {};
  template <> struct AsSigned<uint32_t> { typedef int32_t type; };
  template <> struct AsSigned<uint64_t> { typedef int64_t type; };

  template <class ADDRESS_TYPE, class PARAMETER_TYPE>
  struct Linux
  {
    typedef ADDRESS_TYPE address_type;
    typedef PARAMETER_TYPE parameter_type;
    typedef typename AsSigned<PARAMETER_TYPE>::type sparameter_type;
    typedef Linux<ADDRESS_TYPE, PARAMETER_TYPE> this_type;
	
    struct SysCall
    {
      virtual ~SysCall() {}
      virtual void Execute( Linux& lin, int syscall_id ) const;
      virtual void Describe( Linux& lin, std::ostream& sink ) const = 0;
      virtual char const* GetName() const = 0;
      virtual void Release() {}
      std::string TraceCall( Linux& lin ) const;

      static int HostToLinuxErrno(int host_errno); //< Errno conversion

      template <class ARGS>
      struct ArgsPrint
      {
        ArgsPrint( ARGS const& _args ) : args(_args) {} ARGS const& args;
        friend std::ostream& operator << (std::ostream& sink, ArgsPrint const& ap) { ap.args.Describe(sink); return sink; }
      };
      template <class ARGS> static ArgsPrint<ARGS> argsPrint( ARGS const& args ) { return ArgsPrint<ARGS>(args); }

    protected:
      // SysCall Friend accessing methods
      static int Target2HostFileDescriptor( Linux& lin, int32_t fd );
      static PARAMETER_TYPE GetParam(Linux& lin, int id); // <getting system call status
    };
	
    struct LSCExit { LSCExit( int _status ) : status( _status ) {} int status; };
	
    struct UTSName
    {
      std::string sysname;
      std::string nodename;
      std::string release;
      std::string version;
      std::string machine;
      std::string domainname;
    };
	
    // Target System Specific interface
    struct TargetSystem
    {
      TargetSystem( std::string _name, Linux& _lin ) : name( _name ), lin( _lin ) {}
      virtual ~TargetSystem() {}
      virtual bool SetupTarget() const = 0;
      virtual bool GetAT_HWCAP( ADDRESS_TYPE& hwcap ) const = 0;
      virtual bool SetSystemBlob( unisim::util::blob::Blob<ADDRESS_TYPE>* blob ) const = 0;
      virtual SysCall* GetSystemCall(int& id) const = 0;
      virtual PARAMETER_TYPE GetSystemCallParam(int id) const = 0;
      virtual void SetSystemCallStatus(int64_t ret, bool error) const = 0;
    protected:
      // TargetSystem Friend accessing methods
      unisim::service::interfaces::Registers& RegsIF() const { return *lin.regs_if_; }
      unisim::service::interfaces::Memory<ADDRESS_TYPE>& MemIF() const { return *lin.mem_if_; }
      std::string GetHWCAP() const { return lin.hwcap_; }
      SysCall* GetSysCall( std::string name ) const { return lin.GetSysCall( name ); }
      std::string name;
      Linux& lin;
    };

    Linux(std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream);
    ~Linux();

    void SetInterfaces(unisim::service::interfaces::Registers* regs_if,
                       unisim::service::interfaces::Memory<ADDRESS_TYPE>* mem_if,
                       unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE>* mem_inject_if);
    void SetVerbose(bool verbose) { verbose_ = verbose; }
    bool GetVerbose() const { return verbose_; };

    void SetCommandLine(std::vector<std::string> const &cmd) { argv_ = cmd; }
    std::vector<std::string> GetCommandLine() { return argv_; }

    bool SetEnvironment(std::vector<std::string> const &env);

    std::vector<std::string> GetEnvironment();

    void SetApplyHostEnvironment(bool use_host_environment);

    bool GetApplyHostEnvironment();

    void SetFileBlob(unisim::util::blob::Blob<ADDRESS_TYPE> const* file_blob);

    // Sets and gets the target specific system interface
    void SetTargetSystem(TargetSystem* _target_system) { target_system = _target_system; }
    TargetSystem* GetTargetSystem() { return target_system; }
	
    // Sets and gets the endianness of the system.
    // Note that if the loaded files endianness is different from the set
    // endianness the Load() method will fail.
    void SetEndianness(unisim::util::endian::endian_type endianness) { endianness_ = endianness; }
    unisim::util::endian::endian_type GetEndianness() const { return endianness_; }
	
    // Sets the stack base address that will be used for the application stack.
    // Combined with SetStackSize the memory addresses from stack base to
    // (stack base + stack size) will be reserved for the stack.
    void SetStackBase(ADDRESS_TYPE stack_base);
	
    // Sets the memory page size in bytes that will be used by the linux os
    // emulator
    void SetMemoryPageSize(ADDRESS_TYPE memory_page_size);

    // Sets the values that will be used by the uname system call.
    void SetUname(char const * const utsname_sysname,
                  char const * const utsname_nodename,
                  char const * const utsname_release,
                  char const * const utsname_version,
                  char const * const utsname_machine,
                  char const * const utsname_domainname);
	
    // HWCap
    void SetHWCap(const char *hwcap);

    // Set stdin/stdout/stderr pipe filenames
    void SetStdinPipeFilename(const char *filename);
    void SetStdoutPipeFilename(const char *filename);
    void SetStderrPipeFilename(const char *filename);

    // Compute the memory loading blob.
    bool Load();
    // Checks if the system was already successfully loaded or not.
    bool IsLoad();

    // Setup the target system using the memory loading blob using
    // the register and memory interface.
    bool SetupTarget();

    // Gets the memory loading blob.
    unisim::util::blob::Blob<ADDRESS_TYPE> const * const GetBlob() const { return load_blob; }
	
    // Gets the supplied logging streams
    std::ostream& DebugInfoStream() { return debug_info_stream; }
    std::ostream& DebugWarningStream() { return debug_warning_stream; }
    std::ostream& DebugErrorStream() { return debug_error_stream; }

    // Gets the entry_point_ of the loaded program
    ADDRESS_TYPE GetEntryPoint() const { return entry_point_; }

    // Executes the given system call id depending on the architecture the linux
    // emulation is working on.
    void ExecuteSystemCall(int id, bool& terminated, int& return_status);
    void SetSystemCallStatus(int64_t ret, bool error); // <writing system call status
	
    void LogSystemCall(int id);

    UTSName const& GetUTSName() const { return utsname; }
	
    bool is_load_; // true if a program has been successfully loaded, false
    // otherwise
	
    TargetSystem* target_system;
	
    unisim::util::endian::endian_type endianness_;

    // loaded blobs
    unisim::util::blob::Blob<ADDRESS_TYPE> const* file_blob;

    // program addresses (computed from the given files)
    ADDRESS_TYPE entry_point_;
    bool load_addr_set_;
    ADDRESS_TYPE load_addr_;
    ADDRESS_TYPE start_code_;
    ADDRESS_TYPE end_code_;
    ADDRESS_TYPE start_data_;
    ADDRESS_TYPE end_data_;
    ADDRESS_TYPE elf_stack_;
    ADDRESS_TYPE elf_brk_;
    int num_segments_;
    ADDRESS_TYPE stack_base_;
    uint64_t memory_page_size_;
    ADDRESS_TYPE brk_point_;


    // argc, argv and envp variables
    unsigned int argc_;
    std::vector<std::string> argv_;
    unsigned int envc_;
    std::vector<std::string> envp_;
    bool apply_host_environnement_;
    std::vector<std::string> target_envp_;

    // utsname values
    UTSName utsname;
    // HWCAP
    std::string hwcap_;

    // the structure to keep all the loaded information
    unisim::util::blob::Blob<ADDRESS_TYPE> *load_blob;

    // interfaces to the CPU registers and memory
    unisim::service::interfaces::Registers*                     regs_if_;
    unisim::service::interfaces::Memory<ADDRESS_TYPE>*          mem_if_;
    unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE>* mem_inject_if_;

    // verbosity
    bool verbose_;
    // logger streams
    std::ostream& debug_info_stream;
    std::ostream& debug_warning_stream;
    std::ostream& debug_error_stream;

    // program termination and return status
    bool terminated_;
    int return_status_;
	
    // stdin/stdout/stderr pipes
    std::string stdin_pipe_filename;
    std::string stdout_pipe_filename;
    std::string stderr_pipe_filename;
    int stdin_pipe_fd;
    int stdout_pipe_fd;
    int stderr_pipe_fd;

    // target to host file descriptors
    std::map<int32_t, int> target_to_host_fildes;
    std::queue<int32_t> target_fildes_free_list;

    // Retrieve the Debug Register interface from the target processor.
    unisim::service::interfaces::Register* GetDebugRegister( char const* regname ) const;

    // Read/Write From/To target processor
    bool SetTargetRegister( char const* regname, PARAMETER_TYPE value ) const;
    bool GetTargetRegister( char const* regname, PARAMETER_TYPE& value ) const;
    PARAMETER_TYPE GetTargetRegister( char const* regname ) const;
    bool ClearTargetRegister( char const* regname ) const;
    bool ReadTargetMemory( ADDRESS_TYPE addr, PARAMETER_TYPE& value ) const;
    // Read/Write From/To target processor via injection interface
    bool ReadMemory(ADDRESS_TYPE addr, uint8_t* const buffer, uint32_t size, bool) const;
    bool ReadMemory(ADDRESS_TYPE addr, PARAMETER_TYPE& value, bool) const;
    bool ReadString(ADDRESS_TYPE addr, std::string& str, bool) const;
    bool WriteMemory(ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size) const;
    bool WriteMemory(ADDRESS_TYPE addr, PARAMETER_TYPE value) const;

    // Load the executable blob into the given blob.
    bool CopyFileBlob(unisim::util::blob::Blob<ADDRESS_TYPE>* blob) const;

    // Gets the executable file blob (not the loading blob)
    unisim::util::blob::Blob<ADDRESS_TYPE> const* GetFileBlob() const { return file_blob; }

    // Computes the initial addresses and values that will be used to
    // initialize internal structures and the target processor
    bool ComputeStructuralAddresses();

    // Create the stack memory image and insert it into the given blob
    bool CreateStack(unisim::util::blob::Blob<ADDRESS_TYPE> * blob, uint64_t& stack_size) const;

    // Set the aux table contents that will be added to the stack
    void SetAuxTable(uint8_t * stack_data, ADDRESS_TYPE & sp) const;

    // Set the contents of an aux table entry
    ADDRESS_TYPE SetAuxTableEntry(uint8_t * stack_data, ADDRESS_TYPE sp,
                                  ADDRESS_TYPE entry, ADDRESS_TYPE value) const;

    // File management
    int OpenAt( int dfd, std::string const& filename, int flags, unsigned short mode );

    // File descriptors mapping
    int32_t AllocateFileDescriptor();
    void FreeFileDescriptor(int32_t fd);
    void MapTargetToHostFileDescriptor(int32_t target_fd, int host_fd);
    void UnmapTargetToHostFileDescriptor(int32_t target_fd);
	
    // The generic linux system call factories
    SysCall* GetSysCall( std::string _name );
  };

} // end of linux namespace
} // end of os namespace
} // end of util namespace
} // end of unisim namespace

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_HH__
