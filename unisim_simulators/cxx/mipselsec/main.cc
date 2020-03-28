#include <unisim/component/cxx/processor/mips/isa/disasm.hh>
#include <unisim/component/cxx/processor/mips/isa/mipsel.tcc>
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>

struct Processor
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
  
//   typedef unisim::util::symbolic::FP                   FP;
  typedef unisim::util::symbolic::Expr                 Expr;
  typedef unisim::util::symbolic::ScalarType           ScalarType;
  
   typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;
//   typedef unisim::util::symbolic::binsec::Store        Store;
//   typedef unisim::util::symbolic::binsec::Branch       Br;

  template <typename RID>
  struct RegRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef RegRead<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegRead Super;
    RegRead( RID _id, ScalarType::id_t _tp ) : Super(), tp(_tp), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return tp; }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
    int compare( RegRead const& rhs ) const { if (int delta = int(tp) - int(rhs.tp)) return delta; if (int delta = id.cmp( rhs.id )) return delta; return Super::compare(rhs); }

    ScalarType::id_t tp;
    RID id;
  };

  template <typename RID>
  static Expr newRegRead( RID id, ScalarType::id_t tp ) { return new RegRead<RID>( id, tp ); }

//   struct ForeignRegister : public unisim::util::symbolic::binsec::RegRead
//   {
//     typedef unisim::util::symbolic::binsec::RegRead Super;
//     ForeignRegister( uint8_t _mode, unsigned _idx )
//       : Super(), idx(_idx), mode(_mode)
//     {
//       if (mode == SYSTEM_MODE) mode = USER_MODE;
//     }
//     virtual ForeignRegister* Mutate() const { return new ForeignRegister( *this ); }
//     virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
    
//     char const* mode_ident() const
//     {
//       switch (mode)
//         {
//         case USER_MODE: return "usr";
//         case FIQ_MODE: return "fiq";
//         case IRQ_MODE: return "irq";
//         case SUPERVISOR_MODE: return "svc";
//         case MONITOR_MODE: return "mon";
//         case ABORT_MODE: return "abt";
//         case HYPERVISOR_MODE: return "hyp";
//         case UNDEFINED_MODE: return "und";
//         }
//       throw 0;
//       return "";
//     }

//     virtual void GetRegName( std::ostream& sink ) const
//     {
//       sink << (RegID("r0") + idx).c_str() << '_' << mode_ident();
//     }
//     virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<ForeignRegister const&>( rhs ) ); }
//     int compare( ForeignRegister const& rhs ) const
//     {
//       if (int delta = int(mode) - int(rhs.mode)) return delta;
//       return idx - rhs.idx;
//     }
    
//     unsigned idx;
//     uint8_t mode;
//   };
  
//   template <typename RID>
//   struct RegWrite : public unisim::util::symbolic::binsec::RegWrite
//   {
//     typedef RegWrite<RID> this_type;
//     typedef unisim::util::symbolic::binsec::RegWrite Super;
//     RegWrite( RID _id, Expr const& _value ) : Super(_value), id(_id) {}
//     virtual this_type* Mutate() const { return new this_type( *this ); }
//     virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
//     virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
//     virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
//     int compare( RegWrite const& rhs ) const { if (int delta = id.cmp( rhs.id )) return delta; return Super::compare( rhs ); }
    
//     RID id;
//   };

//   template <typename RID>
//   static Expr newRegWrite( RID id, Expr const& value ) { return new RegWrite<RID>( id, value ); }
  
//   struct Goto : public Br
//   {
//     Goto( Expr const& value ) : Br( value ) {}
//     virtual Goto* Mutate() const override { return new Goto( *this ); }
//     virtual void GetRegName( std::ostream& sink ) const override { sink << "pc"; }
//     virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
//     virtual void annotate(std::ostream& sink) const override { return; }
//   };

//   struct Call : public Goto
//   {
//     Call( Expr const& value, uint32_t ra ) : Goto( value ), return_address( ra ) {}
//     virtual Call* Mutate() const override { return new Call( *this ); }
//     virtual void annotate(std::ostream& sink) const override { sink << " // call (" << unisim::util::symbolic::binsec::dbx(4,return_address) << ",0)"; }

//     uint32_t return_address;
//   };

//   struct ITCond {};
    
//   struct Mode
//   {
//     Mode() {}
//     bool     HasBR( unsigned index ) { return false; }
//     bool     HasSPSR() { return false; }
//     void     SetSPSR(U32 const& value) {}
//     U32      GetSPSR() { throw Unimplemented(); return U32(); }
//     void     Swap( Processor& ) {}
//   };
    
//   typedef std::map<std::pair<uint8_t,uint32_t>,Expr> ForeignRegisters;
  
//   struct CP15Reg
//   {
//     virtual            ~CP15Reg() {}
//     virtual unsigned    RequiredPL() { return 1; }
//     virtual void        Write( Processor& proc, U32 const& value ) { throw Unimplemented(); }
//     virtual U32         Read( Processor& proc ) { throw Unimplemented(); return U32(); }
//     virtual char const* Describe() = 0;
//   };

