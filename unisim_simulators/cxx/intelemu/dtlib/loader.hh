#ifndef DTLIB_LOADER_HH
#define DTLIB_LOADER_HH

#include <dtlib/fwd.hh>
#include <string>
#include <vector>
#include <stdint.h>

namespace dtlib {
namespace ELF {
  struct LEBox {
    uint8_t const*         m_base;
    intptr_t               m_size;
    
    enum Exception_t { BoundsError };
    LEBox() : m_base( 0 ), m_size( 0 ) {}
    LEBox( uint8_t const* _base, intptr_t _size ) : m_base( _base ), m_size( _size ) {}
    LEBox( LEBox const& _leb, intptr_t _base, intptr_t _size ) { init(_leb, _base, _size); }
    LEBox( LEBox const& _leb ) : m_base( _leb.m_base ), m_size( _leb.m_size ) {}
    
    LEBox&                 init( uint8_t const* _base, intptr_t _size )
    { m_base = _base; m_size = _size; return *this; }
    LEBox&                 init( LEBox const& _ber, intptr_t _base, intptr_t _size );
    
    template <class Integer_t>
    Integer_t                 read( intptr_t _addr, Integer_t _init ) const {
      if( _addr < 0 or intptr_t(_addr + sizeof( Integer_t )) > m_size )
        throw BoundsError;
      Integer_t integer = _init;
      for( intptr_t idx = intptr_t( sizeof( Integer_t ) ); (--idx) >= 0; )
        integer = (integer << 8 | Integer_t( m_base[_addr + idx] ));
      return integer;
    }
    
    void                      read( intptr_t _addr, uint8_t* _buffer, intptr_t _size ) const;
    int                       cmp( intptr_t _addr, uint8_t const* _buffer, intptr_t _size ) const;
    char const*               cstring( intptr_t idx ) const;
    
    void                      setsize( intptr_t _size );
  };
  
  enum Exception_t { ImageError, SectionHeaderError, StringTableError };
 
  enum Version_t { versnone = 0, verscurrent = 1, versnum = 2 };
  
  struct Root : public LEBox
  {
    Root( uint8_t const* _start, intptr_t _upbound );
    Root( Root const& _root ) : LEBox( _root ) {}
    
    uint16_t                  object_file_type() const { return this->read( 16, uint16_t() ); }
    uint32_t                  entrypoint() const { return this->read( 24, uint32_t() ); }
    uint32_t                  phoff() const { return this->read( 28, uint32_t() ); }
    uint32_t                  shoff() const { return this->read( 32, uint32_t() ); }
    uint16_t                  phentsize() const { return this->read( 42, uint16_t() ); }
    uint16_t                  phnum() const { return this->read( 44, uint16_t() ); }
    uint16_t                  shentsize() const { return this->read( 46, uint16_t() ); }
    uint16_t                  shnum() const { return this->read( 48, uint16_t() ); }
    uint16_t                  shstrndx() const { return this->read( 50, uint16_t() ); }
    
    static char const* const  s_elfmag; /* "\177ELF" */
    enum Field_t { imagic = 0, iclass = 4, idata, iversion };
    enum IClass_t { classnone = 0, class32 = 1, class64 = 2, classnum = 3 };
    enum IData_t  { datanone = 0, data2lsb = 1, data2msb = 2 };
    enum Type_t { etnone = 0, etrel = 1, etexec = 2, etdyn = 3, etcore = 4, etnum = 5 };
  };
  
  struct SectionHeader : public LEBox {
    enum SType_t {
      null = 0, progbits = 1, symtab = 2, strtab = 3, rela = 4, hashtable = 5,
      dynamic = 6, note = 7, nobits = 8, rel = 9, shlib = 10, dynsym = 11,
    };
    enum Flag_t { write = 1, alloc = 2, exec = 4 };
    SectionHeader()  {}
    SectionHeader( Root const& _node, intptr_t _idx );
    SectionHeader&      init( Root const& _img, intptr_t _idx );
    
    uint32_t            name() const { return this->read( 0, uint32_t() ); }       /* section name */
    uint32_t            type() const { return this->read( 4, uint32_t() ); }       /* SHT_... */
    uint32_t            flags() const { return this->read( 8, uint32_t() ); }      /* SHF_... */
    uint32_t            addr() const { return this->read( 12, uint32_t() ); }      /* virtual addr */
    uint32_t            offset() const { return this->read( 16, uint32_t() ); }    /* file offset */
    uint32_t            size() const { return this->read( 20, uint32_t() ); }      /* section size */
    uint32_t            link() const { return this->read( 24, uint32_t() ); }      /* misc info */
    uint32_t            info() const { return this->read( 28, uint32_t() ); }      /* misc info */
    uint32_t            addralign() const { return this->read( 32, uint32_t() ); } /* memory align */
    uint32_t            entsize() const { return this->read( 36, uint32_t() ); }   /* size if table */
    
  };
  
