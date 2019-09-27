/********************************/
/*******  DBGate binding  *******/
/********************************/

#include <unisim/util/dbgate/client/client.hh>
#include <cstdint>

namespace unisim {
namespace util {
namespace dbgate {
namespace client {

extern "C"
{
  /* Unconnected implementations of dbgate bindings */
  void dbgate_write(int fd, char const* buffer, uintptr_t size) { /* dont't write */ }
  int  dbgate_open(char const* path)                            { return -1; /* can't open */ }
  void dbgate_close(int fd)                                     { /* dont't close */ }
}

} // end of namespace client
} // end of namespace dbgate
} // end of namespace util
} // end of namespace unisim