//   struct Load : public unisim::util::symbolic::binsec::Load
//   {
//     Load( Expr const& addr, unsigned size, unsigned alignment, bool bigendian )
//       : unisim::util::symbolic::binsec::Load(addr, size, alignment, bigendian)
//     {}
//   };
//   //   =====================================================================
//   //   =                      Construction/Destruction                     =
//   //   =====================================================================
  
  

// private:
//   Processor( Processor const& );
// public:
  
   Processor()
     : path(0)
     , reg_values()
//     , next_insn_addr()
//     , branch_type(B_JMP)
//     , cpsr( *this, ref_psr )
//     , spsr( newRegRead(RegID("spsr"), ScalarType::U32) )
//     , sregs()
//     , FPSCR( newRegRead(RegID("fpscr"), ScalarType::U32) )
//     , FPEXC( newRegRead(RegID("fpexc"), ScalarType::U32) )
//     , stores()
//     , unpredictable(false)
//     , is_it_assigned(false)
//     , mode()
//     , foreign_registers()
//     , neonregs()
  {
    // GPR regs
    reg_values[0] = U32(0);
    for (unsigned reg = 1; reg < 32; ++reg)
      reg_values[reg] = U32( newRegRead( RegID("at") + (reg-1), ScalarType::U32 ) );
  }

//   bool close( Processor const& ref, uint32_t linear_nia )
//   {
//     bool complete = path->close();
//     if (branch_type == B_CALL)
//       path->add_sink( new Call( next_insn_addr.expr, linear_nia ) );
//     else
//       path->add_sink( new Goto( next_insn_addr.expr ) );
//     if (unpredictable)
//       {
//         path->add_sink( new unisim::util::symbolic::binsec::AssertFalse() );
//         return complete;
//       }
//     if (cpsr.n != ref.cpsr.n)
//       path->add_sink( newRegWrite( RegID("n"), cpsr.n ) );
//     if (cpsr.z != ref.cpsr.z)
//       path->add_sink( newRegWrite( RegID("z"), cpsr.z ) );
//     if (cpsr.c != ref.cpsr.c)
//       path->add_sink( newRegWrite( RegID("c"), cpsr.c ) );
//     if (cpsr.v != ref.cpsr.v)
//       path->add_sink( newRegWrite( RegID("v"), cpsr.v ) );
//     if (cpsr.itstate.expr != ref.cpsr.itstate.expr)
//       path->add_sink( newRegWrite( RegID("itstate"), cpsr.itstate.expr ) );
//     if (cpsr.bg.expr != ref.cpsr.bg.expr)
//       path->add_sink( newRegWrite( RegID("cpsr"), cpsr.bg.expr ) );
//     if (spsr.expr != ref.spsr.expr)
//       path->add_sink( newRegWrite( RegID("spsr"), spsr.expr ) );
//     for (SRegID reg; reg.next();)
//       if (sregs[reg.idx()].expr != ref.sregs[reg.idx()].expr)
//         path->add_sink( newRegWrite( reg, sregs[reg.idx()].expr ) );
//     if (FPSCR.expr != ref.FPSCR.expr)
//       path->add_sink( newRegWrite( RegID("fpscr"), FPSCR.expr ) );
//     if (FPEXC.expr != ref.FPEXC.expr)
//       path->add_sink( newRegWrite( RegID("fpexc"), FPEXC.expr ) );
//     for (unsigned reg = 0; reg < 15; ++reg)
//       {
//         if (reg_values[reg].expr != ref.reg_values[reg].expr)
//           path->add_sink( newRegWrite( RegID("r0") + reg, reg_values[reg].expr ) );
//       }
//     for (ForeignRegisters::iterator itr = foreign_registers.begin(), end = foreign_registers.end(); itr != end; ++itr)
//       {
//         ForeignRegister ref(itr->first.first, itr->first.second);
//         ref.Retain(); // Prevent deletion of this static instance
//         Expr xref( new ForeignRegister(itr->first.first, itr->first.second) );
//         if (itr->second == Expr(&ref)) continue;
//         std::ostringstream buf;
//         ref.Repr( buf );
//         path->add_sink( newRegWrite( RegID(buf.str().c_str()), itr->second ) );
//       }
//     for (unsigned reg = 0; reg < 32; ++reg)
//       {
//         if (neonregs[reg][0] != ref.neonregs[reg][0])
//           GetNeonSinks(reg);
//       }
//     for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
//       path->add_sink( *itr );
//     return complete;
//   }
  
//   //   =====================================================================
//   //   =                 Internal Instruction Control Flow                 =
//   //   =====================================================================
  
//   void UnpredictableInsnBehaviour() { unpredictable = true; }
  
//   template <typename OP>
//   void UndefinedInstruction( OP* op ) { throw Undefined(); }
    
//   bool concretize( Expr const& cexp )
//   {
//     bool predicate = path->proceed( cexp );
//     path = path->next( predicate );
//     return predicate;
//   }
  
