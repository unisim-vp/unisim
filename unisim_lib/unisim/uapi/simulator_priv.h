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

#ifndef __UNISIM__UAPI__SIMULATOR_PRIV__INCLUDED
#define __UNISIM__UAPI__SIMULATOR_PRIV__INCLUDED

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include "unisim/uapi/simulator.h"

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

#ifndef __UNISIM__UAPI__SIMULATOR_PRIV__C_SRC
#endif

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED FUNCTIONS                                 **/
/*                                                                         **/
/****************************************************************************/

/**
 * Register a variable into the given simulator.
 * The given variable is registered (the pointer is keeped to indicate it is being
 * used by the api user) in the given simulator.
 *
 * @param simulator The simulator into which the variable is going to be registered.
 * @param variable The variable to register.
 */
void usSimulatorRegisterVariable(UnisimSimulator simulator, 
		UnisimVariable variable);

/**
 * Unregister the given variable from the simulator.
 * The given variable is removed from the list of variables registered (that is, in
 * use) of the given simulator.
 *
 * @param simulator The simulator from which the variable is going to be unregistered.
 * @param variable The variable to unregister.
 */
void usSimulatorUnregisterVariable(UnisimSimulator simulator,
		UnisimVariable variable);

/**
 * Register an extended api object into the given simulator.
 * The given extended api object is registered (its pointer is keeped to indicate it is
 * being used by the simulator api user) in the given simulator.
 *
 * @param simulator The simulator into which the extended api object is going to be registered.
 * @param eapi The extended api object to register.
 */
void usSimulatorRegisterExtendedAPI(UnisimSimulator simulator,
		UnisimExtendedAPI eapi);

/**
 * Unregister the given extended api object from the simulator.
 * The given extended api object is removed from the list of extended api objects registered
 * (that is, in use) of the given simulator.
 *
 * @param simulator The simulator from which the extended api object is going to be unregistered.
 * @param eapi The extended api object to unregister.
 */
void usSimulatorUnregisterExtendedAPI(UnisimSimulator simulator,
		UnisimExtendedAPI eapi);

}

#endif

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/



