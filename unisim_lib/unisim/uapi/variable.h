/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : variable.h                                                  **
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
	UNIVAR_NONE,
	UNIVAR_VOID,
	UNIVAR_ARRAY,
	UNIVAR_PARAMETER,
	UNIVAR_STATISTIC,
	UNIVAR_REGISTER,
	UNIVAR_FORMULA
} VariableType;

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
void DestroyVariable(UniVariable var);
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
VariableType GetType(UniVariable var);
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
const char *GetName(UniVariable var);
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
bool IsVisible(UniVariable var);
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
bool IsMutable(UniVariable var);
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
bool IsSerializable(UniVariable var);
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
const char *GetValueAsString(UniVariable var);
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


