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

#define __UNISIM__UAPI__VARIABLE__C_SRC

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include <string>

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

struct _UnisimVariable
{
	unisim::kernel::service::VariableBase *variable;
	UnisimVariableListener *listener;	
	UnisimSimulator simulator;
};

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

void usDestroyVariable(UnisimVariable variable)
{
	if ( variable == 0 ) return;

	if ( variable->listener != 0 ) delete variable->listener;
	usSimulatorUnregisterVariable(variable->simulator, variable);
	variable->variable = 0;
	variable->simulator = 0;
	variable->listener = 0;
	free(variable);
	variable = 0;
}

UnisimSimulator usVariableGetAssociatedSimulator(UnisimVariable variable)
{
	if ( variable == 0 )
		return 0;

	return variable->simulator;
}

UnisimVariableType usVariableGetType(UnisimVariable variable)
{
	if ( variable == 0) return UNISIM_VARIABLE_TYPE_NONE;

	switch ( variable->variable->GetType() )
	{
		case unisim::kernel::service::VariableBase::VAR_VOID:
			return UNISIM_VARIABLE_TYPE_VOID;
			break;
		case unisim::kernel::service::VariableBase::VAR_ARRAY:
			return UNISIM_VARIABLE_TYPE_ARRAY;
			break;
		case unisim::kernel::service::VariableBase::VAR_PARAMETER:
			return UNISIM_VARIABLE_TYPE_PARAMETER;
			break;
		case unisim::kernel::service::VariableBase::VAR_STATISTIC:
			return UNISIM_VARIABLE_TYPE_STATISTIC;
			break;
		case unisim::kernel::service::VariableBase::VAR_REGISTER:
			return UNISIM_VARIABLE_TYPE_REGISTER;
			break;
		case unisim::kernel::service::VariableBase::VAR_FORMULA:
			return UNISIM_VARIABLE_TYPE_FORMULA;
			break;
		default:
			return UNISIM_VARIABLE_TYPE_NONE;
			break;
	}

	return UNISIM_VARIABLE_TYPE_NONE;
}

const char *usVariableGetName(UnisimVariable variable)
{
	if ( variable == 0 ) return 0;

	return variable->variable->GetName();
}

const char *usVariableGetDataTypeName(UnisimVariable variable)
{
	if ( variable == 0 ) return 0;

	return variable->variable->GetTypeName();
}

bool usVariableVisible(UnisimVariable variable)
{
	if ( variable == 0 ) return false;

	return variable->variable->IsVisible();
}

bool usVariableMutable(UnisimVariable variable)
{
	if ( variable == 0 ) return false;

	return variable->variable->IsMutable();
}

bool usVariableSerializable(UnisimVariable variable)
{
	if ( variable == 0 ) return false;

	return variable->variable->IsSerializable();
}

template<class T>
T usVariableGetValue(UnisimVariable variable)
{
	if ( variable == 0 ) return (T)0;

	T value = (T)*(variable->variable);
	return value;
}

template<class T>
void usVariableSetValue(UnisimVariable variable, T value)
{
	if ( variable == 0 ) return;

	*(variable->variable) = value;
}

bool usVariableGetValueAsBoolean(UnisimVariable variable)
{
	return usVariableGetValue<bool>(variable);
}

void usVariableSetValueFromBoolean(UnisimVariable variable, bool value)
{
	usVariableSetValue<bool>(variable, value);
}

unsigned char usVariableGetValueAsUChar(UnisimVariable variable)
{
	return usVariableGetValue<unsigned char>(variable);
}

void usVariableSetValueFromUChar(UnisimVariable variable, unsigned char value)
{
	usVariableSetValue<unsigned char>(variable, value);
}

char usVariableGetValueAsChar(UnisimVariable variable)
{
	return usVariableGetValue<char>(variable);
}

void usVariableSetValueFromChar(UnisimVariable variable, char value)
{
	usVariableSetValue<char>(variable, value);
}

unsigned short usVariableGetValueAsUShort(UnisimVariable variable)
{
	return usVariableGetValue<unsigned short>(variable);
}

void usVariableSetValueFromUShort(UnisimVariable variable, unsigned short value)
{
	usVariableSetValue<unsigned short>(variable, value);
}

short usVariableGetValueAsShort(UnisimVariable variable)
{
	return usVariableGetValue<short>(variable);
}

void usVariableSetValueFromShort(UnisimVariable variable, short value)
{
	usVariableSetValue<short>(variable, value);
}

unsigned int usVariableGetValueAsUInt(UnisimVariable variable)
{
	return usVariableGetValue<unsigned int>(variable);
}

