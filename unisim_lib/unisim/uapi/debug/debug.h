/*
 *  Copyright (c) 2011
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

#ifndef __UNISIM__UAPI__DEBUG__DEBUG__INCLUDED
#define __UNISIM__UAPI__DEBUG__DEBUG__INCLUDED

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include <inttypes.h>

#include "unisim/uapi/types.h"

extern "C"
{

/****************************************************************************/
/*                                                                         **/
/*                      DEFINITIONS AND MACROS                             **/
/*                                                                         **/
/****************************************************************************/

/****************************************************************************/
/*                                                                         **/
/*                      TYPEDEFS AND STRUCTURES                            **/
/*                                                                         **/
/****************************************************************************/

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED VARIABLES                                 **/
/*                                                                         **/
/****************************************************************************/

#ifndef __UNISIM__UAPI__DEBUG__DEBUG__C_SRC
#endif

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED FUNCTIONS                                 **/
/*                                                                         **/
/****************************************************************************/

/**
 * Create a debug api handler from extended api description.
 *
 * @param eapi the extended api description.
 *
 * @return a debug api handler or 0 if error.
 */
UnisimDebugAPI usCreateDebugAPI(UnisimExtendedAPI eapi);

/**
 * Destroys a debug api handler.
 *
 * @param dapi The debug handler to destroy.
 */
void usDestroyDebugAPI(UnisimDebugAPI api);

/**
 * Set the context which will be handled to the breakpoint/watchpoint handlers.
 *
 * @param dapi The debug handler to configure.
 * @param context Pointer to the context that will be given to the handlers.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetContext(UnisimDebugAPI dapi,
		void *context);

/**
 * Set the breakpoint callback handler for the given debug handler.
 * The breakpoint will be called each time a breakpoint is found or under step
 * mode before the execution of any instruction.
 *
 * @param dapi The debug handler to configure.
 * @param callback Pointer to the breakpoint callback handler.
 *
 * @return True if the callback could be set, false otherwise.
 */
bool usDebugAPISetBreakpointHandler(UnisimDebugAPI dapi,
		void (* callback)(void *, uint64_t));

/**
 * Set the watchpoint callback handler for the given debug handler.
 *
 * @param dapi The debug handler to configure.
 * @param callback Pointer to the watchpoint callback handler.
 *
 * @return True if the callback could be set, false otherwise.
 */
bool usDebugAPISetWatchpointHandler(UnisimDebugAPI dapi,
		void (* callback)(void *, uint64_t, bool));

/**
 * Set the debug handler into step mode.
 * The breakpoint handler will be called before the execution of a new instruction.
 *
 * @param dapi The debug handler to configure.
 *
 * @return True if could be set into step mode, false otherwise.
 */
bool usDebugAPISetStepMode(UnisimDebugAPI dapi);

/**
 * Set the debug handler into continue mode.
 * The breakpoint handler will only be called when a breakpoint is found.
 *
 * @param dapi The debug handler to configure.
 *
 * @return True if could be set into step mode, false otherwise.
 */
bool usDebugAPISetContinueMode(UnisimDebugAPI dapi);

/**
 * Returns true if the debugger is in step mode.
 *
 * @param dapi The debug handler to request.
 *
 * @return True if the debug handler is in step mode, false otherwise.
 */
bool usDebugAPIIsModeStep(UnisimDebugAPI dapi);

/**
 * Return true if the debugger is in continue mode.
 *
 * @param dapi The debug handler to request.
 *
 * @return True if the debug handler is in continue mode, false otherwise.
 */
bool usDebugAPIIsModeContinue(UnisimDebugAPI dapi);

/**
 * Get the address of the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param str [in] A null terminated string with the symbol.
 * @param addr [in,out] The address of the resolved symbol.
 *
 * @return True if the debugger could handle the symbol, false otherwise.
 */
bool usDebugAPIGetSymbolAddress(UnisimDebugAPI dapi,
		const char *str, uint64_t *addr);

/**
 * Get the address of the given file.
 *
 * @param dapi The ddebug handler to use.
 * @param str [in] A null terminated string with the file name.
 * @param addr [in,out] The address of the resolved file name.
 *
 * @return True if the debugger could handle the symbol, false otherwise.
 */
