#include <linuxsystem.hh>
#include <unisim/component/cxx/processor/intel/segments.hh>
#include <dtlib/target.hh>
#include <dtlib/loader.hh>
#include <dtlib/misc.hh>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cerrno>
#include <cstring>
// Linux host emulation
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

 
namespace todel
{
  LinuxSystem::LinuxSystem()
    : m_verbose( 0 ), m_dynamic_p( false ), m_gdtentries( 0 )
  {}
  
  LinuxSystem::~LinuxSystem()
  {
    delete m_gdtentries;
  }
  
  void
  LinuxSystem::static_setup( dtlib::Target& _target, dtlib::ELF::Root const& _img, strvec const& args, strvec const& envs )
  {
    if (m_dynamic_p) {
      std::cerr << "Won't statically load a dynamic image...\n";
      throw LoadTimeError;
    }
    _target.seteip( _img.entrypoint() );
    if (m_verbose) {
      dtlib::osprintf( m_sink(), "Entrypoint: %#010x\n", _img.entrypoint() );
    }
    
    // Two-pass stack initialisation
    uint32_t item_count[zone_count];
    uint32_t item_sizes[zone_count];
    uint32_t item_align[zone_count];
    for (int idx = 0; idx < zone_count; ++ idx)
      {  item_count[idx] = 0; item_sizes[idx] = 1; item_align[idx] = 1; }

    // Program headers
    uint32_t  phnum = _img.phnum(), phentsize = _img.phentsize();
    item_count[program_headers] = phnum;
    item_sizes[program_headers] = phentsize;
    item_align[program_headers] = phentsize;
    
    // thread local storage
    item_count[thread_local_storage] = 1;
    item_sizes[thread_local_storage] = 0x1000;
    item_align[thread_local_storage] = 0x1000;
    
    { // Environ
      uint32_t env_values_size = 0;
      for (strvec::const_iterator itr = envs.begin(), end = envs.end(); itr != end; ++itr)
        env_values_size += (itr->size() + 1); // Null terminated string
      item_count[env_values] = env_values_size;
      item_count[env_vector] = envs.size() + 1; // Null terminated vector
      item_sizes[env_vector] = 4; // 4 bytes pointers
      item_align[env_vector] = 4; // 4 bytes pointers
    }
    
    { // Arguments
      uint32_t arg_values_size = 0;
      for (strvec::const_iterator itr = args.begin(), end = args.end(); itr != end; ++itr)
        arg_values_size += (itr->size() + 1);
      item_count[arg_values] = arg_values_size;
      item_count[arg_vector] = args.size() + 1; // Null terminated vector
      item_sizes[arg_vector] = 4; // 4 bytes pointers
      item_align[arg_vector] = 4; // 4 bytes pointers
    }
    
    // Auxillary vector
    item_count[aux_vector] = (4 + 1); // Null terminated vector
    item_sizes[aux_vector] = 8; // 8 bytes Auxillary structures
    item_align[aux_vector] = 4; // on ix86 linux system, AUXV is 4-bytes-aligned
    
    // Arguments count
    item_count[arg_count] = 1;
    item_sizes[arg_count] = 4; // 4 bytes pointer
    item_align[arg_count] = 4; // 4 bytes pointer

    uint32_t frameptr = 0xc0000000;
    uint32_t stackptr = frameptr;

    for (int idx = 0; idx < zone_count; ++ idx) {
      stackptr -= (item_sizes[idx] * item_count[idx]);
      stackptr &= ~(item_align[idx]-1);
    }
    // Fixing misalignment (stack pointer should be 8-aligned when
    // starting). Because AUXV is 4-bytes-aligned, a simple padding
    // may be perform before AUXV.
    item_count[stack_padding] = (stackptr & 0x7);
    
    // Computing setup stack mapping (2nd pass)
    stackptr = frameptr;
    for (int idx = 0; idx < zone_count; ++ idx) {
      stackptr -= (item_sizes[idx] * item_count[idx]);
      stackptr &= ~(item_align[idx]-1);
      m_zones[idx] = stackptr;
    }
    
    assert( (stackptr & 0x7) == 0 );

    // Program headers
    _target.write( m_zones[program_headers], _img.m_base + _img.phoff(), phentsize*phnum );

    // TLS
    uint32_t tls_base = m_zones[thread_local_storage] + 0x800;
    _target.clear( m_zones[thread_local_storage], 0x1000 );
    
    // Auxilary vector
    struct AuxVector_t { uint32_t m_type, m_value; };
    enum AuxType_t { ATNULL=0, ATPHDR=3, ATPHENT=4, ATPHNUM=5, ATPAGESZ=6 };
    AuxVector_t aux_vector_init[] =
      {{ATPAGESZ,0x1000},{ATPHDR,m_zones[program_headers]},{ATPHENT,phentsize},{ATPHNUM,phnum},{ATNULL,0}};
    for (int idx = 0; idx < 5; ++ idx) {
      _target.write32( m_zones[aux_vector] + 8*idx + 0, aux_vector_init[idx].m_type );
      _target.write32( m_zones[aux_vector] + 8*idx + 4, aux_vector_init[idx].m_value );
    }

    { // Environment vector
      uint32_t env_val_ptr = m_zones[env_values], env_vec_ptr = m_zones[env_vector];
      for (strvec::const_iterator itr = envs.begin(), end = envs.end(); itr != end; ++itr) {
        uint32_t len = itr->size() + 1;
        _target.write32( env_vec_ptr, env_val_ptr );
        _target.write( env_val_ptr, (uint8_t const*)(itr->c_str()), len );
        env_vec_ptr += 4;
        env_val_ptr += len;
      }
      _target.write32( env_vec_ptr, 0 ); // Null termination
    }

    { // Arguments vector
      uint32_t arg_val_ptr = m_zones[arg_values], arg_vec_ptr = m_zones[arg_vector];
      for (strvec::const_iterator itr = args.begin(), end = args.end(); itr != end; ++itr) {
        uint32_t len = itr->size() + 1;
        _target.write32( arg_vec_ptr, arg_val_ptr );
        _target.write( arg_val_ptr, (uint8_t const*)(itr->c_str()), len );
        arg_vec_ptr += 4;
        arg_val_ptr += len;
      }
      _target.write32( arg_vec_ptr, 0 ); // Null termination
    }
    // ArgC placeholder
    _target.write32( m_zones[arg_count], args.size() );
    
    // FIXME: and the following ?
    // Stack registers
    _target.setesp( stackptr );
    
    // Initializing segmented memory
    gdtentry( 0xe ).m_baseaddr = 0; // code segment
    gdtentry( 0xf ).m_baseaddr = 0; // data segment
    _target.segregwrite( unisim::component::cxx::processor::intel::ES, 0xf << 3 | 0 << 2 | 0x3 << 0 );
    _target.segregwrite( unisim::component::cxx::processor::intel::CS, 0xe << 3 | 0 << 2 | 0x3 << 0 );
    _target.segregwrite( unisim::component::cxx::processor::intel::SS, 0xf << 3 | 0 << 2 | 0x3 << 0 );
    _target.segregwrite( unisim::component::cxx::processor::intel::DS, 0xf << 3 | 0 << 2 | 0x3 << 0 );
    
    // The following code is clearly a hack since linux doesn't
    // allocate any TLS. Allocating the TLS is the libc's
    // responsability. Nevertheless, because syscalls emulation is
    // incomplete, errno (located in TLS) may be used before any libc
    // attempt to initialize it.
    _target.write32( tls_base, tls_base );
    gdtentry( 0x10 ).m_baseaddr = tls_base;
    _target.segregwrite( unisim::component::cxx::processor::intel::GS, 0x10 << 3 | 0 << 2 | 0x3 << 0 );
    
    // Heap setup
    uint32_t heap_start = _target.firstfreepage();
    assert( heap_start < 0xe0000000 ); // FIXME: should handle that properly
    this->m_brk_addr = (heap_start + uint32_t( 0xfff )) & ~uint32_t( 0xfff );

    // Static setup
    //    _target.m_gpr[1] = 0;
  }
  
