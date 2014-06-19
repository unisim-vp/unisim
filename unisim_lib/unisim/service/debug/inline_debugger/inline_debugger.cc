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

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

using std::cerr;
using std::endl;

bool InlineDebuggerBase::trap = false;
int InlineDebuggerBase::alive_instances = 0;
#ifndef WIN32
void (*InlineDebuggerBase::prev_sig_int_handler)(int);
#endif

InlineDebuggerBase::InlineDebuggerBase()
{
	if(alive_instances == 0)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&InlineDebuggerBase::ConsoleCtrlHandler, TRUE);
#else
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
#endif
	}
	alive_instances++;
}

InlineDebuggerBase::~InlineDebuggerBase()
{
	if(--alive_instances > 0)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&InlineDebuggerBase::ConsoleCtrlHandler, FALSE);
#else
		signal(SIGINT, prev_sig_int_handler);
#endif
	}
}

#ifdef WIN32
BOOL WINAPI InlineDebuggerBase::ConsoleCtrlHandler(DWORD dwCtrlType)
{
	switch(dwCtrlType)
	{
		case CTRL_C_EVENT:
			cerr << "Interrupted by Ctrl-C" << endl;
			trap = true;
			break;
		case CTRL_BREAK_EVENT:
			cerr << "Interrupted by Ctrl-Break" << endl;
			trap = true;
			break;
		case CTRL_CLOSE_EVENT:
			cerr << "Interrupted by a console close" << endl;
			trap = true;
			break;
		case CTRL_LOGOFF_EVENT:
			cerr << "Interrupted because of logoff" << endl;
			trap = true;
			break;
		case CTRL_SHUTDOWN_EVENT:
			cerr << "Interrupted because of shutdown" << endl;
			trap = true;
			break;
	}
	return trap ? TRUE : FALSE;
}
#else
void InlineDebuggerBase::SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	trap = true;
}
#endif

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
