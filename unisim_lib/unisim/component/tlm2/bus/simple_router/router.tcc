#ifndef __UNISIM_COMPONENT_TLM2_BUS_SIMPLE_ROUTER_TCC__
#define __UNISIM_COMPONENT_TLM2_BUS_SIMPLE_ROUTER_TCC__

namespace unisim {
namespace component {
namespace tlm2 {
namespace bus {
namespace simple_router {

template<typename CONFIG>
Router<CONFIG>::
Router(const sc_module_name &name, Object *parent) :
sc_module(name),
Object(name, parent) {
}

template<typename CONFIG>
Router<CONFIG>::
~Router() {
}

template<typename CONFIG>
bool
Router<CONFIG>::
Setup() {
	return false;
}

} // end of namespace simple_router
} // end of namespace bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_BUS_SIMPLE_ROUTER_TCC__
