#include <dtlib/loader.hh>
#include <dtlib/misc.hh>
#include <dtlib/target.hh>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cassert>

namespace dtlib {
namespace ELF {
  LEBox&
  LEBox::init( LEBox const& _ber, intptr_t _base, intptr_t _size ) {
    if( _base < 0 or (_base + _size) > _ber.m_size )
      throw BoundsError;
    m_base = _ber.m_base + _base;
    m_size = _size;
    return *this;
  }

  void
  LEBox::read( intptr_t _addr, uint8_t* _buffer, intptr_t _size ) const {
    if( _addr < 0 or (_addr + _size) > m_size )
      throw BoundsError;
    std::memcpy( (void*)_buffer, (void*)(m_base + _addr), _size );
  }

  int
  LEBox::cmp( intptr_t _addr, uint8_t const* _buffer, intptr_t _size ) const {
    if( _addr < 0 or (_addr + _size) > m_size )
      throw BoundsError;
    for( intptr_t idx = 0; idx < _size; idx++ ) {
      uint8_t a = m_base[_addr+idx], b = _buffer[idx];
      if( a == b ) continue;
      return (a > b ? 1 : -1);
    }
    return 0;
  }
  
  char const*
  LEBox::cstring( intptr_t _addr ) const {
    if( _addr < 0 ) throw BoundsError;
    intptr_t upbound = _addr;
    do { if( upbound >= m_size ) throw BoundsError; } while( m_base[upbound++] != 0 );
    return (char const*)(m_base + _addr);
  }
  
  char const* const Root::s_elfmag = "\177ELF";
  
  Root::Root( uint8_t const* _start, intptr_t _size )
    : LEBox( _start, _size )
  {
    /* Constructor */
    if (this->cmp( imagic, (uint8_t const*)s_elfmag, 4 )) throw ImageError;
    if (this->read( Root::iclass, uint8_t() ) != class32) throw ImageError; // Unupported 64 bit
    if (this->read( 5, uint8_t() ) != 1) throw ImageError; // not little endian
    if (this->read( Root::iversion, uint8_t() ) != verscurrent) throw ImageError; // ELF version
    if (this->read( 20, uint32_t() ) != verscurrent) throw ImageError; // Version
    if (this->read( 40, uint16_t() ) < 52) throw ImageError; // header size
  }
  
  SectionHeader::SectionHeader( Root const& _root, intptr_t _idx )
    : LEBox( _root, _root.shoff() + _idx*_root.shentsize(), _root.shentsize() )
  {
    if (m_size != 40)
      throw BoundsError;
  }
  
  SectionHeader&
  SectionHeader::init( Root const& _root, intptr_t _idx ) {
    LEBox::init( _root, _root.shoff() + _idx*_root.shentsize(), _root.shentsize() );
    if( m_size != 40 )
      throw BoundsError;
    return *this;
  }
  
  StringTable&
  StringTable::init( Root const& _root ) {
    return this->init( _root, SectionHeader( _root, _root.shstrndx() ) );
  }
  
  StringTable&
  StringTable::init( Root const& _root, SectionHeader const& _shdr ) {
    if( _shdr.type() != 3 ) throw StringTableError;
    LEBox::init( _root, _shdr.offset(), _shdr.size() );
    return *this;
  }
  
  ProgramHeader::ProgramHeader( Root const& _root, intptr_t _idx )
    : LEBox( _root, _root.phoff() + _idx*_root.phentsize(), _root.phentsize() )
  {
    if( m_size != 32 or intptr_t(_root.phoff() + (_idx + 1)*_root.phentsize()) > _root.m_size )
      throw BoundsError;
  }
  
  Symbol::Symbol( LEBox const& _table, intptr_t idx )
    : LEBox( _table, idx*s_size, s_size )
  {}
    
  uint32_t Symbol::entrycount( LEBox const& _table ) { return _table.m_size / s_size; }
  
  NoteReader::NoteReader( Root const& _root, ProgramHeader const& phdr )
    : LEBox( _root, phdr.offset(), phdr.filesz() )
  {}
  
