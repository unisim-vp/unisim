/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_XPS_UART_LITE_TCC__
#define __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_XPS_UART_LITE_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <string.h>
#include <unisim/util/queue/queue.tcc>

#include <errno.h>

#ifdef WIN32

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>

#endif

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_uart_lite {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
XPS_UARTLite<CONFIG>::XPS_UARTLite(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::MEMORY_ADDR> >(name, parent)
	, memory_export("memory-export", this)
	, logger(*this)
	, verbose(false)
	, c_baseaddr(CONFIG::C_BASEADDR)
	, c_highaddr(CONFIG::C_HIGHADDR)
	, telnet_tcp_port(23)
	, telnet_sock(-1)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_c_baseaddr("c-baseaddr", this, c_baseaddr, "Base address (C_BASEADDR design parameter)")
	, param_c_highaddr("c-highaddr", this, c_highaddr, "High address (C_HIGHADDR design parameter)")
	, param_telnet_tcp_port("telnet-tcp-port", this, telnet_tcp_port, "TCP/IP port of telnet")
	, telnet_input_buffer_size(0)
	, telnet_input_buffer_index(0)
	, telnet_output_buffer_size(0)
{
	param_c_baseaddr.SetMutable(false);
	param_c_highaddr.SetMutable(false);

	std::stringstream sstr_description;
	sstr_description << "This module implements a Xilinx XPS UART Lite (v1.01a). It has the following characteristics:" << std::endl;
	sstr_description << "PLB data width: " << CONFIG::C_SPLB_DWITH << " bits" << std::endl;
	sstr_description << "Baud rate: " << CONFIG::C_BAUDRATE << " bits/s" << std::endl;
	sstr_description << "Data bits: " << CONFIG::C_DATA_BITS << " bits" << std::endl;
	sstr_description << "Parity: " << (CONFIG::C_USE_PARITY ? (CONFIG::C_ODD_PARITY ? "odd" : "even") : "none") << std::endl;

	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
XPS_UARTLite<CONFIG>::~XPS_UARTLite()
{
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::BeginSetup()
{
	Reset();
	return true;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::EndSetup()
{
	if(telnet_sock >= 0) return true;
	
	struct sockaddr_in addr;
	int server_sock;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if(server_sock < 0)
	{
		logger << DebugError << "socket failed" << EndDebugError;
		return false;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(telnet_tcp_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		logger << DebugError << "Bind failed. TCP Port #" << telnet_tcp_port << " may be already in use. Please specify another port in " << param_telnet_tcp_port.GetName() << EndDebugError;
#ifdef WIN32
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(listen(server_sock, 1))
	{
		logger << DebugError << "Listen failed" << EndDebugError;
#ifdef WIN32
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

#ifdef WIN32
		int addr_len;
#else
		socklen_t addr_len;
#endif

	if(IsVerbose())
	{
		logger << DebugInfo << "Listening on TCP port " << telnet_tcp_port << EndDebugInfo;
	}
	addr_len = sizeof(addr);
	telnet_sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);

	if(telnet_sock < 0)
	{
		logger << DebugError << "accept failed" << EndDebugError;
#ifdef WIN32
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(IsVerbose())
	{
		logger << DebugInfo << "Connection with telnet client established" << EndDebugInfo;
	}

#ifdef WIN32
	u_long NonBlock = 1;
	if(ioctlsocket(telnet_sock, FIONBIO, &NonBlock) != 0)
	{
		logger << DebugError << "ioctlsocket failed" << EndDebugError;
		closesocket(server_sock);
		closesocket(telnet_sock);
		telnet_sock = -1;
		return false;
	}
#else
	int socket_flag = fcntl(telnet_sock, F_GETFL, 0);

	if(socket_flag < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(server_sock);
		close(telnet_sock);
		telnet_sock = -1;
		return false;
	}

	/* Ask for non-blocking reads on socket */
	if(fcntl(telnet_sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(server_sock);
		close(telnet_sock);
		telnet_sock = -1;
		return false;
	}
#endif

#ifdef WIN32
	closesocket(server_sock);
#else
	close(server_sock);
#endif

	return true;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::Reset()
{
	rx_fifo.Clear();
	tx_fifo.Clear();
	stat_reg = CONFIG::STAT_REG_RESET_VALUE;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		uint8_t *dst = (uint8_t *) buffer;
		
		do
		{
			uint8_t value;
			Read(addr, value, true);
			memcpy(dst, &value, 1);
			size --;
			dst++;
			addr ++;
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		const uint8_t *src = (const uint8_t *) buffer;
		
		do
		{
			uint8_t value;
			memcpy(&value, src, 1);
			size--;
			src++;
			addr++;
			Write(addr, value, true);
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::Read(typename CONFIG::MEMORY_ADDR addr, uint8_t& value, bool debug)
{
	bool status = true;
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint8_t reg_value;
	switch(offset)
	{
		case CONFIG::RX_FIFO:
			if(debug)
			{
				reg_value = GetRX_FIFO();
			}
			else
			{
				if(!ReadRX_FIFO(reg_value)) status = false;
			}
			break;
		case CONFIG::STAT_REG:
			reg_value = debug ? GetSTAT_REG() : ReadSTAT_REG();
			break;
		default: reg_value = 0; break;
	}
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Reading 0x" << std::hex << (unsigned int) reg_value << " from register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::RX_FIFO: logger << "Rx FIFO"; break;
			case CONFIG::TX_FIFO: logger << "Tx FIFO"; break;
			case CONFIG::STAT_REG: logger << "STAT_REG"; break;
			case CONFIG::CTRL_REG: logger << "CTRL_REG"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}
	value = unisim::util::endian::Host2BigEndian(reg_value);
	
	return status;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::Write(typename CONFIG::MEMORY_ADDR addr, uint8_t value, bool debug)
{
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint8_t reg_value = unisim::util::endian::BigEndian2Host(value);
	if(IsVerbose())
	{
		logger << DebugInfo << "Writing 0x" << std::hex << (unsigned int) reg_value << " in register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::RX_FIFO: logger << "Rx FIFO"; break;
			case CONFIG::TX_FIFO: logger << "Tx FIFO"; break;
			case CONFIG::STAT_REG: logger << "STAT_REG"; break;
			case CONFIG::CTRL_REG: logger << "CTRL_REG"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}

	switch(offset)
	{
		case CONFIG::TX_FIFO: if(debug) SetTX_FIFO(reg_value); else WriteTX_FIFO(reg_value); break;
		case CONFIG::CTRL_REG: if(!debug) WriteCTRL_REG(reg_value); break;
		default: break;
	}
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const
{
	return (addr >= c_baseaddr) && ((addr + size - 1) <= c_highaddr);
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetRX_FIFO() const
{
	return rx_fifo.Empty() ? 0 : rx_fifo.ConstFront() & CONFIG::DATA_MASK;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::ReadRX_FIFO(uint8_t& value)
{
	if(rx_fifo.Empty())
	{
		logger << DebugWarning << "Reading an empty Rx FIFO returns an undefined result and generates a bus error" << EndDebugWarning;
		return false;
	}
	
	value = rx_fifo.Front() & CONFIG::DATA_MASK;
	rx_fifo.Pop();
	return true;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetSTAT_REG() const
{
	uint8_t v = stat_reg;
	
	if(!rx_fifo.Empty()) v = v | CONFIG::STAT_REG_RX_FIFO_VALID_DATA_MASK;
	if(rx_fifo.Full()) v = v | CONFIG::STAT_REG_RX_FIFO_FULL_MASK;
	if(tx_fifo.Empty()) v = v | CONFIG::STAT_REG_TX_FIFO_EMPTY_MASK;
	if(tx_fifo.Full()) v = v | CONFIG::STAT_REG_TX_FIFO_FULL_MASK;
	
	return v;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::ReadSTAT_REG()
{
	uint8_t v = GetSTAT_REG();
	
	// reading STAT_REG resets Parity error, Frame error, and overrun error bits
	stat_reg = stat_reg & ~(CONFIG::STAT_REG_PARITY_ERROR_MASK | CONFIG::STAT_REG_FRAME_ERROR_MASK | CONFIG::STAT_REG_OVERRUN_ERROR_MASK);
	
	return v;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetSTAT_REG_RX_FIFO_VALID_DATA() const
{
	return rx_fifo.Empty() ? 0 : 1;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetSTAT_REG_INTR_ENABLED() const
{
	return (stat_reg & CONFIG::STAT_REG_INTR_ENABLED_MASK) >> CONFIG::STAT_REG_INTR_ENABLED_OFFSET;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::SetTX_FIFO(uint8_t value)
{
	if(tx_fifo.Empty()) return;
	
	tx_fifo.Front() = value & CONFIG::DATA_MASK;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::WriteTX_FIFO(uint8_t value)
{
	if(tx_fifo.Full())
	{
		logger << DebugWarning << "Tx FIFO overflow" << EndDebugWarning;
		return;
	}
	
	value = value & CONFIG::DATA_MASK;
	tx_fifo.Push(value);
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::WriteCTRL_REG(uint8_t value)
{
	if(value & CONFIG::CTRL_REG_ENABLE_INTR_MASK)
	{
		stat_reg = stat_reg | CONFIG::STAT_REG_INTR_ENABLED_MASK;
	}
	
	if(value & CONFIG::CTRL_REG_RST_RX_FIFO_MASK)
	{
		rx_fifo.Clear();
	}

	if(value & CONFIG::CTRL_REG_RST_TX_FIFO_MASK)
	{
		tx_fifo.Clear();
	}
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::TelnetFlushOutput()
{
	if(telnet_output_buffer_size > 0)
	{
		unsigned int index = 0;
		do
		{
#ifdef WIN32
			int r = send(telnet_sock, telnet_output_buffer + index, telnet_output_buffer_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(telnet_sock, telnet_output_buffer + index, telnet_output_buffer_size);
			if(r <= 0)
#endif
			{
				logger << DebugError << "can't write into socket" << EndDebugError;
				Object::Stop(-1);
				return;
			}

			index += r;
			telnet_output_buffer_size -= r;
		}
		while(telnet_output_buffer_size > 0);
	}
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::TelnetPutChar(char c)
{
	if(telnet_output_buffer_size >= sizeof(telnet_output_buffer))
	{
		TelnetFlushOutput();
	}

	telnet_output_buffer[telnet_output_buffer_size++] = c;

	if(c == '\n')
	{
		TelnetFlushOutput();
	}
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::TelnetGetChar(char& c)
{
	if(telnet_input_buffer_size == 0)
	{
		do
		{
#ifdef WIN32
			int r = recv(telnet_sock, telnet_input_buffer, sizeof(telnet_input_buffer), 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = read(telnet_sock, telnet_input_buffer, sizeof(telnet_input_buffer));
			if(r <= 0)
#endif
			{
#ifdef WIN32
				if(r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
#else
				if(r < 0 && errno == EAGAIN)
#endif
				{
					return false;
				}

				logger << DebugError << "can't read from socket" << EndDebugError;
				Object::Stop(-1);
				return false;
			}
			telnet_input_buffer_index = 0;
			telnet_input_buffer_size = r;
			break;
		} while(1);
	}

	c = telnet_input_buffer[telnet_input_buffer_index++];
	telnet_input_buffer_size--;
	return true;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::TelnetProcess()
{
	char c;
	uint8_t v;
	
	if(!rx_fifo.Full())
	{
		do
		{
			if(!TelnetGetChar(c)) break;

			v = (uint8_t) c;
			rx_fifo.Push(v);
		}
		while(!rx_fifo.Full());
	}
	
	if(!tx_fifo.Empty())
	{
		do
		{
			v = tx_fifo.Front();
			tx_fifo.Pop();
			c = (char) v;
			TelnetPutChar(c);
		}
		while(!tx_fifo.Empty());
	}
}

} // end of namespace xps_uart_lite
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