  struct StringTable : public LEBox
  {
    StringTable() {};
    StringTable( Root const& _img ) { init( _img ); }
    StringTable( Root const& _img, SectionHeader const& _shdr ) { init( _img, _shdr ); }
    
    StringTable&              init( Root const& _img );
    StringTable&              init( Root const& _img, SectionHeader const& _shdr );
  };
  
  struct ProgramHeader : public LEBox
  {
    enum PType_t { null = 0, load = 1, dynamic = 2, interp = 3,
                   note = 4, shlib = 5, phdr = 6, tls = 7, num = 8 };
    
    ProgramHeader( Root const& _node, intptr_t _idx );
    
    uint32_t                  type() const { return this->read( 0, uint32_t() ); }
    uint32_t                  offset() const { return this->read( 4, uint32_t() ); }
    uint32_t                  vaddr() const { return this->read( 8, uint32_t() ); }
    uint32_t                  paddr() const { return this->read( 12, uint32_t() ); }
    uint32_t                  filesz() const { return this->read( 16, uint32_t() ); }
    uint32_t                  memsz() const { return this->read( 20, uint32_t() ); }
    uint32_t                  flags() const { return this->read( 24, uint32_t() ); }
    uint32_t                  align() const { return this->read( 28, uint32_t() ); }
  };
  
  struct NoteReader: public LEBox
  {
    NoteReader( Root const& _node, ProgramHeader const& phdr );
    enum NType_t {
      prstatus=1, /* prstatus struct */ fpregset=2, /* fpregset struct */    prpsinfo=3, /* prpsinfo struct */
      prxreg=4, /* prxregset struct */  taskstruct=4, /* task structure */   platform=5, /* String from sysinfo(SI_PLATFORM) */
      auxv=6, /* auxv array */          gwindows=7, /* gwindows struct */    asrs=8, /* asrset struct */
      pstatus=10, /* pstatus struct */  psinfo=13, /* psinfo struct */       prcred=14, /* prcred struct */
      utsname=15, /* utsname struct */  lwpstatus=16, /* lwpstatus struct */ lwpsinfo=17, /* lwpinfo struct */
      prfpxreg=20, /* fprxregset struct */ siginfo=0x53494749, /* siginfo_t, size might increase */
      file=0x46494c45, /* mapped files */
      _386_tls=0x200, /* i386 TLS slots (struct user_desc) */
      _386_ioperm=0x201, /* x86 io permission bitmap (1=deny) */
      x86_xstate=0x202 /* x86 extended state using xsave */
    };
    
    uint32_t                    namesz() const { return this->read( 0, uint32_t() ); } /* Length of the note's name. */
    uint32_t                    descsz() const { return this->read( 4, uint32_t() ); } /* Length of the note's descriptor. */
    uint32_t                    type() const { return this->read( 8, uint32_t() ); } /* Type of the note. */
    uint32_t                    allsz() const { return 12 + ((this->namesz()+3)&-4) + ((this->descsz()+3)&-4); }
    bool                        good() const { return (m_size >= 12) and (m_size >= this->allsz()); }
    void                        next() { uint32_t offset = std::min( this->allsz(), (uint32_t)m_size ); m_base += offset; m_size -= offset; }
  };

  
  struct Symbol : public LEBox {
    enum Type_t { notype = 0, object = 1, function = 2, section = 3, file = 4, loproc = 14, hiproc = 15 };
    Symbol( LEBox const& _table, intptr_t idx );
    
    uint32_t                  name() const { return this->read( 0, uint32_t() ); }
    uint32_t                  value() const { return this->read( 4, uint32_t() ); }
    uint32_t                  size() const { return this->read( 8, uint32_t() ); }
    uint8_t                   info() const { return this->read( 12, uint8_t() ); }
    uint8_t                   other() const { return this->read( 13, uint8_t() ); }
    uint16_t                  shndx() const { return this->read( 14, uint16_t() ); }
    
    uint8_t                   type() const { return this->read( 12, uint8_t() ) & 0xf; }
    
    static intptr_t const     s_size = 16;
    static uint32_t           entrycount( LEBox const& _table );
  };
  
  struct Error : public std::exception {
    std::string m_what;

    Error( std::string _what )
      : m_what( _what )
    {}
    ~Error() throw() {}

    virtual const char* what() const throw() { return m_what.c_str(); };
  };

  struct Image {
    bool                      m_verbose;
    std::vector<uint8_t>      m_bytearray;

    Image( char const* imgpath );

    uint32_t                  load( Target& elftarget );
    void                      ia32chk();
    uint32_t                  entrypoint();
    uint32_t                  processor_specific_flags();
    uint32_t                  processor_type();
    Root                      getroot();
  };

  struct SymbolTable {
    SymbolTable( Image const& img );
    uintptr_t                 size() const;

    StringTable               m_symstrtable;
    LEBox                     m_symtable;
  };
};
};

#endif // DTLIB_LOADER_HH
