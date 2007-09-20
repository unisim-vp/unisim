/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_ISA_I8042_I8042_HH__
#define __UNISIM_COMPONENT_CXX_ISA_I8042_I8042_HH__

#include <unisim/component/cxx/isa/types.hh>
#include <unisim/service/interfaces/keyboard.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/logger.hh>
#include <queue>

namespace unisim {
namespace component {
namespace cxx {
namespace isa {
namespace i8042 {

using std::queue;
using unisim::component::cxx::isa::isa_address_t;

using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;

using unisim::service::interfaces::Logger;
using unisim::service::interfaces::Keyboard;

class I8042 :
	public Client<Keyboard>,
	public Client<Logger>
{
public:
	static const unsigned int KBD_INDEX = 0;
	static const unsigned int AUX_INDEX = 1;

	static const isa_address_t I8042_DATA_REG    = 0x60; // read/write
	static const isa_address_t I8042_COMMAND_REG = 0x64; // write only
	static const isa_address_t I8042_STATUS_REG  = 0x64; // read only

	// Status register bits
	static const uint8_t I8042_STR_PARITY  = 0x80; // Parity error (1=parity error on transmission, 0=no error)
	static const uint8_t I8042_STR_TIMEOUT = 0x40; // Keyboard Time-out error (1=keyboard time-out, 0=no time-out error)
	static const uint8_t I8042_STR_AUX_OBF = 0x20; // aux output buffer full (1=full, 0=empty)
	static const uint8_t I8042_STR_KEYLOCK = 0x10; // keyboard active (1=enabled, 0=disabled)
	static const uint8_t I8042_STR_CMDDAT  = 0x08; // Command/Data available (0=data available ar port 60h, 1=command available at port 64h)
	static const uint8_t I8042_STR_SYSFLAG = 0x04; // system flag (1= self test passed, 0=failed)
	static const uint8_t I8042_STR_IBF     = 0x02; // input buffer full (1=full, 0=empty)
	static const uint8_t I8042_STR_OBF     = 0x01; // output buffer full (1=full, 0=empty)

	// i8042 on-board microcontroller control register
	// to access command byte, processor must first write a command to port 64h: read=20h (I8042_CMD_RCTR), write=60h (I8042_CMD_WCTR)
	// then the command can be either read or write on port 60h
	static const uint8_t I8042_CTR_KBDINT     = 0x01; // keyboard interrupt (1=enabled, 0=disabled)
	static const uint8_t I8042_CTR_AUXINT     = 0x02; // aux interrupt (1=enabled, 0=disabled)
	static const uint8_t I8042_CTR_SYSFLAG    = 0x04; // system flag (1= self test passed, 0=failed)
	static const uint8_t I8042_CTR_IGNKEYLOCK = 0x08; // PC/AT inhibit override/ignore keylock (1=enabled always)
	static const uint8_t I8042_CTR_KBDDIS     = 0x10; // keyboard disable (1=disable keyboard, 0=no action)
	static const uint8_t I8042_CTR_AUXDIS     = 0x20; //aux disable (1=disable aux, 0=no action) 
	static const uint8_t I8042_CTR_XLATE      = 0x40; // IBM PC Compatibility Mode (1=translate kbd codes to PC scancodes)

	// i8042 on-board microcontroller commands (port 64h)
	static const uint8_t I8042_CMD_CTL_RCTR         = 0x20; // transmit keyboard controller's command byte to system as a scancode at port 60h
	static const uint8_t I8042_CMD_CTL_WCTR         = 0x60; // the next byte written to port 60h will be stored in the keyboard controller's command byte
	static const uint8_t I8042_CMD_CTL_PASSWD_TEST  = 0xa4; // Test if a password is installed (PS/2 only).
	                                                        // result comes back in port 60h. 0FAh means a password is installed, 0F1h means no password
	static const uint8_t I8042_CMD_CTL_PASSWD_SET   = 0xa5; // Transmit password (PS/2 only). Starts receipt of password.
	                                                        // The next sequence of scan codes written to port 60h, ending with a zero byte, are the new password
	static const uint8_t I8042_CMD_CTL_PASSWD_MATCH = 0xa6; // Password match. Characters from the keyboard are compared to password until a match occurs
	static const uint8_t I8042_CMD_CTL_AUX_DISABLE  = 0xa7; // Disable the aux device (i.e. the mouse, PS/2 only). Identical to setting bit five of the command byte
	static const uint8_t I8042_CMD_CTL_AUX_ENABLE   = 0xa8; // Enable the aux device (i.e. the mouse, PS/2 only). Identical to clearing bit five of the command byte
	static const uint8_t I8042_CMD_CTL_AUX_TEST     = 0xa9; // Test the aux device (i.e. the mouse, PS/2 only). Returns 0 if okay, 1 or 2 if there is a stuck clock,
	                                                        // 3 or 4 if there is a stuck data line. results come back in port 60h
	static const uint8_t I8042_CMD_CTL_SELF_TEST    = 0xaa; // Keyboard interface test. Tests the keyboard interface. Returns 0 if okay, 1 or 2 if there is a stuck clock,
	                                                        // 3 or 4 if there is a stuck data line. Results come back in port 60h.

	static const uint8_t I8042_CMD_AUX_LOOP = 0xd3;
	// more commands to be added...

	// TODO: keyboard microcontroller commands (port 60h)

	// command results
	static const uint8_t I8042_RET_CTL_TEST = 0x55; // self test okay
	static const uint8_t I8042_RET_AUX_TEST_OK = 0x00; // aux test okay

	ServiceImport<Keyboard> keyboard_import;
	ServiceImport<Logger> logger_import;
	
	I8042(const char *name, Object *parent = 0);
	virtual ~I8042();
	
	virtual bool Setup();
	
	bool WriteIO(isa_address_t addr, const void *buffer, uint32_t size);
	bool ReadIO(isa_address_t addr, void *buffer, uint32_t size);
	virtual void TriggerInterrupt(unsigned int index, bool in_level);
	void CaptureKey();
protected:
	double typematic_rate; // scancodes per second
private:
	// i8042 registers
	uint8_t input_buffer; // write-only, port 60h (keyboard controller)/ 64h (on-board i8042 microcontroller)
	queue<uint8_t> output_buffer[2]; // read-only, port 60h (on-board i8042 microcontroller)
	uint8_t status; // read-only, port 64h (on-board i8042 microcontroller)

	// some internal registers
	uint8_t control; // on-board i8042 keyboard microcontroller control register

	uint8_t write_command;

	unsigned int isa_bus_frequency;
	unsigned int fsb_frequency;

	Parameter<unsigned int> param_isa_bus_frequency;
	Parameter<unsigned int> param_fsb_frequency;
	Parameter<double> param_typematic_rate;

	uint8_t ReadInputBuffer();
	void WriteInputBuffer(uint8_t value);
	bool IsInputBufferFull();
	bool IsOutputBufferFull(unsigned int index);
	uint8_t ReadOutputBuffer(unsigned int index);
	void WriteOutputBuffer(unsigned int index, uint8_t value);
	void WriteControl(uint8_t value);
	void HandleCommand();
	void HandleWriteCommand();
	void HandleKeyboardCommand();
	void SelfTest();
/*	void AuxTest();
	void AuxLoopback();*/
};

} // end of namespace i8042
} // end of namespace isa
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
