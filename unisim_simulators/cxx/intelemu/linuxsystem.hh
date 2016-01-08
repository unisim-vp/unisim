#ifndef IA32_LINUXSYSTEM_HH
#define IA32_LINUXSYSTEM_HH

#ifndef _LARGEFILE64_SOURCE
# define _LARGEFILE64_SOURCE 1
#endif // _LARGEFILE64_SOURCE

#undef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64

#include <dtlib/fwd.hh>
#include <dtlib/sink.hh>
#include <vector>
#include <string>
#include <inttypes.h>

namespace todel {
  struct LinuxSystem
  {
    /* General*/
    enum Exception_t { ExecutableError, LoadTimeError, RunTimeError };
    
    LinuxSystem();
    ~LinuxSystem();
    
    bool                m_verbose;
    dtlib::Sink         m_sink;
    
    /* Loading support */
    bool                m_dynamic_p;
    void                dynamic_image() { m_dynamic_p = true; }
    
    typedef std::vector<std::string> strvec;
    void static_setup( dtlib::Target& _target, dtlib::ELF::Root const& _img, strvec const& args, strvec const& envs );
    
    // Syscall support
    
    uint32_t            m_brk_addr;
    int                 m_exit_status;
    
    uint32_t            stringlength( uint32_t _addr, dtlib::Target& _arch );
    
    void                syscall( int _sc, dtlib::Target& _arch );
    uint32_t            scident( dtlib::Target& _arch );
    uint32_t            scparam( int _idx, dtlib::Target& _arch );
    void                screturn( uint32_t _res, dtlib::Target& _arch );
    void                scerror( int _err, dtlib::Target& _arch );
    
    // Common unix syscalls
    void                unimplemented_syscall( dtlib::Target& target );
    void                exit( dtlib::Target& target );
    void                uname( dtlib::Target& target );
    void                open( dtlib::Target& target );
    void                read( dtlib::Target& target );
    void                close( dtlib::Target& target );
    void                chdir( dtlib::Target& target );
    void                access( dtlib::Target& target );
    void                time( dtlib::Target& target );
    void                lseek( dtlib::Target& target );
    void                llseek( dtlib::Target& target );
    //void                getdents( dtlib::Target& target );
    void                times( dtlib::Target& target );
    void                brk( dtlib::Target& target );
    void                ioctl( dtlib::Target& target );
    void                gettimeofday( dtlib::Target& target );
    void                getcwd( dtlib::Target& target );
    void                geteuid32( dtlib::Target& target );
    void                getegid32( dtlib::Target& target );
    void                getuid32( dtlib::Target& target );
    void                getgid32( dtlib::Target& target );
    void                ftruncate64( dtlib::Target& target );
    void                stat64( dtlib::Target& target );
    void                fstat64( dtlib::Target& target );
    void                sigaction( dtlib::Target& target );
    void                sigprocmask( dtlib::Target& target );
    void                ugetrlimit( dtlib::Target& target );
    void                readlink( dtlib::Target& target );
    void                mmap( dtlib::Target& target );
    void                write( dtlib::Target& target );
    void                exit_group( dtlib::Target& target );
    void                clock_gettime( dtlib::Target& target );
    void                readv( dtlib::Target& target );
    void                writev( dtlib::Target& target );
    
    // i386 specific syscalls
    
    /** @brief GDT entry -- Segment descriptor emulate the x86 segment
     * descriptor. Some fields are ignored due to the limited segment
     * descriptor emulation. Base address is kept as a 32 bit
     * address. The "limit" fields is suppressed (all segment are
     * 4GB). All attributes are not maintained.
     * 
     */
    struct GDTEntry_t {
      uint32_t          m_number;
      uint32_t          m_baseaddr;
      GDTEntry_t*       m_next;
      
      GDTEntry_t( uint32_t _number, uint32_t _baseaddr, GDTEntry_t* _next )
        : m_number( _number ), m_baseaddr( _baseaddr ), m_next( _next ) {}
      ~GDTEntry_t() { delete m_next; }
    };
    GDTEntry_t*         m_gdtentries;
    GDTEntry_t&         gdtentry( uint32_t _number );
    void                remove_gdtentry( uint32_t _number );
    
    // Linux specific syscalls
    void                set_thread_storage( dtlib::Target& target );
    void                set_tid_address( dtlib::Target& target );
    uint32_t            m_clear_child_tid;
    void                set_robust_list( dtlib::Target& target );
    uint32_t            m_futex_listhead;
    uint32_t            m_futex_itemsize;
    void                tgkill( dtlib::Target& target );
    void                openat( dtlib::Target& target );
    void                futex( dtlib::Target& target );
    
  };
};

#endif // IA32_LINUXSYSTEM_HH
