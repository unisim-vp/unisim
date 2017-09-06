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
 
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>

#ifndef WIN32
#include <signal.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <iostream>

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

std::set<InlineDebuggerBase *> InlineDebuggerBase::instances;
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
void (*InlineDebuggerBase::prev_sig_int_handler)(int);
#endif

InlineDebuggerBase::InlineDebuggerBase()
{
	if(instances.empty())
	{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		SetConsoleCtrlHandler(&InlineDebuggerBase::ConsoleCtrlHandler, TRUE);
#else
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
#endif
	}
	instances.insert(this);
}

InlineDebuggerBase::~InlineDebuggerBase()
{
	instances.erase(this);
	if(instances.empty())
	{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		SetConsoleCtrlHandler(&InlineDebuggerBase::ConsoleCtrlHandler, FALSE);
#else
		signal(SIGINT, prev_sig_int_handler);
#endif
	}
}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
BOOL WINAPI InlineDebuggerBase::ConsoleCtrlHandler(DWORD dwCtrlType)
{
	bool trap = false;
	switch(dwCtrlType)
	{
		case CTRL_C_EVENT:
			std::cerr << "Interrupted by Ctrl-C" << std::endl;
			trap = true;
			break;
		case CTRL_BREAK_EVENT:
			std::cerr << "Interrupted by Ctrl-Break" << std::endl;
			trap = true;
			break;
		case CTRL_CLOSE_EVENT:
			std::cerr << "Interrupted by a console close" << std::endl;
			trap = true;
			break;
		case CTRL_LOGOFF_EVENT:
			std::cerr << "Interrupted because of logoff" << std::endl;
			trap = true;
			break;
		case CTRL_SHUTDOWN_EVENT:
			std::cerr << "Interrupted because of shutdown" << std::endl;
			trap = true;
			break;
	}
	
	if(trap)
	{
		BroadcastInterrupt();
	}
	
	return trap ? TRUE : FALSE;
}
#else
void InlineDebuggerBase::SigIntHandler(int signum)
{
	std::cerr << "Interrupted by Ctrl-C or SIGINT signal" << std::endl;
	BroadcastInterrupt();
}
#endif

void InlineDebuggerBase::BroadcastInterrupt()
{
	std::set<InlineDebuggerBase *>::iterator it;
	
	for(it = instances.begin(); it != instances.end(); it++)
	{
		InlineDebuggerBase *instance = *it;
		
		instance->Interrupt();
	}
}


} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
