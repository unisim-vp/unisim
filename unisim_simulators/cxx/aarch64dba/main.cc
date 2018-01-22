#include <unisim/util/symbolic/symbolic.hh>
#include <aarch64dec.tcc>
#include <iostream>
#include <iomanip>
#include <cstdlib>

namespace armsec
{
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

  struct dbx
  {
    dbx( unsigned _bits, uint64_t _value ) : value(_value), bits(_bits) {} uint64_t value; unsigned bits;
    friend std::ostream& operator << ( std::ostream& sink, dbx const& _ )
    {
      sink << "0x" << std::hex << std::setw(_.bits/4) << std::setfill('0') << _.value << std::dec;
      return sink;
    }
  };
  
  struct State
  {
    typedef armsec::F64  F64;
    typedef armsec::F32  F32;
    typedef armsec::BOOL BOOL;
    typedef armsec::U8   U8;
    typedef armsec::U16  U16;
    typedef armsec::U32  U32;
    typedef armsec::U64  U64;
    typedef armsec::S8   S8;
    typedef armsec::S16  S16;
    typedef armsec::S32  S32;
    typedef armsec::S64  S64;
    
    typedef unisim::util::symbolic::FP         FP;
    typedef unisim::util::symbolic::Expr       Expr;
    typedef unisim::util::symbolic::ActionNode ActionNode;

    template <typename OPER>
    void UndefinedInstruction(OPER const*) { throw unisim::component::cxx::processor::arm::Reject(); }
    
    template <typename T>
    bool Cond( unisim::util::symbolic::SmartValue<T> const& cond )
    {
      if (not cond.expr.good())
        throw std::logic_error( "Not a valid condition" );
      
      Expr cexp( BOOL(cond).expr );
      if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
        return cnode->GetBoolean();
      
      bool predicate = path->proceed( cexp );
      path = path->next( predicate );
      return predicate;
    }

    //   =====================================================================
    //   =             General Purpose Registers access methods              =
    //   =====================================================================
    
    U64  GetGSR(unsigned id) const { return U64(0); }
    void SetGSR(unsigned id, U64 const& val) {}
    void SetGSR(unsigned id, U32 const& val) { this->SetGSR(id,U64(val)); }
    U64  GetGZR(unsigned id) const { return U64(0); }
    void SetGZR(unsigned id, U64 const& val) {}
    void SetGZR(unsigned id, U32 const& val) { this->SetGZR(id,U64(val)); }
    void SetGZR(unsigned id, U16 const& val) { this->SetGZR(id,U64(val)); }
    void SetGZR(unsigned id, U8 const& val) { this->SetGZR(id,U64(val)); }
  
    //   ====================================================================
    //   =                 Vector  Registers access methods                 
    //   ====================================================================

    //   U8  GetVU8 ( unsigned reg, unsigned sub ) { return VectorStorage<U8> (reg)[sub]; }
    //   U16 GetVU16( unsigned reg, unsigned sub ) { return VectorStorage<U16>(reg)[sub]; }
    //   U32 GetVU32( unsigned reg, unsigned sub ) { return VectorStorage<U32>(reg)[sub]; }
    //   U64 GetVU64( unsigned reg, unsigned sub ) { return VectorStorage<U64>(reg)[sub]; }
    //   S8  GetVS8 ( unsigned reg, unsigned sub ) { return VectorStorage<S8> (reg)[sub]; }
    //   S16 GetVS16( unsigned reg, unsigned sub ) { return VectorStorage<S16>(reg)[sub]; }
    //   S32 GetVS32( unsigned reg, unsigned sub ) { return VectorStorage<S32>(reg)[sub]; }
    //   S64 GetVS64( unsigned reg, unsigned sub ) { return VectorStorage<S64>(reg)[sub]; }
  
    //   void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { VectorStorage<U8> (reg)[sub] = value; }
    //   void SetVU16( unsigned reg, unsigned sub, U16 value ) { VectorStorage<U16>(reg)[sub] = value; }
    //   void SetVU32( unsigned reg, unsigned sub, U32 value ) { VectorStorage<U32>(reg)[sub] = value; }
    //   void SetVU64( unsigned reg, unsigned sub, U64 value ) { VectorStorage<U64>(reg)[sub] = value; }
    //   void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { VectorStorage<S8> (reg)[sub] = value; }
    //   void SetVS16( unsigned reg, unsigned sub, S16 value ) { VectorStorage<S16>(reg)[sub] = value; }
    //   void SetVS32( unsigned reg, unsigned sub, S32 value ) { VectorStorage<S32>(reg)[sub] = value; }
    //   void SetVS64( unsigned reg, unsigned sub, S64 value ) { VectorStorage<S64>(reg)[sub] = value; }
  
