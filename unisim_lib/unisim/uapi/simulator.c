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

#define __UNISIM__UAPI__SIMULATOR__C_SRC

/****************************************************************************/
/*                                                                         **/
/*                      MODULES USED                                       **/
/*                                                                         **/
/****************************************************************************/

#include <string>
#include <map>

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

/**
 * Definition of the simulator state.
 * It contains the pointer to the unisim simulator and its status. Additionally
 * it contatins the list of the currently registered (requested by the api user)
 * variables and extended apis.
 */
struct _UnisimSimulator
{
	Simulator *simulator; /**< Pointer to the unisim simulator. */
	UnisimSimulatorStatus simulatorStatus; /**< The status of the simulator. */
	std::map<UnisimVariable, UnisimVariable> *variablesRegistered; /**< Currently registered variables. */
	std::map<UnisimExtendedAPI, UnisimExtendedAPI> *apisRegistered; /**< Currently registered extended apis. */
};

/****************************************************************************/
/*                                                                         **/
/*                      PROTOTYPES OF LOCAL FUNCTIONS                      **/
/*                                                                         **/
/****************************************************************************/

/**
 * Transform the api variable type definition to a unisim (c++) VariableBase type.
 *
 * @param variableType The api variable type to transform.
 * @param unisimVariableType The given api variable type transformed into a unisim VariableBase type.
 *
 * @return True if the transformation was feasible, false otherwise.
 */
bool TransformVariableTypeToUnisimVariableBaseType(
		UnisimVariableType variableType,
		unisim::kernel::service::VariableBase::Type &unisimVariableType);

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

UnisimSimulator usCreateSimulator(char *configurationXmlFile, 
		char **optionList)
{
	UnisimSimulator simulator;
	char **argv;
	int argvSize = 2; //4; // default number of parameters

	simulator = (UnisimSimulator)malloc(sizeof(_UnisimSimulator));
	if ( simulator == 0 ) return 0;
	simulator->variablesRegistered = new std::map<UnisimVariable, UnisimVariable>();
	if ( simulator->variablesRegistered == 0)
	{
		free(simulator);
		simulator = 0;
		return 0;
	}
	simulator->apisRegistered = new std::map<UnisimExtendedAPI, UnisimExtendedAPI>();
	if ( simulator->apisRegistered == 0 )
	{
		delete(simulator->apisRegistered);
		free(simulator);
		simulator = 0;
		return 0;
	}
	simulator->simulator = 0;
	simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_NONE;

	if ( configurationXmlFile != 0 )
	{
		argvSize += 2;
	}

	int optionListSize;
	optionListSize = 0;
	while ( (optionList != 0) && (optionList[optionListSize] != 0) ) 
		optionListSize++;
	argvSize += optionListSize * 2;

	int optionListIndex;
	optionListIndex = 0;
	argv = (char **)malloc(sizeof(char *) * (argvSize + 1));
	if ( argv == 0 ) 
	{
		free(simulator);
		return 0;
	}
	// the following lines are probably not needed
	argv[optionListIndex++] = (char *)"IDontCare";// API__SIM_EXEC_LOCATION;
	// argv[optionListIndex++] = (char *)"-p";
	// argv[optionListIndex++] = (char *)API__LIB_TO_SHARED_DATA_PATH;
	// end of not needed lines
	argv[optionListIndex++] = (char *)"-w";
	if ( configurationXmlFile != 0 )
		argv[optionListIndex++] = configurationXmlFile;
	if ( optionListSize != 0 )
	{
		while ( optionListSize != 0 )
		{
			argv[optionListIndex++] = (char *)"-s";
			argv[optionListIndex++] = optionList[optionListSize - 1];
			optionListSize--;
		}
	}
	argv[optionListIndex] = 0;

	simulator->simulator = new Simulator(argvSize, argv);
	if ( simulator->simulator == 0 ) 
	{
		free(argv);
		delete(simulator->variablesRegistered);
		delete(simulator->apisRegistered);
		free(simulator);
		return 0;
	}

	free(argv);
	simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_CREATED;
	return simulator;
}

