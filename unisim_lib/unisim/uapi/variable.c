/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : variable.c                                                  **
** version   : 1                                                           **
** date      : 12/5/2011                                                   **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2011, Commissariat a l'Energie Atomique (CEA)             **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************

*/

#define __UNISIM__UAPI__VARIABLE__C_SRC

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include <string>

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

struct _UnisimVariable
{
	unisim::kernel::service::VariableBase *variable;
	UnisimSimulator simulator;
};

/****************************************************************************/
/**                                                                        **/
/**                     PROTOTYPES OF LOCAL FUNCTIONS                      **/
/**                                                                        **/
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
void usDestroyVariable(UnisimVariable variable)
/****************************************************************************/
{
	if ( variable == 0 ) return;

	usSimulatorUnregisterVariable(variable->simulator, variable);
	variable->variable = 0;
	variable->simulator = 0;
	free(variable);
}

/****************************************************************************/
UnisimVariableType usVariableGetType(UnisimVariable variable)
/****************************************************************************/
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

/****************************************************************************/
const char *usVariableGetName(UnisimVariable variable)
/****************************************************************************/
{
	if ( variable == 0 ) return 0;

	return variable->variable->GetName();
}

/****************************************************************************/
bool usVariableVisible(UnisimVariable variable)
/****************************************************************************/
{
	if ( variable == 0 ) return 0;

	return variable->variable->IsVisible();
}

/****************************************************************************/
bool usVariableMutable(UnisimVariable variable)
/****************************************************************************/
{
	if ( variable == 0 ) return 0;

	return variable->variable->IsMutable();
}

/****************************************************************************/
bool usVariableSerializable(UnisimVariable variable)
/****************************************************************************/
{
	if ( variable == 0 ) return 0;

	return variable->variable->IsSerializable();
}

/****************************************************************************/
const char *usVariableGetValueAsString(UnisimVariable variable)
/****************************************************************************/
{
	if ( variable == 0 ) return 0;

	const char *value = ((std::string)*(variable->variable)).c_str();
	return value;
}

/****************************************************************************/
/**                                                                        **/
/**                 PRIVATELY EXPORTED FUNCTIONS                           **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
UnisimVariable usCreateVariable(UnisimSimulator simulator,
		unisim::kernel::service::VariableBase *unisimVariable)
/****************************************************************************/
{
	if ( simulator == 0 ) return 0;
	if ( unisimVariable == 0 ) return 0;

	UnisimVariable variable = (UnisimVariable)malloc(sizeof(_UnisimVariable));
	if ( variable == 0 ) return 0;

	variable->simulator = simulator;
	variable->variable = unisimVariable;
	usSimulatorRegisterVariable(simulator, variable);
	return variable;
}

/****************************************************************************/
void usDestroyUnregisteredVariable(UnisimVariable variable)
/****************************************************************************/
{
	if ( variable == 0 ) return;

	variable->variable = 0;
	variable->simulator = 0;
	free(variable);
}

/****************************************************************************/
/**                                                                        **/
/**                     LOCAL FUNCTIONS                                    **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