  void
  LinuxSystem::setup( dtlib::Target& _target, dtlib::ELF::Root const& _img, int argc, char** argv, char** envp )
  {
    _target.seteip( _img.entrypoint() );
    if (m_verbose) {
      dtlib::osprintf( m_sink(), "Entrypoint: %#010x\n", _img.entrypoint() );
    }
    
    // Two-pass stack initialisation
    uint32_t item_count[zone_count];
    uint32_t item_sizes[zone_count];
    uint32_t item_align[zone_count];
    for (int idx = 0; idx < zone_count; ++ idx)
      {  item_count[idx] = 0; item_sizes[idx] = 1; item_align[idx] = 1; }

    // Program headers
    uint32_t  phnum = _img.phnum(), phentsize = _img.phentsize();
    item_count[program_headers] = phnum;
    item_sizes[program_headers] = phentsize;
    item_align[program_headers] = phentsize;
    
    // thread local storage
    item_count[thread_local_storage] = 1;
    item_sizes[thread_local_storage] = 0x1000;
    item_align[thread_local_storage] = 0x1000;
    
    { // Environ
      uint32_t env_vector_size = 0, env_values_size = 0;
      for (int eidx = 0; envp[eidx]; ++eidx) {
        env_values_size += (::strlen( envp[eidx] ) + 1); // Null terminated string
        env_vector_size += 1;
      }
      item_count[env_values] = env_values_size;
      item_count[env_vector] = env_vector_size + 1; // Null terminated vector
      item_sizes[env_vector] = 4; // 4 bytes pointers
      item_align[env_vector] = 4; // 4 bytes pointers
    }
    
    { // Arguments
      uint32_t arg_values_size = 0;
      for (int aidx = 0; aidx < argc; ++aidx)
        arg_values_size += (::strlen( argv[aidx] ) + 1);
      item_count[arg_values] = arg_values_size;
      item_count[arg_vector] = argc + 1; // Null terminated vector
      item_sizes[arg_vector] = 4; // 4 bytes pointers
      item_align[arg_vector] = 4; // 4 bytes pointers
    }
    
    // Auxillary vector
    item_count[aux_vector] = (4 + 1); // Null terminated vector
    item_sizes[aux_vector] = 8; // 8 bytes Auxillary structures
    item_align[aux_vector] = 4; // on ix86 linux system, AUXV is 4-bytes-aligned
    
    // Arguments count
    item_count[arg_count] = 1;
    item_sizes[arg_count] = 4; // 4 bytes pointer
    item_align[arg_count] = 4; // 4 bytes pointer

    uint32_t frameptr = 0xc0000000;
    uint32_t stackptr = frameptr;

    for (int idx = 0; idx < zone_count; ++ idx) {
      stackptr -= (item_sizes[idx] * item_count[idx]);
      stackptr &= ~(item_align[idx]-1);
    }
    // Fixing misalignment (stack pointer should be 8-aligned when
    // starting). Because AUXV is 4-bytes-aligned, a simple padding
    // may be perform before AUXV.
    item_count[stack_padding] = (stackptr & 0x7);
    
    // Computing setup stack mapping (2nd pass)
    stackptr = frameptr;
    for (int idx = 0; idx < zone_count; ++ idx) {
      stackptr -= (item_sizes[idx] * item_count[idx]);
      stackptr &= ~(item_align[idx]-1);
      m_zones[idx] = stackptr;
    }
    
    assert( (stackptr & 0x7) == 0 );

    // Program headers
    _target.write( m_zones[program_headers], _img.m_base + _img.phoff(), phentsize*phnum );

    // TLS
    uint32_t tls_base = m_zones[thread_local_storage] + 0x800;
    _target.clear( m_zones[thread_local_storage], 0x1000 );
    
    // Auxilary vector
    struct AuxVector_t { uint32_t m_type, m_value; };
    enum AuxType_t { ATNULL=0, ATPHDR=3, ATPHENT=4, ATPHNUM=5, ATPAGESZ=6 };
    AuxVector_t aux_vector_init[] =
      {{ATPAGESZ,0x1000},{ATPHDR,m_zones[program_headers]},{ATPHENT,phentsize},{ATPHNUM,phnum},{ATNULL,0}};
    for (int idx = 0; idx < 5; ++ idx) {
      _target.write32( m_zones[aux_vector] + 8*idx + 0, aux_vector_init[idx].m_type );
      _target.write32( m_zones[aux_vector] + 8*idx + 4, aux_vector_init[idx].m_value );
    }

    { // Environment vector
      uint32_t env_val_ptr = m_zones[env_values], env_vec_ptr = m_zones[env_vector];
      for (int eidx = 0; envp[eidx]; eidx++) {
        uint32_t len = ::strlen( envp[eidx] ) + 1;
        _target.write32( env_vec_ptr, env_val_ptr );
        _target.write( env_val_ptr, (uint8_t*)(envp[eidx]), len );
        env_vec_ptr += 4;
        env_val_ptr += len;
      }
      _target.write32( env_vec_ptr, 0 ); // Null termination
    }

    { // Arguments vector
      uint32_t arg_val_ptr = m_zones[arg_values], arg_vec_ptr = m_zones[arg_vector];
      for (int aidx = 0; aidx < argc; aidx++) {
        uint32_t len = ::strlen( argv[aidx] ) + 1;
        _target.write32( arg_vec_ptr, arg_val_ptr );
        _target.write( arg_val_ptr, (uint8_t*)argv[aidx], len );
        arg_vec_ptr += 4;
        arg_val_ptr += len;
      }
      _target.write32( arg_vec_ptr, 0 );
    }
    // Argc placeholder
    _target.write32( m_zones[arg_count], argc );
    
    // FIXME: and the following ?
    // Stack registers
    _target.setesp( stackptr );
    
    // Initializing segmented memory
    gdtentry( 0xe ).m_baseaddr = 0; // code segment
    gdtentry( 0xf ).m_baseaddr = 0; // data segment
    _target.segregwrite( unisim::component::cxx::processor::intel::ES, 0xf << 3 | 0 << 2 | 0x3 << 0 );
    _target.segregwrite( unisim::component::cxx::processor::intel::CS, 0xe << 3 | 0 << 2 | 0x3 << 0 );
    _target.segregwrite( unisim::component::cxx::processor::intel::SS, 0xf << 3 | 0 << 2 | 0x3 << 0 );
    _target.segregwrite( unisim::component::cxx::processor::intel::DS, 0xf << 3 | 0 << 2 | 0x3 << 0 );
    
    // The following code is clearly a hack since linux doesn't
    // allocate any TLS. Allocating the TLS is the libc's
    // responsability. Nevertheless, because syscalls emulation is
    // incomplete, errno (located in TLS) may be used before any libc
    // attempt to initialize it.
    _target.write32( tls_base, tls_base );
    gdtentry( 0x10 ).m_baseaddr = tls_base;
    _target.segregwrite( unisim::component::cxx::processor::intel::GS, 0x10 << 3 | 0 << 2 | 0x3 << 0 );
    
    // Heap setup
    uint32_t heap_start = _target.firstfreepage();
    assert( heap_start < 0xe0000000 ); // FIXME: should handle that properly
    this->m_brk_addr = (heap_start + uint32_t( 0xfff )) & ~uint32_t( 0xfff );

    // Static setup
    //    _target.m_gpr[1] = 0;
  }