void usVariableSetValueFromUInt(UnisimVariable variable, unsigned int value)
{
	usVariableSetValue<unsigned int>(variable, value);
}

int usVariableGetValueAsInt(UnisimVariable variable)
{
	return usVariableGetValue<int>(variable);
}

void usVariableSetValueFromInt(UnisimVariable variable, int value)
{
	usVariableSetValue<int>(variable, value);
}

unsigned long usVariableGetValueAsULong(UnisimVariable variable)
{
	return usVariableGetValue<unsigned long>(variable);
}

void usVariableSetValueFromULong(UnisimVariable variable, unsigned long value)
{
	usVariableSetValue<unsigned long>(variable, value);
}

long usVariableGetValueAsLong(UnisimVariable variable)
{
	return usVariableGetValue<long>(variable);
}

void usVariableSetValueFromLong(UnisimVariable variable, long value)
{
	usVariableSetValue<long>(variable, value);
}

unsigned long long usVariableGetValueAsULongLong(UnisimVariable variable)
{
	return usVariableGetValue<unsigned long long>(variable);
}

void usVariableSetValueFromULongLong(UnisimVariable variable, 
		unsigned long long value)
{
	usVariableSetValue<unsigned long long>(variable, value);
}

long long usVariableGetValueAsLongLong(UnisimVariable variable)
{
	return usVariableGetValue<long long>(variable);
}

void usVariableSetValueFromLongLong(UnisimVariable variable, long long value)
{
	usVariableSetValue<long long>(variable, value);
}

float usVariableGetValueAsFloat(UnisimVariable variable)
{
	return usVariableGetValue<float>(variable);
}

void usVariableSetValueFromFloat(UnisimVariable variable, float value)
{
	usVariableSetValue<float>(variable, value);
}

double usVariableGetValueAsDouble(UnisimVariable variable)
{
	return usVariableGetValue<double>(variable);
}

void usVariableSetValueFromDouble(UnisimVariable variable, double value)
{
	usVariableSetValue<double>(variable, value);
}

const char *usVariableGetValueAsString(UnisimVariable variable)
{
	if ( variable == 0 ) return 0;

	const char *value = ((std::string)*(variable->variable)).c_str();
	return value;
}

void usVariableSetValueFromString(UnisimVariable variable, const char *value)
{
	usVariableSetValue<const char *>(variable, value);
}

bool usVariableSetListener(UnisimVariable variable, 
		void (*listener)(UnisimVariable context))
{
	if ( variable == 0 ) return false;
	if ( variable->listener != 0 ) return false;

	variable->listener = new UnisimVariableListener(variable, listener);

	if ( variable->listener == 0 ) return false;

	return true;
}

void usVariableRemoveListener(UnisimVariable variable)
{
	if ( variable == 0 ) return;
	if ( variable->listener == 0 ) return;

	delete variable->listener;

	variable->listener = 0;
}

/****************************************************************************/
/*                                                                         **/
/*                  PRIVATELY EXPORTED FUNCTIONS                           **/
/*                                                                         **/
/****************************************************************************/

UnisimVariable usCreateVariable(UnisimSimulator simulator,
		unisim::kernel::service::VariableBase *unisimVariable)
{
	if ( simulator == 0 ) return 0;
	if ( unisimVariable == 0 ) return 0;

	UnisimVariable variable = (UnisimVariable)malloc(sizeof(_UnisimVariable));
	if ( variable == 0 ) return 0;

	variable->simulator = simulator;
	variable->variable = unisimVariable;
	variable->listener = 0;
	usSimulatorRegisterVariable(simulator, variable);
	return variable;
}

void usDestroyUnregisteredVariable(UnisimVariable variable)
{
	if ( variable == 0 ) return;

	if ( variable->listener != 0 ) delete variable->listener;
	variable->variable = 0;
	variable->simulator = 0;
	variable->listener = 0;
	free(variable);
}

UnisimVariableListener ::
UnisimVariableListener(UnisimVariable _variable,
		void (* _listener)(UnisimVariable context))
	: unisim::kernel::service::VariableBaseListener()
	, listener(_listener)
	, variable(_variable)
{
	variable->variable->AddListener(this);
}

UnisimVariableListener ::
~UnisimVariableListener()
{
	variable->variable->RemoveListener(this);
	listener = 0;
	variable = 0;
}

void 
UnisimVariableListener ::
VariableBaseNotify(const unisim::kernel::service::VariableBase *unisimVariable)
{
	(*listener)(variable);
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

