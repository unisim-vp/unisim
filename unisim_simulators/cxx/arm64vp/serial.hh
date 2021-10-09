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

#ifndef __ARM64VP_SERIAL_HH__
#define __ARM64VP_SERIAL_HH__

#include <unisim/util/netstreamer/netstreamer.hh>
#include <fstream>
#include <pthread.h>
#include <inttypes.h>

struct Serial
{
  Serial();
  void Initialize();
  bool GetChar(char& c);
  void PutChar(char c);
  void FlushOutput();

  unisim::util::netstreamer::NetStreamer netstreamer;
  std::ofstream logger;
};

struct HostTerm
{
  HostTerm();
  
  void ResetCharIO();
  bool GetChar(char& c);
  void PutChar(char c);
  void FlushChars();
  void ReceiveChars();
  void Start();
  void Kill();
    
  static unsigned const qsize = 32;
    
  pthread_t rx_thread;
  pthread_mutex_t rx_hang;
  char rx_buf[qsize];
  unsigned rx_count() const { return rx_source.head - rx_sink.tail; }
  struct {
    unsigned head :   16;
    unsigned locked :  1;
    unsigned pad :    15;
  }        rx_source;
  struct {
    unsigned tail :   16;
    unsigned locked :  1;
    unsigned kill :    1;
    unsigned pad :    14;
  }        rx_sink;
};
  
#endif /* __ARM64VP_SERIAL_HH__ */