//   template <typename T>
//   bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
//   {
//     if (not cond.expr.good())
//       throw std::logic_error( "Not a valid condition" );

//     Expr cexp( BOOL(cond).expr );
//     if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
//       return cnode->Get( bool() );

//     return concretize( cexp );
//   }
  
//   void FPTrap( unsigned exc )
//   {
//     throw Unimplemented();
//   }
    
//   //   =====================================================================
//   //   =             General Purpose Registers access methods              =
//   //   =====================================================================
    
//   U32  GetGPR( uint32_t id ) { return reg_values[id]; }
  
//   // TODO: interworking branches are not correctly handled
//   void SetGPR_mem( uint32_t id, U32 const& value )
//   {
//     if (id != 15)
//       reg_values[id] = value;
//     else
//       SetNIA( value, B_JMP );
//   }
//   void SetGPR( uint32_t id, U32 const& value ) {
//     if (id != 15)
//       reg_values[id] = value;
//     else
//       SetNIA( value, B_JMP );
//   }
    
//   void SetGPR_usr( uint32_t id, U32 const& value ) { /* system mode */ throw Unimplemented(); }
//   U32  GetGPR_usr( uint32_t id ) { /* system mode */ throw Unimplemented(); return U32(); }
    
//   U32  GetNIA() { return next_insn_addr; }
//   enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
//   void SetNIA( U32 const& nia, branch_type_t bt )
//   {
//     branch_type = bt;
//     next_insn_addr = nia;
//   }

//   Expr& GetForeignRegister( uint8_t foreign_mode, uint32_t idx )
//   {
//     Expr& result = foreign_registers[std::make_pair( foreign_mode, idx )];
//     if (not result.node)
//       result = new ForeignRegister( foreign_mode, idx );
//     return result;
//   }
    
//   U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx )
//   {
//     if ((cpsr.mode == foreign_mode) or
//         (idx < 8) or
//         (idx >= 15) or
//         ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
//         )
//       return GetGPR( idx );
//     return U32( GetForeignRegister( foreign_mode, idx ) );
//   }
    
//   void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value )
//   {
//     if ((cpsr.mode == foreign_mode) or
//         (idx < 8) or
//         (idx >= 15) or
//         ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
//         )
//       return SetGPR( idx, value );
//     GetForeignRegister( foreign_mode, idx ) = value.expr;
//   }
    
//   //   =====================================================================
//   //   =              Special/System Registers access methods              =
//   //   =====================================================================

//   PSR& CPSR() { return cpsr; }
  
//   U32  GetCPSR()                                 { return cpsr.GetBits(); }
//   void SetCPSR( U32 const& bits, uint32_t mask ) { cpsr.SetBits( bits, mask ); }
    
//   U32& SPSR() { throw Unimplemented(); static U32 spsr_dummy; return spsr_dummy; }
  
//   ITCond itcond() const { return ITCond(); }
//   bool itblock() { return Test(cpsr.InITBlock()); }
  
//   void ITSetState( uint32_t cond, uint32_t mask )
//   {
//     cpsr.SetITState( (cond << 4) | mask );
//     is_it_assigned = true;
//   }
  
//   void ITAdvance()
//   {
//     if (is_it_assigned)
//       is_it_assigned = false;
//     else if (itblock())
//       {
//         U8 itstate( cpsr.itstate );
//         itstate = (Test((itstate & U8(7)) != U8(0))) ? ((itstate & U8(-32)) | ((itstate << 1) & U8(31))) : U8(0);
//         cpsr.itstate = itstate;
//       }
//   }
  
//   Mode&  CurrentMode() { /* throw Unimplemented(); */ return mode; }
//   Mode&  GetMode(uint8_t) { throw Unimplemented(); return mode; }
  
//   virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  
//   U32         CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
//   { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Read( *this ); }
//   void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 const& value )
//   { CP15GetRegister( crn, opcode1, crm, opcode2 ).Write( *this, value ); }
//   char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
//   { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Describe(); }

//   //   ====================================================================
//   //   =         Vector and Floating-point Registers access methods       =
//   //   ====================================================================

//   U32 RoundTowardsZeroFPSCR() const
//   {
//     U32 fpscr = FPSCR;
//     unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundTowardsZero) );
//     return fpscr;
//   }
    
//   U32 RoundToNearestFPSCR() const
//   {
//     U32 fpscr = FPSCR;
//     unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundToNearest) );
//     return fpscr;
//   }
    
//   // U32 StandardValuedFPSCR() const   { return AHP.Mask( FPSCR ) | 0x03000000; }
    
//   struct NeonName
//   { // Convenience class for name construction
//     NeonName( unsigned idx ) : begin(&buf[sizeof(buf)]) { _('\0'); do { _('0'+idx%10); idx /= 10; } while (idx); _('d'); }
//     void _(char c) { *--begin = c; } operator char const* () const { return begin; } char buf[4]; char* begin;
//   };
  