bool usDebugAPIGetFileSystemAddress(UnisimDebugAPI dapi,
		const char *str, uint64_t *addr);

/**
 * Request the debugger if a breakpoint for the given address exists.
 *
 * @param dapi The debug handler to use.
 * @param addr The breakpoint address to check.
 *
 * @return True if there is a breakpoint at the given address, false otherwise.
 */
bool usDebugAPIHasBreakpoint(UnisimDebugAPI dapi, uint64_t addr);

/**
 * Set a breakpoint at the given address.
 *
 * @param dapi The debug handler to use.
 * @param addr The address at which the breakpoint should be set.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetBreakpoint(UnisimDebugAPI dapi, uint64_t addr);

/**
 * Delete an existing breakpoint using its address.
 *
 * @param dapi The debug handler to use.
 * @param addr The address at which a breakpoint should be set.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteBreakpoint(UnisimDebugAPI dapi, uint64_t addr);

/**
 * Request the debugger if a breakpoint from the given symbol exists.
 *
 * @param dapi The debug handler to use.
 * @param str [in] The symbol at which the breakpoint should be set.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIHasBreakpointSymbol(UnisimDebugAPI dapi, const char *str);

/**
 * Set a breakpoint at the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param str [in] The symbol at which a breakpoint should be set.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetBreakpointSymbol(UnisimDebugAPI dapi, const char *str);

/**
 * Delete an existing breakpoint using the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param str [in] The symbol at which a breakpoint should be set.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteBreakpointSymbol(UnisimDebugAPI dapi, const char *str);

/**
 * Set a watchpoint at the given address.
 *
 * @param dapi The debug handler to use.
 * @param addr The watchpoint address.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetWatchpoint(UnisimDebugAPI dapi, uint64_t addr, uint32_t size);

/**
 * Set a read watchpoint at the given address.
 *
 * @param dapi The debug handler to use.
 * @param addr The watchpoint address.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetReadWatchpoint(UnisimDebugAPI dapi, uint64_t addr, uint32_t size);

/**
 * Set a write watchpoint at the given address.
 *
 * @param dapi The debug handler to use.
 * @param addr The watchpoint address.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetWriteWatchpoint(UnisimDebugAPI dapi, uint64_t addr, uint32_t size);

/**
 * Delete any watchpoint at the given address/size.
 *
 * @param dapi The debug handler to use.
 * @param addr The watchpoint address.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteWatchpoint(UnisimDebugAPI dapi, uint64_t addr, uint32_t size);

/**
 * Delete any read watchpoint at the given address/size.
 *
 * @param dapi The debug handler to use.
 * @param addr The watchpoint address.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteReadWatchpoint(UnisimDebugAPI dapi, uint64_t addr, uint32_t size);

/**
 * Delete any write watchpoint at the given address/size.
 *
 * @param dapi The debug handler to use.
 * @param addr The watchpoint address.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteWriteWatchpoint(UnisimDebugAPI dapi, uint64_t addr, uint32_t size);

/**
 * Set a watchpoint at the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param [in] str The watchpoint symbol.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetWatchpointSymbol(UnisimDebugAPI dapi, const char *str, uint32_t size);

/**
 * Set a read watchpoint at the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param [in] str The watchpoint symbol.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetReadWatchpointSymbol(UnisimDebugAPI dapi, const char *str, uint32_t size);

/**
 * Set a write watchpoint at the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param [in] str The watchpoint symbol.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPISetWriteWatchpointSymbol(UnisimDebugAPI dapi, const char *str, uint32_t size);

/**
 * Delete any watchpoint at the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param [in] str The watchpoint symbol.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteWatchpointSymbol(UnisimDebugAPI dapi, const char *str, uint32_t size);

/**
 * Delete any read watchpoint at the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param [in] str The watchpoint symbol.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteReadWatchpointSymbol(UnisimDebugAPI dapi, const char *str, uint32_t size);

/**
 * Delete any write watchpoint at the given symbol.
 *
 * @param dapi The debug handler to use.
 * @param [in] str The watchpoint symbol.
 * @param size The size of the watchpoint.
 *
 * @return True on success, false otherwise.
 */
bool usDebugAPIDeleteWriteWatchpointSymbol(UnisimDebugAPI dapi, const char *str, uint32_t size);

}

#endif

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/

