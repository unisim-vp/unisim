#include <inttypes.h>
#include <systemc.h>
#include <tlm.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace str7_eic {

template <bool DEBUG = false>
class STR7_EIC
{
private:
	/* interrupt controller registers */
	uint32_t icr;
	uint32_t cicr;
	uint32_t cipr;
	uint32_t ivr;
	uint32_t fir;
	uint32_t ier0;
	uint32_t ipr0;
	uint32_t sir[32];

	/* interrupt controller accessors */
	uint32_t ICR();
	uint32_t CICR();
	uint32_t CIPR();
	uint32_t IVR();
	uint32_t FIR();
	uint32_t IER0();
	uint32_t IPR0();
	uint32_t SIR(unsigned int index);

	bool FIQ_EN();
	bool IRQ_EN();
	uint32_t CIC();
	uint32_t CIP();
	uint32_t IVRhigh();
	uint32_t IVRlow();
	uint32_t FIP();
	uint32_t FIE();
	uint32_t SIV(unsigned int index);
	uint32_t SIPL(unsigned int index);
};

} // end of namespace str7_eic
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

