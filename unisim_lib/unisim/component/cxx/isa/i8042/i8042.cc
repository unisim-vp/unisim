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

#include <unisim/component/cxx/isa/i8042/i8042.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace isa {
namespace i8042 {

//using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;

I8042::I8042(const char *name, Object *parent) :
	Object(name, parent),
	Client<Keyboard>(name, parent),
	Client<Logger>(name, parent),
	keyboard_import("keyboard-import", this),
	logger_import("logger-import", this),
	input_buffer(0),
	status(I8042_STR_KEYLOCK),
	control(0),
	write_command(0),
	fsb_frequency(0),
	isa_bus_frequency(8),
	typematic_rate(10.0),
	param_fsb_frequency("fsb-frequency", this, fsb_frequency),
	param_isa_bus_frequency("isa-bus-frequency", this, isa_bus_frequency),
	param_typematic_rate("typematic-rate", this, typematic_rate)
{
}

I8042::~I8042()
{
}

bool I8042::Setup()
{
	input_buffer = 0;
	unsigned int index;
	for(index = 0; index < 2; index++)
	{
		while(!output_buffer[index].empty()) output_buffer[index].pop();
	}
	status = I8042_STR_KEYLOCK;
	control = 0;
	return true;
}

bool I8042::WriteIO(isa_address_t addr, const void *buffer, uint32_t size)
{
	if(size == 1)
	{
		switch(addr)
		{
			case I8042_DATA_REG:
				// fill-in input buffer
				if(logger_import)
				{
					(*logger_import) << DebugInfo;
					(*logger_import) << "Write I/O. port 0x" << Hex << addr << ", value=" << (unsigned int) *(uint8_t *) buffer << Dec << Endl;
					(*logger_import) << EndDebugInfo;
				}
				// reset bit 'Command/Data' in status register
				// to indicate that input buffer contains a data byte
				status = status & (~I8042_STR_CMDDAT);
				WriteInputBuffer(*(uint8_t *) buffer);
				return true;
			case I8042_COMMAND_REG:
				// fill-in input buffer
				if(logger_import)
				{
					(*logger_import) << DebugInfo;
					(*logger_import) << "Write I/O. port 0x" << Hex << addr << ", value=" << (unsigned int) *(uint8_t *) buffer << Dec << Endl;
					(*logger_import) << EndDebugInfo;
				}
				// set bit 'Command/Data' in status register
				// to indicate that input buffer contains a command byte
				status = status | I8042_STR_CMDDAT;
				WriteInputBuffer(*(uint8_t *) buffer);
				return true;
		}
	}
	return false;
}

bool I8042::ReadIO(isa_address_t addr, void *buffer, uint32_t size)
{
	if(size == 1)
	{
		switch(addr)
		{
			case I8042_DATA_REG:
				// read the output buffer
				if(IsOutputBufferFull(KBD_INDEX) && !IsOutputBufferFull(AUX_INDEX))
					*(uint8_t *) buffer = ReadOutputBuffer(KBD_INDEX);
				else if(IsOutputBufferFull(AUX_INDEX))
					*(uint8_t *) buffer = ReadOutputBuffer(AUX_INDEX);
				else
					*(uint8_t *) buffer = 0;

				if(logger_import)
				{
					(*logger_import) << DebugInfo;
					(*logger_import) << "Read I/O. port 0x" << Hex << addr << ", value=" << (unsigned int) *(uint8_t *) buffer << Dec << Endl;
					(*logger_import) << EndDebugInfo;
				}
				return true;
			case I8042_STATUS_REG:
				// read the status register
				*(uint8_t *) buffer = status;
				if(logger_import)
				{
					(*logger_import) << DebugInfo;
					(*logger_import) << "Read I/O. port 0x" << Hex << addr << ", value=" << (unsigned int) status << Dec << "(OBF=" << ((status & I8042_STR_OBF) ? 1:0) << ",AUXOBF=" << ((status & I8042_STR_AUX_OBF) ? 1:0) << ",IBF=" << ((status & I8042_STR_IBF) ? 1:0) << ")" << Endl;
					(*logger_import) << EndDebugInfo;
				}
				return true;
		}
	}
	return false;
}

uint8_t I8042::ReadOutputBuffer(unsigned int index)
{
	if(!output_buffer[index].empty())
	{
		uint8_t value = output_buffer[index].front();
		output_buffer[index].pop();

		if(logger_import)
		{
			(*logger_import) << DebugInfo;
			(*logger_import) << "Reading output buffer[" << index << "] (0x" << Hex << (unsigned int) value << Dec << ")" << Endl;
			(*logger_import) << EndDebugInfo;
		}

		if(output_buffer[index].empty())
		{
			// reset bit 'output buffer full' in status register
			// to indicate that output buffer is empty
			status = status & (~I8042_STR_OBF);
			if(index == AUX_INDEX)
				status = status & (~I8042_STR_AUX_OBF);
		}
		else
		{
			if((index == AUX_INDEX && (control & I8042_CTR_AUXINT)) ||
			   (index == KBD_INDEX && (control & I8042_CTR_KBDINT)))
			{
				// trigger an interrupt
				TriggerInterrupt(index, true);
			}
		}

		return value;
	}
	return 0;
}

void I8042::WriteOutputBuffer(unsigned int index, uint8_t value)
{
	if(logger_import)
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "Writing output buffer[" << index << "] (0x" << Hex << (unsigned int) value << Dec << ")" << Endl;
		(*logger_import) << EndDebugInfo;
	}
	output_buffer[index].push(value);
	// set bit 'output buffer full' in status register
	// to indicate that output buffer is full
	status = status | I8042_STR_OBF;
	if(index == AUX_INDEX)
		status = status | I8042_STR_AUX_OBF;

