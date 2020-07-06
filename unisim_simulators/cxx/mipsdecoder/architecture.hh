#ifndef __MIPS_INTERPRETER_HH__
#define __MIPS_INTERPRETER_HH__

#include <instructions.hh>
#include <unisim/util/forbint/debug/debug.hh>
#include <unisim/util/forbint/contract/contract.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <set>
#include <vector>
#include <memory>

namespace Mips
{
  struct Interpreter
  {
    struct Unimplemented {};
    struct Undefined {};
    
    
    //   =====================================================================
    //   =                             Data Types                            =
    //   =====================================================================
    typedef unisim::util::symbolic::SmartValue<double>   F64;
    typedef unisim::util::symbolic::SmartValue<float>    F32;
    typedef unisim::util::symbolic::SmartValue<bool>     BOOL;
    typedef unisim::util::symbolic::SmartValue<uint8_t>  U8;
    typedef unisim::util::symbolic::SmartValue<uint16_t> U16;
    typedef unisim::util::symbolic::SmartValue<uint32_t> U32;
    typedef unisim::util::symbolic::SmartValue<uint64_t> U64;
    typedef unisim::util::symbolic::SmartValue<int8_t>   S8;
    typedef unisim::util::symbolic::SmartValue<int16_t>  S16;
    typedef unisim::util::symbolic::SmartValue<int32_t>  S32;
    typedef unisim::util::symbolic::SmartValue<int64_t>  S64;
  
    enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  
    //   typedef unisim::util::symbolic::FP                   FP;
    typedef unisim::util::symbolic::Expr                 Expr;
    typedef unisim::util::symbolic::ScalarType           ScalarType;

    typedef unisim::util::forbint::debug::ScalarElement    FDScalarElement;
    typedef unisim::util::forbint::debug::MemoryState      FDMemoryState;
    typedef unisim::util::forbint::debug::MemoryFlags      FDMemoryFlags;
    typedef unisim::util::forbint::debug::Target           FDTarget;
    typedef unisim::util::forbint::debug::Iteration        FDIteration;

    typedef unisim::util::forbint::contract::MemoryState   FCMemoryState;
    typedef unisim::util::forbint::contract::DomainValue   FCDomainValue;

    struct INode : public unisim::util::symbolic::ExprNode
    {
      typedef unisim::util::symbolic::Expr Expr;

      /**DEBUG**/
      static void ComputeForward(Expr const&, FDScalarElement& res, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags);
      virtual void ComputeForward(FDScalarElement& res, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const = 0;

      /**CONTRACT**/
      static FCDomainValue Compute(Expr const&, FCMemoryState& mem, DomainElementFunctions* def);
      virtual FCDomainValue Compute(FCMemoryState& mem, DomainElementFunctions* def) const = 0;
    };
  
    struct FDSideEffect
    {
      virtual ~FDSideEffect() {}
      virtual void Commit(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) = 0;
    };
    
    struct FCSideEffect
    {
      virtual ~FCSideEffect() {}
      virtual void Commit(FCMemoryState& memory, DomainElementFunctions* def) = 0;
    };
    
    struct Update : public unisim::util::symbolic::ExprNode
    {
      typedef unisim::util::symbolic::Expr Expr;
      
      virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
      /**DEBUG**/
      virtual std::unique_ptr<FDSideEffect> InterpretForward(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const = 0;
      /**CONTRACT**/
      virtual std::unique_ptr<FCSideEffect> Interpret(FCMemoryState& mem, DomainElementFunctions* def) const = 0;
    };
    
    struct Ctrl : public Update
    {
      virtual ~Ctrl() {}

      struct InterpretShouldNotBranch {};
      /**DEBUG**/
      virtual std::unique_ptr<FDSideEffect>
      InterpretForward(FDMemoryState&, FDTarget&, FDMemoryFlags&) const override { throw InterpretShouldNotBranch(); return 0; };
      virtual void retrieveFamily( FDIteration::FamilyInstruction& family, uint32_t origin ) const = 0;
      virtual void retrieveTargets( FDIteration& iteration ) const = 0;
      
