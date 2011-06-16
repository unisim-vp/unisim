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
		void (* callback)(UnisimDebugAPI , uint64_t));

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

}

#endif

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/