//   struct NeonRead : public unisim::util::symbolic::binsec::RegRead
//   {
//     typedef NeonRead this_type;
//     typedef unisim::util::symbolic::binsec::RegRead Super;
//     NeonRead( unsigned _reg ) : Super(), reg(_reg) {}
//     virtual this_type* Mutate() const { return new this_type( *this ); }
//     virtual ScalarType::id_t GetType() const { return ScalarType::U64; }
//     virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg; }
//     virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<NeonRead const&>( rhs ) ); }
//     int compare( this_type const& rhs ) const { return int(reg) - int(rhs.reg); }
    
//     unsigned reg;
//   };

//   struct NeonWrite : public unisim::util::symbolic::binsec::RegWrite
//   {
//     typedef NeonWrite this_type;
//     typedef unisim::util::symbolic::binsec::RegWrite Super;
//     NeonWrite( unsigned _reg, Expr const& value ) : Super(value), reg(_reg) {}
//     virtual this_type* Mutate() const { return new this_type( *this ); }
//     virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
//     virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg; }
//     virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
//     int compare( this_type const& rhs ) const { if (int delta = int(reg) - int(rhs.reg)) return delta; return Super::compare( rhs ); }
    
//     unsigned reg;
//   };

//   struct NeonPartialWrite : public unisim::util::symbolic::binsec::RegWrite
//   {
//     typedef NeonPartialWrite this_type;
//     typedef unisim::util::symbolic::binsec::RegWrite Super;
//     typedef unisim::util::symbolic::binsec::Label Label;
//     typedef unisim::util::symbolic::binsec::Variables Variables;
//     typedef unisim::util::symbolic::binsec::GetCode GetCode;

//     NeonPartialWrite( unsigned _reg, unsigned _beg, unsigned _end, Expr const& _value ) : Super(_value), reg(_reg), beg(_beg), end(_end) {}
//     virtual this_type* Mutate() const { return new this_type( *this ); }
//     virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
//     virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg << '_' << beg << '_' << end; }
//     virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
//     {
//       sink << 'd' << std::dec << reg << '{' << beg << ',' << end << '}' << " := " << GetCode(value, vars, label);
//       return 0;
//     }
//     virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
//     int compare( this_type const& rhs ) const
//     {
//       if (int delta = int(reg) - int(rhs.reg)) return delta;
//       if (int delta = int(beg) - int(rhs.beg)) return delta;
//       if (int delta = int(end) - int(rhs.end)) return delta;
//       return Super::compare( rhs );
//     }
    
//     unsigned reg, beg, end;
//   };
  
//   static unsigned const NEONSIZE = 8;

//   void
//   GetNeonSinks( unsigned reg )
//   {
//     using unisim::util::symbolic::binsec::BitFilter;
//     // using unisim::util::symbolic::make_const;
    
//     { // Check for constant values
//       Expr dr = unisim::util::symbolic::binsec::ASExprNode::Simplify( GetVDU(reg).expr );
//       if (dr.ConstSimplify())
//         {
//           path->add_sink( new NeonWrite( reg, dr ) );
//           return;
//         }
//     }

//     // Check for monolithic value
//     if (not neonregs[reg][NEONSIZE/2].node)
//       {
//         path->add_sink( new NeonWrite( reg, eneonread(reg,NEONSIZE,0) ) );
//         return;
//       }
    
//     // Requested read is a concatenation of multiple source values
//     struct _
//     {
//       _( Processor& _core, unsigned _reg ) : core(_core), reg(_reg) { Process( 0, NEONSIZE ); } Processor& core; unsigned reg;
//       void Process( unsigned pos, unsigned size )
//       {
//         unsigned half = size / 2, mid = pos+half;
//         if (size > 1 and core.neonregs[reg][mid].node)
//           {
//             Process( pos, half );
//             Process( mid, half );
//           }
//         else
//           {
//             unsigned begin = pos*8, end = begin+size*8;
//             Expr value( new BitFilter( core.eneonread(reg,size,pos), 64, size*8, size*8, false ) );
//             core.path->add_sink( new NeonPartialWrite( reg, begin, end, value ) );
//           }
//       }
//     } concat( *this, reg );
//   }
 
//   Expr eneonread( unsigned reg, unsigned size, unsigned pos )
//   {
//     using unisim::util::symbolic::ExprNode;
//     using unisim::util::symbolic::make_const;
    
//     struct
//     {
//       Expr ui( unsigned sz, Expr const& src ) const
//       {
//         switch (sz) {
//         default: throw 0;
//         case 1: return new unisim::util::symbolic::CastNode<uint8_t,uint64_t>( src );
//         case 2: return new unisim::util::symbolic::CastNode<uint16_t,uint64_t>( src );
//         case 4: return new unisim::util::symbolic::CastNode<uint32_t,uint64_t>( src );
//         case 8: return new unisim::util::symbolic::CastNode<uint64_t,uint64_t>( src );
//         }
//         return 0;
//       }
//     } cast;
    
