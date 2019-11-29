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
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/cpu.hh>
#include <unisim/util/symbolic/symbolic.hh>

struct mpc57Processor
  : public Processor
  , public unisim::component::cxx::processor::powerpc::e200::mpc57xx::cpu
{
  typedef unisim::component::cxx::processor::arm::CPU<ARMv7cfg> CP15CPU;
  typedef typename CP15CPU::CP15Reg CP15Reg;

  ArmProcessor( char const* name, bool is_thumb );
  ~ArmProcessor();

  virtual void Sync() override { throw 0; }

  static unisim::component::cxx::processor::arm::isa::arm32::Decoder<ArmProcessor> arm32_decoder;
  static unisim::component::cxx::processor::arm::isa::thumb::Decoder<ArmProcessor> thumb_decoder;

  static ArmProcessor& Self( Processor& proc ) { return dynamic_cast<ArmProcessor&>( proc ); }

  Processor::RegView const* get_reg(int regid) override;

  enum { OPPAGESIZE = 4096 };
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor> AOperation;
  typedef OpPage<AOperation,OPPAGESIZE> AOpPage;
  typedef unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor> TOperation;
  typedef OpPage<TOperation,OPPAGESIZE> TOpPage;

  std::map< uint32_t, AOpPage > arm32_op_cache;
  std::map< uint32_t, TOpPage > thumb_op_cache;

  enum mem_acc_type_t { mat_write = 0, mat_read, mat_exec };

  uint32_t MemURead32( uint32_t address ) { return PerformUReadAccess( address, 4 ); }
  uint32_t MemRead32( uint32_t address ) { return PerformReadAccess( address, 4 ); }
  uint32_t MemURead16( uint32_t address ) { return PerformUReadAccess( address, 2 ); }
  uint32_t MemRead16( uint32_t address ) { return PerformReadAccess( address, 2 ); }
  uint32_t MemRead8( uint32_t address ) { return PerformReadAccess( address, 1 ); }
  void     MemUWrite32( uint32_t address, uint32_t value ) { PerformUWriteAccess( address, 4, value ); }
  void     MemWrite32( uint32_t address, uint32_t value ) { PerformWriteAccess( address, 4, value ); }
  void     MemUWrite16( uint32_t address, uint16_t value ) { PerformUWriteAccess( address, 2, value ); }
  void     MemWrite16( uint32_t address, uint16_t value ) { PerformWriteAccess( address, 2, value ); }
  void     MemWrite8( uint32_t address, uint8_t value ) { PerformWriteAccess( address, 1, value ); }

  void     PerformPrefetchAccess( uint32_t addr ) {}
  void     PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value );
  uint32_t PerformReadAccess( uint32_t addr, uint32_t size );
  void     PerformUWriteAccess( uint32_t addr, uint32_t size, uint32_t value );
  uint32_t PerformUReadAccess( uint32_t addr, uint32_t size );

  void     SetExclusiveMonitors( uint32_t addr, unsigned size ) { throw 0; }
  bool     ExclusiveMonitorsPass( uint32_t addr, unsigned size ) { throw 0; return true; }
  void     ClearExclusiveLocal() { throw 0; }

  void ReadInsn( uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn );
  void ReadInsn( uint32_t address, unisim::component::cxx::processor::arm::isa::thumb::CodeType& insn );

  bool PhysicalWriteMemory( uint32_t addr, uint8_t const* buffer, uint32_t size )
  {
    auto pi = mem_page(addr, size);
    if (pi == pages.end()) return false;
    pi->access(pi->Write);
    uint32_t pos = addr - pi->base;
    std::copy(&buffer[0], &buffer[size], pi->at(pos));
    return true;
  }

  bool PhysicalReadMemory( uint32_t addr, uint8_t* buffer, uint32_t size )
  {
    auto pi = mem_page(addr, size);
    if (pi == pages.end()) return false;
    pi->access(pi->Read);
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }
  bool PhysicalFetchMemory( uint32_t addr, uint8_t* buffer, uint32_t size )
  {
    auto pi = mem_page(addr, size);
    if (pi == pages.end()) return false;
    pi->access(pi->Execute);
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }

  int emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count ) override;

  template <class Decoder> void Step(Decoder&);

  bool bblock;

  void UndefinedInstruction( unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor>* insn );
  void UndefinedInstruction( unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor>* insn );
  void DataAbort(uint32_t addr, mem_acc_type_t mat, unisim::component::cxx::processor::arm::DAbort type);
  void BKPT( int ) { throw 0; }
};

#endif /* __VLE4FUZR_VLE_HH__ */
