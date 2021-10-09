#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>

namespace armsec {
  struct Processor {
    struct StatusRegister {
      enum InstructionSet { Arm, Thumb, Jazelle, ThumbEE };
    };
  };
  int decode(enum Processor::StatusRegister::InstructionSet, bool, uint32_t,
	     unsigned, uint32_t, uint32_t, std::ostream&);
}

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/fail.h>

extern "C" value arm32dba_decode(value vmode, value vendian,
				 value vitstate, value vaddr, value vopcode) {
  std::stringstream s;
  enum armsec::Processor::StatusRegister::InstructionSet iset =
    Long_val(vmode) ?
    armsec::Processor::StatusRegister::Thumb :
    armsec::Processor::StatusRegister::Arm;
  armsec::decode(iset, Long_val(vendian), 0b10000, Long_val(vitstate),
		 Long_val(vaddr), Long_val(vopcode), s);
  return caml_copy_string(s.str().c_str());
}