  void
  LinuxSystem::syscall( int _sc, dtlib::Target& _target ) {
    switch( _sc ) {
    case 1: this->exit( _target ); break;
    case 3: this->read( _target ); break;
    case 4: this->write( _target ); break;
    case 5: this->open( _target ); break;
    case 6: this->close( _target ); break;
    case 12: this->chdir( _target ); break;
    case 13: this->time( _target ); break;
    case 19: this->lseek( _target ); break;
    case 33: this->access( _target ); break;
    case 43: this->times( _target ); break;
    case 45: this->brk( _target ); break;
    case 54: this->ioctl( _target ); break;
    case 78: this->gettimeofday( _target ); break;
    case 85: this->readlink( _target ); break;
    case 90: this->mmap( _target ); break; // old mmap
    case 122: this->uname( _target ); break;
    case 140: this->llseek( _target ); break;
    case 146: this->writev( _target ); break;
    case 174: this->sigaction( _target ); break;
    case 175: this->sigprocmask( _target ); break;
    case 183: this->getcwd( _target ); break;
    case 191: this->ugetrlimit( _target ); break;
    case 192: this->mmap( _target ); break; // new mmap2
    case 195: this->stat64( _target ); break;
    case 197: this->fstat64( _target ); break;
    case 199: this->getuid32( _target ); break;
    case 200: this->getgid32( _target ); break;
    case 201: this->geteuid32( _target ); break;
    case 202: this->getegid32( _target ); break;
    case 240: this->futex( _target ); break;
    case 243: this->set_thread_storage( _target ); break;
    case 252: this->exit_group( _target ); break;
    case 258: this->set_tid_address( _target ); break;
    case 265: this->clock_gettime( _target ); break;
    case 270: this->tgkill( _target ); break;
    case 311: this->set_robust_list( _target ); break;
    default:
      std::cerr << "Unhandled system call #" << _sc << " (@" << std::hex << _target.geteip() << ")\n";
      throw RunTimeError;
      break;
    }
  }
  
  uint32_t
  LinuxSystem::scident( dtlib::Target& _target ) {
    assert( false ); /* FIXME */
    //     return _target.m_EREG[1];
    return 0;
  }
  
  uint32_t
  LinuxSystem::scparam( int _idx, dtlib::Target& _target ) {
    uint32_t reg_idx;
    switch( _idx ) {
    case 0: reg_idx = 3; break;
    case 1: reg_idx = 1; break;
    case 2: reg_idx = 2; break;
    case 3: reg_idx = 6; break;
    case 4: reg_idx = 7; break;
    case 5: reg_idx = 5; break;
    default: assert( false );
    }
    
    return _target.regread32( reg_idx );
  }

