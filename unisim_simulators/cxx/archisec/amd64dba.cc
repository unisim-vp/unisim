#include <amd64/decoder.hh>
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
  if (('0' <= n) && (n <= '9'))
    return n - '0';
  else if (('a' <= n) && (n <= 'f'))
    return n - 'a' + 10;
  else if (('A' <= n) && (n <= 'F'))
    return n - 'A' + 10;
  else
    caml_invalid_argument("not a valid hexadecimal digit");
}

extern "C" value amd64dba_decode(value vmode, value vaddr, value vopcode) {
  const char *opcode = String_val(vopcode);
  std::vector<uint8_t> code;
  for (int i = 0; opcode[i] != '\0'; i += 2) {
    code.push_back(nibble_value(opcode[i]) << 4 | nibble_value(opcode[i + 1]));
  }
  std::stringstream s;
  intel::Decoder decoder;

  decoder.mode64 = Long_val(vmode);

  decoder.process(s, Unsigned_long_val(vaddr), std::move(code));

  return caml_copy_string(s.str().c_str());
}
