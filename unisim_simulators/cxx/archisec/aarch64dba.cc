#include <aarch64/decoder.hh>

#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/fail.h>

extern "C" value aarch64dba_decode(value vaddr, value vopcode) {
  std::stringstream s;
  aarch64::Decoder decoder;

  decoder.process(s, Long_val(vaddr), Long_val(vopcode));

  return caml_copy_string(s.str().c_str());
}
