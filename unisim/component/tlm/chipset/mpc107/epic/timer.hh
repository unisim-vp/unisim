#ifndef __FS_TLM_CHIPSETS_MPC107_EPIC_TIMER_HH__
#define __FS_TLM_CHIPSETS_MPC107_EPIC_TIMER_HH__

#include <systemc.h>
#include <list>

namespace full_system {
namespace tlm {
namespace chipsets {
namespace mpc107 {
namespace epic {

using std::list;
	
class Timer {
private:
	/* forward declaration */
	class TimeHandler;
	
public:
	virtual void TimeEvent() = 0;

	void SetTimer(sc_time &time);
	
private:
	void TimeHandlerProcess(TimeHandler *handler);
	
	class TimeHandler {
	public:
		sc_time time;
		sc_event event;
	};
	
	list<TimeHandler *> free_handlers;
};

} // end of epic namespace
} // end of mpc107 namespace	
} // end of chipsets namespace
} // end of tlm namespace	
} // end of full_system namespace

#endif /* __FS_TLM_CHIPSETS_MPC107_EPIC_TIMER_HH__ */
