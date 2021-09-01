#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>

namespace aarch64 {
  int decode(uint64_t, uint32_t, std::ostream&);
}

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/fail.h>

extern "C" value aarch64dba_decode(value vaddr, value vopcode) {
  std::stringstream s;
  aarch64::decode(Long_val(vaddr), Long_val(vopcode), s);
  return caml_copy_string(s.str().c_str());
}
