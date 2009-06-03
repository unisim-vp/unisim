

//Authours: Khaled Rahmouni

#include "unisim/component/tlm2/com/arga/arga.hh"

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;



static const uint8_t mirror_binary_pattern [256] = {
                0x00U, 0x80U, 0x40U, 0xC0U, 0x20U, 0xA0U, 0x60U, 0xE0U, 0x10U, 0x90U, 0x50U, 0xD0U, 0x30U, 0xB0U, 0x70U, 0xF0U,
                0x08U, 0x88U, 0x48U, 0xC8U, 0x28U, 0xA8U, 0x68U, 0xE8U, 0x18U, 0x98U, 0x58U, 0xD8U, 0x38U, 0xB8U, 0x78U, 0xF8U,
                0x04U, 0x84U, 0x44U, 0xC4U, 0x24U, 0xA4U, 0x64U, 0xE4U, 0x14U, 0x94U, 0x54U, 0xD4U, 0x34U, 0xB4U, 0x74U, 0xF4U,
                0x0CU, 0x8CU, 0x4CU, 0xCCU, 0x2CU, 0xACU, 0x6CU, 0xECU, 0x1CU, 0x9CU, 0x5CU, 0xDCU, 0x3CU, 0xBCU, 0x7CU, 0xFCU,
                0x02U, 0x82U, 0x42U, 0xC2U, 0x22U, 0xA2U, 0x62U, 0xE2U, 0x12U, 0x92U, 0x52U, 0xD2U, 0x32U, 0xB2U, 0x72U, 0xF2U,
                0x0AU, 0x8AU, 0x4AU, 0xCAU, 0x2AU, 0xAAU, 0x6AU, 0xEAU, 0x1AU, 0x9AU, 0x5AU, 0xDAU, 0x3AU, 0xBAU, 0x7AU, 0xFAU,
                0x06U, 0x86U, 0x46U, 0xC6U, 0x26U, 0xA6U, 0x66U, 0xE6U, 0x16U, 0x96U, 0x56U, 0xD6U, 0x36U, 0xB6U, 0x76U, 0xF6U,
                0x0EU, 0x8EU, 0x4EU, 0xCEU, 0x2EU, 0xAEU, 0x6EU, 0xEEU, 0x1EU, 0x9EU, 0x5EU, 0xDEU, 0x3EU, 0xBEU, 0x7EU, 0xFEU,
                0x01U, 0x81U, 0x41U, 0xC1U, 0x21U, 0xA1U, 0x61U, 0xE1U, 0x11U, 0x91U, 0x51U, 0xD1U, 0x31U, 0xB1U, 0x71U, 0xF1U,
                0x09U, 0x89U, 0x49U, 0xC9U, 0x29U, 0xA9U, 0x69U, 0xE9U, 0x19U, 0x99U, 0x59U, 0xD9U, 0x39U, 0xB9U, 0x79U, 0xF9U,
                0x05U, 0x85U, 0x45U, 0xC5U, 0x25U, 0xA5U, 0x65U, 0xE5U, 0x15U, 0x95U, 0x55U, 0xD5U, 0x35U, 0xB5U, 0x75U, 0xF5U,
                0x0DU, 0x8DU, 0x4DU, 0xCDU, 0x2DU, 0xADU, 0x6DU, 0xEDU, 0x1DU, 0x9DU, 0x5DU, 0xDDU, 0x3DU, 0xBDU, 0x7DU, 0xFDU,
                0x03U, 0x83U, 0x43U, 0xC3U, 0x23U, 0xA3U, 0x63U, 0xE3U, 0x13U, 0x93U, 0x53U, 0xD3U, 0x33U, 0xB3U, 0x73U, 0xF3U,
                0x0BU, 0x8BU, 0x4BU, 0xCBU, 0x2BU, 0xABU, 0x6BU, 0xEBU, 0x1BU, 0x9BU, 0x5BU, 0xDBU, 0x3BU, 0xBBU, 0x7BU, 0xFBU,
                0x07U, 0x87U, 0x47U, 0xC7U, 0x27U, 0xA7U, 0x67U, 0xE7U, 0x17U, 0x97U, 0x57U, 0xD7U, 0x37U, 0xB7U, 0x77U, 0xF7U,
                0x0FU, 0x8FU, 0x4FU, 0xCFU, 0x2FU, 0xAFU, 0x6FU, 0xEFU, 0x1FU, 0x9FU, 0x5FU, 0xDFU, 0x3FU, 0xBFU, 0x7FU, 0xFFU
            };

