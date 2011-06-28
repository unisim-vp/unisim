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

#ifndef __UNISIM_API_DEBUG_DEBUGAPI_HH__
#define __UNISIM_API_DEBUG_DEBUGAPI_HH__

#include <inttypes.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/api/api.hh"

namespace unisim {
namespace api {
namespace debug {

class DebugAPI
	: public unisim::kernel::api::APIBase
{
public:
	static const std::string DEBUGAPI_ID;
	DebugAPI(const char *_name, unisim::kernel::service::Object *parent);
	virtual ~DebugAPI();
	virtual bool SetStepMode() = 0;
	virtual bool SetContinueMode() = 0;
	virtual bool IsModeStep() = 0;
	virtual bool IsModeContinue() = 0;
	virtual bool GetSymbolAddress(const char *str, uint64_t &addr) = 0;
	virtual bool GetFileSystemAddress(const char *str, uint64_t &addr) = 0;
	virtual bool HasBreakpoint(uint64_t addr) = 0;
	virtual bool SetBreakpoint(uint64_t addr) = 0;
	virtual bool DeleteBreakpoint(uint64_t addr) = 0;
	virtual bool SetWatchpoint(uint64_t addr, uint32_t size) = 0;
	virtual bool SetReadWatchpoint(uint64_t addr, uint32_t size) = 0;
	virtual bool SetWriteWatchpoint(uint64_t addr, uint32_t size) = 0;
	virtual bool DeleteWatchpoint(uint64_t addr, uint32_t size) = 0;
	virtual bool DeleteReadWatchpoint(uint64_t addr, uint32_t size) = 0;
	virtual bool DeleteWriteWatchpoint(uint64_t addr, uint32_t size) = 0;

	virtual bool HasBreakpoint(const char *str);
	virtual bool SetBreakpoint(const char *str);
	virtual bool DeleteBreakpoint(const char *str);
	virtual bool SetWatchpoint(const char *str, uint32_t size);
	virtual bool SetReadWatchpoint(const char *str, uint32_t size);
	virtual bool SetWriteWatchpoint(const char *str, uint32_t size);
	virtual bool DeleteWatchpoint(const char *str, uint32_t size);
	virtual bool DeleteReadWatchpoint(const char *str, uint32_t size);
	virtual bool DeleteWriteWatchpoint(const char *str, uint32_t size);
	virtual bool SetHandlerContext(void *context);
	virtual bool SetBreakpointHandler(void (*function)(void *, uint64_t));
	virtual bool SetWatchpointHandler(void (*function)(void *, uint64_t, bool));

protected:
	void CallBreakpointHandler(uint64_t addr);
	void CallWatchpointHandler(uint64_t addr, bool read);

private:
	unisim::kernel::service::Object *debuggerhandler_parent_object;
	void *handler_context;
	void (*breakpoint_handler_function)(void *, uint64_t);
	void (*watchpoint_handler_function)(void *, uint64_t, bool);
};

} // end of namespace debug
} // end of namespace api
} // end of namespace unisim

#endif // __UNISIM_API_DEBUG_DEBUG_API__
