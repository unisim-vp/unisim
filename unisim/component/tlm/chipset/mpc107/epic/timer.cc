#include "tlm/chipsets/mpc107/epic/timer.hh"

namespace full_system {
namespace tlm {
namespace chipsets {
namespace mpc107 {
namespace epic {
	
void Timer::SetTimer(sc_time &time) {
	TimeHandler *handler;
	
	if(free_handlers.empty()) {
		handler = new TimeHandler();
		sc_spawn(sc_bind(&Timer::TimeHandlerProcess, this, handler));
	} else {
		handler = free_handlers.front();
		free_handlers.pop_front();
	}
	
	handler->time = time;
	handler->event.notify(SC_ZERO_TIME);
}

void Timer::TimeHandlerProcess(TimeHandler *handler) {
	while(1) {
		wait(handler->event);
		wait(handler->time);
		TimeEvent();
		free_handlers.push_back(handler);
	}
}

} // end of epic namespace
} // end of mpc107 namespace	
} // end of chipsets namespace
} // end of tlm namespace	
} // end of full_system namespace