  Image::Image( char const* imgpath )
    : m_verbose( false )
  {
    std::ifstream imgsrc( imgpath );

    if (not imgsrc.good()) {
      throw Error( std::string( "Can't open ELF file: " ) + imgpath );
    }

    intptr_t size = imgsrc.seekg( 0, std::ios::end ).tellg();
    m_bytearray.resize( size );
    imgsrc.seekg( 0, std::ios::beg ).read( (char*)(&(m_bytearray.front())), size );
  }

  uint32_t
  Image::entrypoint() {
    Root root( (uint8_t*)(&(m_bytearray.front())), m_bytearray.size() );
    
    return root.entrypoint();
  }
  
  uint32_t
  Image::processor_specific_flags() {
    Root root( (uint8_t*)(&(m_bytearray.front())), m_bytearray.size() );

    return root.read( 36, uint32_t() );
  }
  
  uint32_t
  Image::processor_type() {
    Root root( (uint8_t*)(&(m_bytearray.front())), m_bytearray.size() );

    return root.read( 18, uint16_t() );
  }
  
  Root
  Image::getroot() {
    return Root( (uint8_t*)(&(m_bytearray.front())), m_bytearray.size() );
  }
  
  uint32_t
  Image::load( Target& elftarget )
  {
    Root root( (uint8_t*)(&(m_bytearray.front())), m_bytearray.size() );
    
    uint32_t phnum = root.phnum();
    
    if (m_verbose) std::cerr << "Program Headers:\n";
    uint32_t loadedbytes = 0;
    for (uint32_t idx = 0; idx < phnum; ++idx) {
      ProgramHeader programhdr( root, idx );
      if (m_verbose) {
        dtlib::osprintf( std::cerr, "* %2d/%-2d {offset: %#x", idx+1, phnum, programhdr.offset()  );
        
        uint32_t vaddr = programhdr.vaddr(), paddr = programhdr.paddr();
        if( vaddr != paddr ) {
          dtlib::osprintf( std::cerr, ", vaddr: %#x", vaddr );
          dtlib::osprintf( std::cerr, ", paddr: %#x", paddr );
        } else 
          dtlib::osprintf( std::cerr, ", addr: %#x", vaddr );
        
        uint32_t filesz = programhdr.filesz(), memsz = programhdr.memsz();
        if( filesz != memsz ) {
          dtlib::osprintf( std::cerr, ", filesz: %#x", filesz );
          dtlib::osprintf( std::cerr, ", memsz: %#x", memsz );
        } else 
          dtlib::osprintf( std::cerr, ", size: %#x", filesz );
        
        dtlib::osprintf( std::cerr, ", flags: %#x", programhdr.flags() );
        dtlib::osprintf( std::cerr, ", align: %#x }", programhdr.align() );
      }
      
      switch (programhdr.type()) {
      case ProgramHeader::null: std::cerr << " null\n"; break;
      case ProgramHeader::load: {
        if (m_verbose) std::cerr << " load\n";
        uint32_t filesz = programhdr.filesz();
        uint32_t memsz = programhdr.memsz();
        LEBox prog( root, programhdr.offset(), filesz );
        elftarget.write( programhdr.vaddr(), prog.m_base, filesz );
        // zeroed data ?
        if( filesz < memsz ) {
          elftarget.clear( (programhdr.vaddr() + filesz), (memsz - filesz) );
        }
        
        loadedbytes += filesz;
      } break;
      case ProgramHeader::dynamic: if (m_verbose) std::cerr << " dynamic linking information \n"; elftarget.dynamic_image(); break;
      case ProgramHeader::interp: if (m_verbose) std::cerr << " program interpreter\n"; elftarget.dynamic_image(); break;
      case ProgramHeader::note: {
        if (m_verbose) std::cerr << " note\n";
        if (root.object_file_type() != Root::etcore)
          break; /* Only core notes are relevant */
        for (NoteReader note( root, programhdr ); note.good(); note.next()) {
          if (m_verbose)
            dtlib::osprintf( std::cerr, "  * {namesz: %#x, descsz: %#x, type: %#x, name: %s}",
                             note.namesz(), note.descsz(), note.type(), note.cstring( 12 ) );
          switch (note.type()) {
          case NoteReader::prstatus: {
            std::cerr << " prstatus\n";
            assert( note.descsz() >= 144 );
            uint32_t base = (15 + note.namesz()) & -4; /* aligned beginning of prstatus structure */
            uint32_t eip = note.read( base + 0x78, uint32_t() );
            //uint32_t eflags = note.read( base + 0x80, uint32_t() );
            uint32_t esp = note.read( base + 0x84, uint32_t() );
            dtlib::osprintf( std::cerr, "    * eip:    %#010x\n", eip );
            elftarget.seteip( eip );
            //dtlib::osprintf( std::cerr, "    * eflags: %#010x\n", eflags );
            dtlib::osprintf( std::cerr, "    * esp:    %#010x\n", esp );
            elftarget.setesp( esp );
          } break;
          case NoteReader::prpsinfo: std::cerr << " prpsinfo\n"; break;
          case NoteReader::auxv: std::cerr << " auxv array\n"; break;
          case NoteReader::x86_xstate: std::cerr << " x86_xstate\n"; break;
          case NoteReader::siginfo: std::cerr << " siginfos\n"; break;
          case NoteReader::file: std::cerr << " mapped files\n"; break;
          default:
            dtlib::osprintf( std::cerr, " %#x\n", note.type() );
            break;
          }
        }
      } break;
      case ProgramHeader::phdr: if (m_verbose) std::cerr << " program header\n"; break;
      case ProgramHeader::tls: if (m_verbose) std::cerr << " thread-local storage segment\n"; break;
      case 0x6474e550: if (m_verbose) std::cerr << " gnu_eh_frame\n"; break; // TODO: we should process that
      case 0x6474e551: if (m_verbose) std::cerr << " gnu_stack\n"; break; // TODO: we should process that
      case 0x6474e552: if (m_verbose) std::cerr << " gnu_relro\n"; break; // TODO: we should process that
      default:
        dtlib::osprintf( std::cerr, " %#010x\n", programhdr.type() );
        throw Error( "Unknown p_type" );
      }
    }
    
    if (m_verbose) {
      std::cerr << "Machine ID: " << root.read( 18, uint16_t() ) << ".\n";
      dtlib::osprintf( std::cerr, "Loaded %d bytes into memory.\n", loadedbytes );
    }
    
    return loadedbytes;
  }
  
