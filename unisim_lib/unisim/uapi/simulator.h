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

#ifndef __UNISIM__UAPI__SIMULATOR__INCLUDED
#define __UNISIM__UAPI__SIMULATOR__INCLUDED

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include "unisim/uapi/types.h"
#include "unisim/uapi/variable.h"

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
 * Definition of the different simulator setup status values.
 * Describes the setup status of a simulator after performing its setup.
 */
typedef enum
{
	UNISIM_SIMULATOR_SETUP_STATUS_OK, /**< The setup was successfully performed. */
	UNISIM_SIMULATOR_SETUP_STATUS_WARNING, /**< The setup was performed but one or more warning were issued. */
	UNISIM_SIMULATOR_SETUP_STATUS_ERROR /**< The setup could not be performed. */
} UnisimSimulatorSetupStatus;

/**
 * Definition of the different simulator status values.
 */
typedef enum
{
	UNISIM_SIMULATOR_STATUS_NONE,       /**< The simulator has not been created. */
	UNISIM_SIMULATOR_STATUS_CREATED,    /**< The simulator has been created. */
	UNISIM_SIMULATOR_STATUS_SETUP,      /**< The setup phase has been successfully done. */
	UNISIM_SIMULATOR_STATUS_RUNNING,    /**< The simulator is running (within the Run method). */
	UNISIM_SIMULATOR_STATUS_STOPPED,    /**< The simulator is stopped. */
	UNISIM_SIMULATOR_STATUS_FINISHED    /**< The simulation has finished. */
} UnisimSimulatorStatus;

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED VARIABLES                                 **/
/*                                                                         **/
/****************************************************************************/

#ifndef __UNISIM__UAPI__SIMULATOR__C_SRC
#endif

/****************************************************************************/
/*                                                                         **/
/*                      EXPORTED FUNCTIONS                                 **/
/*                                                                         **/
/****************************************************************************/

/**
 * Creates a simulator.
 *
 * @param configurationXmlFile A null terminated string with the name of the xml file
 *                             containing the configuration to be used. It can be omitted by
 *                             setting it to 0.
 * @param optionList A null terminated array of null terminated strings defining
 *                   the values of the different parameters in the form of 
 *                   "<parameter_name>=<value>". It can be omitted by setting it to 0.
 *
 * @return A pointer to the new simulator.
 */
UnisimSimulator usCreateSimulator(char *configurationXmlFile, 
		char **optionList);

/**
 * Destroy the simulator by freeing the simulator resources. 
 * Note that this method will not work within the context of Run method 
 * (i.e., in a callback called when executing the Run method).
 * Instead Stop the simulator and get out of the Run method where you 
 * can destroy it.
 *
 * @param simulator The simulator to destroy.
 *
 * @return True if the simulator was destroyed, false otherwise.
 */
bool usDestroySimulator(UnisimSimulator simulator);

/**
 * Get the status of the simulator.
 *
 * @param simulator The simulator.
 *
 * @return The status of the simulator.
 */
UnisimSimulatorStatus usSimulatorGetStatus(UnisimSimulator simulator);

/**
 * Performs the setup of face of the simulator to initialize the different
 * components of the simulator.
 *
 * @param simulator A pointer to the simulator to setup.
 *
 * @return Whether the simulator was correctly setup or not in the form
 *         of an UniSimulatorSetupStatus.
 */
UnisimSimulatorSetupStatus usSimulatorSetup(UnisimSimulator simulator);

/**
 * Returns the version description of the handled simulator.
 *
 * @param simulator A pointer to the simulator to setup.
 *
 * @return A null terminated string (char *) with the version description
 *         of the simulator.
 */
const char *usSimulatorGetVersion(UnisimSimulator simulator);

/**
 * Returns a list of the APIs provided by the simulator.
 *
 * @param simulator The simulator.
 *
 * @return A null terminated list of the APIs provided by the simulator.
 */
UnisimExtendedAPI *usSimulatorGetExtendedAPIList(UnisimSimulator simulator);

/**
 * Run the simulator. 
 * This method should just be called once at the beginning of the simulation.
 *
 * @param simulator Pointer to the simulator to run.
 *
 * @return True if the simulation was launched, false otherwise. For example
 *         it returns false if the user tries to call Run two times, or if
 *         the setup was not completed.
 */
bool usSimulatorRun(UnisimSimulator simulator);

/**
 * Stop the simulator.
 * This method can be called once the Run method has finished or when any 
 * kind of callback is called in order to properly finish the simulation.
 *
 * @param simulator Pointer to the simulator to stop.
 */
void usSimulatorStop(UnisimSimulator simulator);

/**
 * Returns a variable handler to the variable requested from the given
 * simulator.
 *
 * @param simulator Pointer to the simulator.
 * @param variableName Null terminated string containing the variable full name.
 *
 * @return A variable handler.
 */
UnisimVariable usSimulatorGetVariable(UnisimSimulator simulator, 
		const char *variableName);

/**
 * Returns a variable handler to the requested parameter from the given
 * simulator.
 *
 * @param simulator Pointer to the simulator.
 * @param variableName Null terminated string containing the parameter full name.
 * @param variableType The type of the requested variable as defined by VariableType.
 *
 * @return A variable handler.
 */
UnisimVariable usSimulatorGetVariableWithType(UnisimSimulator simulator,
		const char *variableName, UnisimVariableType variableType);

/**
 * Returns a list of all the variables (in the form of variable handlers)
 * avalaible in the simulator.
 *
 * @param simulator Pointer to the simulator.
 *
 * @return A null terminated list of the available variables.
 */
UnisimVariable *usSimulatorGetVariableList(UnisimSimulator simulator);

/**
 * Returns a list of all the variables (in the form of variable handlers)
 * avalaible in the simulator with the given type as defined by VariableType.
 *
 * @param simulator Pointer to the simulator.
 * @param variableType The type of the requested variables as defined by Variabletype.
 *
 * @return A null terminated list of the available variables with the requested
 *         type.
 */
UnisimVariable *usSimulatorGetVariableListWithType(UnisimSimulator simulator,
		UnisimVariableType variableType);

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
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/