  /* x86 Linux uses a negative return value to indicate syscall
   * errors, unlike most Unices, which use the condition codes' carry
   * flag. */
  void
  LinuxSystem::scerror( uint32_t _err, dtlib::Target& _target ) {
    _target.regwrite32( 0 , _err );
  }

  /* Linus said he will make sure the no syscall returns a value in
   * -1..-4095 as a valid result */
  void
  LinuxSystem::screturn( uint32_t _res, dtlib::Target& _target ) {
    assert( _res <= 0xfffff000 );
    _target.regwrite32( 0 , _res );
  }
  
  uint32_t
  LinuxSystem::stringlength( uint32_t begaddr, dtlib::Target& _target )
  {
    uint32_t endaddr = begaddr & -4, misalignment = begaddr & 3, word = _target.read32( endaddr ), zerobytes;
    
    if (misalignment) word |= (0xffffffff >> (32 - misalignment*8));
    
    while (!(zerobytes = (word-1) & ~word & 0x80808080))
      {
        endaddr += 4;
        word = _target.read32( endaddr );
      }
    
    endaddr += (__builtin_ctzl( zerobytes ) / 8 );
    return endaddr - begaddr;
  }

  void
  LinuxSystem::unimplemented_syscall( dtlib::Target& _target ) {
    int syscall_id = int( this->scident( _target ) );
    dtlib::osprintf( std::cerr, "[syscall] unimplemented (%d)...\n", syscall_id );
    assert( false );
  }
  
