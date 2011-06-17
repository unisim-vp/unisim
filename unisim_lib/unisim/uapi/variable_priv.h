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

#ifndef __UNISIM__UAPI__VARIABLE_PRIV__INCLUDED
#define __UNISIM__UAPI__VARIABLE_PRIV__INCLUDED

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include "unisim/kernel/service/service.hh"
#include "unisim/uapi/variable.h"

extern "C"
{

/****************************************************************************/
/*                                                                         **/
/*                      DEFINITIONS AND MACROS                             **/
/*                                                                         **/
/****************************************************************************/

/**
 * Variable listener class used by the uapi to set listeners to unisim variables.
 * This class is required to bridge the gap from the unisim c++ code to the uapi
 * c code.
 */
class
UnisimVariableListener 
	: public unisim::kernel::service::VariableBaseListener
{
public:
	/**
	 * Constructor.
	 * 
	 * @param _variable The variable to which the listener should be attached.
	 * @param _listener Pointer to the function that will be called when the variable
	 *                  is modified.
	 *
	 * @return A UnisimVariableListener.
	 */
	UnisimVariableListener(UnisimVariable _variable,
			void (* _listener)(UnisimVariable context));

	/**
	 * Destructor.
	 */
	~UnisimVariableListener();

	/**
	 * Notify the unisim variable that the api variable has been modified (probably).
	 *
	 * @param unisimVariable The variable that should receive the notification.
	 */
	void VariableBaseNotify(const unisim::kernel::service::VariableBase *unisimVariable);

private:
	/**
	 * The function that will be called when the variable is modified.
	 */
	void (* listener)(UnisimVariable);
	/**
	 * The api variable attached to the given listener.
	 */
	UnisimVariable variable;
};

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

#ifndef __UNISIM__UAPI__VARIABLE_PRIV__C_SRC
#endif

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED FUNCTIONS                                 **/
/*                                                                         **/
/****************************************************************************/

/**
 * Creates an api compliant variable from the given variable.
 *
 * @param simulator The simulator to which the variable has to be associated.
 * @param unisimVariable The variable from which an api compliant variable must be created.
 *
 * @return The variable created from the given unisim variable.
 */
UnisimVariable usCreateVariable(UnisimSimulator simulator,
		unisim::kernel::service::VariableBase *unisimVariable);

/**
 * Destroy a variable without unregistering it from the simulator.
 *
 * @param variable The variable to destroy.
 */
void usDestroyUnregisteredVariable(UnisimVariable variable);

}

#endif

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/

