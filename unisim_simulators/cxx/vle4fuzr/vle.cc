/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *  
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "vle.hh"
#include "top_vle.hh"
#include <iostream>

namespace vle {
namespace concrete {
  
  Processor::Processor()
  {     
    for (unsigned reg = 0; reg < 32; ++reg)
      reg_values[reg] = U32();
  }

  void
  Processor::ProcessException( char const* msg )
  {
    std::cerr << "exception(" << msg << ")\n";
    throw TODO();
  }

} // end of namespace concrete
} // end of namespace vle

