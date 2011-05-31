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
UnisimSimulator usVariableGetAssociatedSimulator(UnisimVariable variable);
/****************************************************************************/
/*
 * Returns the simulator associtated to the variable.
 *
 * Parameters:
 *   - variable: the variable to consider.
 *
 * Returns: the associated simulator.
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

/****************************************************************************/
void usVariableSetValueFromString(UnisimVariable variable, const char *value);
/****************************************************************************/
/*
 * Sets the value of the given variable from a null terminated string.
 *
 * Parameters:
 *   - variable: the variable to handle.
 *   - value: the null terminated string containing the value.
 *
 * Returns: None.
 */

/****************************************************************************/
unsigned long long usVariableGetValueAsUnsignedLongLong(UnisimVariable variable);
/****************************************************************************/
/*
 * Gets the value of the given variable as unsigned long long.
 *
 * Parameters:
 *   - var: the variable to handle.
 *
 * Returns: unsigned long long representation of the reguested variable.
 */

/****************************************************************************/
void usVariableSetValueFromUnsignedLongLong(UnisimVariable variable, 
		unsigned long long value);
/****************************************************************************/
/*
 * Sets the value of the given variable from a unsigned long long.
 *
 * Parameters:
 *   - variable: the variable to handle.
 *   - value: the unsigned long long value.
 *
 * Returns: None.
 */

/****************************************************************************/
bool usVariableSetListener(UnisimVariable variable, 
		void (* listener)(UnisimVariable context));
/****************************************************************************/
/*
 * Sets a function to call when the variable is modified (actually notified).
 *
 * Parameters:
 *   - variable: the variable to handle.
 *   - listener: the function to call when the variable is modified.
 *
 * Returns: true on success, false if could not set the handler (or if a
 *          handler already exists.
 */

/****************************************************************************/
void usVariableRemoveListener(UnisimVariable variable);
/****************************************************************************/
/*
 * Removes the current variable listener.
 *
 * Parameters:
 *   - variable: the variable to handle.
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


