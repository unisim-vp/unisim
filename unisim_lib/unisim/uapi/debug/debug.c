/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : debug/debug.c                                               **
** version   : 1                                                           **
** date      : 31/05/2011                                                  **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2011, Commissariat a l'Energie Atomique (CEA)             **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************

*/

#define __UNISIM__UAPI__DEBUG__DEBUG__C_SRC

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include <string.h>

#include "unisim/uapi/uapi_priv.h"

/****************************************************************************/
/**                                                                        **/
/**                     DEFINITIONS AND MACROS                             **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                     TYPEDEFS AND STRUCTURES                            **/
/**                                                                        **/
/****************************************************************************/

struct _UnisimDebugAPI
{
	struct _UnisimExtendedAPI eapi;
	unisim::api::debug::DebugAPI *api;
};

/****************************************************************************/
/**                                                                        **/
/**                     PROTOTYPES OF LOCAL FUNCTIONS                      **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
void usDestroyUnregisteredDebugAPI(UnisimDebugAPI api);
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED VARIABLES                                 **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                     GLOBAL VARIABLES                                   **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED FUNCTIONS                                 **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
UnisimDebugAPI usCreateDebugAPI(UnisimExtendedAPI eapi)
/****************************************************************************/
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

/****************************************************************************/
void usDestroyDebugAPI(UnisimDebugAPI api)
/****************************************************************************/
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
/**                                                                        **/
/**                 PRIVATELY EXPORTED FUNCTIONS                           **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                     LOCAL FUNCTIONS                                    **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
void usDestroyUnregisteredDebugAPI(UnisimDebugAPI api)
/****************************************************************************/
{
	if ( api == 0 ) return;

	api->eapi.api = 0;
	api->eapi.simulator = 0;
	api->eapi.usDestroyAPI = 0;
	api->api = 0;
	free(api);
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

