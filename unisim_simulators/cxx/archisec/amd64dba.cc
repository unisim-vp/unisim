#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>

namespace intel {
  int decode(bool, uint64_t, std::vector<uint8_t>, std::ostream&);
}

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/fail.h>

uint8_t nibble_value(char n) {
  if ('0' <= n || n <= '9')
    return n - '0';
  else if ('a' <= n || n <= 'z')
    return n - 'a';
  else if ('A' <= n || n <= 'Z')
    return n - 'A';
  else
    caml_invalid_argument("not a valid hexadecimal digit");
}

extern "C" value amd64dba_decode(value vmode, value vaddr, value vopcode) {
  const char *opcode = String_val(vopcode);
  std::vector<uint8_t> code;
  for (int i = 0; opcode[i] != '\0'; i += 2) {
    code.push_back(nibble_value(opcode[i + 1]) << 4 | nibble_value(opcode[i]));
  }
  std::stringstream s;
  intel::decode(Long_val(vmode), Unsigned_long_val(vaddr), code, s);
  return caml_copy_string(s.str().c_str());
}