bool usDestroySimulator(UnisimSimulator simulator)
{
	if ( simulator == 0 ) return true;

	
	if ( simulator->simulatorStatus == UNISIM_SIMULATOR_STATUS_RUNNING ) 
		return false;

	std::map<UnisimVariable, UnisimVariable>::iterator
		variablesRegisteredIterator;
	for ( variablesRegisteredIterator = simulator->variablesRegistered->begin();
			variablesRegisteredIterator != simulator->variablesRegistered->end();
			variablesRegisteredIterator = simulator->variablesRegistered->begin())
	{
		UnisimVariable variable = variablesRegisteredIterator->second;
		usSimulatorUnregisterVariable(simulator, 
				variable);
		usDestroyUnregisteredVariable(variable);
	}

	std::map<UnisimExtendedAPI, UnisimExtendedAPI>::iterator
		apisRegisteredIterator;
	for ( apisRegisteredIterator = simulator->apisRegistered->begin();
			apisRegisteredIterator != simulator->apisRegistered->end();
			apisRegisteredIterator = simulator->apisRegistered->begin() )
	{
		UnisimExtendedAPI api = apisRegisteredIterator->second;
		usSimulatorUnregisterExtendedAPI(simulator,
				api);
		usDestroyUnregisteredExtendedAPI(api);
	}

	delete(simulator->variablesRegistered);
	delete(simulator->apisRegistered);
	delete(simulator->simulator);
	simulator->simulator = 0;
	free(simulator);
	simulator = 0;
	return true;
}

UnisimSimulatorStatus usSimulatorGetStatus(UnisimSimulator simulator)
{
	UnisimSimulatorStatus simulatorStatus;

	if ( simulator == 0 ) simulatorStatus = UNISIM_SIMULATOR_STATUS_NONE;
	else simulatorStatus = simulator->simulatorStatus;

	return simulatorStatus;
}

UnisimSimulatorSetupStatus usSimulatorSetup(UnisimSimulator simulator)
{
	if ( simulator == 0 ) return UNISIM_SIMULATOR_SETUP_STATUS_ERROR;

	// this constraint maybe relaxed in the future, but for the moment it
	// is safer to just allow the execution of setup once in the simulator
	// lifetime
	if ( simulator->simulatorStatus != UNISIM_SIMULATOR_STATUS_CREATED )
	{
		return UNISIM_SIMULATOR_SETUP_STATUS_ERROR;
	}

	unisim::kernel::service::Simulator::SetupStatus status;
	UnisimSimulatorSetupStatus simulatorStatus;
	status = simulator->simulator->Setup();
	switch ( status )
	{
		case unisim::kernel::service::Simulator::ST_OK_TO_START:
			simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_SETUP;
			simulatorStatus = UNISIM_SIMULATOR_SETUP_STATUS_OK;
			break;
		case unisim::kernel::service::Simulator::ST_OK_DONT_START:
			simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_FINISHED;
			simulatorStatus = UNISIM_SIMULATOR_SETUP_STATUS_OK;
			break;
		case unisim::kernel::service::Simulator::ST_WARNING:
			simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_CREATED;
			simulatorStatus = UNISIM_SIMULATOR_SETUP_STATUS_WARNING;
			break;
		case unisim::kernel::service::Simulator::ST_ERROR:
			simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_CREATED;
			simulatorStatus = UNISIM_SIMULATOR_SETUP_STATUS_ERROR;
			break;
		default:
			simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_FINISHED;
			simulatorStatus = UNISIM_SIMULATOR_SETUP_STATUS_ERROR;
			break;
	}

	return simulatorStatus;
}

const char *usSimulatorGetVersion(UnisimSimulator simulator)
{
	if ( simulator == 0 ) return 0;

	std::string simulatorVersion = 
		(std::string)*simulator->simulator->FindVariable("version");
	return simulatorVersion.c_str();
}

UnisimExtendedAPI *usSimulatorGetExtendedAPIList(UnisimSimulator simulator)
{
	std::list<unisim::kernel::api::APIBase *> unisimApiList;
	UnisimExtendedAPI *apiList;

	if ( simulator == 0 ) return 0;

	simulator->simulator->GetAPIs(unisimApiList);

	if ( unisimApiList.size() == 0 ) return 0;

	apiList = (UnisimExtendedAPI *)malloc(sizeof(UnisimExtendedAPI) *
			(unisimApiList.size() + 1));

	if ( apiList == 0 ) return 0;

	apiList = (UnisimExtendedAPI *)memset(apiList,
			0,
			sizeof(UnisimExtendedAPI) * (unisimApiList.size() + 1));
	
	std::list<unisim::kernel::api::APIBase *>::iterator unisimApiListIterator;
	int apiListIndex;
	bool error;
	apiListIndex = 0;
	error = false;
	for ( unisimApiListIterator = unisimApiList.begin();
			(!error) && (unisimApiListIterator != unisimApiList.end());
			unisimApiListIterator++ )
	{
		UnisimExtendedAPI api =
			usCreateExtendedAPI(simulator, *unisimApiListIterator);
		if ( api == 0 ) error = true;
		else apiList[apiListIndex] = api;
		apiListIndex++;
	}

	if ( error )
	{
		for ( apiListIndex = 0;
				apiList[apiListIndex] != 0;
				apiListIndex++ )
		{
			usDestroyExtendedAPI(apiList[apiListIndex]);
			apiList[apiListIndex] = 0;
		}
		free(apiList);
		return 0;
	}

	return apiList;
}