      /**CONTRACT**/
      virtual std::unique_ptr<FCSideEffect>
      Interpret(FCMemoryState& mem, DomainElementFunctions* def) const override { throw InterpretShouldNotBranch(); return 0; }
      virtual void next_addresses(std::set<unsigned int>& addresses, FCMemoryState& mem, DomainElementFunctions* def) const = 0;
    };
  
    struct ActionNode : public unisim::util::symbolic::Conditional<ActionNode>
    {
      ActionNode() : Conditional<ActionNode>(), updates() {}
      
      void                    add_update( Expr expr ) { expr.ConstSimplify(); updates.insert( expr ); }
      void                    simplify();

      std::set<Expr> updates;
    };

    struct Load : public INode
    {
      Load( Expr const& _addr, unsigned _bytes ) : addr(_addr), bytes(_bytes) {}
      virtual Load* Mutate() const override { return new Load(*this); }
      virtual ScalarType::id_t GetType() const override { return ScalarType::IntegerType(false, bytes); }
      virtual void Repr(std::ostream& sink) const override;
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
      int compare( Load const& rhs ) const { return int(bytes) - int(rhs.bytes); }
      virtual unsigned SubCount() const override { return 1; }
      virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }

      void ComputeForward(FDScalarElement& res, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const override;

      FCDomainValue Compute(Mips::Interpreter::FCMemoryState&, DomainElementFunctions*) const override;

      Expr addr;
      unsigned bytes;
    };

    struct Store : public Update
    {
      Store( Expr const& _addr, unsigned _bytes, Expr const& _value ) : addr(_addr), value(_value), bytes(_bytes) {}
      virtual Store* Mutate() const override { return new Store(*this); };
      virtual unsigned SubCount() const override { return 2; }
      virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
      virtual void Repr( std::ostream& sink ) const override {}
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Store const&>( rhs ) ); }
      int compare( Store const& rhs ) const { return int(bytes) - int(rhs.bytes); }

      std::unique_ptr<FDSideEffect> InterpretForward(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const override;
      std::unique_ptr<FCSideEffect> Interpret(FCMemoryState& mem, DomainElementFunctions* def) const override;

      unisim::util::symbolic::Expr addr, value;
      unsigned bytes;
    };

    struct RegRead : public INode
    {
      RegisterIndex reg;
      
      RegRead( RegisterIndex _reg ) : reg(_reg) {}
      virtual RegRead* Mutate() const override { return new RegRead( *this ); }
      virtual ScalarType::id_t GetType() const override { return ScalarType::U32; }
      virtual void Repr(std::ostream& sink) const override;
      virtual unsigned SubCount() const override { return 0; }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
      int compare( RegRead const& rhs ) const { return reg.cmp( rhs.reg ); }
      
      void ComputeForward(FDScalarElement& res, FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const override;
      FCDomainValue Compute(Mips::Interpreter::FCMemoryState&, DomainElementFunctions*) const override;
    };

    struct RegWrite : public Update
    {
      RegWrite( RegisterIndex _reg, Expr const& _value ) : value(_value), reg(_reg) {}
      virtual RegWrite* Mutate() const override { return new RegWrite(*this); }
      virtual unsigned SubCount() const override { return 1; }
      virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return value; }
      virtual void Repr( std::ostream& sink ) const {}
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
      int compare( RegWrite const& rhs ) const { return reg.cmp( rhs.reg ); }
      
      std::unique_ptr<FDSideEffect> InterpretForward(FDMemoryState& memory, FDTarget& dest, FDMemoryFlags& flags) const override;
      std::unique_ptr<FCSideEffect> Interpret(FCMemoryState& mem, DomainElementFunctions* def) const override;

      unisim::util::symbolic::Expr value;
      RegisterIndex reg;
    };

