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

    typedef unisim::util::forbint::debug::ScalarElement  FDScalarElement;
    typedef unisim::util::forbint::debug::MemoryState    FDMemoryState;
    typedef unisim::util::forbint::debug::MemoryFlags    FDMemoryFlags;
    typedef unisim::util::forbint::debug::Target         FDTarget;
    typedef unisim::util::forbint::debug::Iteration      FDIteration;

    typedef unisim::util::forbint::contract::MemoryState FCMemoryState;
    typedef unisim::util::forbint::contract::DomainValue FCDomainValue;

    class ComputationResult;
    class DebugMemoryComputationResult;
    class DebugIterationComputationResult;
    class ContractComputationResult;
    struct INode : public unisim::util::symbolic::ExprNode
    {
      typedef unisim::util::symbolic::Expr Expr;

      // template <class TComputationResult>
      static void ComputeForward(Expr const&, ComputationResult& res);
      virtual void ComputeForward(ComputationResult& res) const = 0;
      static bool requiresOriginValue(const Expr& expr, FDIteration& iteration,
          RequirementLevel requirementLevel);
      virtual bool requiresOriginValue(FDIteration& iteration, RequirementLevel requirementLevel) const = 0;

    };
  
    struct SideEffect
    {
      virtual ~SideEffect() {}
      virtual void Commit(ComputationResult& res) = 0;
    };
    
    struct Update : public unisim::util::symbolic::ExprNode
    {
      typedef unisim::util::symbolic::Expr Expr;
      
      virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
      virtual std::unique_ptr<SideEffect> InterpretForward(ComputationResult& res) const = 0;
    };
    
    struct Ctrl : public Update
    {
      virtual ~Ctrl() {}

      /**DEBUG**/
      /**CONTRACT**/
    };
  
    struct ActionNode : public unisim::util::symbolic::Conditional<ActionNode>
    {
      ActionNode() : Conditional<ActionNode>(), updates() {}
      
      void add_update( Expr expr ) { expr.ConstSimplify(); updates.insert( expr ); }
      void simplify();

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

      void ComputeForward(ComputationResult& res) const override;
      bool requiresOriginValue(FDIteration& iteration, RequirementLevel requirementLevel) const override;

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

      std::unique_ptr<SideEffect> InterpretForward(ComputationResult& res) const override;

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
      
      void ComputeForward(ComputationResult& res) const override;
      bool requiresOriginValue(FDIteration& iteration, RequirementLevel requirementLevel) const override;
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
      
      std::unique_ptr<SideEffect> InterpretForward(ComputationResult& res) const override;

      unisim::util::symbolic::Expr value;
      RegisterIndex reg;
    };

    // struct RegID : public unisim::util::identifier::Identifier<RegID>
    // {
    //   enum Code
    //     {
    //      NA = 0, at, v0, v1, a0, a1, a2, a3,
    //      t0, t1, t2, t3, t4, t5, t6, t7,
    //      s0, s1, s2, s3, s4, s5, s6, s7,
    //      t8, t9, k0, k1, gp, sp, fp, ra,
    //      npc, hi, lo,
    //      end
    //     } code;

    //   char const* c_str() const
    //   {
    //     switch (code)
    //       {
    //       case at:  return "at";
    //       case v0:  return "v0";
    //       case v1:  return "v1";
    //       case a0:  return "a0";
    //       case a1:  return "a1";
    //       case a2:  return "a2";
    //       case a3:  return "a3";
    //       case t0:  return "t0";
    //       case t1:  return "t1";
    //       case t2:  return "t2";
    //       case t3:  return "t3";
    //       case t4:  return "t4";
    //       case t5:  return "t5";
    //       case t6:  return "t6";
    //       case t7:  return "t7";
    //       case s0:  return "s0";
    //       case s1:  return "s1";
    //       case s2:  return "s2";
    //       case s3:  return "s3";
    //       case s4:  return "s4";
    //       case s5:  return "s5";
    //       case s6:  return "s6";
    //       case s7:  return "s7";
    //       case t8:  return "t8";
    //       case t9:  return "t9";
    //       case k0:  return "k0";
    //       case k1:  return "k1";
    //       case gp:  return "gp";
    //       case sp:  return "sp";
    //       case fp:  return "fp";
    //       case ra:  return "ra";
    //       case npc: return "npc";
    //       case lo:  return "lo";
    //       case hi:  return "hi";
    //       case NA:
    //       case end: break;
    //       }
    //     return "NA";
    //   }

    //   RegID() : code(end) {}
    //   RegID( Code _code ) : code(_code) {}
    //   RegID( char const* _code ) : code(end) { init( _code ); }
    // };

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
          if (regs[reg.idx()].expr != ref.regs[reg.idx()].expr)
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
    struct Goto : public Update
    {
      struct InterpretShouldNotBranch {};
      
      Goto( U32 const& _target, branch_type_t _btype ) : target(_target.expr), btype(_btype) {}
      virtual Goto* Mutate() const override { return new Goto(*this); }
      virtual unsigned SubCount() const override { return 1; }
      virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return target; }
      virtual void Repr(std::ostream& sink) const override;
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Goto const&>( rhs ) ); }
      int compare( Goto const& rhs ) const { return int(btype) - int(rhs.btype); }
      
      virtual std::unique_ptr<SideEffect>
      InterpretForward(ComputationResult&) const override { throw InterpretShouldNotBranch(); return 0; };
      
      Expr target;
      branch_type_t btype;
    };
    
    void Branch( U32 const& target, branch_type_t btype )
    {
      if (in_delay_slot) { struct Ouch {}; throw Ouch(); }
      path->add_update( new Goto(target,btype) );
    }
    
    void CancelDelaySlot()
    {
      if (in_delay_slot) { struct Ouch {}; throw Ouch(); }
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

  class Interpreter::ComputationResult {
  public:
    virtual ComputationResult* Mutate() const = 0;

    virtual void setBoolResultFromConstant(bool value) = 0;
    virtual void setMultiBitResultFromConstant(uint64_t* value, int size, bool isSigned) = 0;
    virtual void setMultiFloatResultFromConstant(long double value, int size) = 0;
    virtual void applyBitNegate() = 0;
    virtual void applyMultiBitNegate() = 0;
    virtual void applyMultiBitOpposite() = 0;
    virtual void applyMultiBitByteSwap(int bytesNumber) = 0;
    virtual void applyMultiBitCast(bool isSourceInteger, bool isDestInteger, bool isDestSigned, int destSize) = 0;
    virtual void applyBitExclusiveOr(ComputationResult& arg) = 0;
    virtual void applyMultiBitExclusiveOr(ComputationResult& arg) = 0;
    virtual void applyBitAnd(ComputationResult& arg) = 0;
    virtual void applyMultiBitAnd(ComputationResult& arg) = 0;
    virtual void applyBitOr(ComputationResult& arg) = 0;
    virtual void applyMultiBitOr(ComputationResult& arg) = 0;
    virtual void applyMultiBitLeftShift(ComputationResult& arg) = 0;
    virtual void applyMultiBitArithmeticRightShift(ComputationResult& arg) = 0;
    virtual void applyMultiBitLogicalRightShift(ComputationResult& arg) = 0;
    virtual void applyMultiBitLeftRotate(ComputationResult& arg) = 0;
    virtual void applyMultiBitRightRotate(ComputationResult& arg) = 0;
    virtual void applyMultiBitPlusSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitPlusUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatPlus(ComputationResult& arg) = 0;
    virtual void applyMultiBitMinusSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitMinusUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatMinus(ComputationResult& arg) = 0;
    virtual void applyMultiBitTimesSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitTimesUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatTimes(ComputationResult& arg) = 0;
    virtual void applyMultiBitDivideSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitDivideUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatDivide(ComputationResult& arg) = 0;
    virtual void applyMultiBitModuloSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitModuloUnsigned(ComputationResult& arg) = 0;
    virtual void applyBitCompareLess(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareLessSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareLessUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatCompareLess(ComputationResult& arg) = 0;
    virtual void applyBitCompareGreater(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareGreaterSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareGreaterUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatCompareGreater(ComputationResult& arg) = 0;
    virtual void applyBitCompareLessOrEqual(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareLessOrEqualSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareLessOrEqualUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatCompareLessOrEqual(ComputationResult& arg) = 0;
    virtual void applyBitCompareGreaterOrEqual(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareGreaterOrEqualSigned(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareGreaterOrEqualUnsigned(ComputationResult& arg) = 0;
    virtual void applyMultiFloatCompareGreaterOrEqual(ComputationResult& arg) = 0;
    virtual void applyBitCompareEqual(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareEqual(ComputationResult& arg) = 0;
    virtual void applyMultiFloatCompareEqual(ComputationResult& arg) = 0;
    virtual void applyBitCompareDifferent(ComputationResult& arg) = 0;
    virtual void applyMultiBitCompareDifferent(ComputationResult& arg) = 0;
    virtual void applyMultiFloatCompareDifferent(ComputationResult& arg) = 0;

    virtual void loadValue(int bytesize) = 0;
    virtual void storeValue(ComputationResult& addr, ComputationResult& value) = 0;
    virtual void getRegisterValue(int index) = 0;
    virtual void setRegisterValue(int index, ComputationResult& value) = 0;
  };

  class Interpreter::DebugMemoryComputationResult : public ComputationResult {
  public:
    typedef unisim::util::forbint::debug::MultiBit::Operation MultiBitOperation;
    typedef unisim::util::forbint::debug::Bit::Operation BitOperation;
    typedef unisim::util::forbint::debug::Floating::Operation FloatingOperation;
    typedef unisim::util::forbint::debug::MultiBit::CastMultiBitOperation CastMultiBitOperation;
    typedef unisim::util::forbint::debug::MultiBit::ReduceOperation MultiBitReduceOperation;
    typedef unisim::util::forbint::debug::MultiBit::BitSetOperation MultiBitBitSetOperation;
    typedef unisim::util::forbint::debug::MultiBit::CastMultiBitOperation MultiBitCastMultiBitOperation;
    typedef unisim::util::forbint::debug::Bit::CastMultiBitOperation BitCastMultiBitOperation;
    typedef unisim::util::forbint::debug::LocalProperties LocalProperties;

  private:
    FDScalarElement res; // a tree to backward propagate the constraints
    FDMemoryState& memory;
    FDMemoryFlags& flags;

  public:
    DebugMemoryComputationResult(FDMemoryState& amemory, FDMemoryFlags& aflags)
      : memory(amemory), flags(aflags) {} 
    DebugMemoryComputationResult(const DebugMemoryComputationResult& source)
      : memory(source.memory), flags(source.flags) {}

    virtual ComputationResult* Mutate() const override { return new DebugMemoryComputationResult(*this); }
    virtual void setBoolResultFromConstant(bool value) override
      { res = flags.newBitConstant(value); }
    virtual void setMultiBitResultFromConstant(uint64_t* value, int size, bool isSigned) override
      { res = flags.newMultiBitConstant(value, size); }
    virtual void setMultiFloatResultFromConstant(long double value, int size) override
      { res = flags.newTopFloat(size == 32 ? 8 : (size == 64 ? 11 : 15),
            size == 32 ? 23 : (size == 64 ? 52 : 64));
        { struct NotYet {}; throw NotYet(); }
      }
    virtual void applyBitNegate() override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setNegate(), flags.scalarPart());
      }
    virtual void applyMultiBitNegate() override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setBitNegate(), flags.scalarPart());
      }
    virtual void applyMultiBitOpposite() override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setOppositeSigned(), flags.scalarPart());
      }
    virtual void applyMultiBitByteSwap(int bytesNumber) override
      {
        LocalProperties localProperties(flags);
        for (int index = 0; index < bytesNumber/2; ++index) {
          localProperties.setScalar();
          FDScalarElement first(res, FDScalarElement::Duplicate()), last(res, FDScalarElement::Duplicate());
          first.applyAssign(MultiBitReduceOperation().setLowBit(index*8+0).setHighBit(index*8+7),
                flags.scalarPart());
          localProperties.clear();
          localProperties.setScalar(); 
          last.applyAssign(MultiBitReduceOperation().setLowBit((bytesNumber-index-1)*8+0)
                .setHighBit((bytesNumber-index)*8-1), flags.scalarPart());
          localProperties.clear();
          localProperties.setScalar(); 
          res.applyAssign(MultiBitBitSetOperation().setLowBit(index*8+0).setHighBit(index*8+7),
                last, flags.scalarPart());
          localProperties.clear();
          localProperties.setScalar(); 
          res.applyAssign(MultiBitBitSetOperation().setLowBit((bytesNumber-index-1)*8+0)
                .setHighBit((bytesNumber-index)*8-1), first, flags.scalarPart());
          localProperties.clear();
        }
      }
    virtual void applyMultiBitCast(bool isSourceInteger, bool isDestInteger, bool isDestSigned, int destSize) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        if (isSourceInteger) {
           if (isDestInteger)
             res.applyAssign(MultiBitCastMultiBitOperation().setSize(destSize), flags.scalarPart());
           else
             res.applyAssign(MultiBitOperation().setCastBit(), flags.scalarPart());
        }
        else
           res.applyAssign(BitCastMultiBitOperation().setSize(destSize), flags.scalarPart());
      }
    virtual void applyBitExclusiveOr(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setExclusiveOr(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitExclusiveOr(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setBitExclusiveOr(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitAnd(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setAnd(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitAnd(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setBitAnd(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitOr(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setOr(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitOr(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setBitOr(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitLeftShift(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setLeftShift(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitArithmeticRightShift(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setArithmeticRightShift(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitLogicalRightShift(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setLogicalRightShift(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitLeftRotate(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setLeftRotate(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitRightRotate(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setRightRotate(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitPlusSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setPlusSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitPlusUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setPlusUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatPlus(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setPlus(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitMinusSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setMinusSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitMinusUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setMinusUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatMinus(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setMinus(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitTimesSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setTimesSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitTimesUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setTimesUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatTimes(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setTimes(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitDivideSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setDivideSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitDivideUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setDivideUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatDivide(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setDivide(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitModuloSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setModuloSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitModuloUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setModuloUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitCompareLess(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setCompareLess(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareLessSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareLessSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareLessUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareLessUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatCompareLess(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setCompareLess(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitCompareGreater(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setCompareGreater(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareGreaterSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareGreaterSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareGreaterUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareGreaterUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatCompareGreater(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setCompareGreater(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitCompareLessOrEqual(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setCompareLessOrEqual(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareLessOrEqualSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareLessOrEqualSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareLessOrEqualUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareLessOrEqualUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatCompareLessOrEqual(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setCompareLessOrEqual(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitCompareGreaterOrEqual(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setCompareGreaterOrEqual(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareGreaterOrEqualSigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualSigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareGreaterOrEqualUnsigned(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualUnsigned(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatCompareGreaterOrEqual(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setCompareGreaterOrEqual(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitCompareEqual(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setCompareEqual(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareEqual(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareEqual(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatCompareEqual(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setCompareEqual(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyBitCompareDifferent(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(BitOperation().setCompareDifferent(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiBitCompareDifferent(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(MultiBitOperation().setCompareDifferent(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }
    virtual void applyMultiFloatCompareDifferent(ComputationResult& arg) override
      { LocalProperties localProperties(flags);
        localProperties.setScalar(); 
        res.applyAssign(FloatingOperation().setCompareDifferent(), static_cast<DebugMemoryComputationResult&>(arg).res, flags.scalarPart());
      }

    virtual void loadValue(int bytesize) override
      { LocalProperties localProperties(flags);
        localProperties.setMemory();
        res = memory.loadValue(res, bytesize*8, flags);
      }
    virtual void storeValue(ComputationResult& addr, ComputationResult& value) override
      {
        LocalProperties localProperties(flags);
        localProperties.setAcceptRequirements(1).setMemory();
        memory.storeValue(static_cast<DebugMemoryComputationResult&>(addr).res,
            static_cast<DebugMemoryComputationResult&>(value).res, flags);
      }
    virtual void getRegisterValue(int index) override
      { LocalProperties localProperties(flags);
        localProperties/* .setAcceptRequirements(1) */.setMemory();
        res = memory.getRegisterValue(index, flags, unisim::util::forbint::debug::TIMultiBit);
      }
    virtual void setRegisterValue(int index, ComputationResult& value) override
      { LocalProperties localProperties(flags);
        localProperties.setMemory();
        memory.setRegisterValue(index, static_cast<DebugMemoryComputationResult&>(value).res,
            flags);
      }
  };

  class Interpreter::DebugIterationComputationResult : public ComputationResult {
  public:
    typedef unisim::util::forbint::debug::MultiBit::Operation MultiBitOperation;
    typedef unisim::util::forbint::debug::Bit::Operation BitOperation;
    typedef unisim::util::forbint::debug::Floating::Operation FloatingOperation;
    typedef unisim::util::forbint::debug::MultiBit::CastMultiBitOperation CastMultiBitOperation;
    typedef unisim::util::forbint::debug::MultiBit::ReduceOperation MultiBitReduceOperation;
    typedef unisim::util::forbint::debug::MultiBit::BitSetOperation MultiBitBitSetOperation;
    typedef unisim::util::forbint::debug::MultiBit::CastMultiBitOperation MultiBitCastMultiBitOperation;
    typedef unisim::util::forbint::debug::Bit::CastMultiBitOperation BitCastMultiBitOperation;

  public:
    FDScalarElement res;

  private:
    unisim::util::forbint::debug::Flags flags;
    FDIteration& iteration;

  public:
    DebugIterationComputationResult(FDIteration& aiteration)
      : iteration(aiteration) {} 
    DebugIterationComputationResult(const DebugIterationComputationResult& source)
      : iteration(source.iteration) {}

    void clear() { res = FDScalarElement(); flags.clear(); }
    virtual ComputationResult* Mutate() const { return new DebugIterationComputationResult(*this); }
    virtual void setBoolResultFromConstant(bool value)
      { res = iteration.newBitConstant(value); }
    virtual void setMultiBitResultFromConstant(uint64_t* value, int size, bool isSigned)
      { res = iteration.newMultiBitConstant(value, size); }
    virtual void setMultiFloatResultFromConstant(long double value, int size)
      { res = iteration.newTopFloat(size == 32 ? 8 : (size == 64 ? 11 : 15),
            size == 32 ? 23 : (size == 64 ? 52 : 64));
        { struct NotYet {}; throw NotYet(); }
      }
    virtual void applyBitNegate() override
      { res.applyAssign(BitOperation().setNegate(), flags);
        flags.clear();
      }
    virtual void applyMultiBitNegate() override
      { res.applyAssign(MultiBitOperation().setBitNegate(), flags);
        flags.clear();
      }
    virtual void applyMultiBitOpposite() override
      { res.applyAssign(MultiBitOperation().setOppositeSigned(), flags);
        flags.clear();
      }
    virtual void applyMultiBitByteSwap(int bytesNumber) override
      {
        for (int index = 0; index < bytesNumber/2; ++index) {
          FDScalarElement first(res, FDScalarElement::Duplicate()), last(res, FDScalarElement::Duplicate());
          first.applyAssign(MultiBitReduceOperation().setLowBit(index*8+0).setHighBit(index*8+7),
                flags);
          flags.clear();
          last.applyAssign(MultiBitReduceOperation().setLowBit((bytesNumber-index-1)*8+0)
                .setHighBit((bytesNumber-index)*8-1), flags);
          flags.clear();
          res.applyAssign(MultiBitBitSetOperation().setLowBit(index*8+0).setHighBit(index*8+7),
                last, flags);
          flags.clear();
          res.applyAssign(MultiBitBitSetOperation().setLowBit((bytesNumber-index-1)*8+0)
                .setHighBit((bytesNumber-index)*8-1), first, flags);
          flags.clear();
        }
      }
    virtual void applyMultiBitCast(bool isSourceInteger, bool isDestInteger, bool isDestSigned, int destSize) override
      { if (isSourceInteger) {
           if (isDestInteger)
             res.applyAssign(MultiBitCastMultiBitOperation().setSize(destSize), flags);
           else
             res.applyAssign(MultiBitOperation().setCastBit(), flags);
        }
        else
           res.applyAssign(BitCastMultiBitOperation().setSize(destSize), flags);
        flags.clear();
      }
    virtual void applyBitExclusiveOr(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setExclusiveOr(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitExclusiveOr(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setBitExclusiveOr(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitAnd(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setAnd(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitAnd(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setBitAnd(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitOr(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setOr(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitOr(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setBitOr(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitLeftShift(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setLeftShift(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitArithmeticRightShift(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setArithmeticRightShift(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitLogicalRightShift(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setLogicalRightShift(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitLeftRotate(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setLeftRotate(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitRightRotate(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setRightRotate(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitPlusSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setPlusSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitPlusUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setPlusUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatPlus(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setPlus(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitMinusSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setMinusSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitMinusUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setMinusUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatMinus(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setMinus(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitTimesSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setTimesSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitTimesUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setTimesUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatTimes(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setTimes(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitDivideSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setDivideSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitDivideUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setDivideUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatDivide(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setDivide(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitModuloSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setModuloSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitModuloUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setModuloUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitCompareLess(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setCompareLess(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareLessSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareLessSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareLessUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareLessUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatCompareLess(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setCompareLess(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitCompareGreater(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setCompareGreater(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareGreaterSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareGreaterSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareGreaterUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareGreaterUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatCompareGreater(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setCompareGreater(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitCompareLessOrEqual(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setCompareLessOrEqual(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareLessOrEqualSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareLessOrEqualSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareLessOrEqualUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareLessOrEqualUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatCompareLessOrEqual(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setCompareLessOrEqual(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitCompareGreaterOrEqual(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setCompareGreaterOrEqual(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareGreaterOrEqualSigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualSigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareGreaterOrEqualUnsigned(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareGreaterOrEqualUnsigned(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatCompareGreaterOrEqual(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setCompareGreaterOrEqual(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitCompareEqual(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setCompareEqual(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareEqual(ComputationResult& arg) override
      { res.applyAssign(MultiBitOperation().setCompareEqual(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatCompareEqual(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setCompareEqual(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyBitCompareDifferent(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setCompareDifferent(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiBitCompareDifferent(ComputationResult& arg) override
      { res.applyAssign(BitOperation().setCompareDifferent(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }
    virtual void applyMultiFloatCompareDifferent(ComputationResult& arg) override
      { res.applyAssign(FloatingOperation().setCompareDifferent(), static_cast<DebugIterationComputationResult&>(arg).res, flags);
        flags.clear();
      }

    virtual void loadValue(int bytesize) override
      { res = iteration.loadValue(res, bytesize*8); }
    virtual void storeValue(ComputationResult& addr, ComputationResult& value)
      { res = FDScalarElement(); }
    virtual void getRegisterValue(int index) override
      { res = iteration.getRegisterValue(index, unisim::util::forbint::debug::TIMultiBit); }
    virtual void setRegisterValue(int index, ComputationResult& value) override {}
  };

  class Interpreter::ContractComputationResult : public ComputationResult {
  public:
    FCDomainValue res;

  private:
    FCMemoryState& memory;
    DomainElementFunctions* def;

  public:
    ContractComputationResult(FCMemoryState& amemory, DomainElementFunctions* adef)
      : memory(amemory), def(adef) {} 
    ContractComputationResult(const ContractComputationResult& source)
      : memory(source.memory) {}

    virtual ComputationResult* Mutate() const override { return new ContractComputationResult(*this); }
    virtual void setBoolResultFromConstant(bool value) override
      { res = FCDomainValue( def->bit_create_constant(value), def, memory.env() );
      }
    virtual void setMultiBitResultFromConstant(uint64_t* value, int size, bool isSigned) override
      { DomainIntegerConstant constantValue{size,isSigned,*value};
        res = Interpreter::FCDomainValue( def->multibit_create_constant(constantValue), def, memory.env() );
      }
    virtual void setMultiFloatResultFromConstant(long double value, int size) override
      { DomainFloatingPointConstant constantValue{size, value};
        res = Interpreter::FCDomainValue( def->multifloat_create_constant(constantValue), def, memory.env() );
      }

    virtual void applyBitNegate() override
      { (*def->bit_unary_apply_assign)(&res.svalue(), DBUONegate, memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitNegate() override
      { (*def->multibit_unary_apply_assign)(&res.svalue(), DMBUOBitNegate, memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitOpposite() override
      { (*def->multibit_unary_apply_assign)(&res.svalue(), DMBUOOppositeSigned, memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitByteSwap(int bytesNumber) override
      { for (int index = 0; index < bytesNumber/2; ++index) {
          FCDomainValue first(res), last(res);
          (*def->multibit_reduce_apply_assign)(&first.svalue(),
             DomainMultiBitReduceOperation{index*8+0, index*8+7}, memory.env());
          memory.clearEnv();
          (*def->multibit_reduce_apply_assign)(&last.svalue(),
             DomainMultiBitReduceOperation{(bytesNumber-index-1)*8+0, (bytesNumber-index)*8-1},
             memory.env());
          memory.clearEnv();
          (*def->multibit_bitset_apply_assign)(&res.svalue(),
              DomainMultiBitSetOperation{index*8+0, index*8+7}, last.value(), memory.env());
          memory.clearEnv();
          (*def->multibit_bitset_apply_assign)(&res.svalue(),
              DomainMultiBitSetOperation{(bytesNumber-index-1)*8+0, (bytesNumber-index)*8-1},
              first.value(), memory.env());
          memory.clearEnv();
        }
      }
    virtual void applyMultiBitCast(bool isSourceInteger, bool isDestInteger, bool isDestSigned, int destSize) override
      { if (!isDestInteger)
          res.assign( (*def->bit_create_cast_multibit)(res.value(), destSize, memory.env()) );
        else if (!isSourceInteger)
          res.assign( (*def->multibit_create_cast_bit)(res.value(), memory.env()) );
        else
          res.assign( (*def->multibit_create_cast_multibit)(res.value(), destSize, isDestSigned, memory.env()) );
        memory.clearEnv();
      }
    virtual void applyBitExclusiveOr(ComputationResult& arg) override
      { (*def->bit_binary_apply_assign)(&res.svalue(), DBBOExclusiveOr,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitExclusiveOr(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOBitExclusiveOr,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyBitAnd(ComputationResult& arg) override
      { (*def->bit_binary_apply_assign)(&res.svalue(), DBBOAnd,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitAnd(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOBitAnd,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyBitOr(ComputationResult& arg) override
      { (*def->bit_binary_apply_assign)(&res.svalue(), DBBOOr,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitOr(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOBitOr,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitLeftShift(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOLeftShift,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitArithmeticRightShift(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOArithmeticRightShift,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitLogicalRightShift(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOLogicalRightShift,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitLeftRotate(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOLeftRotate,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitRightRotate(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBORightRotate,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitPlusSigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOPlusSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitPlusUnsigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOPlusUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiFloatPlus(ComputationResult& arg) override
      { (*def->multifloat_binary_apply_assign)(&res.svalue(), DMFBOPlus,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitMinusSigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOMinusSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitMinusUnsigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOMinusUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiFloatMinus(ComputationResult& arg) override
      { (*def->multifloat_binary_apply_assign)(&res.svalue(), DMFBOMinus,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitTimesSigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOTimesSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitTimesUnsigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOTimesUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiFloatTimes(ComputationResult& arg) override
      { (*def->multifloat_binary_apply_assign)(&res.svalue(), DMFBOTimes,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitDivideSigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBODivideSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitDivideUnsigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBODivideUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiFloatDivide(ComputationResult& arg) override
      { (*def->multifloat_binary_apply_assign)(&res.svalue(), DMFBODivide,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitModuloSigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOModuloSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyMultiBitModuloUnsigned(ComputationResult& arg) override
      { (*def->multibit_binary_apply_assign)(&res.svalue(), DMBBOModuloUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env());
        memory.clearEnv();
      }
    virtual void applyBitCompareLess(ComputationResult& arg) override
      { res.assign( (*def->bit_binary_compare_domain)(res.value(), DBCOCompareLess,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareLessSigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareLessSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareLessUnsigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareLessUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiFloatCompareLess(ComputationResult& arg) override
      { res.assign( (*def->multifloat_binary_compare_domain)(res.value(), DMFCOCompareLess,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyBitCompareGreater(ComputationResult& arg) override
      { res.assign( (*def->bit_binary_compare_domain)(res.value(), DBCOCompareGreater,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareGreaterSigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareGreaterSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareGreaterUnsigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareGreaterUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiFloatCompareGreater(ComputationResult& arg) override
      { res.assign( (*def->multifloat_binary_compare_domain)(res.value(), DMFCOCompareGreater,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyBitCompareLessOrEqual(ComputationResult& arg) override
      { res.assign( (*def->bit_binary_compare_domain)(res.value(), DBCOCompareLessOrEqual,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareLessOrEqualSigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareLessOrEqualSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareLessOrEqualUnsigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareLessOrEqualUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiFloatCompareLessOrEqual(ComputationResult& arg) override
      { res.assign( (*def->multifloat_binary_compare_domain)(res.value(), DMFCOCompareLessOrEqual,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyBitCompareGreaterOrEqual(ComputationResult& arg) override
      { res.assign( (*def->bit_binary_compare_domain)(res.value(), DBCOCompareGreaterOrEqual,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareGreaterOrEqualSigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareGreaterOrEqualSigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareGreaterOrEqualUnsigned(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareGreaterOrEqualUnsigned,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiFloatCompareGreaterOrEqual(ComputationResult& arg) override
      { res.assign( (*def->multifloat_binary_compare_domain)(res.value(), DMFCOCompareGreaterOrEqual,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyBitCompareEqual(ComputationResult& arg) override
      { res.assign( (*def->bit_binary_compare_domain)(res.value(), DBCOCompareEqual,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareEqual(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareEqual,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiFloatCompareEqual(ComputationResult& arg) override
      { res.assign( (*def->multifloat_binary_compare_domain)(res.value(), DMFCOCompareEqual,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyBitCompareDifferent(ComputationResult& arg) override
      { res.assign( (*def->bit_binary_compare_domain)(res.value(), DBCOCompareDifferent,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiBitCompareDifferent(ComputationResult& arg) override
      { res.assign( (*def->multibit_binary_compare_domain)(res.value(), DMBCOCompareDifferent,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }
    virtual void applyMultiFloatCompareDifferent(ComputationResult& arg) override
      { res.assign( (*def->multifloat_binary_compare_domain)(res.value(), DMFCOCompareDifferent,
              static_cast<ContractComputationResult&>(arg).res.value(), memory.env()) );
        memory.clearEnv();
      }

    virtual void loadValue(int bytesize) override
      { res = Interpreter::FCDomainValue( memory.loadMultiBit(res, bytesize), def, memory.env() ); }
    virtual void storeValue(ComputationResult& addr, ComputationResult& value)
      { memory.valueStore(std::move(static_cast<ContractComputationResult&>(addr).res),
            std::move(static_cast<ContractComputationResult&>(value).res));
      }
    virtual void getRegisterValue(int index) override
      { res = Interpreter::FCDomainValue( memory.getRegisterValueAsElement(index), def,
            memory.env() );
      }
    virtual void setRegisterValue(int index, ComputationResult& value) override
      { memory.setRegisterValue(index, static_cast<ContractComputationResult&>(value).res); }
  };

}

#endif /*__MIPS_INTERPRETER_HH__*/
