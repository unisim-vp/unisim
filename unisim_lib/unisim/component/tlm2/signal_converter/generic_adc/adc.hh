


//Authors:Khaled Rahmouni

#ifndef __UNISIM_COMPONENT_TLM2_COM_ARGA_ARGA_HH__
#define __UNISIM_COMPONENT_TLM2_COM_ARGA_ARGA_HH__

#include <systemc>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "unisim/component/tlm2/interrupt/types.hh"
#include "unisim/component/tlm2/com/arga/CONFIG_GENERATOR.hh"
#include "unisim/component/tlm2/com/arga/CONFIG_ARGA_SPI.hh" 




#define PERIODE_7200HZ   (double)(1000000000.0/7200.0) //periode d'echantillonage de ARGA par voie

#define         FRAME_SIZE                                      7
#define 	NB_VOIES 					4
#define		VOIE0						0
#define		VOIE1						1
#define		VOIE2						2
#define		VOIE3						3


using namespace sc_core;
using namespace sc_dt;
using namespace std;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;

class ARGA : public sc_module {
public:

/* socket for spi management */
tlm_utils::simple_target_socket<ARGA> mosi;//master out slave in port
tlm_utils::simple_initiator_socket<ARGA> miso;//slave in master out port
/* socket for spi management */

/* input and outputs sockets for interrupt */
tlm_utils::simple_initiator_socket<ARGA, 1, InterruptProtocolTypes> data_ready; //out interrupt port
tlm_utils::simple_target_socket<ARGA, 1, InterruptProtocolTypes> chip_select;   //in chip select port
/* input and outputs sockets for interrupt */

ARGA(const sc_module_name &name);

// Get command code in stream received
uint8_t Code(uint16_t header);
// Get address in stream received
uint8_t Address(uint16_t header);
// calculate checksum of stream received
uint8_t CheckSum();
double Funct(double time, int type, int freq, int phase, double amp);

void arga_receive_frame(tlm::tlm_generic_payload& trans, sc_time& delay);
void chip_select_in(TLMInterruptPayload& trans, sc_time& delay);
void Gene_ADC_process();

int16_t Ram_I[NB_VOIES], Ram_U[NB_VOIES];
double val[NB_VOIES], RelativeTime, C0_AMPL, C1_AMPL, C2_AMPL, C3_AMPL;
uint8_t Num_voie, stream2send[FRAME_SIZE], stream2receive[FRAME_SIZE], NbByte;
uint16_t Header;

/*Specific payload for it management*/
TLMInterruptPayload* data_ready_signal;
/*Specific payload for it management*/


};


#endif // __UNISIM_COMPONENT_TLM2_COM_ARGA_ARGA_HH__
