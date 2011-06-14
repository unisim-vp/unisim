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

#define __UNISIM__UAPI__DEBUG__DEBUG__C_SRC

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include <string.h>

#include "unisim/uapi/uapi_priv.h"

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

/**
 * Definition of the debug api state.
 */
struct _UnisimDebugAPI
{
	struct _UnisimExtendedAPI eapi; /**< Pointer to the generic extended api state */
	unisim::api::debug::DebugAPI *api; /**< Pointer to the c++ unisim debug api */
};

/****************************************************************************/
/*                                                                         **/
/*                      PROTOTYPES OF LOCAL FUNCTIONS                      **/
/*                                                                         **/
/****************************************************************************/

/**
 * Destroy an unregistered debug api object.
 * This method should only be called when the simulator is being destroyed without
 * having previously destroyed the debug api object itself.
 *
 * @param dapi The debug api object to destroy.
 */
void usDestroyUnregisteredDebugAPI(UnisimDebugAPI dapi);

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED VARIABLES                                 **/
/*                                                                         **/
/****************************************************************************/

/****************************************************************************/
/*                                                                         **/
/*                      GLOBAL VARIABLES                                   **/
/*                                                                         **/
/****************************************************************************/

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED FUNCTIONS                                 **/
/*                                                                         **/
/****************************************************************************/

UnisimDebugAPI usCreateDebugAPI(UnisimExtendedAPI eapi)
{
	if ( eapi == 0 ) return 0;

	if ( strcmp(usExtendedAPIGetTypeName(eapi), 
				unisim::api::debug::DebugAPI::DEBUGAPI_ID.c_str()) != 0 )
		return 0;

	unisim::kernel::api::APIBase *unisimApi = 
		usExtendedAPIGetUnisimAPI(eapi);
	UnisimSimulator simulator = usExtendedAPIGetSimulator(eapi);
	if ( (unisimApi == 0) || (simulator == 0) )
		return 0;

	UnisimDebugAPI debugApi = (UnisimDebugAPI)malloc(sizeof(struct _UnisimDebugAPI));
	if ( debugApi == 0 )
		return 0;

	debugApi->eapi.api = unisimApi;
	debugApi->eapi.simulator = simulator;
	debugApi->eapi.usDestroyAPI = (void (*)(UnisimExtendedAPI))usDestroyDebugAPI;
	debugApi->eapi.usDestroyUnregisteredAPI = (void (*)(UnisimExtendedAPI))usDestroyUnregisteredDebugAPI;
	debugApi->api = static_cast<unisim::api::debug::DebugAPI *>(unisimApi);
	usSimulatorRegisterExtendedAPI(simulator, (UnisimExtendedAPI)debugApi);
	
	return debugApi;
}

void usDestroyDebugAPI(UnisimDebugAPI api)
{
	if ( api == 0 ) return;

	usSimulatorUnregisterExtendedAPI(api->eapi.simulator, (UnisimExtendedAPI)api);
	api->eapi.api = 0;
	api->eapi.simulator = 0;
	api->eapi.usDestroyAPI = 0;
	api->api = 0;
	free(api);
}

/****************************************************************************/
/*                                                                         **/
/*                  PRIVATELY EXPORTED FUNCTIONS                           **/
/*                                                                         **/
/****************************************************************************/

/****************************************************************************/
/*                                                                         **/
/*                      LOCAL FUNCTIONS                                    **/
/*                                                                         **/
/****************************************************************************/

void usDestroyUnregisteredDebugAPI(UnisimDebugAPI api)
{
	if ( api == 0 ) return;

	api->eapi.api = 0;
	api->eapi.simulator = 0;
	api->eapi.usDestroyAPI = 0;
	api->api = 0;
	free(api);
}

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/

