/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : simulator.h                                                 **
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
              * Defined basic simulator interface.

*/

#ifndef __UNISIM__UAPI__SIMULATOR__INCLUDED
#define __UNISIM__UAPI__SIMULATOR__INCLUDED

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include "unisim/uapi/types.h"
#include "unisim/uapi/variable.h"

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
	OK,
	WARNING,
	ERROR
} UniSimulatorSetupStatus;

typedef enum
{
	NONE,       // the simulator has not been created
	CREATED,    // the simulator has been created
	SETUP,      // the setup phase has been successfully done
	RUNNING,    // the simulator is running (within Run method)
	STOPPED,    // the simulator is stopped
	FINISHED    // the simulation has finished
} UniSimulatorStatus;

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED VARIABLES                                 **/
/**                                                                        **/
/****************************************************************************/

#ifndef __UNISIM__UAPI__SIMULATOR__C_SRC
#endif

/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED FUNCTIONS                                 **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
UniSimulator CreateSimulator(char *xml_file, char **options);
/****************************************************************************/
/*
 * Creates a simulator.
 *
 * Parameters:
 *   - xml_file: a null terminated string with the name of the xml file
 *          containing the configuration to be used. It can be omitted by
 *          setting it to 0.
 *   - options: a null terminated array of null terminated strings defining
 *          the values of the different parameters in the form of 
 *          "<parameter_name>=<value>". It can be omitted by setting it to 0.
 *
 * Returns: A pointer to the new simulator.
 */

/****************************************************************************/
bool DestroySimulator(UniSimulator sim);
/****************************************************************************/
/*
 * Destroy the simulator by freeing the simulator resources. Note that this
 * method will not work within the context of Run method (i.e., in a callback
 * called when executing the Run method). Instead Stop the simulator and get
 * out of the Run method where you can destroy it.
 *
 * Parameters:
 *   - sim: the simulator to destroy.
 *
 * Returns: true if the simulator was destroyed, false otherwise.
 */

/****************************************************************************/
UniSimulatorStatus GetSimulatorStatus(UniSimulator sim);
/****************************************************************************/
/*
 * Get the status of the simulator.
 *
 * Parameters:
 *   - sim: the simulator.
 *
 * Returns: the status of the simulator.
 */

/****************************************************************************/
UniSimulatorSetupStatus Setup(UniSimulator sim);
/****************************************************************************/
/*
 * Performs the setup of face of the simulator to initialize the different
 * components of the simulator.
 *
 * Parameters:
 *   - sim: a pointer to the simulator to setup.
 *
 * Returns: Whether the simulator was correctly setup or not in the form
 *          of an UniSimulatorSetupStatus:
 *          OK_TO_START       Success and ready to start simulation
 *          OK_DONT_START     Success but do not start simulation
 *          WARNING           A failure was found
 *          ERROR             A fatal failure was found
 */

/****************************************************************************/
const char *Version(UniSimulator sim);
/****************************************************************************/
/*
 * Returns the version description of the handled simulator.
 *
 * Parameters:
 *   - sim: a pointer to the simulator to setup.
 *
 * Returns: A null terminated string (char *) with the version description
 *          of the simulator.
 */

/****************************************************************************/
bool Run(UniSimulator sim);
/****************************************************************************/
/*
 * Run the simulator. This method should just be called once at the beginning
 * of the simulation.
 *
 * Parameters:
 *   - sim: pointer to the simulator to run.
 *
 * Returns: true if the simulation was launched, false otherwise. For example
 *          it returns false if the user tries to call Run two times, or if
 *          the setup was not completed.
 */

/****************************************************************************/
void Stop(UniSimulator sim);
/****************************************************************************/
/*
 * Stop the simulator. This method can be called once the Run method has
 * finished or when any kind of callback is called in order to properly
 * finish the simulation.
 *
 * Parameters:
 *   - sim: pointer to the simulator to stop.
 *
 * Returns: None.
 */

/****************************************************************************/
UniVariable GetVariable(UniSimulator sim, const char *varname);
/****************************************************************************/
/*
 * Returns a variable handler to the variable requested from the given
 * simulator.
 *
 * Parameters:
 *   - sim: pointer to the simulator.
 *   - varname: null terminated string containing the variable full name.
 *
 * Returns: A variable handler.
 */

/****************************************************************************/
UniVariable GetVariableWithType(UniSimulator sim, const char *varname, 
		VariableType type);
/****************************************************************************/
/*
 * Returns a variable handler to the requested parameter from the given
 * simulator.
 *
 * Parameters:
 *   - sim: pointer to the simulator.
 *   - varname: null terminated string containing the parameter full name.
 *   - type: the type of the requested variable as defined by VariableType.
 *
 * Returns: A variable handler.
 */

/****************************************************************************/
UniVariable *GetVariables(UniSimulator sim);
/****************************************************************************/
/*
 * Returns a list of all the variables (in the form of variable handlers)
 * avalaible in the simulator.
 *
 * Parameters:
 *   - sim: pointer to the simulator.
 *
 * Returns: A null terminated list of the available variables.
 */

/****************************************************************************/
UniVariable *GetVariablesWithType(UniSimulator sim, VariableType type);
/****************************************************************************/
/*
 * Returns a list of all the variables (in the form of variable handlers)
 * avalaible in the simulator with the given type as defined by VariableType.
 *
 * Parameters:
 *   - sim: pointer to the simulator.
 *   - type: the type of the requested variables as defined by Variabletype.
 *
 * Returns: A null terminated list of the available variables with the requested
 *          type.
 */

/* TODO:
 * Methods to know if the setup has been done, the simulator is running,
   the simulator has finished its execution, the simulator is stopped.
 * Set/unset trap handlers, for example:
   bool SetTrapHandler(UniSimulator *, (void *)(* handler)(void *context));
   bool RemoveTrapHandler(UniSimulator *, (void *)(* handler)(void *context));
 * Generic API interface:
   bool HasAPI();
   API GetAPI(API_type);
 */

}

#endif

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

