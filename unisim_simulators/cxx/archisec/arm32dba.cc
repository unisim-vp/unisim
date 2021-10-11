#include <aarch32/decoder.hh>

#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/fail.h>

extern "C" value arm32dba_decode(value vmode, value vendian,
				 value vitstate, value vaddr, value vopcode) {
  std::stringstream s;
  armsec::Decoder decoder;

  decoder.iset = Long_val(vmode) ? decoder.Thumb : decoder.Arm;
  decoder.bigendian = Long_val(vendian);
  decoder.mode = unisim::component::cxx::processor::arm::PSR::USER_MODE;
  decoder.itstate = Long_val(vitstate);

  decoder.process( s, Long_val(vaddr), Long_val(vopcode) );

  return caml_copy_string(s.str().c_str());
}