uint8_t ARGA::Code(uint16_t header) {
                return (uint8_t) ((header & 0x007EU) >> 1);
            }

uint8_t ARGA::Address(uint16_t header) {
                return (uint8_t) ((header & 0xFF80U) >> 7);
            }

uint8_t ARGA::CheckSum() {
                return ~(uint8_t) (stream2send[0] + stream2send[1] + stream2send[2] + stream2send[3] + stream2send[4] + stream2send[5]);
            }

double ARGA::Funct(double time, int type, int freq, int phase, double amp) {
    switch (type) {
        case 0: // NULL FUNCTION
            return 0;
        case 1: // SINUSOIDE
            return amp * sin((2 * PI * freq * time) + ((2 * PI * phase) / 360));
        case 2: // COSINUSOIDE
            return amp * cos((2 * PI * freq * time) + ((2 * PI * phase) / 360));
            // TO DO :
            // IMPLEMENT OTHERS FUNCTIONS IF NECESSARY
        default:
            return amp;
    }
    return 0;

}

ARGA::ARGA(const sc_module_name &name) : sc_module(name),
mosi("arga_mosi"),
miso("arga_miso"),
data_ready("arga_it_in"),
chip_select("arga_it_out")
{

SC_HAS_PROCESS(ARGA);
SC_THREAD(Gene_ADC_process);

// Register callback for incoming b_transport interface method call
mosi.register_b_transport(this, &ARGA::arga_receive_frame);
chip_select.register_b_transport(this, &ARGA::chip_select_in);

data_ready_signal = new TLMInterruptPayload;

C1_AMPL = 0.1;
C2_AMPL = 0.3;
C3_AMPL = 0.5;
NbByte = 0;

for (int i = 0; i < 7; i++)
stream2send [i] = 0x00;
}

void ARGA::arga_receive_frame(tlm::tlm_generic_payload& trans, sc_time& delay) {

                uint8_t *data;
                data = (uint8_t *) trans.get_data_ptr();
                stream2receive[NbByte] = *data;
              //  printf("DATA_READ=%x\n", *data);


                tlm::tlm_generic_payload byte2send;
                sc_time time(0, SC_NS);
                byte2send.set_command(tlm::TLM_WRITE_COMMAND);
                byte2send.set_address(0x00);
                byte2send.set_data_length(1);
                byte2send.set_data_ptr((unsigned char *) &(mirror_binary_pattern [stream2send[NbByte]]));
                miso->b_transport(byte2send, time);

                if (NbByte == 6)// on teste si on a recu la trame de 7 octects
                {
                    NbByte = 0;
                    Header = stream2receive[0] + (stream2receive[1] << 8);
                    switch (Code(Header))
                    {
                    case ARGA_CMD_RAM:
                        stream2send[0] = stream2receive[0];
                        stream2send[1] = stream2receive[1];

                        switch (Address(Header))
                        {
                        case ARGA_RAM_I_V_VALUE0:
                            stream2send[2] = (Ram_U[1] & 0x00FF);
                            stream2send[3] = (Ram_U[1] >> 8);
                            stream2send[4] = (Ram_I[1] & 0x00FF);
                            stream2send[5] = (Ram_I[1] >> 8);

                            break;

                        case ARGA_RAM_I_V_VALUE1:
                            stream2send[2] = (Ram_U[2] & 0x00FF);
                            stream2send[3] = (Ram_U[2] >> 8);
                            stream2send[4] = (Ram_I[2] & 0x00FF);
                            stream2send[5] = (Ram_I[2] >> 8);
                            break;

                        case ARGA_RAM_I_V_VALUE2:
                            stream2send[2] = (Ram_U[3] & 0x00FF);
                            stream2send[3] = (Ram_U[3] >> 8);
                            stream2send[4] = (Ram_I[3] & 0x00FF);
                            stream2send[5] = (Ram_I[3] >> 8);
                            break;

                        case ARGA_RAM_I_V_VALUE3:
                            stream2send[2] = (Ram_U[0] & 0x00FF);
                            stream2send[3] = (Ram_U[0] >> 8);
                            stream2send[4] = (Ram_I[0] & 0x00FF);
                            stream2send[5] = (Ram_I[0] >> 8);
                            break;
                        }

                        stream2send[6] = CheckSum();

                        break;

                    default:
                        break;
                    } //end switch(Code(stream2receive))

                }//end if(NbByte==6)
    else {
        NbByte++;
    }

}

