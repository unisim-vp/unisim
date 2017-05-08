/*
 *  Copyright (c) 2015,
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

#ifndef ARCH_HH
#define ARCH_HH

#include <unisim/util/symbolic/symbolic.hh>
#include <map>
#include <vector>
#include <list>

namespace ut
{
  using unisim::util::symbolic::SmartValue;
  typedef SmartValue<bool>     BOOL;
  typedef SmartValue<uint8_t>  U8;
  typedef SmartValue<uint16_t> U16;
  typedef SmartValue<uint32_t> U32;
  typedef SmartValue<uint64_t> U64;
  typedef SmartValue<int8_t>   S8;
  typedef SmartValue<int16_t>  S16;
  typedef SmartValue<int32_t>  S32;
  typedef SmartValue<int64_t>  S64;

  struct UniqueVId {
    UniqueVId() : index(0) {} unsigned index;
    unsigned next() { return index++; }
    void reset() { index = 0; }
  };
  
  struct VirtualRegister
  {
    VirtualRegister() : vindex(0), source(false), destination(false), bad(true) {}
    VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
    uint8_t vindex      : 5;
    uint8_t source      : 1;
    uint8_t destination : 1;
    uint8_t bad         : 1;
    
    void allocate( UniqueVId& uvi ) { if (not bad) throw 0; vindex = uvi.next(); bad = false; }
    void addaccess( bool w ) { source |= not w; destination |= w; }
    
    int  cmp( VirtualRegister const& ) const;
  };
  
  struct Interface
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    Interface()
      : irmap(), iruse(), load_addrs(), store_addrs()
      , psr(0), length(0), itsensitive(false), base_register(-1), aligned(true)
    {}
    
    std::map<unsigned,VirtualRegister> irmap;
    std::vector<unsigned>              iruse;
    std::list<Expr>                    load_addrs;
    std::list<Expr>                    store_addrs;
    VirtualRegister                    psr;
    uint8_t                            length;
    bool                               itsensitive;
    uint8_t                            base_register;
    bool                               aligned;
    
    void irappend( uint8_t _index, bool w, UniqueVId& uvi );
    void readflags() { psr.addaccess( false ); }
    void writeflags() { psr.addaccess( true ); }
    
    int  cmp( Interface const& ) const;
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    bool usemem() const { return store_addrs.size() or load_addrs.size(); }
    
    void finalize( uint8_t _length );
    
    struct Prologue
    {
      struct Error {};
      
      typedef std::map<unsigned,uint32_t> Regs;
      Prologue( Regs const& _regs, uint32_t _offset, bool _sign, uint8_t _base )
        : regs( _regs ), offset( _offset ), sign( _sign ), base( _base )
      {}
      Regs regs; uint32_t offset; bool sign; uint8_t base;
    };
    
    Prologue GetPrologue() const;
    
    void PCRelPrologue( Prologue const& pc ) const;
    struct Untestable
    {
      std::string argument;
      Untestable( std::string part ) : argument( part ) {}
      Untestable( std::string part, Untestable const& ut ) : argument( part + ' ' + ut.argument ) {}
    };
  };
  
  struct SourceReg : public unisim::util::symbolic::ExprNode
  {
    SourceReg( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    virtual void Repr( std::ostream& sink ) const;
    virtual void Traverse( ExprNode::Visitor& visitor ) {}
    virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const
    {
      unsigned ref = dynamic_cast<SourceReg const&>( brhs ).reg;
      return (reg < ref) ? -1 : (reg > ref) ? +1 : 0;
    }
    virtual unisim::util::symbolic::ExprNode* GetConstNode() { return 0; }
  };
  
  struct Reject {};
  struct CPU;
  
  struct XER
  {
    struct OV {};
    struct SO {};
    struct CA {};
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) {}
    template <typename PART> U8 Get() { return U8(0); }
  };

  struct CR
  {
    struct CR0
    {
      struct OV {};
      struct SO {};
      struct LT {};
      struct GT {};
      struct EQ {};
    };
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) {}
    template <typename PART> U8 Get() { return U8(0); }
  };

  struct CPU : public XER, public CR
  {
    typedef ut::BOOL BOOL;
    typedef ut::U8   U8;
    typedef ut::U16  U16;
    typedef ut::U32  U32;
    typedef ut::U64  U64;
    typedef ut::S8   S8;
    typedef ut::S16  S16;
    typedef ut::S16  S32;
    typedef ut::S16  S64;
    
    struct ProgramInterrupt
    {
      struct UnimplementedInstruction {};
      struct IllegalInstruction {};
    };
    
    XER& GetXER() { return *this; }
    CR& GetCR() { return *this; }
    
    template <class T> void ThrowException() { reject(); }

    Interface interface;
    UniqueVId gpr_uvi;
    U32   reg_values[32];
    
    U32 GetGPR(unsigned n) { return reg_values[n]; };
    void SetGPR(unsigned n, U32 value) { reg_values[n] = value; }
    
    void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w, gpr_uvi ); }

    void reject();
    
    void donttest_system();
    void donttest_branch();

  };
  
  U32 SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, U8 carry_in);
}

#endif // ARCH_HH
