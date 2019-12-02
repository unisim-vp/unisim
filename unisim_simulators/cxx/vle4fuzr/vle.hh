/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */


#ifndef __VLE4FUZR_VLE_HH__
#define __VLE4FUZR_VLE_HH__

#include <emu.hh>
#include <unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh>
#include <unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh>
#include <unisim/component/cxx/processor/powerpc/disasm.hh>
#include <unisim/component/cxx/processor/powerpc/cpu.hh>
//#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <map>

typedef Processor EmuProcessor;

namespace vle {
namespace concrete {

  struct Operation;

  typedef bool  BOOL;
  typedef uint8_t   U8;
  typedef uint16_t  U16;
  typedef uint32_t  U32;
  typedef uint64_t  U64;
  typedef int8_t    S8;
  typedef int16_t   S16;
  typedef int32_t   S32;
  typedef int64_t   S64;

  typedef uint32_t  UINT;
  typedef int32_t   SINT;
  typedef uint32_t  ADDRESS;

  struct XER
  {
    typedef unisim::component::cxx::processor::powerpc::XER::OV   OV;
    typedef unisim::component::cxx::processor::powerpc::XER::SO   SO;
    typedef unisim::component::cxx::processor::powerpc::XER::CA   CA;
    typedef unisim::component::cxx::processor::powerpc::XER::_0_3 _0_3;

    struct TODO {};
    template <typename PART> void Set( U32 value ) { throw TODO(); }
    template <typename PART> U32 Get() { throw TODO(); return U32(); }
    operator U32 () { throw TODO(); return U32(); }
    XER& operator= ( U32 value ) { throw TODO(); return *this; }
    XER& GetXER() { return *this; }
    
    XER() {}
  };

  struct CR
  {
    typedef unisim::component::cxx::processor::powerpc::CR::CR0 CR0;
    typedef unisim::component::cxx::processor::powerpc::CR::CR1 CR1;
    typedef unisim::component::cxx::processor::powerpc::CR::CR2 CR2;
    typedef unisim::component::cxx::processor::powerpc::CR::CR3 CR3;
    typedef unisim::component::cxx::processor::powerpc::CR::CR4 CR4;
    typedef unisim::component::cxx::processor::powerpc::CR::CR5 CR5;
    typedef unisim::component::cxx::processor::powerpc::CR::CR6 CR6;
    typedef unisim::component::cxx::processor::powerpc::CR::CR7 CR7;

    struct TODO {};
    template <typename PART> void Set( uint32_t value ) { throw TODO(); }
    template <typename PART> U32 Get() { throw TODO(); return U32(); }
    operator U32 () { throw TODO(); return U32(); }
    CR& operator= ( U32 const& value ) { throw TODO(); return *this; }
    CR& GetCR() { return *this; }
    
    CR() {}
  };
  
  struct LR
  {
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(); }
    operator U32 () { return U32(); }
    LR& operator= (U32 const& v) { return *this; }
    LR& GetLR() { return *this; }
    void SetLR(U32 const& v) {}
  };
  
