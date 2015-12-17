#ifndef INTEL_DISASM_HH
#define INTEL_DISASM_HH

#include <unisim/component/cxx/processor/intel/segments.hh>
#include <unisim/component/cxx/processor/intel/fwd.hh>
#include <unisim/component/cxx/processor/intel/tmp.hh>
#include <stdint.h>
#include <iosfwd>

namespace intel
{
  struct DisasmObject
  {
    virtual void operator() ( std::ostream& sink ) const = 0;
    virtual ~DisasmObject() {};
  };
  
  std::ostream& operator << ( std::ostream& sink, DisasmObject const& dobj );
  
  /* ModRM disassembly */
  struct DisasmRoM : public DisasmObject
  {
    DisasmRoM( MOp const* _mop ) : mop( _mop ) {} MOp const* mop;
    void operator() ( std::ostream& sink ) const;
    virtual void disasm_register( std::ostream& sink, unsigned reg ) const = 0;
  };
  struct DisasmEb : public DisasmRoM
  {
    DisasmEb( MOp const* _mop ) : DisasmRoM( _mop ) {}
    void disasm_register( std::ostream& sink, unsigned reg ) const;
  };
  struct DisasmEw : public DisasmRoM
  {
    DisasmEw( MOp const* _mop ) : DisasmRoM( _mop ) {}
    void disasm_register( std::ostream& sink, unsigned reg ) const;
  };
  struct DisasmEd : public DisasmRoM
  {
    DisasmEd( MOp const* _mop ) : DisasmRoM( _mop ) {}
    void disasm_register( std::ostream& sink, unsigned reg ) const;
  };
  template <unsigned OPSIZE>
  struct DisasmE : public DisasmObject
  {
    DisasmE( MOp const* _mop ) : mop( _mop ) {} MOp const* mop;
    void operator() ( std::ostream& sink ) const {
      if      (OPSIZE == 8)  sink << DisasmEb( mop );
      else if (OPSIZE == 16) sink << DisasmEw( mop );
      else if (OPSIZE == 32) sink << DisasmEd( mop );
      else throw 0;
    }
  };
  struct DisasmQq : public DisasmRoM
  {
    DisasmQq( MOp const* _mop ) : DisasmRoM( _mop ) {}
    void disasm_register( std::ostream& sink, unsigned reg ) const;
  };
  struct DisasmWdq : public DisasmRoM
  {
    DisasmWdq( MOp const* _mop ) : DisasmRoM( _mop ) {}
    void disasm_register( std::ostream& sink, unsigned reg ) const;
  };
  struct DisasmM : public DisasmRoM
  {
    DisasmM( MOp const* _mop ) : DisasmRoM( _mop ) {}
    void disasm_register( std::ostream& sink, unsigned reg ) const;
  };
  
  /* General purpose registers */
  struct DisasmRb : public DisasmObject
  {
    DisasmRb( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  struct DisasmRw : public DisasmObject
  {
    DisasmRw( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  struct DisasmRd : public DisasmObject
  {
    DisasmRd( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };

  template <unsigned OPSIZE>
  struct DisasmR : public DisasmObject
  {
    DisasmR( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const {
      if      (OPSIZE == 8)  sink << DisasmRb( reg );
      else if (OPSIZE == 16) sink << DisasmRw( reg );
      else if (OPSIZE == 32) sink << DisasmRd( reg );
      else throw 0;
    }
  };
  
  /* Quadword registers*/
  struct DisasmRq : public DisasmObject
  {
    DisasmRq( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  
  /* Double quadword registers */
  struct DisasmRdq : public DisasmObject
  {
    DisasmRdq( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  
  /* Debug registers */
  struct DisasmDd : public DisasmObject
  {
    DisasmDd( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  /* Control registers */
  struct DisasmCd : public DisasmObject
  {
    DisasmCd( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };

  /* Segments */
  struct DisasmS : public DisasmObject
  {
    DisasmS( unsigned _seg ) : segment( _seg ) {} unsigned segment;
    void operator () ( std::ostream& sink ) const;
  };
  struct DisasmMS : public DisasmObject
  {
    DisasmMS( unsigned _seg ) : segment( _seg ) {} unsigned segment;
    void operator () ( std::ostream& sink ) const;
  };
  
  void PutString( std::ostream& sink, char const* string );
  void PutChar( std::ostream& sink, char chr );
  
  /* Immediate disassembly */
  template <typename T>
  struct DisasmHex : public DisasmObject
  {
    DisasmHex( T _imm ) : imm( _imm ) {} T imm;
    
    void operator () ( std::ostream& sink ) const
    {
      uintptr_t const bcnt = sizeof(T);
      char buf[4+2*bcnt];
      char* ptr = &buf[sizeof(buf)];
      *--ptr = '\0';
      T val = imm;
      for (uintptr_t idx = 0; idx < 2*sizeof (T); idx++) {
        *--ptr = "0123456789abcdef"[val&0xf];
        val >>= 4;
        if (not val) break;
      }
      *--ptr = 'x';
      *--ptr = '0';
      *--ptr = '$';
      PutString( sink, ptr );
    }
  };
  template <typename T> DisasmHex<T> DisasmI( T imm ) { return DisasmHex<T>( imm ); }
  
  struct DisasmCond : public DisasmObject
  {
    DisasmCond( unsigned _cond ) : cond( _cond ) {} unsigned cond;
    
    void operator() ( std::ostream& _sink ) const
    {
      static char const* conds[] = {"o", "no", "b", "ae", "e", "ne", "be", "a", "s", "ns", "p", "np", "l", "ge", "le", "g"};
      PutString( _sink, (cond & -16) ? "?" : conds[cond] );
    }
    
  };

  bool has_implicit_size( MOp const* _mop );
  
  template <unsigned OPSIZE>
  struct DisasmMnemonic : public DisasmObject
  {
    DisasmMnemonic( char const* _mnemonic, MOp const* _mop ) : mnemonic( _mnemonic ), implicit_size( has_implicit_size( _mop ) ) {}
    DisasmMnemonic( char const* _mnemonic ) : mnemonic( _mnemonic ), implicit_size( false ) {}
    char const* mnemonic;
    bool implicit_size;
    
    void operator() ( std::ostream& _sink ) const
    {
      PutString( _sink, mnemonic );
      if (implicit_size) PutChar( _sink, ("bwlq"[SB<OPSIZE/8>::begin]) );
      PutChar( _sink, ' ' );
    }
  };
  
  struct DisasmFPR : public DisasmObject
  {
    DisasmFPR( uint8_t _reg ) : reg( _reg ) {} uint8_t reg;
    
    void operator() ( std::ostream& _sink ) const;
  };
  
  struct DisasmBytes : public DisasmObject
  {
    DisasmBytes( uint8_t const* _bytes, unsigned _length ) : bytes( _bytes ), length( _length ) {} uint8_t const* bytes; unsigned length;
    
    void operator() ( std::ostream& _sink ) const;
  };
};

#endif // INTEL_DISASM_HH