bool usSimulatorRun(UnisimSimulator simulator)
{
	if ( simulator == 0 ) return false;
	if ( simulator->simulatorStatus != UNISIM_SIMULATOR_STATUS_SETUP )
		return false;

	simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_RUNNING;
	
	simulator->simulator->Run();

	simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_FINISHED;

	return true;
}

void usSimulatorStop(UnisimSimulator simulator)
{
	if ( simulator == 0 ) return;

	if ( simulator->simulatorStatus != UNISIM_SIMULATOR_STATUS_RUNNING )
		return;

	simulator->simulator->Stop(0, 0);

	simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_STOPPED;
}

UnisimVariable usSimulatorGetVariable(UnisimSimulator simulator, 
		const char *variableName)
{
	return usSimulatorGetVariableWithType(simulator, variableName, 
			UNISIM_VARIABLE_TYPE_VOID);
}

UnisimVariable usSimulatorGetVariableWithType(UnisimSimulator simulator,
		const char *variableName,
		UnisimVariableType variableType)
{
	if ( simulator == 0 ) return 0;

	unisim::kernel::service::VariableBase::Type unisimVariableBaseType;
	if ( !TransformVariableTypeToUnisimVariableBaseType(variableType, 
				unisimVariableBaseType) )
		return 0;

	unisim::kernel::service::VariableBase *unisimVariable =
		simulator->simulator->FindVariable(
				variableName, unisimVariableBaseType);
	if ( unisimVariable == simulator->simulator->void_variable ) return 0;

	UnisimVariable variable = usCreateVariable(simulator, unisimVariable);
	if ( variable == 0 ) return 0;
	return variable;
}

UnisimVariable *usSimulatorGetVariableList(UnisimSimulator simulator)
{
	return usSimulatorGetVariableListWithType(simulator,
			UNISIM_VARIABLE_TYPE_VOID);
}

UnisimVariable *usSimulatorGetVariableListWithType(UnisimSimulator simulator,
		UnisimVariableType variableType)
{
	std::list<unisim::kernel::service::VariableBase *> unisimVariableBaseList;
	UnisimVariable *variableList;

	if ( simulator == 0 ) return 0;

	unisim::kernel::service::VariableBase::Type unisimVariableBaseType;
	if ( !TransformVariableTypeToUnisimVariableBaseType(variableType,
				unisimVariableBaseType) )
		return 0;

	simulator->simulator->GetVariables(unisimVariableBaseList,
			unisimVariableBaseType);

	if ( unisimVariableBaseList.size() == 0 ) return 0;

	variableList = (UnisimVariable *)malloc(sizeof(UnisimVariable) * 
			(unisimVariableBaseList.size() + 1));
	
	if ( variableList == 0 ) return 0;

	variableList = (UnisimVariable *)memset(variableList, 
			0, 
			sizeof(UnisimVariable) * (unisimVariableBaseList.size() + 1));

	std::list<unisim::kernel::service::VariableBase *>::iterator 
		unisimVariableBaseListIterator;
	int variableListIndex;
	bool error;
	variableListIndex = 0;
	error = false;
	for ( unisimVariableBaseListIterator = unisimVariableBaseList.begin(); 
			(!error) && 
			(unisimVariableBaseListIterator != unisimVariableBaseList.end());
			unisimVariableBaseListIterator++ )
	{
		UnisimVariable variable = 
			usCreateVariable(simulator, *unisimVariableBaseListIterator);
		if ( variable == 0 ) error = true;
		else variableList[variableListIndex] = variable;
		variableListIndex++;
	}

	if ( error )
	{
		for ( variableListIndex = 0; 
				variableList[variableListIndex] != 0; 
				variableListIndex++ )
		{
			usDestroyVariable(variableList[variableListIndex]);
			variableList[variableListIndex] = 0;
		}
		free(variableList);
		return 0;
	}
	
	return variableList;
}

/****************************************************************************/
/*                                                                         **/
/*                  PRIVATELY EXPORTED FUNCTIONS                           **/
/*                                                                         **/
/****************************************************************************/

void usSimulatorRegisterVariable(UnisimSimulator simulator, 
		UnisimVariable variable)
{
	std::map<UnisimVariable, UnisimVariable>::iterator variableIterator;

	if ( simulator == 0 )
	{
		std::cerr << "Warning!!!: trying to register variable without"
			<< " handling simulator '" << usVariableGetName(variable) << "'"
			<< std::endl;
	}
	if ( variable == 0 )
	{
		std::cerr << "Warning!!!: trying to register variable without"
			<< " handling variable"
			<< std::endl;
	}
	if ( (simulator == 0) || (variable == 0) ) 
		return;

	variableIterator = simulator->variablesRegistered->find(variable);
	if ( variableIterator != simulator->variablesRegistered->end() )
	{
		std::cerr << "Warning!!!: trying to register a variable multiple"
			<< " times '" << usVariableGetName(variable) << "'"
			<< std::endl;
		return;
	}

	(*simulator->variablesRegistered)[variable] = variable;
}

