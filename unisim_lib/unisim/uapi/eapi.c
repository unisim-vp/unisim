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

#define __UNISIM__UAPI__EAPI__C_SRC

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

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

/****************************************************************************/
/*                                                                         **/
/*                      PROTOTYPES OF LOCAL FUNCTIONS                      **/
/*                                                                         **/
/****************************************************************************/

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

void usDestroyExtendedAPI(UnisimExtendedAPI eapi)
{
	if ( eapi == 0 ) return;
	
	if ( eapi->usDestroyAPI != 0 )
		eapi->usDestroyAPI(eapi);
	else
	{
		usSimulatorUnregisterExtendedAPI(eapi->simulator, eapi);
		eapi->api = 0;
		eapi->simulator = 0;
		free(eapi);
	}
}

const char *usExtendedAPIGetName(UnisimExtendedAPI eapi)
{
	if ( eapi == 0 ) return 0;

	return eapi->api->GetName();
}

const char *usExtendedAPIGetTypeName(UnisimExtendedAPI eapi)
{
	if ( eapi == 0 ) return 0;

	return eapi->api->GetAPITypeName();
}

/****************************************************************************/
/*                                                                         **/
/*                  PRIVATELY EXPORTED FUNCTIONS                           **/
/*                                                                         **/
/****************************************************************************/

UnisimExtendedAPI usCreateExtendedAPI(UnisimSimulator simulator,
		unisim::kernel::api::APIBase *unisimApi)
{
	if ( simulator == 0 ) return 0;
	if ( unisimApi == 0 ) return 0;

	UnisimExtendedAPI eapi = (UnisimExtendedAPI)malloc(sizeof(_UnisimExtendedAPI));
	if ( eapi == 0 ) return 0;

	eapi->simulator = simulator;
	eapi->api = unisimApi;
	eapi->usDestroyAPI = 0;
	eapi->usDestroyUnregisteredAPI = 0;
	usSimulatorRegisterExtendedAPI(simulator, eapi);
	return eapi;
}

void usDestroyUnregisteredExtendedAPI(UnisimExtendedAPI eapi)
{
	if ( eapi == 0 ) return;

	if ( eapi->usDestroyUnregisteredAPI != 0 )
		eapi->usDestroyUnregisteredAPI(eapi);
	else
	{
		eapi->api = 0;
		eapi->simulator = 0;
		free(eapi);
	}
}

unisim::kernel::api::APIBase *usExtendedAPIGetUnisimAPI(UnisimExtendedAPI eapi)
{
	if ( eapi == 0 ) return 0;

	return eapi->api;
}

UnisimSimulator usExtendedAPIGetSimulator(UnisimExtendedAPI eapi)
{
	if ( eapi == 0 ) return 0;

	return eapi->simulator;
}

/****************************************************************************/
/*                                                                         **/
/*                      LOCAL FUNCTIONS                                    **/
/*                                                                         **/
/****************************************************************************/

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/