	if((index == AUX_INDEX && (control & I8042_CTR_AUXINT)) ||
		(index == KBD_INDEX && (control & I8042_CTR_KBDINT)))
	{
		// trigger an interrupt
		TriggerInterrupt(index, true);
	}
}

bool I8042::IsOutputBufferFull(unsigned int index)
{
	return (index == KBD_INDEX && (status & I8042_STR_OBF) != 0) ||
	       (index == AUX_INDEX && (status & I8042_STR_AUX_OBF) != 0);
}

uint8_t I8042::ReadInputBuffer()
{
	if(logger_import)
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "Reading input buffer (0x" << Hex << (unsigned int) input_buffer << Dec << ")" << Endl;
		(*logger_import) << EndDebugInfo;
	}
	// reset bit 'input buffer full' in status register
	// to indicate that input buffer is empty
	status = status & (~I8042_STR_IBF);
	return input_buffer;
}

void I8042::WriteInputBuffer(uint8_t value)
{
	if(logger_import)
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "Writing input buffer (0x" << Hex << (unsigned int) value << Dec << ")" << Endl;
		(*logger_import) << EndDebugInfo;
	}
	input_buffer = value;
	// set bit 'input buffer full' in status register
	// to indicate that input buffer is full
	status = status | I8042_STR_IBF;
	
	if(status & I8042_STR_CMDDAT)
	{
		HandleCommand();
	}
	else
	{
		if(write_command)
			HandleWriteCommand();
		else
			HandleKeyboardCommand();
	}
}

bool I8042::IsInputBufferFull()
{
	return (status & I8042_STR_IBF) != 0;
}

void I8042::WriteControl(uint8_t value)
{
	control = value;
	// update sysflag bit in the status register
	if(control & I8042_CTR_SYSFLAG)
		status = status | I8042_STR_SYSFLAG;
	else
		status = status & (~I8042_STR_SYSFLAG);
}

void I8042::HandleCommand()
{
	uint8_t command = ReadInputBuffer();
	// Process i8042 commands
	switch(command)
	{
		case I8042_CMD_CTL_RCTR:
			WriteOutputBuffer(KBD_INDEX, control);
			break;
		case I8042_CMD_CTL_SELF_TEST:
			// self test is okay
			control |= I8042_CTR_SYSFLAG;
			status |= I8042_STR_SYSFLAG;
			WriteOutputBuffer(KBD_INDEX, I8042_RET_CTL_TEST);
			break;
		case I8042_CMD_CTL_WCTR:
		case I8042_CMD_AUX_LOOP:
			write_command = command;
			break;
		case I8042_CMD_CTL_AUX_TEST:
			WriteOutputBuffer(AUX_INDEX, I8042_RET_AUX_TEST_OK);
			break;
		case I8042_CMD_CTL_AUX_DISABLE:
			control |= I8042_CTR_AUXDIS;
			break;
		case I8042_CMD_CTL_AUX_ENABLE:
			control &= ~I8042_CTR_AUXDIS;
			break;
		default:
			if(logger_import)
			{
				(*logger_import) << DebugWarning;
				(*logger_import) << "unknown command 0x" << Hex << (unsigned int) command << Dec << Endl;	
				(*logger_import) << EndDebugWarning;
			}
	}
}

void I8042::HandleWriteCommand()
{
	uint8_t data = ReadInputBuffer();
	switch(write_command)
	{
		case I8042_CMD_CTL_WCTR:
			WriteControl(data);
			break;
		case I8042_CMD_AUX_LOOP:
			WriteOutputBuffer(AUX_INDEX, data);
			break;

		default:
			if(logger_import)
			{
				(*logger_import) << DebugWarning;
				(*logger_import) << "command 0x" << Hex << (unsigned int) write_command << Dec << " has no additional data needed" << Endl;	
				(*logger_import) << EndDebugWarning;
			}
	}
}

void I8042::HandleKeyboardCommand()
{
	uint8_t command = ReadInputBuffer();
	if(logger_import)
	{
		(*logger_import) << DebugWarning;
		(*logger_import) << "unknown keyboard command 0x" << Hex << (unsigned int) command << Dec << Endl;	
		(*logger_import) << EndDebugWarning;
	}
}

void I8042::CaptureKey()
{
	// check if output buffer is empty
	if(!(control & I8042_CTR_KBDDIS))
	{
		// output buffer is empty
		uint8_t scancode;
		// get a scancode from the keyboard
		return;
		if(keyboard_import->GetScancode(scancode))
		{
			// fill-in output buffer with the keyboard scancode
			WriteOutputBuffer(KBD_INDEX, scancode);
		}
	}
}

void I8042::TriggerInterrupt(unsigned int index, bool in_level)
{
	// this should be implemented elsewhere as it is a virtual method
}

} // end of namespace i8042
} // end of namespace isa
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
