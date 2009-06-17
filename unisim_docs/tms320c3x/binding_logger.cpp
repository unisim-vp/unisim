#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
 
using unisim::kernel::service::Object;
using namespace unisim::kernel::logger;
 
class MyObject : public Object {
private:
    Logger logger;
 
public:
    MyObject(const char *name, const Object *parent = 0) : 
            Object(name, parent),
            logger(*this) {
        ...
    }
};
