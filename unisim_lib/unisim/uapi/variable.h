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

#ifndef __UNISIM__UAPI__VARIABLE__INCLUDED
#define __UNISIM__UAPI__VARIABLE__INCLUDED

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

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

/**
 * Definition of the possible variable types.
 */
typedef enum
{
	UNISIM_VARIABLE_TYPE_NONE, /**< Special system type to indicate that the type is not defined. */
	UNISIM_VARIABLE_TYPE_VOID, /**< Special system type to indicate any type. */
	UNISIM_VARIABLE_TYPE_ARRAY, /**< Array type. */
	UNISIM_VARIABLE_TYPE_PARAMETER, /**< Parameter type. */
	UNISIM_VARIABLE_TYPE_STATISTIC, /**< Statistic type. */
	UNISIM_VARIABLE_TYPE_REGISTER, /**< Register type. */
	UNISIM_VARIABLE_TYPE_FORMULA /**< Formula type. */
} UnisimVariableType;

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED VARIABLES                                 **/
/*                                                                         **/
/****************************************************************************/

#ifndef __UNISIM__UAPI__VARIABLE__C_SRC
#endif

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED FUNCTIONS                                 **/
/*                                                                         **/
/****************************************************************************/

/**
 * Destroys the given variable handler
 *
 * @param variable The variable handler to destroy.
 */
void usDestroyVariable(UnisimVariable variable);

/**
 * Returns the simulator associtated to the variable.
 *
 * @param variable The variable to consider.
 *
 * @return The associated simulator.
 */
UnisimSimulator usVariableGetAssociatedSimulator(UnisimVariable variable);

/**
 * Gets the type of the variable as defined by VariableType.
 *
 * @param variable The variable to inspect.
 *
 * @return The type of the variable as defined by VariableType.
 */
UnisimVariableType usVariableGetType(UnisimVariable variable);

/**
 * Gets the name of the given variable handler.
 *
 * @param variable The variable to handle.
 *
 * @return A null terminated string with the name of the given variable.
 */
const char *usVariableGetName(UnisimVariable variable);

/**
 * Is the variable visible.
 *
 * @param variable The variable to handle.
 *
 * @return True if the variable is visible, false if not. Note that
 *         it may return false also if the variable is corrupted.
 */
bool usVariableVisible(UnisimVariable variable);

/**
 * Is the variable mutable.
 *
 * @param variable The variable to handle.
 *
 * @return True if the variable is mutable, false if not. Note that
 *         it may return false also if the variable is corrupted.
 */
bool usVariableMutable(UnisimVariable varable);

/**
 * Is the variable serializable.
 *
 * @param variable The variable to handle.
 *
 * @return True if the variable is serializable, false if not. Note 
 *         that it may return false also if the variable is corrupted.
 */
bool usVariableSerializable(UnisimVariable variable);

/**
 * Gets the value of the given variable as a null terminated string.
 *
 * @param variable The variable to handle.
 *
 * @return A null terminated string with the value of the given variable.
 */
const char *usVariableGetValueAsString(UnisimVariable variable);

/**
 * Sets the value of the given variable from a null terminated string.
 *
 * @param variable The variable to handle.
 * @param value The null terminated string containing the value.
 */
void usVariableSetValueFromString(UnisimVariable variable, const char *value);

/**
 * Gets the value of the given variable as unsigned long long.
 *
 * @param variable The variable to handle.
 *
 * @return Unsigned long long representation of the reguested variable.
 */
unsigned long long usVariableGetValueAsUnsignedLongLong(UnisimVariable variable);

/**
 * Sets the value of the given variable from a unsigned long long.
 *
 * @param variable The variable to handle.
 * @param value The unsigned long long value.
 */
void usVariableSetValueFromUnsignedLongLong(UnisimVariable variable, 
		unsigned long long value);

/**
 * Sets a function to call when the variable is modified (actually notified).
 *
 * @param variable The variable to handle.
 * @param listener The function to call when the variable is modified.
 *
 * @return True on success, false if could not set the handler (or if a
 *          handler already exists.
 */
bool usVariableSetListener(UnisimVariable variable, 
		void (* listener)(UnisimVariable context));

/**
 * Removes the current variable listener.
 *
 * @param variable The variable to handle.
 */
void usVariableRemoveListener(UnisimVariable variable);
}

#endif

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/