  void
  LinuxSystem::exit( dtlib::Target& _target ) {
    m_exit_status = this->scparam( 0, _target );
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] exit( status=%d )\n", _target.geteip(), m_exit_status );
    _target.stop();
  }

  void
  LinuxSystem::uname( dtlib::Target& _target ) {
    uint32_t utsname_dst_ptr = this->scparam( 0, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] uname( base=%#x )\n", _target.geteip(), utsname_dst_ptr );
    
    // static char const* uname_strings[] = {
    //   "Linux",
    //   "localhost",
    //   "2.6.33.7-desktop-2mnb",
    //   "#1",
    //   "i686",
    //   "(none)"
    // };
    
    // uint32_t const uname_string_length = 257;
    
    // for (int idx = 0; idx < 6; ++ idx) {
    //   uint32_t len = ::strlen( uname_strings[idx] ) + 1;
    //   assert( len <= uname_string_length );
    //   _target.write( utsname_dst_ptr + idx*uname_string_length, (uint8_t const*)(uname_strings[idx]), len );
    //   utsname_dst_ptr += uname_string_length;
    // }
    
    // this->screturn( 0, _target );
    
    // A systematic error forces glibc to inspect "/proc/sys/kernel/osrelease"
    this->scerror( uint32_t( -EINVAL ), _target );
  }

  void
  LinuxSystem::access( dtlib::Target& _target )
  {
    uint32_t path_addr = this->scparam( 0, _target );
    uint32_t path_len = this->stringlength( path_addr, _target ) + 1;
    int mode = int( this->scparam( 1, _target ) );
    
    char path[path_len];
    _target.read( (uint8_t*)path, path_addr, path_len );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] access( path=%s, mode=%#x )\n", _target.geteip(), path, mode );
    
    int ret = ::access( path, mode );
    
    if (ret <  0) this->scerror( uint32_t( -(errno) ), _target );
    else          this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::open( dtlib::Target& _target )
  {
    uint32_t path_addr = this->scparam( 0, _target );
    uint32_t path_len = this->stringlength( path_addr, _target ) + 1;
    int flags = int( this->scparam( 1, _target ) );
    mode_t mode = mode_t( this->scparam( 2, _target ) );
    
    char path[path_len];
    _target.read( (uint8_t*)path, path_addr, path_len );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] open( path=%s, flags=%#x, mode=%#x )\n", _target.geteip(), path, flags, mode );
    
    int ret = ::open( path, flags, mode );
    
    if (ret <  0) this->scerror( uint32_t( -(errno) ), _target );
    else          this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::read( dtlib::Target& _target ) {
    int fd = int( this->scparam( 0, _target ) );
    uint32_t bufaddr = this->scparam( 1, _target );
    size_t count = size_t( this->scparam( 2, _target ) );
    size_t ret;
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] read( fd=%d, bufaddr=%#x, count=%#x )\n", _target.geteip(), fd, bufaddr, count );
    assert( count < 0x1000000 );
    uint8_t buffer[count];
    ret = ::read( fd, buffer, count );
    if (ret < 0) { this->scerror( uint32_t( -(errno) ), _target ); return; }
    if (ret > 0) _target.write( bufaddr, buffer, count );
    this->screturn( ret, _target );
  }
  
  void LinuxSystem::write( dtlib::Target& _target ) {
    int fd = int( this->scparam( 0, _target ) );
    uint32_t bufaddr = this->scparam( 1, _target );
    size_t count = size_t( this->scparam( 2, _target ) );
    size_t ret;
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] write( fd=%d, base=%#x, count=%#x )\n", _target.geteip(), fd, bufaddr, count );
    assert( count < 0x1000000 );
    uint8_t buffer[count];
    _target.read( buffer, bufaddr, count );
    ret = ::write( fd, buffer, count );
    if (ret < 0) this->scerror( uint32_t( -(errno) ), _target );
    else         this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::close( dtlib::Target& _target ) {
    int fd = this->scparam( 0, _target );
    int ret = ::close( fd );
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] close( fd=%d )\n", _target.geteip(), fd );
    
    if (ret <  0) this->scerror( uint32_t( -(errno) ), _target );
    else          this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::chdir( dtlib::Target& _target )
  {
    uint32_t path_addr = this->scparam( 0, _target );
    uint32_t path_len = this->stringlength( path_addr, _target ) + 1;
    char path[path_len];
    _target.read( (uint8_t*)path, path_addr, path_len );
    
    int ret = ::chdir( &path[0] );
    
    if (ret <  0) this->scerror( uint32_t( -(errno) ), _target );
    else          this->screturn( 0, _target );
  }
  
  void
  LinuxSystem::time( dtlib::Target& _target ) {
    uint32_t time_ptr = this->scparam( 0, _target );
    assert( sizeof (time_t) == 4 );
    time_t ret = ::time( 0 );
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] time( time_ptr=0x%x )\n", _target.geteip(), time_ptr );
    if (ret != (time_t)-1 and time_ptr != 0)
      _target.write32( time_ptr, ret );
    this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::llseek( dtlib::Target& _target )
  {
    int fd = this->scparam( 0, _target );;
    uint32_t offset_high = this->scparam( 1, _target );
    uint32_t offset_low = this->scparam( 2, _target );
    uint32_t result_addr = this->scparam( 3, _target );
    int whence = this->scparam( 4, _target );
    
    assert( sizeof (off_t) == 4 );
    assert( sizeof (loff_t) == 8 );

    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] llseek( fd=%d, "
                       "offset_high=0x%x, offset_low=0x%x, "
                       "result=0x%x, whence=%d )\n",
                       _target.geteip(), fd, offset_high, offset_low,
                       result_addr, whence );
    
    loff_t loff;
    int res = ::syscall( SYS__llseek, fd, offset_high, offset_low, &loff, whence );
    
    if (res == 0) {
      /* Success */
      uint8_t result[8];
      for (int idx = 0; idx < 8; ++idx)
        result[idx] = (loff >> (idx*8)) & 0xff;
      _target.write( result_addr, &result[0], sizeof (result) );
      this->screturn( res, _target );
    } else {
      this->scerror( uint32_t( -(errno) ), _target );
    }
  }
  
  void
  LinuxSystem::lseek( dtlib::Target& _target )
  {
    int fd = this->scparam( 0, _target );
    uint32_t offset = this->scparam( 1, _target );
    int whence = this->scparam( 2, _target );
    assert( sizeof (off_t) == 4 );

    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] lseek( fd=%d, offset=0x%x, whence=%d )\n", _target.geteip(), fd, offset, whence );
    
    off_t ret = ::lseek( fd, offset, whence );
    
    if (ret <  0) this->scerror( uint32_t( -(errno) ), _target );
    else          this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::times( dtlib::Target& _target )
  {
    uint32_t buf_addr = this->scparam( 0, _target );
    
    assert( sizeof (clock_t) == 4 );
    
    struct tms buf;
    clock_t ret = ::times( &buf );
    
    if (ret != (clock_t)(-1))
      _target.write( buf_addr, (uint8_t*)&buf, sizeof (buf) );
    
    this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::brk( dtlib::Target& _target ) {
    uint32_t new_brk_addr = this->scparam( 0, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] brk( new_brk_addr=%#x )\n", _target.geteip(), new_brk_addr );
    if (new_brk_addr > m_brk_addr)
      m_brk_addr = new_brk_addr;

    this->screturn( m_brk_addr, _target );
  }
  
  void
  LinuxSystem::ioctl( dtlib::Target& _target )
  {
    int fd = this->scparam( 0, _target );
    int request = this->scparam( 1, _target );

    if (m_verbose)
      dtlib::osprintf( m_sink(),
                       "%#010x: [syscall] ioctl( fd=%d, request=%#x )\n",
                       _target.geteip(), fd, request );
    
    
    // switch (request) {
    // case 0x5401: {
    //   uint32_t termios_addr = this->scparam( 2, _target );
    //   struct termios termios_buf;
    //   int ret = ::ioctl( fd, request, &termios_buf );
    //   if (ret < 0)
    //     this->scerror( uint32_t( -(errno) ), _target );
    //   else {
    //     _target.write( termios_addr, (uint8_t*)&termios_buf, sizeof (termios_buf) );
    //     this->screturn( ret, _target );
    //   }
    // } break;
    // default:  
    //   std::cerr << "Unhandled ioctl call 0x" << std::hex << request << " (@" << _target.geteip() << std::dec << ")\n";
    //   throw RunTimeError;
    // }
    
    this->scerror( uint32_t( -(EINVAL) ), _target );
  }
  
  void
  LinuxSystem::gettimeofday( dtlib::Target& _target )
  {
    uint32_t tv_addr = this->scparam( 0, _target );
    uint32_t tz_addr = this->scparam( 1, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] "
                       "gettimeofday( tv=%#x, tz=%#x )\n",
                       _target.geteip(), tv_addr, tz_addr );
    
    struct timeval  tv;
    struct timezone tz;
    int ret = ::gettimeofday( &tv, &tz );
    
    if (ret != 0)
      this->scerror( uint32_t( -(errno) ), _target );
    else {
      _target.write( tv_addr, (uint8_t*)&tv, sizeof (tv) );
      _target.write( tz_addr, (uint8_t*)&tz, sizeof (tz) );
      this->screturn( ret, _target );
    }
  }

  void
  LinuxSystem::clock_gettime( dtlib::Target& _target )
  {
    clockid_t clk_id = clockid_t( this->scparam( 0, _target ) );
    uint32_t ts_addr = uint32_t( this->scparam( 1, _target ) );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] "
                       "clock_gettime( clk_id=%d, ts=%#x )\n",
                       _target.geteip(), clk_id, ts_addr );
    
    struct timespec ts;
    int ret = ::clock_gettime( clk_id, &ts );
    
    if (ret != 0)
      this->scerror( uint32_t( -(errno) ), _target );
    else {
      _target.write( ts_addr, (uint8_t*)&ts, sizeof (ts) );
      this->screturn( ret, _target );
    }
  }

  void LinuxSystem::getuid32( dtlib::Target& _target ) { this->screturn( uint32_t( ::getuid() ), _target ); }

  void LinuxSystem::getgid32( dtlib::Target& _target ) { this->screturn( uint32_t( ::getgid() ), _target ); }

  void LinuxSystem::geteuid32( dtlib::Target& _target ) { this->screturn( uint32_t( ::geteuid() ), _target ); }

  void LinuxSystem::getegid32( dtlib::Target& _target ) { this->screturn( uint32_t( ::getegid() ), _target ); }
  
  template <typename dstT, typename srcT>
  void write_le( uint8_t* where, dstT typeval, srcT value )
  {
    assert( srcT( dstT( value ) ) == value );
    for (unsigned int idx = 0; idx < sizeof (dstT); ++idx) where[idx] = uint8_t( value >> (idx*8) );
  }
  
  void LinuxSystem::stat64( dtlib::Target& _target )
  {
    uint32_t path_addr = this->scparam( 0, _target );
    uint32_t path_len = this->stringlength( path_addr, _target ) + 1;
    uint32_t bufaddr = this->scparam( 1, _target );
    int ret;
    
    char path[path_len];
    _target.read( (uint8_t*)path, path_addr, path_len );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] stat64( path=%#x, base=%#x )\n", _target.geteip(), path_addr, bufaddr );
    
    // Emulating with a single stat struct, which is sufficient for
    // file < 4GB. On some systems, st_dev may size 4 whereas target
    // st_dev is 8 bytes; these 4 bytes will be zero extended to 8
    // bytes...
    struct stat64 host_stat;
    ret = ::stat64( &path[0], &host_stat );
    if (ret < 0) { this->scerror( uint32_t( -(errno) ), _target ); return; }

    uint8_t                     scratch[0x60];
    ::memset( (void*)&scratch[0], 0, sizeof (scratch) );
    
    write_le( &scratch[0x00], uint64_t(), host_stat.st_dev );
    write_le( &scratch[0x0c], uint32_t(), host_stat.__st_ino );
    write_le( &scratch[0x10], uint32_t(), host_stat.st_mode );
    write_le( &scratch[0x14], uint32_t(), host_stat.st_nlink );
    write_le( &scratch[0x18], uint32_t(), host_stat.st_uid );
    write_le( &scratch[0x1c], uint32_t(), host_stat.st_gid );
    write_le( &scratch[0x20], uint64_t(), host_stat.st_rdev );
    write_le( &scratch[0x2c], uint64_t(), host_stat.st_size );
    write_le( &scratch[0x34], uint32_t(), host_stat.st_blksize );
    write_le( &scratch[0x38], uint64_t(), host_stat.st_blocks );
    write_le( &scratch[0x40], uint32_t(), host_stat.st_atim.tv_sec );
    write_le( &scratch[0x44], uint32_t(), host_stat.st_atim.tv_nsec );
    write_le( &scratch[0x48], uint32_t(), host_stat.st_mtim.tv_sec );
    write_le( &scratch[0x4c], uint32_t(), host_stat.st_mtim.tv_nsec );
    write_le( &scratch[0x50], uint32_t(), host_stat.st_ctim.tv_sec );
    write_le( &scratch[0x54], uint32_t(), host_stat.st_ctim.tv_nsec );
    write_le( &scratch[0x58], uint64_t(), host_stat.st_ino );
    
    _target.write( bufaddr, &scratch[0], sizeof (scratch) );
    
    this->screturn( ret, _target );
  }

  void LinuxSystem::fstat64( dtlib::Target& _target )
  {
    int fd = this->scparam( 0, _target );
    uint32_t bufaddr = this->scparam( 1, _target );
    int ret;
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] fstat64( fd=%d, base=%#x )\n", _target.geteip(), fd, bufaddr );
    
    // Emulating with a single stat struct, which is sufficient for
    // file < 4GB. On some systems, st_dev may size 4 whereas target
    // st_dev is 8 bytes; these 4 bytes will be zero extended to 8
    // bytes...
    struct stat64 host_stat;
    ret = ::fstat64( fd, &host_stat );
    if (ret < 0) { this->scerror( uint32_t( -(errno) ), _target ); return; }

    uint8_t                     scratch[0x60];
    ::memset( (void*)&scratch[0], 0, sizeof (scratch) );
    
    write_le( &scratch[0x00], uint64_t(), host_stat.st_dev );
    write_le( &scratch[0x0c], uint32_t(), host_stat.__st_ino );
    write_le( &scratch[0x10], uint32_t(), host_stat.st_mode );
    write_le( &scratch[0x14], uint32_t(), host_stat.st_nlink );
    write_le( &scratch[0x18], uint32_t(), host_stat.st_uid );
    write_le( &scratch[0x1c], uint32_t(), host_stat.st_gid );
    write_le( &scratch[0x20], uint64_t(), host_stat.st_rdev );
    write_le( &scratch[0x2c], uint64_t(), host_stat.st_size );
    write_le( &scratch[0x34], uint32_t(), host_stat.st_blksize );
    write_le( &scratch[0x38], uint64_t(), host_stat.st_blocks );
    write_le( &scratch[0x40], uint32_t(), host_stat.st_atim.tv_sec );
    write_le( &scratch[0x44], uint32_t(), host_stat.st_atim.tv_nsec );
    write_le( &scratch[0x48], uint32_t(), host_stat.st_mtim.tv_sec );
    write_le( &scratch[0x4c], uint32_t(), host_stat.st_mtim.tv_nsec );
    write_le( &scratch[0x50], uint32_t(), host_stat.st_ctim.tv_sec );
    write_le( &scratch[0x54], uint32_t(), host_stat.st_ctim.tv_nsec );
    write_le( &scratch[0x58], uint64_t(), host_stat.st_ino );
    
    _target.write( bufaddr, &scratch[0], sizeof (scratch) );
    
    this->screturn( ret, _target );
  }

  void
  LinuxSystem::sigaction( dtlib::Target& _target )
  {
    int signum = this->scparam( 0, _target );
    uint32_t 
      newaction = this->scparam( 1, _target ),
      oldaction = this->scparam( 2, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(),
                       "%#010x: [syscall] sigaction( signum=%d, newaction=%#x, oldaction=%#x )\n",
                       _target.geteip(), signum, newaction, oldaction );
    
    this->screturn( 0, _target );
  }
  
  void
  LinuxSystem::sigprocmask( dtlib::Target& _target )
  {
    int      how = this->scparam( 0, _target );
    uint32_t newset = this->scparam( 1, _target );
    uint32_t oldset = this->scparam( 2, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(),
                       "%#010x: [syscall] sigprocmask( how=%d, newset=%#x, oldset=%#x )\n",
                       _target.geteip(), how, newset, oldset );
    
    this->screturn( 0, _target );
  }
  
  void
  LinuxSystem::readlink( dtlib::Target& _target )
  {
    uint32_t path_addr = this->scparam( 0, _target );
    uint32_t path_len = this->stringlength( path_addr, _target ) + 1;
    uint32_t buf_addr  = this->scparam( 1, _target );
    uint32_t buf_size  = this->scparam( 2, _target );
    
    char path[path_len];
    _target.read( (uint8_t*)path, path_addr, path_len );
    
    char buffer[buf_size];
    
    int ret = ::readlink( &path[0], &buffer[0], buf_size );
    
    if (ret >= 0) {
      _target.write( buf_addr, (uint8_t*)&buffer[0], std::min( uint32_t( ret ), buf_size ) );
      this->screturn( ret, _target );
    } else {
      this->scerror( uint32_t( -(errno) ), _target );
    }
  }

  void
  LinuxSystem::mmap( dtlib::Target& _target )
  {
    uint32_t start = this->scparam( 0, _target );
    uint32_t length = this->scparam( 1, _target );
    uint32_t prot = this->scparam( 2, _target );
    uint32_t flags = this->scparam( 3, _target );
    int      fd = this->scparam( 4, _target );
    uint32_t offset = this->scparam( 5, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(),
                       "%#010x: [syscall] mmap( start=%#x, length=%#x, "
                       "prot=%#x, flags=%#x, fd=%d, offset=%#x )\n",
                       _target.geteip(), start, length, prot, flags, fd, offset );
    
    this->scerror( uint32_t( -(EINVAL) ), _target );
  }
  
  void
  LinuxSystem::getcwd( dtlib::Target& _target )
  {
    uint32_t buf_addr = this->scparam( 0, _target );
    uint32_t buf_size = this->scparam( 1, _target );
    
    char buffer[buf_size];
    
    char* ret = ::getcwd( &buffer[0], buf_size );
    if (ret) {
      uint32_t len = strlen( ret ) + 1;
      _target.write( buf_addr, (uint8_t*)&buffer[0], len );
      this->screturn( len, _target );
    } else {
      this->scerror( uint32_t( -(errno) ), _target );
    }
  }

  void
  LinuxSystem::ugetrlimit( dtlib::Target& _target )
  {
    int resource = this->scparam( 0, _target );
    uint32_t rlim_addr = this->scparam( 1, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] ugetrlimit( resource=%d, rlim=0x%#x )\n", _target.geteip(), resource, rlim_addr );
    
    struct rlimit rlim;
    int ret = ::getrlimit( resource, &rlim );
    
    if (ret <  0) {
      this->scerror( uint32_t( -(errno) ), _target );
      return;
    }
    
    _target.write( rlim_addr, (uint8_t const*)( &rlim ), sizeof (rlim) );
    this->screturn( ret, _target );
  }
  
  void
  LinuxSystem::exit_group( dtlib::Target& _target )
  {
    m_exit_status = this->scparam( 0, _target );
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] exit_group( status=%d )\n", _target.geteip(), m_exit_status );
    _target.stop();
  }
  
  void
  LinuxSystem::readv( dtlib::Target& _target ) {
    int fd = int( this->scparam( 0, _target ) );
    uint32_t iovecaddr = this->scparam( 1, _target );
    int count = int( this->scparam( 2, _target ) );
    int sum = 0;
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] readv( fd=%d, vector=%#x, count=%d )\n", _target.geteip(), fd, iovecaddr, count );

    for (int step = count; (--step) >= 0; iovecaddr += 8) {
      uint32_t iov_base = _target.read32( iovecaddr + 0 );
      size_t   iov_len  = _target.read32( iovecaddr + 4 );
      assert( iov_len < 0x100000 );
      uint8_t buffer[iov_len];
      int ret = ::read( fd, buffer, iov_len );
      if (ret < 0) { this->scerror( uint32_t( -(errno) ), _target ); return; }
      if (ret > 0) _target.write( iov_base, buffer, iov_len );
      sum += ret;
    }
    
    this->screturn( sum, _target );
  }
  
  void LinuxSystem::writev( dtlib::Target& _target ) {
    int fd = int( this->scparam( 0, _target ) );
    uint32_t iovecaddr = this->scparam( 1, _target );
    int count = size_t( this->scparam( 2, _target ) );
    int sum = 0;
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] writev( fd=%d, vector=%#x, count=%d )\n", _target.geteip(), fd, iovecaddr, count );
    
    for (int step = count; (--step) >= 0; iovecaddr += 8) {
      uint32_t iov_base = _target.read32( iovecaddr + 0 );
      size_t   iov_len  = _target.read32( iovecaddr + 4 );
      assert( iov_len < 0x100000 );
      uint8_t buffer[iov_len];
      _target.read( buffer, iov_base, iov_len );
      int ret = ::write( fd, buffer, iov_len );
      if (ret < 0) { this->scerror( uint32_t( -(errno) ), _target ); return; }
      sum += ret;
    }

    this->screturn( sum, _target );
  }
  
  void
  LinuxSystem::set_thread_storage( dtlib::Target& _target )
  {
    uint32_t user_desc_ptr = this->scparam( 0, _target );
    int32_t  entry_number  = _target.read32( user_desc_ptr + 0x0 );
    uint32_t base_addr     = _target.read32( user_desc_ptr + 0x4 );
    uint32_t limit         = _target.read32( user_desc_ptr + 0x8 );
    uint32_t attributes    = _target.read32( user_desc_ptr + 0xc );
    
    if (m_verbose) {
      dtlib::osprintf( m_sink(),
                       "%#010x: [syscall] set_thread_storage( user_desc_ptr=%#x ) "
                       "{entry_number=%d, base_addr=%#x, limit=%#x, attributes=%#x}\n",
                       _target.geteip(), user_desc_ptr, entry_number, base_addr, limit, attributes );
    }
    
    assert( entry_number == -1 );
    GDTEntry_t& tlsgdt = gdtentry( 6 );
    tlsgdt.m_baseaddr = base_addr;
    _target.write32( user_desc_ptr + 0x0, 6 );
      
    this->screturn( 0, _target );
  }

  LinuxSystem::GDTEntry_t&
  LinuxSystem::gdtentry( uint32_t _number )
  {
    for (GDTEntry_t *node = m_gdtentries; node; node = node->m_next)
      if (node->m_number == _number) return *node;
    m_gdtentries = new GDTEntry_t( _number, 0, m_gdtentries );
    return *m_gdtentries;
  }
  
  void
  LinuxSystem::remove_gdtentry( uint32_t _number )
  {
    for (GDTEntry_t **node = &m_gdtentries; *node; node = &(**node).m_next) {
      if ((**node).m_number != _number) continue;
      GDTEntry_t* entry = *node;
      *node = entry->m_next;
      entry->m_next = 0;
      delete entry;
    }
  }

  void
  LinuxSystem::set_tid_address( dtlib::Target& _target )
  {
    uint32_t tidptr = this->scparam( 0, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] set_tid_address( tidptr=0x%08x )\n", _target.geteip(), tidptr );

    this->m_clear_child_tid = tidptr;
    
    this->screturn( ::getpid(), _target );
  }
  
  void
  LinuxSystem::set_robust_list( dtlib::Target& _target )
  {
    uint32_t head = this->scparam( 0, _target );
    uint32_t len = this->scparam( 1, _target );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] set_robust_list( head=0x%08x, len=%u )\n", _target.geteip(), head, len );
    
    this->m_futex_listhead = head;
    this->m_futex_itemsize = len;
    
    this->screturn( 0, _target );
  }
  
  void
  LinuxSystem::tgkill( dtlib::Target& _target )
  {
    int tid = int( this->scparam( 0, _target ) );
    int sig = int( this->scparam( 0, _target ) );
    
    if (m_verbose)
      dtlib::osprintf( m_sink(), "%#010x: [syscall] tgkill( tid=%d, sig=%d )\n", _target.geteip(), tid, sig );
    
    _target.stop();
    
    this->screturn( 0, _target );
  }

  void
  LinuxSystem::futex( dtlib::Target& _target )
  {
    uint32_t uaddr = this->scparam( 0, _target );
    int      op = this->scparam( 1, _target );
    int      val = this->scparam( 2, _target );
    uint32_t timeout = this->scparam( 3, _target );
    uint32_t uaddr2 = this->scparam( 4, _target );
    int      val3 = this->scparam( 5, _target );
    
    if (m_verbose) {
      dtlib::osprintf( m_sink(),
                       "%#010x: [syscall] futex( uaddr=0x%08x, op=%d, val=%d, "
                       "timeout=0x%08x, uaddr2=0x%08x, val3=%d ) {",
                       _target.geteip(), uaddr, op, val,
                       timeout, uaddr2, val3 );
    }
    
    int opflags = op & -128;
    op &= (128-1);
    
    if (m_verbose) {
      m_sink() << "opflags=";
      char const* sep = "";
      if (opflags & 128) { m_sink() << "PRIVATE_FLAG"; sep = " | "; }
      if (opflags & 256) m_sink() << sep << "CLOCK_REALTIME";
      switch (op) {
      case 0: m_sink() << ", op=WAIT"; break;
      case 1: m_sink() << ", op=WAKE"; break;
      case 2: m_sink() << ", op=FD"; break;
      case 3: m_sink() << ", op=REQUEUE"; break;
      case 4: m_sink() << ", op=CMP_REQUEUE"; break;
      case 5: m_sink() << ", WAKE_OP"; break;
      case 6: m_sink() << ", op=LOCK_PI"; break;
      case 7: m_sink() << ", op=UNLOCK_PI"; break;
      case 8: m_sink() << ", op=TRYLOCK_PI"; break;
      case 9: m_sink() << ", op=WAIT_BITSET"; break;
      case 10: m_sink() << ", op=WAKE_BITSET"; break;
      case 11: m_sink() << ", op=WAIT_REQUEUE_PI"; break;
      case 12: m_sink() << ", op=CMP_REQUEUE_PI"; break;
      default: m_sink() << ", op=???"; break;
      }
    }
    
    int res;
    switch (op)
      {
      case 0: /* FUTEX_WAIT */
      case 9: /* FUTEX_WAIT_BITSET */
        {
          int mval = int( _target.read32( uaddr ) );
          if (mval != val) {
            this->scerror( uint32_t( -EWOULDBLOCK ), _target );
            return;
          }
          if (m_verbose)
            dtlib::osprintf( m_sink(), ", (*uaddr)=%d", mval );
          res = 0; /* zero cond OK */
        } break;
      
      case 1: /* FUTEX_WAKE */ 
      case 10: /* FUTEX_WAKE_BITSET */
        res = 0; /* zero process waken up */
        break;
      
      case 2: /* FUTEX_FD */
      case 3: /* FUTEX_REQUEUE */
      case 4: /* FUTEX_CMP_REQUEUE */
      case 5: /* FUTEX_WAKE_OP */
      case 6: /* FUTEX_LOCK_PI */
      case 7: /* FUTEX_UNLOCK_PI */
      case 8: /* FUTEX_TRYLOCK_PI */
      case 11: /* FUTEX_WAIT_REQUEUE_PI */
      case 12: /* FUTEX_CMP_REQUEUE_PI */
      default:
        assert (! "not implemented");
        break;
      }
    if (m_verbose)
      m_sink() << " }\n";
    
    this->screturn( res, _target );
  }

} // end of namespace todel

