#ifndef INTEL_MODRM_HH
#define INTEL_MODRM_HH

#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/processor/intel/segments.hh>
#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/fwd.hh>
#include <iosfwd>
#include <iostream>

namespace intel
{
  struct MOp
  {
    MOp( uint8_t _segment ) : segment( _segment ) {} uint8_t segment;
    virtual ~MOp() {}
    virtual void  disasm_memory_operand( std::ostream& _sink ) const { throw 0; };
    virtual u32_t effective_address( Arch& _arch ) const { throw 0; return u32_t( 0 ); };
  };
  
  struct _RMOp
  {
    MOp const* mop;
    _RMOp( MOp const* _mop ) : mop( _mop ) {}
    _RMOp( _RMOp const& _rmop ) : mop( _rmop.mop ) {}
    bool is_memory_operand() const { return (uintptr_t( mop ) > 0x1000); }
    MOp const* operator -> () const { return mop; }
    unsigned ereg() const { return unsigned( uintptr_t( mop ) ); }
    operator MOp const* () const { return mop; }
  };
    
  struct RMOp : public _RMOp
  {
    RMOp( MOp* _mop ) : _RMOp( _mop ) {}
    RMOp( RMOp const& ) = delete;
    RMOp( _RMOp const& ) = delete;
    ~RMOp() { if (is_memory_operand()) delete mop; }
  };
  
}

#endif // INTEL_MODRM_HH
