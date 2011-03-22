/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
#include "unisim/util/debug/debugger_handler/debugger_handler.hh"

namespace unisim {
namespace util {
namespace debug {
namespace debugger_handler {

DebuggerHandler::
DebuggerHandler()
	: handler_context(0)
	, breakpoint_handler_function(0)
	, watchpoint_handler_function(0)
{}

DebuggerHandler::
~DebuggerHandler()
{}

bool
DebuggerHandler::
HasBreakpoint(const char *str)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return HasBreakpoint(addr);
}

bool
DebuggerHandler::
SetBreakpoint(const char *str)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return SetBreakpoint(addr);
}

bool
DebuggerHandler::
DeleteBreakpoint(const char *str)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return DeleteBreakpoint(addr);
}

bool
DebuggerHandler::
SetWatchpoint(const char *str, uint32_t size)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return SetWatchpoint(addr, size);
}

bool
DebuggerHandler::
SetReadWatchpoint(const char *str, uint32_t size)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return SetReadWatchpoint(addr, size);
}

bool
DebuggerHandler::
SetWriteWatchpoint(const char *str, uint32_t size)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return SetWriteWatchpoint(addr, size);
}

bool
DebuggerHandler::
DeleteWatchpoint(const char *str, uint32_t size)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return DeleteWatchpoint(addr, size);
}

bool
DebuggerHandler::
DeleteReadWatchpoint(const char *str, uint32_t size)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return DeleteReadWatchpoint(addr, size);
}

bool
DebuggerHandler::
DeleteWriteWatchpoint(const char *str, uint32_t size)
{
	uint64_t addr = 0;
	bool has_addr = false;

	if ( GetSymbolAddress(str, addr) ) has_addr = true;
	if ( !has_addr && GetFileSystemAddress(str, addr) ) has_addr = true;

	return DeleteWriteWatchpoint(addr, size);
}

bool
DebuggerHandler::
SetHandlerContext(void *context)
{
	handler_context = context;
	return true;
}

bool
DebuggerHandler::
SetBreakpointHandler(
		void (*function)(void *, uint64_t))
{
	breakpoint_handler_function = function;
	return true;
}

bool
DebuggerHandler::
SetWatchpointHandler(
		void (*function)(void *, uint64_t, bool))
{
	watchpoint_handler_function = function;
	return true;
}

void
DebuggerHandler::
CallBreakpointHandler(uint64_t addr)
{
	if ( breakpoint_handler_function )
		breakpoint_handler_function(handler_context, addr);
}

void
DebuggerHandler::
CallWatchpointHandler(uint64_t addr, bool read)
{
	if ( watchpoint_handler_function )
		watchpoint_handler_function(handler_context, addr, read);
}

} // end of namespace debugger_handler
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
