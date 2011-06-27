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

#ifndef __UNISIM__UAPI__EAPI_PRIV__INCLUDED
#define __UNISIM__UAPI__EAPI_PRIV__INCLUDED

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include "unisim/uapi/eapi.h"

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

/**
 * Definition of the extended api state.
 * It contains the pointer to the actual c++ unisim APIBase object and its associated
 * simulator.
 * Additionally it provides the pointers to the methods to destroy the specific
 * extended apis (like the debug api, etc).
 */
struct _UnisimExtendedAPI
{
	unisim::kernel::api::APIBase *api; /**< Pointer to the actual c++ unisim APIBase. */
	UnisimSimulator simulator; /**< Pointer to the simulator from which this extended api object was created. */
	void (*usDestroyAPI)(UnisimExtendedAPI); /**< Pointer to the specific extended api destroy method. */
	void (*usDestroyUnregisteredAPI)(UnisimExtendedAPI); /**< Pointer to the specific extended api destroy method when the extended api is not registered in the simulator. */
};

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED VARIABLES                                 **/
/*                                                                         **/
/****************************************************************************/

#ifndef __UNISIM__UAPI__EAPI_PRIV__C_SRC
#endif

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED FUNCTIONS                                 **/
/*                                                                         **/
/****************************************************************************/

/**
 * Creates an extended api object handler from the given c++ unisim APIBase and associates it to the simulator.
 *
 * @param simulator The simulator which is creating the extended api and to which it will be registered.
 * @param unisimAPI The c++ unisim APIBase to create the extended api object handler.
 *
 * @return A pointer to the created extended api on success. 0 if the extended api could not be created.
 */
UnisimExtendedAPI usCreateExtendedAPI(UnisimSimulator simulator,
		unisim::kernel::api::APIBase *unisimAPI);

/**
 * Destroy an unregistered extended api.
 * Note that this method should only be called when the extended api object is not registered, and thus
 * only from the simulator destructor.
 *
 * @param eapi The unregistered extended api to destroy.
 */
void usDestroyUnregisteredExtendedAPI(UnisimExtendedAPI eapi);

/**
 * Get the c++ unisim APIBase object from the given extended api.
 *
 * @param eapi The extended api object handler from which to obtain the unisim APIBase object.
 *
 * @return The c++ unisim APIBase object.
 */
unisim::kernel::api::APIBase *usExtendedAPIGetUnisimAPI(UnisimExtendedAPI eapi);

/**
 * Get the simulator to which the given extended api is associated.
 *
 * @param eapi The extended api object handler from which to obtain the simulator it is associated to.
 *
 * @return The associated simulator.
 */
UnisimSimulator usExtendedAPIGetSimulator(UnisimExtendedAPI eapi);

}

#endif

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/