//     if (not neonregs[reg][pos].node)
//       {
//         // requested read is in the middle of a larger value
//         unsigned src = pos;
//         do { src = src & (src-1); } while (not neonregs[reg][src].node);
//         unsigned shift = 8*(pos - src);
//         return cast.ui( size, make_operation( "Lsr", neonregs[reg][src], make_const( shift ) ) );
//       }
//     else if (not neonregs[reg][(pos|size)&(NEONSIZE-1)].node)
//       {
//         // requested read is in lower bits of a larger value
//         return cast.ui( size, neonregs[reg][pos] );
//       }
//     else if ((size > 1) and (neonregs[reg][pos|(size >> 1)].node))
//       {
//         // requested read is a concatenation of multiple source values
//         Expr concat = cast.ui( size, neonregs[reg][pos] );
//         for (unsigned idx = 0; ++idx < size;)
//           {
//             if (not neonregs[reg][pos+idx].node)
//               continue;
//             concat = make_operation( "Or", make_operation( "Lsl", cast.ui( size, neonregs[reg][idx] ), make_const( 8*idx ) ), concat );
//           }
//         return concat;
//       }
    
//     // requested read is directly available
//     return neonregs[reg][pos];
//   }
  
//   void eneonwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr )
//   {
//     Expr nxt[NEONSIZE];
    
//     for (unsigned ipos = pos, isize = size, cpos;
//          cpos = (ipos^isize) & (NEONSIZE-1), (not neonregs[reg][ipos].node) or (not neonregs[reg][cpos].node);
//          isize *= 2, ipos &= -isize
//          )
//       {
//         nxt[cpos] = eneonread( reg, isize, cpos );
//       }
    
//     for (unsigned ipos = 0; ipos < NEONSIZE; ++ipos)
//       {
//         if (nxt[ipos].node)
//           neonregs[reg][ipos] = nxt[ipos];
//       }
    
//     neonregs[reg][pos] = xpr;
    
//     for (unsigned rem = 1; rem < size; ++rem)
//       {
//         neonregs[reg][pos+rem] = 0;
//       }
//   }

//   U32  GetVSU( unsigned idx ) { return U32( U64( eneonread( idx / 2, 4, (idx*4) & 4 ) ) ); }
//   void SetVSU( unsigned idx, U32 val ) { eneonwrite( idx / 2, 4, (idx*4) & 4, U64(val).expr ); }
//   U64  GetVDU( unsigned idx ) { return U64( eneonread( idx, 8, 0 ) ); }
//   void SetVDU( unsigned idx, U64 val ) { eneonwrite( idx, 8, 0, U64(val).expr ); }
//   F32  GetVSR( unsigned idx ) { return F32(); }
//   void SetVSR( unsigned idx, F32 val ) {}
//   F64  GetVDR( unsigned idx ) { return F64(); }
//   void SetVDR( unsigned idx, F64 val ) {}

//   static unsigned usizeof( U8 const& )  { return  1; }
//   static unsigned usizeof( U16 const& ) { return  2; }
//   static unsigned usizeof( U32 const& ) { return  4; }
//   static unsigned usizeof( U64 const& ) { return  8; }

//   template <typename T> T ucast( T const& x ) { return x; }
//   U8 ucast( S8 const& x ) { return U8(x); }
//   U16 ucast( S16 const& x ) { return U16(x); }
//   U32 ucast( S32 const& x ) { return U32(x); }
//   U64 ucast( S64 const& x ) { return U64(x); }
//   // Get|Set elements
//   template <class ELEMT>
//   void
//   SetVDE( unsigned reg, unsigned idx, ELEMT const& value )
//   {
//     using unisim::util::symbolic::binsec::BitFilter;
//     auto uvalue = ucast( value );
//     unsigned usz = usizeof( uvalue );
//     Expr neonval( new BitFilter( uvalue.expr, usz*8, usz*8, 64, false ) );
//     eneonwrite( reg, usz, usz*idx, neonval );
//   }

//   template <class ELEMT>
//   ELEMT GetVDE( unsigned reg, unsigned idx, ELEMT const& trait )
//   {
//     unsigned usz = usizeof( ucast(trait) );
//     return ELEMT( U64( eneonread( reg, usz, usz*idx ) ) );
//   }
  
//   //   =====================================================================
//   //   =                      Control Transfer methods                     =
//   //   =====================================================================

//   void BranchExchange( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
//   void Branch( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
    
//   void WaitForInterrupt() { throw Unimplemented(); }
//   void SWI( uint32_t imm ) { throw Unimplemented(); }
//   void BKPT( uint32_t imm ) { throw Unimplemented(); }
//   void CallSupervisor( uint32_t imm ) { throw Unimplemented(); }
//   bool IntegerZeroDivide( BOOL const& condition ) { return false; }
  
//   //   =====================================================================
//   //   =                       Memory access methods                       =
//   //   =====================================================================
  
//   U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 0, false ) ) ); }
//   U16  MemURead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 0, false ) ) ); }
//   U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 2, false ) ) ); }
//   U16  MemRead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 1, false ) ) ); }
//   U8   MemRead8( U32 const& addr ) { return U8( Expr( new Load( addr.expr, 0, 0, false ) ) ); }
  
