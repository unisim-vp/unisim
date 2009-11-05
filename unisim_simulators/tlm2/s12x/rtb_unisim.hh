/*
 */


#include "atd_pwm_stub.hh"


// Cob includes
extern "C" {
	#include "network.h"
	#include "data.h"
	#include "dataline.h"
	#include "error.h"
	#include "control.h"
	#include "dialog.h"
}


// A module that should implement communication with RT-Builder
class RTBStub :
	public ATD_PWM_STUB
{


private:
	bool connected;
	client_struct * client_ctl;
	client_struct * client_in;
	client_struct * client_out;

	dataline_struct * dataline_in;
	data_struct * data_in;
	data_struct * data_out;


public:
	typedef ATD_PWM_STUB inherited;


	int init_client();
	void control();
	void exchange();

	RTBStub(const sc_module_name& name, Object *parent = 0);
	~RTBStub();

	virtual void ProcessATD();
	virtual void ProcessPWM();

private:

};