  void
  Image::ia32chk()
  {
    if (processor_specific_flags() != 0) {
      std::cerr << "Processor specific flags not null (as expected) ???\n";
      throw 0;
    }
    
    {
      uint32_t proctype = processor_type();
      if (proctype != 3) { // machine (3 = EM_386, see <elf.h>)
        dtlib::osprintf( std::cerr, "Wrong processor type (%u).\n", proctype );
        throw 0;
      }
    }
  }
    
  SymbolTable::SymbolTable( Image const& img ) {
    Root root( (uint8_t*)&(img.m_bytearray.front()), img.m_bytearray.size() );
    
    if (root.shstrndx() == 0) throw Error( "invalid shstrndx" );
      
    StringTable shstrtable( root );
    
    SectionHeader symshdr;
    uint32_t shnum = root.shnum();
    bool found = false;
    for (uint32_t idx = 0; idx < shnum; ++idx) {
      symshdr.init( root, idx );
      if (symshdr.type() != SectionHeader::symtab or (symshdr.flags() & SectionHeader::alloc ))
        continue;
      found = true;
      //tprintf( Trace::chan("sys"), "Symbol Table \"%s\"\n", shstrtable.cstring( symshdr.name() ) );
      break;
    }
      
    if (not found) throw Error( "cannot find symbol table" );
      
    m_symstrtable.init( root, SectionHeader( root, symshdr.link() ) );
    m_symtable.init( root, symshdr.offset(), symshdr.size() );
      
    uint32_t symcount = symshdr.size() / symshdr.entsize();
    assert( symcount == Symbol::entrycount( m_symtable ) );
    
  }
  
  uintptr_t
  SymbolTable::size() const {
    return Symbol::entrycount( m_symtable );
  }
  
};
};