  struct CTR
  {
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(); }
    operator U32 () { return U32(); }
    CTR& operator= (U32 const& v) { return *this; }
    CTR& GetCTR() { return *this; }
    void SetCTR(U32 const& v) {}
  };
  
  struct MSR
  {
    struct PR {};
    struct EE {};
    
    template <typename PART> void Set( U32 value ) { SetDispatch( PART(), value ); }
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () { return U32(0); }
    MSR& operator= (U32 const& v) { return *this; }
    MSR& GetMSR() { return *this; }

    struct TODO {};
    
    void SetDispatch( EE const&, U32 value ) { throw TODO(); }
    void SetDispatch( PR const&, U32 value ) { throw TODO(); }
  };

  struct Processor
    : public EmuProcessor
    , public XER, public CR, public MSR, public LR, public CTR
  //  , public unisim::component::cxx::processor::powerpc::e200::mpc57xx::cpu
  {
    //  typedef unisim::component::cxx::processor::arm::CPU<ARMv7cfg> CP15CPU;
    struct TODO {};
  
    Processor();

    static Processor& Self( EmuProcessor& proc ) { return dynamic_cast<Processor&>( proc ); }
  
    EmuProcessor::RegView const* get_reg(char const* id, uintptr_t size) override;
    
    enum { OPPAGESIZE = 4096 };
    typedef vle::concrete::Operation Operation;
    typedef OpPage<Operation,OPPAGESIZE> InsnPage;

    std::map< uint32_t, InsnPage > insn_cache;

    //   typedef MSR MSR;
    //   typedef SPEFSCR SPEFSCR;
    
    //   CPU( Interface& _interface, ActionNode& root )
    //     : interface(_interface), path(&root), cia( new CIA )
    
    //   virtual void XERAccess( bool is_write ) { interface.xer.addaccess(is_write); }
    //   virtual void CRAccess( bool is_write ) { interface.cr.addaccess(is_write); }
    //   virtual void SPEFSCRAccess( bool is_write ) { interface.spefscr.addaccess(is_write); }
    
    struct Interrupt { void SetELEV(unsigned x) { throw TODO(); } };
    
    template <class T> Interrupt ThrowException() { DispatchException( T() ); return Interrupt(); }

    void ProcessException( char const* msg );
    
    template <class T> void DispatchException( T const& exc ) { ProcessException( "ANY" ); }
    
    struct ProgramInterrupt
    {
      struct UnimplementedInstruction {};
      struct IllegalInstruction {};
      struct Trap {};
      struct PrivilegeViolation {};
    };

    void DispatchException( ProgramInterrupt::UnimplementedInstruction const& exc ) { ProcessException( "UD" ); }
    
    struct SystemCallInterrupt
    {
      struct SystemCall {};
    };
    
    void DispatchException( SystemCallInterrupt::SystemCall const& exc ) { ProcessException( "SYS" ); }
    
    struct AlignmentInterrupt { struct UnalignedLoadStoreMultiple {}; };
    // void DispatchException( AlignmentInterrupt::UnalignedLoadStoreMultiple const& exc ) { interface.aligned = true; }
    
    bool Cond(bool c) { return c; }
    
    U32          reg_values[32];
    U32          cia, nia;
    U32 GetGPR(unsigned n) { return reg_values[n]; };
    void SetGPR(unsigned n, U32 value) { reg_values[n] = value; }
    //   bool EqualCIA(uint32_t pc) { return false; };
    U32 GetCIA() { return cia; };
    bool Branch(U32 addr) { nia = addr; return false; }

    U32 Fetch(U32 address)
    {
      uint32_t insn;
      PhysicalFetchMemory( address, (uint8_t*)&insn, 4 );
      return Target2Host(unisim::util::endian::E_BIG_ENDIAN, insn);
    }
    
    U32 MemRead(U32 address, unsigned size, bool sext, bool bigendian)
    {
      unsigned mask = size - 1, endian_mask = mask*(not bigendian);
      if (size >= 4 or mask & size) { throw "illegal size"; }
      uint8_t buf[4];
      PhysicalReadMemory( address, &buf[0], size );
      U32 result = 0;
      for (unsigned byte = 0; byte < size; ++byte)
        result = (result << 8) | buf[byte^endian_mask];
      if (sext) { int sh = (32 - size*8); result = int32_t(result << sh) >> sh; }
      return result;
    }
    void MemWrite( U32 address, unsigned size, U32 value, bool bigendian )
    {
      unsigned mask = size - 1, endian_mask = mask*bigendian;
      if (size >= 4 or mask & size) { throw "illegal size"; }
      uint8_t buf[4];
      for (unsigned byte = 0; byte < size; ++byte)
        { buf[byte^endian_mask] = value; value >>= 8; }
      PhysicalWriteMemory( address, &buf[0], size );
    }
    
    bool Int8Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 1, false, true)); return true; }
    bool Int16Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 2, false, true)); return true; }
    bool Int32Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 4, false, true)); return true; }
    
    bool SInt8Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 1, true, true)); return true; }
    bool SInt16Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 2, true, true)); return true; }

    bool Int16LoadByteReverse(unsigned n, U32 address) { SetGPR(n, MemRead(address, 2, false, false)); return true; }
    bool Int32LoadByteReverse(unsigned n, U32 address) { SetGPR(n, MemRead(address, 4, false, false)); return true; }
    
    bool Int8Store(unsigned n, U32 address ) { MemWrite(address, 1, GetGPR(n),true); return true; }
    bool Int16Store(unsigned n, U32 address ) { MemWrite(address, 2, GetGPR(n), true); return true; }
    bool Int32Store(unsigned n, U32 address ) { MemWrite(address, 4, GetGPR(n), true); return true; }

    bool Int16StoreByteReverse(unsigned n, U32 address ) { MemWrite( address, 2, GetGPR(n), false); return true; }
    bool Int32StoreByteReverse(unsigned n, U32 address ) { MemWrite( address, 4, GetGPR(n), false); return true; }

    //   void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w ); }

    //   void donttest_branch();
    //   void donttest_illegal();
    
    //   char const* GetObjectFriendlyName(U32) { return "???"; }
    
    bool Rfmci() { throw TODO(); return false; }
    bool Rfci() { throw TODO(); return false; }
    bool Rfdi() { throw TODO(); return false; }
    bool Rfi() { throw TODO(); return false; }

    bool Dcba(U32 const& addr) { throw TODO(); return false; }
    bool Dcbf(U32 const& addr) { throw TODO(); return false; }
    bool Dcbst(U32 const& addr) { throw TODO(); return false; }
    bool Dcbz(U32 const& addr) { throw TODO(); return false; }
    bool Dcbi(U32 const& addr) { throw TODO(); return false; }
    bool Icbi(U32 const& addr) { throw TODO(); return false; }
    bool Icbt(U32 const& addr) { throw TODO(); return false; }
    
    bool Msync() { throw TODO(); return false; }
    bool Isync() { throw TODO(); return false; }
    bool Mpure() { throw TODO(); return false; }
    bool Mpuwe() { throw TODO(); return false; }
    bool Mpusync() { throw TODO(); return false; }
    
    bool Lbarx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Lharx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Lwarx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Stbcx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Sthcx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Stwcx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool MoveFromDCR(unsigned dcrn, U32& result) { throw TODO(); return false; }
    bool MoveFromSPR(unsigned dcrn, U32& result) { throw TODO(); return false; }
    bool MoveToSPR(unsigned dcrn, U32 const& result) { throw TODO(); return false; }
    
    bool CheckSPV() { return true; }
    bool Wait() { throw TODO(); return false; }

    //   struct __EFPProcessInput__
    //   {
    //     __EFPProcessInput__( CPU& _cpu ) : cpu(_cpu), finv(false) {};
    //     template <class FLOAT>
    //     __EFPProcessInput__& set( FLOAT& input ) { finv |= not check_input( input ) ; return *this; }
    //     template <class FLOAT>
    //     static bool check_input( FLOAT& input )
    //     {
    //       // if (unlikely(input.isDenormalized()))
    //       //   {
    //       //     input.setZero(input.isNegative());
    //       //     return false;
    //       //   }
			
    //       // if (unlikely(input.hasInftyExponent()))
    //       //   return false;
			
    //       return true;
    //     }
    //     bool proceed()
    //     {
    //       // cpu.GetSPEFSCR().Set<typename SPEFSCR::FINV>(finv);
    //       // if (finv)
    //       //   {
    //       //     cpu.GetSPEFSCR().Set<typename SPEFSCR::FINVS>(true);
    //       //     if (cpu.GetSPEFSCR().Get<typename SPEFSCR::FINVE>())
    //       //       {
    //       //         cpu.ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
    //       //         return false;
    //       //       }
    //       //   }
    //       return true;
    //     }
		
    //     CPU& cpu;
    //     bool finv;
    //   };
	
    //   __EFPProcessInput__
    //   EFPProcessInput()
    //   {
    //     // GetSPEFSCR().Set<typename SPEFSCR::FG>(false);
    //     // GetSPEFSCR().Set<typename SPEFSCR::FX>(false);
    //     // GetSPEFSCR().Set<typename SPEFSCR::FGH>(false);
    //     // GetSPEFSCR().Set<typename SPEFSCR::FXH>(false);
    //     // GetSPEFSCR().Set<typename SPEFSCR::FDBZ>(false);
    //     // GetSPEFSCR().Set<typename SPEFSCR::FDBZH>(false);
    //     // GetSPEFSCR().SetDivideByZero( false );
    //     return __EFPProcessInput__( *this );
    //   }
	
    //   template <class FLOAT, class FLAGS>
    //   bool
    //   EFPProcessOutput( FLOAT& output, FLAGS const& flags )
    //   {
    //     // if (output.hasInftyExponent())
    //     //   {
    //     //     bool neg = output.isNegative();
    //     //     output.setInfty();
    //     //     output.setToPrevious();
    //     //     output.setNegative(neg);
    //     //   }
    //     // bool inexact = flags.isApproximate() and not spefscr.template Get<typename CPU::SPEFSCR::FINV>();
    //     // bool overflow = inexact and flags.isOverflow();
    //     // if (not GetSPEFSCR().SetOverflow( overflow ))
    //     //   return false;
    //     // bool underflow = inexact and flags.isUnderflow();
    //     // if (output.isDenormalized())
    //     //   {
    //     //     output.setZero(output.isNegative());
    //     //     inexact = true, underflow = true;
    //     //   }
    //     // if (not GetSPEFSCR().SetUnderflow( underflow ))
    //     //   return false;

    //     // if (inexact)
    //     //   {
    //     //     // Compute inexact flags (FG, FX)
    //     //     GetSPEFSCR().Set<typename SPEFSCR::FINXS>(true);
    //     //     if (spefscr.template Get<typename SPEFSCR::FINXE>())
    //     //       {
    //     //         this->template ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
    //     //         return false;
    //     //       }
    //     //   }

    //     return true;
    //   }

    virtual int emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count ) override;
  };
  
  using unisim::util::arithmetic::BitScanReverse;
  using unisim::util::arithmetic::RotateLeft;

} // end of namespace concrete

  using unisim::component::cxx::processor::powerpc::GPRPrint;
  using unisim::component::cxx::processor::powerpc::HexPrint;
  using unisim::component::cxx::processor::powerpc::CRPrint;
  using unisim::component::cxx::processor::powerpc::CondPrint;
  using unisim::component::cxx::processor::powerpc::EAPrint;
  using unisim::component::cxx::processor::powerpc::Mask64;
  using unisim::component::cxx::processor::powerpc::scaled_immediate;

} // end of namespace vle

#endif /* __VLE4FUZR_VLE_HH__ */

