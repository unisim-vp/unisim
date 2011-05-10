/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : variable.c                                                  **
** version   : 1                                                           **
** date      : 4/5/2011                                                    **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2011, Commissariat a l'Energie Atomique (CEA)             **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************

VERSION HISTORY:
----------------

Version     : 1
Date        : 4/5/2011
Revised by  : Daniel Gracia Perez
Description : Original version.

*/

#define __UNISIM__UAPI__VARIABLE__C_SRC

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include <string>
#include "simulator.hh"

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

struct _UniVariable
{
	unisim::kernel::service::VariableBase *var;
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
void DestroyVariable(UniVariable var)
/****************************************************************************/
{
	if ( var == 0 ) return;

	var->var = 0;
	free(var);
}

/****************************************************************************/
VariableType GetType(UniVariable var)
/****************************************************************************/
{
	if ( var == 0) return UNIVAR_NONE;

	switch ( var->var->GetType() )
	{
		case unisim::kernel::service::VariableBase::VAR_VOID:
			return UNIVAR_VOID;
			break;
		case unisim::kernel::service::VariableBase::VAR_ARRAY:
			return UNIVAR_ARRAY;
			break;
		case unisim::kernel::service::VariableBase::VAR_PARAMETER:
			return UNIVAR_PARAMETER;
			break;
		case unisim::kernel::service::VariableBase::VAR_STATISTIC:
			return UNIVAR_STATISTIC;
			break;
		case unisim::kernel::service::VariableBase::VAR_REGISTER:
			return UNIVAR_REGISTER;
			break;
		case unisim::kernel::service::VariableBase::VAR_FORMULA:
			return UNIVAR_FORMULA;
			break;
		default:
			return UNIVAR_NONE;
			break;
	}

	return UNIVAR_NONE;
}

/****************************************************************************/
const char *GetName(UniVariable var)
/****************************************************************************/
{
	if ( var == 0 ) return 0;

	return var->var->GetName();
}

/****************************************************************************/
bool IsVisible(UniVariable var)
/****************************************************************************/
{
	if ( var == 0 ) return 0;

	return var->var->IsVisible();
}

/****************************************************************************/
bool IsMutable(UniVariable var)
/****************************************************************************/
{
	if ( var == 0 ) return 0;

	return var->var->IsMutable();
}

/****************************************************************************/
bool IsSerializable(UniVariable var)
/****************************************************************************/
{
	if ( var == 0 ) return 0;

	return var->var->IsSerializable();
}

/****************************************************************************/
const char *GetValueAsString(UniVariable var)
/****************************************************************************/
{
	if ( var == 0 ) return 0;

	const char *value = ((std::string)*(var->var)).c_str();
	return value;
}

/****************************************************************************/
/**                                                                        **/
/**                 PRIVATELY EXPORTED FUNCTIONS                           **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
UniVariable CreateVariable(unisim::kernel::service::VariableBase *var)
/****************************************************************************/
{
	if ( var == 0 ) return 0;

	UniVariable univar = (UniVariable)malloc(sizeof(_UniVariable));
	if ( univar == 0 ) return 0;

	univar->var = var;
	return univar;
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