void usSimulatorUnregisterVariable(UnisimSimulator simulator,
		UnisimVariable variable)
{
	std::map<UnisimVariable, UnisimVariable>::iterator variableIterator;

	if ( simulator == 0 )
	{
		std::cerr << "Warning!!!: trying to unregister variable without"
			<< " handling simulator '" << usVariableGetName(variable) << "'"
			<< std::endl;
	}
	if ( variable == 0 )
	{
		std::cerr << "Warning!!!: trying to register api without"
			<< " handling variable"
			<< std::endl;
	}
	if ( (simulator == 0) || (variable == 0) ) 
		return;

	variableIterator = simulator->variablesRegistered->find(variable);
	if ( variableIterator == simulator->variablesRegistered->end() )
	{
		std::cerr << "Warning!!!: trying to unregister a variable that was"
			<< " not registered '" << usVariableGetName(variable)  << "'"
			<< std::endl;
		return;
	}

	simulator->variablesRegistered->erase(variableIterator);
}

void usSimulatorRegisterExtendedAPI(UnisimSimulator simulator, 
		UnisimExtendedAPI api)
{
	std::map<UnisimExtendedAPI, UnisimExtendedAPI>::iterator apiIterator;

	if ( simulator == 0 )
	{
		std::cerr << "Warning!!!: trying to register api without"
			<< " handling simulator '" << usExtendedAPIGetName(api) << "'"
			<< std::endl;
	}
	if ( api == 0 )
	{
		std::cerr << "Warning!!!: trying to register api without"
			<< " handling api"
			<< std::endl;
	}
	if ( (simulator == 0) || (api == 0) ) 
		return;

	apiIterator = simulator->apisRegistered->find(api);
	if ( apiIterator != simulator->apisRegistered->end() )
	{
		std::cerr << "Warning!!!: trying to register a api multiple"
			<< " times '" << usExtendedAPIGetName(api) << "'"
			<< std::endl;
		return;
	}

	(*simulator->apisRegistered)[api] = api;
}

void usSimulatorUnregisterExtendedAPI(UnisimSimulator simulator,
		UnisimExtendedAPI api)
{
	std::map<UnisimExtendedAPI, UnisimExtendedAPI>::iterator apiIterator;

	if ( simulator == 0 )
	{
		std::cerr << "Warning!!!: trying to unregister variable without"
			<< " handling simulator '" << usExtendedAPIGetName(api) << "'"
			<< std::endl;
	}
	if ( api == 0 )
	{
		std::cerr << "Warning!!!: trying to unregister api without"
			<< " handling api"
			<< std::endl;
	}
	if ( (simulator == 0) || (api == 0) ) 
		return;

	apiIterator = simulator->apisRegistered->find(api);
	if ( apiIterator == simulator->apisRegistered->end() )
	{
		std::cerr << "Warning!!!: trying to unregister a api that was"
			<< " not registered '" << usExtendedAPIGetName(api)  << "'"
			<< std::endl;
		return;
	}

	simulator->apisRegistered->erase(apiIterator);
}

/****************************************************************************/
/*                                                                         **/
/*                      LOCAL FUNCTIONS                                    **/
/*                                                                         **/
/****************************************************************************/

bool TransformVariableTypeToUnisimVariableBaseType(
		UnisimVariableType variableType,
		unisim::kernel::service::VariableBase::Type &unisimVariableBaseType)
{
	switch ( variableType )
	{
		case UNISIM_VARIABLE_TYPE_VOID: 
			unisimVariableBaseType = unisim::kernel::service::VariableBase::VAR_VOID; 
			break;
		case UNISIM_VARIABLE_TYPE_ARRAY:
			unisimVariableBaseType = unisim::kernel::service::VariableBase::VAR_ARRAY;
			break;
		case UNISIM_VARIABLE_TYPE_PARAMETER:
			unisimVariableBaseType = unisim::kernel::service::VariableBase::VAR_PARAMETER;
			break;
		case UNISIM_VARIABLE_TYPE_STATISTIC:
			unisimVariableBaseType = unisim::kernel::service::VariableBase::VAR_STATISTIC;
			break;
		case UNISIM_VARIABLE_TYPE_REGISTER:
			unisimVariableBaseType = unisim::kernel::service::VariableBase::VAR_REGISTER;
			break;
		case UNISIM_VARIABLE_TYPE_FORMULA:
			unisimVariableBaseType = unisim::kernel::service::VariableBase::VAR_FORMULA;
			break;
		default:
			return false;
			break;
	}
	return true;
}

/****************************************************************************/
/*                                                                         **/
/*                                EOF                                      **/
/*                                                                         **/
/****************************************************************************/