    //   void SetVU8 ( unsigned reg, U8 value )  { VectorZeroedStorage<U8> (reg)[0] = value; }
    //   void SetVU16( unsigned reg, U16 value ) { VectorZeroedStorage<U16>(reg)[0] = value; }
    //   void SetVU32( unsigned reg, U32 value ) { VectorZeroedStorage<U32>(reg)[0] = value; }
    //   void SetVU64( unsigned reg, U64 value ) { VectorZeroedStorage<U64>(reg)[0] = value; }
    //   void SetVS8 ( unsigned reg, S8 value )  { VectorZeroedStorage<S8> (reg)[0] = value; }
    //   void SetVS16( unsigned reg, S16 value ) { VectorZeroedStorage<S16>(reg)[0] = value; }
    //   void SetVS32( unsigned reg, S32 value ) { VectorZeroedStorage<S32>(reg)[0] = value; }
    //   void SetVS64( unsigned reg, S64 value ) { VectorZeroedStorage<S64>(reg)[0] = value; }
  
    //   void ClearHighV( unsigned reg, unsigned bytes ) { for (unsigned idx = bytes; idx < VUnion::BYTECOUNT; idx+=1 ) vector_data[reg][idx] = 0; }

    //   =====================================================================
    //   =              Special/System Registers access methods              =
    //   =====================================================================

    void SetNZCV(BOOL const& N, BOOL const& Z, BOOL const& C, BOOL const& V) {}
    U8   GetNZCV() const { return U8(0); }
    BOOL GetCarry() const { return BOOL(0); }
    
    U64  GetPC() const { return U64(0); }
    U64  GetNPC() const { return U64(0); }

  
    void        CheckSystemAccess( uint8_t op1 ) { throw 0; }
    U64         ReadSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 ) { throw 0; return U64(); }
    void        WriteSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, U64 value ) { throw 0; }
    char const* DescribeSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 ) { throw 0; return "???"; }
    char const* NameSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 ) { throw 0; return "???"; }
    //   char const* NameSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  
    //   =====================================================================
    //   =                      Control Transfer methods                     =
    //   =====================================================================
  
    // Set the next Program Counter
    enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
    void BranchTo(U64 const& npc, branch_type_t branch_type) const {}
  
    void CallSupervisor( uint16_t imm ) { throw 0;  }
  
    //   =====================================================================
    //   =                       Memory access methods                       =
    //   =====================================================================
    
    // template <typename T>
    // T
    // MemReadT(U64 addr)
    // {
    //   unsigned const size = sizeof (T);
    //   U8 buffer[size];
    //   MemRead( &buffer[0], addr, size );
    //   T res(0);
    //   for (unsigned idx = size; --idx < size; )
    //     res = (res << 8) | T(buffer[idx]);
    //   return res;
    // }

    U64 MemRead64(U64 addr) { return U64(0); }
    U32 MemRead32(U64 addr) { return U32(0); }
    U16 MemRead16(U64 addr) { return U16(0); }
    U8  MemRead8 (U64 addr) { return U8 (0); }
    
    // void MemRead( U8* buffer, U64 addr, unsigned size );
  
    // template <typename T>
    // void
    // MemWriteT(U64 addr, T val)
    // {
    //   unsigned const size = sizeof (T);
    //   U8 buffer[size];
    //   for (unsigned idx = 0; idx < size; ++idx)
    //     { buffer[idx] = val; val >>= 8; }
    //   MemWrite( addr, &buffer[0], size );
    // }
    
    void MemWrite64(U64 addr, U64 val) {  }
    void MemWrite32(U64 addr, U32 val) {  }
    void MemWrite16(U64 addr, U16 val) {  }
    void MemWrite8 (U64 addr, U8  val) {  }
    
    // void MemWrite( U64 addr, U8 const* buffer, unsigned size );

    void ClearExclusiveLocal() { throw 0; }
    void SetExclusiveMonitors( U64, unsigned size ) { throw 0; }
    bool     ExclusiveMonitorsPass( U64 addr, unsigned size ) { throw 0; }
  
  //   virtual SysReg&  GetSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  //   virtual void     ResetSystemRegisters();
    
    ActionNode* path;
  };
}