//   void MemUWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 0, false ) ); }
//   void MemUWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 0, false ) ); }
//   void MemWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 2, false ) ); }
//   void MemWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 1, false ) ); }
//   void MemWrite8( U32 const& addr, U8 const& value ) { stores.insert( new Store( addr.expr, value.expr, 0, 0, false ) ); }
    
    
  struct RegID : public unisim::util::identifier::Identifier<RegID>
  {
    enum Code
      {
       NA = 0, at, v0, v1, a0, a1, a2, a3,
       t0, t1, t2, t3, t4, t5, t6, t7,
       s0, s1, s2, s3, s4, s5, s6, s7,
       t8, t9, k0, k1, gp, sp, fp, ra,
       end
      } code;

    char const* c_str() const
    {
      switch (code)
        {
        case at: return "at";
        case v0: return "  v0";
        case v1: return " v1";
        case a0: return " a0";
        case a1: return " a1";
        case a2: return " a2";
        case a3: return " a3";
        case t0: return "        t0";
        case t1: return " t1";
        case t2: return " t2";
        case t3: return " t3";
        case t4: return " t4";
        case t5: return " t5";
        case t6: return " t6";
        case t7: return " t7";
        case s0: return "        s0";
        case s1: return " s1";
        case s2: return " s2";
        case s3: return " s3";
        case s4: return " s4";
        case s5: return " s5";
        case s6: return " s6";
        case s7: return " s7";
        case t8: return "        t8";
        case t9: return " t9";
        case k0: return " k0";
        case k1: return " k1";
        case gp: return " gp";
        case sp: return " sp";
        case fp: return " fp";
        case ra: return " ra";
        case NA:
        case end: break;
        }
      return "NA";
    }
      
    RegID() : code(end) {}
    RegID( Code _code ) : code(_code) {}
    RegID( char const* _code ) : code(end) { init( _code ); }
  };

  ActionNode*      path;
  U32              reg_values[32];
  U32              insn_addrs[3];
//   U32              next_insn_addr;
//   branch_type_t    branch_type;
//   PSR              cpsr;
//   U32              spsr;
//   U32              sregs[SRegID::end];
//   U32              FPSCR, FPEXC;
//   std::set<Expr>   stores;
//   bool             unpredictable;
//   bool             is_it_assigned; /* determines wether current instruction is an IT one. */
//   Mode             mode;
//   ForeignRegisters foreign_registers;
//   Expr             neonregs[32][NEONSIZE];
};

;

struct NextInstructionAddress : public unisim::util::symbolic::binsec::ASExprNode
{
  NextInstructionAddress() {}
  virtual NextInstructionAddress* Mutate() const override { return new NextInstructionAddress( *this ); }
  virtual unsigned SubCount() const override { return 0; }
  virtual unisim::util::symbolic::ScalarType::id_t GetType() const override { return unisim::util::symbolic::ScalarType::U32; }
  virtual void Repr( std::ostream& sink ) const { sink << "nia"; }
  virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<NextInstructionAddress const&>( rhs ) ); }
  int compare( NextInstructionAddress const& rhs ) const { return 0; }
  typedef unisim::util::symbolic::binsec::Label Label;
  typedef unisim::util::symbolic::binsec::Variables Variables;
  typedef unisim::util::symbolic::binsec::GetCode GetCode;
  virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const { sink << "nia"; return 32; }
};

struct Translator
{
  typedef unisim::util::symbolic::binsec::ActionNode ActionNode;
  
  Translator( uint32_t _addr, uint32_t _code )
    : addr(_addr), code(_code), coderoot(new ActionNode)
  {}
  ~Translator() { delete coderoot; }
  
  template <class ISA>
  void
  extract( std::ostream& sink, ISA& isa )
  {
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(4, addr) << ")\n";
  
    // Instruction decoding
    struct Instruction
    {
      typedef typename ISA::Operation Operation;
      Instruction(ISA& isa, uint32_t addr, uint32_t code)
        : operation(0), bytecount(0)
      {
        operation = isa.NCDecode( addr, ISA::mkcode( code ) );
        unsigned bitlength = operation->GetLength(); 
        if ((bitlength != 32) and ((bitlength != 16) or not ISA::is_thumb))
          { delete operation; operation = 0; }
        bytecount = bitlength/8;
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }
      
      Operation* operation;
      unsigned   bytecount;
    };
    
    Instruction instruction( isa, addr, code );
    
    {
      uint32_t encoding = instruction->GetEncoding();
      if (instruction.bytecount == 2)
        encoding &= 0xffff;
      
      sink << "(opcode . " << unisim::util::symbolic::binsec::dbx(instruction.bytecount, encoding) << ")\n(size . " << (instruction.bytecount) << ")\n";
    }
    
    Processor::U32      insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    // Processor::U32      insn_addr = Expr(new InstructionAddress()); //< symbolic instruction address
    Processor reference;
    
    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( reference, sink ); }
    catch (...) { sink << "(bad)"; }
    sink << "\")\n";
    
