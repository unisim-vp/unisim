/*
 *  Copyright (c) 2019-2021,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <serial.hh>
#include <iostream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include <inttypes.h>

HostTerm::HostTerm()
  : rx_thread(), rx_hang(), rx_buf(), rx_source(), rx_sink()
{
  pthread_mutex_init(&rx_hang,0);
  pthread_mutex_lock(&rx_hang);

  rx_source.head = 0;
  rx_source.locked = 0;
  rx_sink.tail = 0;
  rx_sink.locked = 0;
  rx_sink.kill = 0;
}

void
HostTerm::Start()
{
  struct RX { static void* loop(void* p) { auto t = reinterpret_cast<HostTerm*>(p); t->ReceiveChars(); return 0; } };
  
  if (pthread_create(&rx_thread, 0, &RX::loop, reinterpret_cast<void*>(this)) != 0)
    { struct Bad {}; throw Bad(); }
}

void
HostTerm::ReceiveChars()
{
  struct termios saved_termios;
  if (0 != tcgetattr(0, &saved_termios))
    throw saved_termios;
  {
    struct termios raw_termios = saved_termios;
    cfmakeraw(&raw_termios);
    if (0 != tcsetattr(0, TCSANOW, &raw_termios))
      throw raw_termios;
  }

  while (not rx_sink.kill)
    {
      if (rx_count() >= qsize)
        {
          rx_source.locked ^= 1;
          pthread_mutex_lock(&rx_hang);
        }
      
      if (not std::cin.get(rx_buf[rx_source.head % qsize]))
        { struct BrokenPipe {}; throw BrokenPipe(); }

      rx_source.head += 1;
    }

  if (0 != tcsetattr(0, TCSANOW, &saved_termios))
    throw saved_termios;
}

void
HostTerm::Kill()
{
  rx_sink.kill = 1;
}

bool
HostTerm::GetChar(char& ch)
{
  unsigned size = rx_count();
  
  if (size <= 1 and rx_source.locked != rx_sink.locked)
    {
      rx_sink.locked ^= 1;
      pthread_mutex_unlock(&rx_hang);
    }
  
  if (size == 0)
    return false;
  
  ch = rx_buf[rx_sink.tail % qsize];
  rx_sink.tail += 1;
  return true;
}

void
HostTerm::PutChar(char ch)
{
  std::cout << ch;
}

void
HostTerm::FlushChars()
{
  std::cout.flush();
}

Serial::Serial( char const* name, unisim::kernel::Object* parent )
  : unisim::kernel::Object(name, 0)
  , unisim::kernel::Client<unisim::service::interfaces::CharIO>(name, parent)
  , char_io_import("char-io-import", this)
  , logger("serial.log")
{
}

bool
Serial::GetChar(char& c)
{
  if (not char_io_import->GetChar(c))
    return false;
  logger << "r:'\\x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(uint8_t(c)) << std::dec << "'" << std::endl;
  logger.flush();
  return true;
}

void
Serial::PutChar(char c)
{
  logger << "w:'\\x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(uint8_t(c)) << std::dec << "'" << std::endl;
  logger.flush();
  char_io_import->PutChar(c);
}

// void
// Serial::FlushOutput()
// {
//   char_io_import->FlushOutput();
// }
