/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : variable.h                                                  **
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

#ifndef __UNISIM__UAPI__VARIABLE__INCLUDED
#define __UNISIM__UAPI__VARIABLE__INCLUDED

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include "unisim/uapi/types.h"

extern "C"
{

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

typedef enum
{
	UNISIM_VARIABLE_TYPE_NONE,
	UNISIM_VARIABLE_TYPE_VOID,
	UNISIM_VARIABLE_TYPE_ARRAY,
	UNISIM_VARIABLE_TYPE_PARAMETER,
	UNISIM_VARIABLE_TYPE_STATISTIC,
	UNISIM_VARIABLE_TYPE_REGISTER,
	UNISIM_VARIABLE_TYPE_FORMULA
} UnisimVariableType;

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED VARIABLES                                 **/
/**                                                                        **/
/****************************************************************************/

#ifndef __UNISIM__UAPI__VARIABLE__C_SRC
#endif

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED FUNCTIONS                                 **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
void usDestroyVariable(UnisimVariable variable);
/****************************************************************************/
/*
 * Destroys the given variable handler
 *
 * Parameters:
 *   - var: the variable handler to destroy.
 *
 * Returns: None.
 */

/****************************************************************************/
UnisimVariableType usVariableGetType(UnisimVariable variable);
/****************************************************************************/
/*
 * Gets the type of the variable as defined by VariableType.
 *
 * Parameters:
 *   - var: the variable to inspect.
 *
 * Returns: the type of the variable as defined by VariableType.
 */

/****************************************************************************/
const char *usVariableGetName(UnisimVariable variable);
/****************************************************************************/
/*
 * Gets the name of the given variable handler.
 *
 * Parameters:
 *   - var: the variable to handle.
 *
 * Returns: A null terminated string with the name of the given variable.
 */

/****************************************************************************/
bool usVariableVisible(UnisimVariable variable);
/****************************************************************************/
/*
 * Is the variable visible.
 *
 * Parameters:
 *   - var: the variable to handle.
 *
 * Returns: returns true if the variable is visible, false if not. Note that
 *          it may return false also if the variable is corrupted.
 */

/****************************************************************************/
bool usVariableMutable(UnisimVariable varable);
/****************************************************************************/
/*
 * Is the variable mutable.
 *
 * Parameters:
 *   - var: the variable to handle.
 *
 * Returns: returns true if the variable is mutable, false if not. Note that
 *          it may return false also if the variable is corrupted.
 */

/****************************************************************************/
bool usVariableSerializable(UnisimVariable variable);
/****************************************************************************/
/*
 * Is the variable serializable.
 *
 * Parameters:
 *   - var: the variable to handle.
 *
 * Returns: returns true if the variable is serializable, false if not. Note 
 *          that it may return false also if the variable is corrupted.
 */

/****************************************************************************/
const char *usVariableGetValueAsString(UnisimVariable variable);
/****************************************************************************/
/*
 * Gets the value of the given variable as a null terminated string.
 *
 * Parameters:
 *   - var: the variable to handle.
 *
 * Returns: A null terminated string with the value of the given variable.
 */

}

#endif

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/