void ARGA::chip_select_in(TLMInterruptPayload& trans, sc_time& delay) {
    
}

void ARGA::Gene_ADC_process() {

    sc_time delay(0, SC_NS);
    while (true) {

        switch (Num_voie) {
            case VOIE0:
                val[0] = C0_AMPL / coeff_capteur;
                Ram_U[0] = (int16_t) (val[0] * (ARGA_ADC_FULLSCALE / ARGA_VREF_U));
                if (Ram_U[0] > (ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[0] = (int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                if (Ram_U[0] <-(ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[0] = -(int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                Ram_I[0] = Ram_U[0];
                break;

            case VOIE1:
                val[1] = (Funct(RelativeTime, C1_TYPE, C1_FREQUENCY, C1_DEPHASAGE, C1_AMPL / coeff_capteur));
                Ram_U[1] = (int16_t) (val[1] * (ARGA_ADC_FULLSCALE / ARGA_VREF_U));
                if (Ram_U[1] > (ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[1] = (int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                if (Ram_U[1] <-(ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[1] = -(int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                Ram_I[1] = Ram_U[1];
                break;

            case VOIE2:
                val[2] = (Funct(RelativeTime, C2_TYPE, C2_FREQUENCY, C2_DEPHASAGE, C2_AMPL / coeff_capteur));

                Ram_U[2] = (int16_t) (val[2] * (ARGA_ADC_FULLSCALE / ARGA_VREF_U));
                if (Ram_U[2] > (ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[2] = (int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                if (Ram_U[2] <-(ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[2] = -(int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                Ram_I[2] = Ram_U[2];
                break;

            case VOIE3:
                val[3] = (Funct(RelativeTime, C3_TYPE, C3_FREQUENCY, C3_DEPHASAGE, C3_AMPL / coeff_capteur));
                Ram_U[3] = (int16_t) (val[3] * (ARGA_ADC_FULLSCALE / ARGA_VREF_U));
                if (Ram_U[3] > (ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[3] = (int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                if (Ram_U[3] <-(ARGA_ADC_FULLSCALE / (2.0 * ARGA_VREF_U))) Ram_U[3] = -(int16_t) (ARGA_ADC_FULLSCALE / 2.0 * ARGA_VREF_U);
                Ram_I[3] = Ram_U[3];
                break;
        }

        if (++Num_voie == NB_VOIES) {
            Num_voie = 0;
            RelativeTime += (1.000 / ARGA_FREQ_SAMPLING);
            if (RelativeTime == (35.000 / ARGA_FREQ_SAMPLING)) {
                RelativeTime = 0.0;
            }
        }
        wait(PERIODE_7200HZ, SC_NS); //to simulate the sampling frequency
        //     cout << " the time is " << sc_time_stamp() << endl;

        data_ready_signal->level = true;
        data_ready->b_transport(*data_ready_signal, delay); // generate interrupt to signal that a data is ready
    }
}

