/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : simulator.c                                                 **
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

#define __UNISIM__UAPI__SIMULATOR__C_SRC

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include <string>
#include <map>

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

struct _UnisimSimulator
{
	Simulator *simulator;
	UnisimSimulatorStatus simulatorStatus;
	std::map<UnisimVariable, UnisimVariable> variablesRegistered;
};

/****************************************************************************/
/**                                                                        **/
/**                     PROTOTYPES OF LOCAL FUNCTIONS                      **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
bool TransformVariableTypeToUnisimVariableBaseType(
		UnisimVariableType,
		unisim::kernel::service::VariableBase::Type &);
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
UnisimSimulator usCreateSimulator(char *configurationXmlFile, 
		char **optionList)
/****************************************************************************/
{
	_UnisimSimulator *simulator;
	char **argv;
	int argvSize = 2; //4; // default number of parameters

	simulator = new _UnisimSimulator();
	if ( simulator == 0 ) return 0;
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

	//	unisimSimulator->sim = init_func(argvSize, argv);
	// unisimSimulator->sim = NewSimulator(argvSize, argv);
	simulator->simulator = new Simulator(argvSize, argv);
	if ( simulator->simulator == 0 ) 
	{
		free(argv);
		free(simulator);
		return 0;
	}

	simulator->simulatorStatus = UNISIM_SIMULATOR_STATUS_CREATED;
	return simulator;
}

/****************************************************************************/
bool usDestroySimulator(UnisimSimulator simulator)
/****************************************************************************/
{
	if ( simulator == 0 ) return true;

	
	if ( simulator->simulatorStatus == UNISIM_SIMULATOR_STATUS_RUNNING ) 
		return false;

	std::map<UnisimVariable, UnisimVariable>::iterator
		variablesRegisteredIterator;
	for ( variablesRegisteredIterator = simulator->variablesRegistered.begin();
			variablesRegisteredIterator != simulator->variablesRegistered.end();
			variablesRegisteredIterator = simulator->variablesRegistered.begin())
	{
		usSimulatorUnregisterVariable(simulator, 
				variablesRegisteredIterator->second);
		usDestroyUnregisteredVariable(variablesRegisteredIterator->second);
	}

	delete(simulator->simulator);
	simulator->simulator = 0;
	free(simulator);
	simulator = 0;
	return true;
}

/****************************************************************************/
UnisimSimulatorStatus usSimulatorGetStatus(UnisimSimulator simulator)
/****************************************************************************/
{
	UnisimSimulatorStatus simulatorStatus;

	if ( simulator == 0 ) simulatorStatus = UNISIM_SIMULATOR_STATUS_NONE;
	else simulatorStatus = simulator->simulatorStatus;

	return simulatorStatus;
}

/****************************************************************************/
UnisimSimulatorSetupStatus usSimulatorSetup(UnisimSimulator simulator)
/****************************************************************************/
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

/****************************************************************************/
const char *usSimulatorGetVersion(UnisimSimulator simulator)
/****************************************************************************/
{
	if ( simulator == 0 ) return 0;

	std::string simulatorVersion = 
		(std::string)*simulator->simulator->FindVariable("version");
	return simulatorVersion.c_str();
}

/****************************************************************************/
bool usSimulatorRun(UnisimSimulator simulator)
/****************************************************************************/
{
	return false;
}

/****************************************************************************/
void usSimulatorStop(UnisimSimulator simulator)
/****************************************************************************/
{
}

/****************************************************************************/
UnisimVariable usSimulatorGetVariable(UnisimSimulator simulator, 
		const char *variableName)
/****************************************************************************/
{
	return usSimulatorGetVariableWithType(simulator, variableName, 
			UNISIM_VARIABLE_TYPE_VOID);
}

/****************************************************************************/
UnisimVariable usSimulatorGetVariableWithType(UnisimSimulator simulator,
		const char *variableName,
		UnisimVariableType variableType)
/****************************************************************************/
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

/****************************************************************************/
UnisimVariable *usSimulatorGetVariableList(UnisimSimulator simulator)
/****************************************************************************/
{
	return usSimulatorGetVariableListWithType(simulator,
			UNISIM_VARIABLE_TYPE_VOID);
}

/****************************************************************************/
UnisimVariable *usSimulatorGetVariableListWithType(UnisimSimulator simulator,
		UnisimVariableType variableType)
/****************************************************************************/
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
/**                                                                        **/
/**                     LOCAL FUNCTIONS                                    **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
bool TransformVariableTypeToUnisimVariableBaseType(
		UnisimVariableType variableType,
		unisim::kernel::service::VariableBase::Type &unisimVariableBaseType)
/****************************************************************************/
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
void usSimulatorRegisterVariable(UnisimSimulator simulator, 
		UnisimVariable variable)
/****************************************************************************/
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

	variableIterator = simulator->variablesRegistered.find(variable);
	if ( variableIterator != simulator->variablesRegistered.end() )
	{
		std::cerr << "Warning!!!: trying to register a variable multiple"
			<< " times '" << usVariableGetName(variable) << "'"
			<< std::endl;
		return;
	}

	simulator->variablesRegistered[variable] = variable;
}

/****************************************************************************/
void usSimulatorUnregisterVariable(UnisimSimulator simulator,
		UnisimVariable variable)
/****************************************************************************/
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

	variableIterator = simulator->variablesRegistered.find(variable);
	if ( variableIterator == simulator->variablesRegistered.end() )
	{
		std::cerr << "Warning!!!: trying to unregister a variable that was"
			<< " not registered '" << usVariableGetName(variable)  << "'"
			<< std::endl;
		return;
	}

	simulator->variablesRegistered.erase(variableIterator);
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