struct Decoder : public unisim::component::cxx::processor::arm::isa::arm64::Decoder<armsec::State>
{
  typedef unisim::component::cxx::processor::arm::isa::arm64::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<armsec::State> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }

  void
  translate( uint32_t addr, uint32_t code/*, unsigned running_mode*/ )
  {
    std::cout << "(address . " << armsec::dbx( 32, addr ) << ")\n";

    // struct Translation
    // {
    //   typedef typename ISA::Operation Insn;
    //   typedef typename unisim::util::symbolic::PathNode PathNode;
    //   typedef typename unisim::util::symbolic::Expr Expr;
    //   typedef typename armsec::State State;

    //   ~Translation() { delete insn; delete coderoot; }
    //   Translation( uint32_t _addr, bool bigendian, unsigned running_mode )
    //     : insn(0)
    //     , coderoot(new armsec::PathNode)
    //     , addr( _addr )
    //     , reference( coderoot, ISA::iset, bigendian, running_mode )
    //   {}

    //   int
    //   decode( ISA& isa, uint32_t _code )
    //   {
    //     try { insn = isa.NCDecode( addr, ISA::mkcode( _code ) ); }

    //     catch (unisim::component::cxx::processor::arm::Reject const&) { return 0; }

    //     return insn->GetLength();
    //   }

    //   void
    //   disasm( std::ostream& sink )
    //   {
    //     try { insn->disasm( reference, sink ); }

    //     catch (...) { sink << "(bad)"; }
    //   }

    //   bool
    //   Generate( armsec::Label::Program& program )
    //   {
    //     // Expr insn_addr( new InstructionAddress ); //< symbolic instruction address
    //     Expr insn_addr( unisim::util::symbolic::make_const( addr ) ); //< concrete instruction address

    //     reference.SetInsnProps( insn_addr, insn->GetLength() );

    //     try
    //       {
    //         for (bool end = false; not end;)
    //           {
                
    //             State state( coderoot, ISA::iset, reference.cpsr.bigendian, reference.cpsr.mode );
    //             state.SetInsnProps( insn_addr, insn->GetLength() );
    //             insn->execute( state );
    //             if (ISA::is_thumb)
    //               state.ITAdvance();
    //             end = state.close( reference );
    //           }
    //       }
    //     catch (...)
    //       {
    //         return false;
    //       }

    //     coderoot->factorize();
    //     coderoot->remove_dead_paths();

    //     armsec::Label beglabel(program), endlabel(program);
    //     beglabel.allocate();
    //     coderoot->GenCode( beglabel, endlabel, 0 );

    //     return true;
    //   }

    //   Insn*     insn;
    //   PathNode* coderoot;
    //   uint32_t  addr;
    //   State     reference;
    // } trans( addr, bigendian, running_mode );

    // unsigned bitlen = trans.decode( isa, code );
    // if (bitlen != 16 and bitlen != 32)
    //   {
    //     std::cout << "(opcode . " << armsec::dbx( 32, code ) << ")\n(illegal)\n";
    //     return;
    //   }

    // uint32_t encoding =  trans.insn->GetEncoding();
    // if (bitlen == 16)
    //   encoding &= 0xffff;
    
    // std::cout << "(opcode . " << armsec::dbx( bitlen, encoding ) << ")\n(size . " << (bitlen/8) << ")\n";

    // // std::cout << "(int_name,\"" << trans.insn->GetName() << "\")\n";

    // std::cout << "(mnemonic . \""; trans.disasm( std::cout ); std::cout << "\")\n";

    // armsec::Label::Program program;

    // if (trans.Generate( program ))
    //   {
    //     typedef armsec::Label::Program::const_iterator Iterator;
    //     for (Iterator itr = program.begin(), end = program.end(); itr != end; ++itr)
    //       std::cout << "(" << armsec::dbx( 32, addr ) << ',' << itr->first << ") " << itr->second << std::endl;
    //   }
    // else
    //   {
    //     std::cout << "(unimplemented)\n";
    //   }
  }};

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
    "usage: <program> [<flags>] <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc < 3)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }
  
  uint32_t addr, code;

  if (not getu32(addr, argv[argc-2]) or not getu32(code, argv[argc-1]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  Decoder decoder;
  
  decoder.translate( addr, code/*, running_mode*/ );

  return 0;
}
