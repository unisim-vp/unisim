#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXTENSION_REGISTER_BANK_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXTENSION_REGISTER_BANK_HH__

#include <cstring>
#include <inttypes.h>

namespace intel
{
  
  template <typename T>
  struct VectorTypeInfo
  {
    enum { bytecount = sizeof (T) };
    static void ToBytes( uint8_t* dst, T src )
    {
      for (unsigned idx = 0; idx < sizeof (T); ++idx)
        { dst[idx] = src & 0xff; src >>= 8; }
    }
    static void FromBytes( T& dst, uint8_t const* src )
    {
      T tmp = 0;
      for (unsigned idx = sizeof (T); idx-- > 0;)
        { tmp <<= 8; tmp |= uint32_t( src[idx] ); }
      dst = tmp;
    }
  };

  template <> struct VectorTypeInfo<float>
  {
    enum bytecount_t { bytecount = 4 };
    static void ToBytes( uint8_t* dst, float const& src ) { VectorTypeInfo<uint32_t>::ToBytes( dst, reinterpret_cast<uint32_t const&>( src ) ); }
    static void FromBytes( float& dst, uint8_t const* src ) { VectorTypeInfo<uint32_t>::FromBytes( reinterpret_cast<uint32_t&>( dst ), src ); }
  };

  template <> struct VectorTypeInfo<double>
  {
    enum bytecount_t { bytecount = 8 };
    static void ToBytes( uint8_t* dst, double const& src ) { VectorTypeInfo<uint64_t>::ToBytes( dst, reinterpret_cast<uint64_t const&>( src ) ); }
    static void FromBytes( double& dst, uint8_t const* src ) { VectorTypeInfo<uint64_t>::FromBytes( reinterpret_cast<uint64_t&>( dst ), src ); }
  };
  
  enum ecstate_t { invalid = 0, valid = 1, exclusive = 3 };

  struct Invalidate
  {
    unsigned reg;
  
    Invalidate( unsigned idx ) : reg( idx ) {}
  };

  struct Copy
  {
    unsigned reg, size;
    uint8_t* buffer;
  
    template <unsigned sizeT>
    Copy( unsigned idx, uint8_t (&_buffer) [sizeT] ) : reg( idx ), size( sizeT ), buffer( &_buffer[0] ) {}
  };

  template <typename valT, unsigned _RegCount, unsigned _RegSize>
  struct VectorBankCache
  {
    static uintptr_t const sub_count = _RegSize / VectorTypeInfo<valT>::bytecount;
    valT        regs[_RegCount][sub_count];
    uint8_t     state[_RegCount];
    
    VectorBankCache() { for (unsigned idx = 0; idx < _RegCount; ++idx) state[idx] = invalid; }
    
    template <typename dirT>
    valT
    GetReg( dirT& dir, unsigned idx, unsigned sub )
    {
      // Force validity
      if (state[idx] == invalid) {
        uint8_t buffer[_RegSize];
        Copy copy( idx, buffer );
        dir.DoAll( copy );
        state[idx] = valid;
        for (unsigned sub = 0; sub < sub_count; ++sub)
          VectorTypeInfo<valT>::FromBytes( regs[idx][sub], &buffer[sub*VectorTypeInfo<valT>::bytecount] );
      }
      return regs[idx][sub];
    }
  
    template <typename dirT>
    void
    SetReg( dirT& dir, unsigned idx, unsigned sub, valT value )
    {
      // Force exclusivity
      if (state[idx] != exclusive) {
        GetReg( dir, idx, sub );
        Invalidate invalidate( idx );
        dir.DoAll( invalidate );
        state[idx] = exclusive;
      }
      regs[idx][sub] = value;
    }
    
    void Do( Invalidate& inval )
    {
      unsigned idx = inval.reg;
      if (idx >= _RegCount) return;
      state[idx] = invalid;
    }
  
    void Do( Copy& copy )
    {
      unsigned idx = copy.reg;
      if (idx >= _RegCount) return;
      if (copy.size != _RegSize) throw 0;
      if (state[idx] == invalid) return;
      state[idx] = valid;
      if (not copy.buffer) return;
      for (unsigned sub = 0; sub < sub_count; ++sub)
        VectorTypeInfo<valT>::ToBytes( &copy.buffer[sub*VectorTypeInfo<valT>::bytecount], regs[idx][sub] );
      copy.buffer = 0;
    }
  };

} // end of namespace intel

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXTENSION_REGISTER_BANK_HH__ */
