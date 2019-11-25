/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *  
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "arm.hh"
#include <unisim/component/cxx/processor/arm/isa_arm32.tcc>

ArmProcessor::ArmProcessor( char const* name, bool is_thumb )
  : unisim::kernel::Object( name, 0 )
  , Processor()
  , CPU( name, 0 )
{
  cpsr.Set( unisim::component::cxx::processor::arm::T, uint32_t(is_thumb) );
}

ArmProcessor::~ArmProcessor()
{
}