    // Get actions
    for (bool end = false; not end;)
      {
        Processor state;
        state.path = coderoot;
        // Fetch
        uint32_t insn_addr = instruction->GetAddr(), nia = insn_addr + instruction.bytecount;
        state.insn_addrs[0] = Processor::U32(insn_addr);
        state.insn_addrs[1] = Processor::U32(new NextInstructionAddress());
        state.insn_addrs[2] = state.insn_addrs[1] + Processor::U32(4);
        // Execute
        instruction->execute( state );
        // Commit
        end = state.close( reference, nia );
      }
    coderoot->simplify();
    coderoot->commit_stats();
  }

  void translate( std::ostream& sink )
  {
    try
      {
        if      (status.iset == status.Arm)
          {
            ARMISA armisa;
            extract( sink, armisa );
          }
        else if (status.iset == status.Thumb)
          {
            THUMBISA thumbisa;
            extract( sink, thumbisa );
          }
        else
          throw 0;
      }
    catch (Processor::Undefined const&)
      {
        sink << "(undefined)\n";
        return;
      }
    catch (...)
      {
        sink << "(unimplemented)\n";
        return;
      }

    // Translate to DBA
    unisim::util::symbolic::binsec::Program program;
    program.Generate( coderoot );
    typedef unisim::util::symbolic::binsec::Program::const_iterator Iterator;
    for (Iterator itr = program.begin(), end = program.end(); itr != end; ++itr)
      sink << "(" << unisim::util::symbolic::binsec::dbx(4, addr) << ',' << itr->first << ") " << itr->second << std::endl;
  }

  uint32_t    addr, code;
  ActionNode* coderoot;
};
  