    struct RegID : public unisim::util::identifier::Identifier<RegID>
    {
      enum Code
        {
         NA = 0, at, v0, v1, a0, a1, a2, a3,
         t0, t1, t2, t3, t4, t5, t6, t7,
         s0, s1, s2, s3, s4, s5, s6, s7,
         t8, t9, k0, k1, gp, sp, fp, ra,
         npc, hi, lo,
         end
        } code;

      char const* c_str() const
      {
        switch (code)
          {
          case at:  return "at";
          case v0:  return "v0";
          case v1:  return "v1";
          case a0:  return "a0";
          case a1:  return "a1";
          case a2:  return "a2";
          case a3:  return "a3";
          case t0:  return "t0";
          case t1:  return "t1";
          case t2:  return "t2";
          case t3:  return "t3";
          case t4:  return "t4";
          case t5:  return "t5";
          case t6:  return "t6";
          case t7:  return "t7";
          case s0:  return "s0";
          case s1:  return "s1";
          case s2:  return "s2";
          case s3:  return "s3";
          case s4:  return "s4";
          case s5:  return "s5";
          case s6:  return "s6";
          case s7:  return "s7";
          case t8:  return "t8";
          case t9:  return "t9";
          case k0:  return "k0";
          case k1:  return "k1";
          case gp:  return "gp";
          case sp:  return "sp";
          case fp:  return "fp";
          case ra:  return "ra";
          case npc: return "npc";
          case lo:  return "lo";
          case hi:  return "hi";
          case NA:
          case end: break;
          }
        return "NA";
      }

      RegID() : code(end) {}
      RegID( Code _code ) : code(_code) {}
      RegID( char const* _code ) : code(end) { init( _code ); }
    };

    //   =====================================================================
    //   =                      Construction/Destruction                     =
    //   =====================================================================

    Interpreter( Interpreter const& ) = delete;

    Interpreter(uint32_t addr, ActionNode* root)
      : path(root)
      , regs()
      , current_address()
      , return_address()
      , in_delay_slot(false)
    {
      // GPR regs
      regs[0] = U32(0);
      for (RegisterIndex reg(RegisterIndex::zero); reg.next();)
        regs[reg.idx()] = U32( Expr( new RegRead(reg) ) );
      current_address = U32(addr);
      return_address = addr + 8;
    }
    
    bool close( Interpreter const& ref )
    {
      bool complete = path->close();
      for (RegisterIndex reg(RegisterIndex::zero); reg.next();)
        {
          if (regs[reg.idx()].expr != regs[reg.idx()].expr)
            path->add_update( new RegWrite( reg, regs[reg.idx()].expr ) );
        }
      return complete;
    }

    bool concretize( Expr const& cexp )
    {
      bool predicate = path->proceed( cexp );
      path = path->next( predicate );
      return predicate;
    }

    template <typename T>
    bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
    {
      if (not cond.expr.good())
        throw std::logic_error( "Not a valid condition" );

      Expr cexp( BOOL(cond).expr );
      if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
        return cnode->Get( bool() );

      return concretize( cexp );
    }

    bool     IsBigEndian() const { return false; }

    //   =====================================================================
    //   =             General Purpose Registers access methods              =
    //   =====================================================================
    void     SetGPR(unsigned idx, U32 value) { if (idx) regs[idx] = value; }
    U32      GetGPR(unsigned idx) { return regs[idx]; }
    void     SetDivU(U32 rs, U32 rt) { regs[RegisterIndex::lo] = rs / rt; regs[RegisterIndex::hi] = rs % rt; }
    void     SetHI(unsigned ra, U32 value) { if (ra) throw "nope"; regs[RegisterIndex::hi] = value; }
    void     SetLO(unsigned ra, U32 value) { if (ra) throw "nope"; regs[RegisterIndex::lo] = value; }
    U32      GetHI(unsigned ra) { if (ra) throw "nope"; return regs[RegisterIndex::hi]; }
    U32      GetLO(unsigned ra) { if (ra) throw "nope"; return regs[RegisterIndex::lo]; }

    U32      GetHWR(unsigned idx) { throw Unimplemented(); return U32(); }

    //   =====================================================================
    //   =                      Control Transfer methods                     =
    //   =====================================================================
    
