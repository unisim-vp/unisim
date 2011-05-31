/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : variable_priv.h                                             **
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

#ifndef __UNISIM__UAPI__VARIABLE_PRIV__INCLUDED
#define __UNISIM__UAPI__VARIABLE_PRIV__INCLUDED

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include "unisim/kernel/service/service.hh"
#include "unisim/uapi/variable.h"

extern "C"
{

/****************************************************************************/
/**                                                                        **/
/**                     DEFINITIONS AND MACROS                             **/
/**                                                                        **/
/****************************************************************************/

class
UnisimVariableListener 
	: public unisim::kernel::service::VariableBaseListener
{
public:
	UnisimVariableListener(UnisimVariable _variable,
			void (* _listener)(UnisimVariable context));
//		: unisim::kernel::service::VariableBaseListener(_variable->variable)
//		, listener(_listener)
//		, variable(_variable)
//	{
//		variable->variable->AddListener(this);
//	}

	~UnisimVariableListener();
//	{
//		variable->variable->RemoveListener(this);
//		listener = 0;
//		variable = 0;
//	}
//
	void VariableBaseNotify(const unisim::kernel::service::VariableBase *unisimVariable);
//	{
//		(*listener)(variable);
//	}

private:
	void (* listener)(UnisimVariable);
	UnisimVariable variable;
};

/****************************************************************************/
/**                                                                        **/
/**                     TYPEDEFS AND STRUCTURES                            **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED VARIABLES                                 **/
/**                                                                        **/
/****************************************************************************/

#ifndef __UNISIM__UAPI__VARIABLE_PRIV__C_SRC
#endif

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED FUNCTIONS                                 **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
UnisimVariable usCreateVariable(UnisimSimulator simulator,
		unisim::kernel::service::VariableBase *unisimVariable);
/****************************************************************************/
/*
 * Creates an api compliant variable from the given variable.
 *
 * Parameters:
 *   - var: the variable from which an api compliant variable must be created.
 *
 * Returns: An api compliant variable.
 */

/****************************************************************************/
void usDestroyUnregisteredVariable(UnisimVariable variable);
/****************************************************************************/
/*
 * Destroy a variable without unregistering it from the simulator.
 *
 * Parameters:
 *   - var: the variable to destroy.
 *
 * Returns: None. 
 */

}

#endif

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