uint32_t getu32( uint32_t& res, char const* arg )
{
  char *end;
  uint32_t tmp = strtoul( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

char const* usage()
{
  return
    "usage: <program> arm|thumb <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc != 3)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }

  uint32_t addr, code;

  if (not getu32(addr, argv[1]) or not getu32(code, argv[2]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  Translator actset( addr, code );
  
  actset.translate( std::cout );
  
  return 0;
}

Processor::CP15Reg&
Processor::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
    {
      /****************************
       * Identification registers *
       ****************************/
    case CP15ENCODE( 0, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CTR, Cache Type Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ctr"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 0, 0, 5 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "MPIDR, Multiprocessor Affinity Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("mpidr"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 0, 1, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ID_PFR0, Processor Feature Register 0"; }
          U32 Read( Processor& proc ) { return proc.SReg("id_pfr0"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 1, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CCSIDR, Cache Size ID Registers"; }
          U32 Read( Processor& proc ) { return proc.SReg("ccsidr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 0, 1, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CLIDR, Cache Level ID Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("clidr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 0, 2, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CSSELR, Cache Size Selection Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("csselr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("csselr") = value; }
        } x;
        return x;
      } break;
      
      /****************************
       * System control registers *
       ****************************/
    case CP15ENCODE( 1, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "SCTLR, System Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("sctlr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("sctlr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ACTLR, Auxiliary Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("actlr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("actlr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CPACR, Coprocessor Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("cpacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("cpacr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 1, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "NSACR, Non-Secure Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("nsacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("nsacr") = value; }
        } x;
        return x;
      } break;

      /*******************************************
       * Memory protection and control registers *
       *******************************************/
    case CP15ENCODE( 2, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR0, Translation Table Base Register 0"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbr0") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbr0"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 2, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR1, Translation Table Base Register 1"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbr1") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbr1"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 2, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBCR, Translation Table Base Control Register"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbcr") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbcr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 3, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DACR, Domain Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dacr") = value; }
        } x;
        return x;
      } break;


      /*********************************
       * Memory system fault registers *
       *********************************/
    case CP15ENCODE( 5, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DFSR, Data Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dfsr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dfsr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 5, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFSR, Instruction Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ifsr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ifsr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 6, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DFAR, Data Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dfar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dfar") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 6, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFAR, Instruction Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ifar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ifar") = value; }
        } x;
        return x;
      } break;

      /***************************************************************
       * Cache maintenance, address translation, and other functions *
       ***************************************************************/
          
    case CP15ENCODE( 7, 0, 1, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLUIS, Invalidate all instruction caches to PoU Inner Shareable"; }
          U32 Read( Processor& proc ) { return proc.SReg("icialluis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("icialluis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 1, 6 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "BPIALLIS, Invalidate all branch predictors Inner Shareable"; }
          U32 Read( Processor& proc ) { return proc.SReg("bpiallis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("bpiallis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("iciallu"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("iciallu") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIMVAU, Clean data* cache line by MVA to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("icimvau"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("icimvau") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 6 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "BPIALL, Invalidate all branch predictors"; }
          U32 Read( Processor& proc ) { return proc.SReg("bpiall"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("bpiall") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 6, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCIMVAC, Invalidate data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dcimvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dcimvac") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 6, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCISW, Invalidate data* cache line by set/way"; }
          U32 Read( Processor& proc ) { return proc.SReg("dcisw"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dcisw") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 7, 0, 10, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAC, Clean data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccmvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccmvac") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 10, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCSW, Clean data* cache line by set/way"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccsw"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccsw") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 7, 0, 11, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAU, Clean data* cache line by MVA to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccmvau"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccmvau") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 14, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCIMVAC, Clean and invalidate data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccimvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccimvac") = value; }
        } x;
        return x;
      } break;
          
      /******************************
       * TLB maintenance operations *
       ******************************/

    case CP15ENCODE( 8, 0, 3, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALLIS, Invalidate entire TLB Inner Shareable"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiallis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiallis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 8, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiall"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiall") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 8, 0, 7, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIASID, invalidate unified TLB by ASID match"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiasid"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiasid") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 12, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "VBAR, Vector Base Address Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("vbar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("vbar") = value; }
        } x;
        return x;
      } break;
          
      /***********************************/
      /* Context and thread ID registers */
      /***********************************/

    case CP15ENCODE( 13, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CONTEXTIDR, Context ID Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("contextidr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("contextidr") = value; }
        } x;
        return x;
      } break;

      /* BOARD specific */
          
    case CP15ENCODE( 15, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DIAGCR, undocumented Diagnostic Control register"; }
          U32 Read( Processor& proc ) { return proc.SReg("diagcr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("diagcr") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 15, 4, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CBAR, Configuration Base Address"; }
          U32 Read( Processor& proc ) { return proc.SReg("cbar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("cbar") = value; }
        } x;
        return x;
      } break;

    }

  static struct CP15Error : public CP15Reg {
    char const* Describe() { return "Unknown CP15 register"; }
  } err;
  return err;
}

void
Processor::PSR::Set( NZCVRF const& _, U32 const& value )
{
  n = BOOL( unisim::component::cxx::processor::arm::RegisterField< 3,1>().Get( value ) ).expr;
  z = BOOL( unisim::component::cxx::processor::arm::RegisterField< 2,1>().Get( value ) ).expr;
  c = BOOL( unisim::component::cxx::processor::arm::RegisterField< 1,1>().Get( value ) ).expr;
  v = BOOL( unisim::component::cxx::processor::arm::RegisterField< 0,1>().Get( value ) ).expr;
}

Processor::U32
Processor::PSR::GetBits()
{
  return
    (U32(BOOL(n)) << 31) |
    (U32(BOOL(z)) << 30) |
    (U32(BOOL(c)) << 29) |
    (U32(BOOL(v)) << 28) |
    (U32(itstate >> U8(2)) << 10) | (U32(itstate & U8(0b11)) << 25) |
    U32((uint32_t(GetJ()) << 24) | (uint32_t(GetT()) << 5) | uint32_t(mode)) |
    bg;
}
      
void
Processor::PSR::SetBits( U32 const& bits, uint32_t mask )
{
  if (NRF().Get(mask)) { n = BOOL( NRF().Get(bits) ).expr; NRF().Set(mask, 0u); }
  if (ZRF().Get(mask)) { z = BOOL( ZRF().Get(bits) ).expr; ZRF().Set(mask, 0u); }
  if (CRF().Get(mask)) { c = BOOL( CRF().Get(bits) ).expr; CRF().Set(mask, 0u); }
  if (VRF().Get(mask)) { v = BOOL( VRF().Get(bits) ).expr; VRF().Set(mask, 0u); }
        
  if (ITHIRF().Get(mask) or ITLORF().Get(mask))
    {
      itstate = U8((ITHIRF().Get(bits) << 2) | ITLORF().Get(bits));
      uint32_t itmask = ITHIRF().getmask<uint32_t>() | ITLORF().getmask<uint32_t>();
      if ((mask & itmask) != itmask)
        throw 0;
      mask &= ~itmask;
      ITHIRF().Set(mask, 0u); ITLORF().Set(mask, 0u);
    }
        
  if (MRF().Get(mask))
    {
      if (MRF().Get(mask) != 0x1f)
        throw 0;
      U32       nmode = MRF().Get(bits);
      MRF().Set(mask, 0u);
      if (proc.Test(nmode != U32(mode)))
        proc.UnpredictableInsnBehaviour();
    }
        
  if (JRF().Get(mask)) { if (proc.Test(JRF().Get(bits) != U32(GetJ())))    { proc.UnpredictableInsnBehaviour(); } JRF().Set(mask, 0u); }
  if (TRF().Get(mask)) { if (proc.Test(TRF().Get(bits) != U32(GetT())))    { proc.UnpredictableInsnBehaviour(); } TRF().Set(mask, 0u); }
  if (ERF().Get(mask)) { if (proc.Test(ERF().Get(bits) != U32(bigendian))) { proc.UnpredictableInsnBehaviour(); } ERF().Set(mask, 0u); }
        
  bg = (bg & U32(~mask)) | (bits & U32(mask));
}