    U32  GetPC() { return current_address; }
    struct Goto : public Ctrl
    {
      Goto( U32 const& _target, branch_type_t _btype ) : target(_target.expr), btype(_btype) {}
      virtual Goto* Mutate() const override { return new Goto(*this); }
      virtual unsigned SubCount() const override { return 1; }
      virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return target; }
      virtual void Repr(std::ostream& sink) const override;
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Goto const&>( rhs ) ); }
      int compare( Goto const& rhs ) const { return int(btype) - int(rhs.btype); }
      
      /**DEBUG**/
      virtual void retrieveFamily( FDIteration::FamilyInstruction& family, uint32_t origin ) const override;
      virtual void retrieveTargets( FDIteration& iteration ) const override;

      /**CONTRACT**/
      virtual void next_addresses(std::set<unsigned int>& addresses, FCMemoryState& mem, DomainElementFunctions*) const override;
      
      Expr target;
      branch_type_t btype;
    };
    
    void Branch( U32 const& target, branch_type_t btype )
    {
      if (in_delay_slot) { struct Ouch {}; throw Ouch(); }
      path->add_update( new Goto(target,btype) );
    }
    
    struct CancelDS : public Ctrl
    {
      virtual CancelDS* Mutate() const override { return new CancelDS(*this); }
      virtual unsigned SubCount() const override { return 0; }
      virtual void Repr(std::ostream& sink) const override;
      virtual int cmp( ExprNode const& rhs ) const override { return 0; }
      
      /**DEBUG**/
      virtual void retrieveFamily( FDIteration::FamilyInstruction& family, uint32_t origin ) const override {}
      virtual void retrieveTargets( FDIteration& iteration ) const override { struct Issue {}; throw Issue (); }
      
      /**CONTRACT**/
      virtual void next_addresses(std::set<unsigned int>& addresses, FCMemoryState& mem, DomainElementFunctions* def) const override;
    };
    
    void CancelDelaySlot()
    {
      if (in_delay_slot) { struct Ouch {}; throw Ouch(); }
      path->add_update(new CancelDS);
      in_delay_slot = true;
    }

    void SysCall( unsigned imm ) { throw Unimplemented(); }

    //   =====================================================================
    //   =                       Memory access methods                       =
    //   =====================================================================
    U32 MemLoad( U32 const&, Expr const& addr ) { return U32(Expr( new Load(addr, 4) ) ); }
    U16 MemLoad( U16 const&, Expr const& addr ) { return U16(Expr( new Load(addr, 2) ) ); }
    U8  MemLoad( U8  const&, Expr const& addr ) { return  U8(Expr( new Load(addr, 1) ) ); }
    S32 MemLoad( S32 const&, Expr const& addr ) { return S32( U32(Expr( new Load(addr, 4) ) ) ); }
    S16 MemLoad( S16 const&, Expr const& addr ) { return S16( U16(Expr( new Load(addr, 2) ) ) ); }
    S8  MemLoad( S8  const&, Expr const& addr ) { return  S8(  U8(Expr( new Load(addr, 1) ) ) ); }
    template <class T> T MemRead( U32 const& addr ) { return MemLoad( T(), addr.expr ); }

    void MemStore( Expr const& addr, U32 const& value ) { path->add_update( new Store( addr, 4, value.expr ) ); }
    void MemStore( Expr const& addr, U16 const& value ) { path->add_update( new Store( addr, 2, value.expr ) ); }
    void MemStore( Expr const& addr, U8  const& value ) { path->add_update( new Store( addr, 1, value.expr ) ); }
    template <typename U> void MemWrite( U32 const& addr, U value ) { return MemStore( addr.expr, value ); }

    void AtomicBegin(U32 const&) { }
    BOOL AtomicUpdate(U32 const&) { return BOOL(true); }

    ActionNode*      path;
    U32              regs[RegisterIndex::end];
    U32              current_address;
    uint32_t         return_address;
    bool             in_delay_slot;
  };
}

#endif /*__MIPS_INTERPRETER_HH__*/
