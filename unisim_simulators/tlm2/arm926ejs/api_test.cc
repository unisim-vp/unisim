#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <map>
#include <dlfcn.h>
#include <inttypes.h>
#include "unisim/uapi/uapi.h"

void *simlib;

// simulator methods
typedef UnisimSimulator (* _usCreateSimulator_type)(char *, char **);
_usCreateSimulator_type _usCreateSimulator;
typedef bool (* _usDestroySimulator_type)(UnisimSimulator);
_usDestroySimulator_type _usDestroySimulator;
typedef UnisimSimulatorStatus (* _usSimulatorGetStatus_type)(UnisimSimulator);
_usSimulatorGetStatus_type _usSimulatorGetStatus;
typedef UnisimSimulatorSetupStatus (*_usSimulatorSetup_type)(UnisimSimulator);
_usSimulatorSetup_type _usSimulatorSetup;
typedef const char *(*_usSimulatorGetVersion_type)(UnisimSimulator);
_usSimulatorGetVersion_type _usSimulatorGetVersion;
typedef UnisimExtendedAPI *(* _usSimulatorGetExtendedAPIList_type)(UnisimSimulator);
_usSimulatorGetExtendedAPIList_type _usSimulatorGetExtendedAPIList;
typedef bool (* _usSimulatorRun_type)(UnisimSimulator);
_usSimulatorRun_type _usSimulatorRun;
typedef void (* _usSimulatorStop_type)(UnisimSimulator);
_usSimulatorStop_type _usSimulatorStop;
typedef UnisimVariable (* _usSimulatorGetVariable_type)(UnisimSimulator, const char *);
_usSimulatorGetVariable_type _usSimulatorGetVariable;
typedef UnisimVariable (* _usSimulatorGetVariableWithType_type)(UnisimSimulator, const char *, UnisimVariableType);
_usSimulatorGetVariableWithType_type _usSimulatorGetVariableWithType;
typedef UnisimVariable *(* _usSimulatorGetVariableList_type)(UnisimSimulator);
_usSimulatorGetVariableList_type _usSimulatorGetVariableList;
typedef UnisimVariable *(* _usSimulatorGetVariableListWithType_type)(UnisimSimulator, UnisimVariableType);
_usSimulatorGetVariableListWithType_type _usSimulatorGetVariableListWithType;

// variable methods
typedef UnisimSimulator (* _usVariableGetAssociatedSimulator_type)(UnisimVariable);
_usVariableGetAssociatedSimulator_type _usVariableGetAssociatedSimulator;
typedef const char *(* _usVariableGetName_type)(UnisimVariable);
_usVariableGetName_type _usVariableGetName;
typedef UnisimVariableType (* _usVariableGetType_type)(UnisimVariable);
_usVariableGetType_type _usVariableGetType;
typedef bool (* _usVariableVisible_type)(UnisimVariable);
_usVariableVisible_type _usVariableVisible;
typedef bool (* _usVariableMutable_type)(UnisimVariable);
_usVariableMutable_type _usVariableMutable;
typedef bool (* _usVariableSerializable_type)(UnisimVariable);
_usVariableSerializable_type _usVariableSerializable;
typedef const char *(* _usVariableGetValueAsString_type)(UnisimVariable);
_usVariableGetValueAsString_type _usVariableGetValueAsString;
typedef void (* _usVariableSetValueFromString_type)(UnisimVariable, const char *);
_usVariableSetValueFromString_type _usVariableSetValueFromString;
typedef unsigned long long (* _usVariableGetValueAsUnsignedLongLong_type)(UnisimVariable);
_usVariableGetValueAsUnsignedLongLong_type _usVariableGetValueAsUnsignedLongLong;
typedef void (* _usVariableSetValueFromUnsignedLongLong_type)(UnisimVariable, unsigned long long);
_usVariableSetValueFromUnsignedLongLong_type _usVariableSetValueFromUnsignedLongLong;
typedef bool (* _usVariableSetListener_type)(UnisimVariable, void (* listener)(UnisimVariable));
_usVariableSetListener_type _usVariableSetListener;
typedef void (* _usVariableRemoveListener_type)(UnisimVariable);
_usVariableRemoveListener_type _usVariableRemoveListener;
typedef void (* _usDestroyVariable_type)(UnisimVariable);
_usDestroyVariable_type _usDestroyVariable;

// api methods
typedef const char *(* _usExtendedAPIGetName_type)(UnisimExtendedAPI);
_usExtendedAPIGetName_type _usExtendedAPIGetName;
typedef const char *(* _usExtendedAPIGetTypeName_type)(UnisimExtendedAPI);
_usExtendedAPIGetTypeName_type _usExtendedAPIGetTypeName;
typedef void (* _usDestroyExtendedAPI_type)(UnisimExtendedAPI);
_usDestroyExtendedAPI_type _usDestroyExtendedAPI;

// debug api methods
typedef UnisimDebugAPI (* _usCreateDebugAPI_type)(UnisimExtendedAPI);
_usCreateDebugAPI_type _usCreateDebugAPI;
typedef void (* _usDestroyDebugAPI_type)(UnisimDebugAPI);
_usDestroyDebugAPI_type _usDestroyDebugAPI;
typedef bool (* _usDebugAPISetBreakpointHandler_type)(UnisimDebugAPI, void (* callback)(UnisimDebugAPI, uint64_t));
_usDebugAPISetBreakpointHandler_type _usDebugAPISetBreakpointHandler;
typedef bool (* _usDebugAPISetStepMode_type)(UnisimDebugAPI);
_usDebugAPISetStepMode_type _usDebugAPISetStepMode;
typedef bool (* _usDebugAPISetContinueMode_type)(UnisimDebugAPI);
_usDebugAPISetContinueMode_type _usDebugAPISetContinueMode;
typedef bool (* _usDebugAPIIsModeStep_type)(UnisimDebugAPI);
_usDebugAPIIsModeStep_type _usDebugAPIIsModeStep;
typedef bool (* _usDebugAPIIsModeContinue_type)(UnisimDebugAPI);
_usDebugAPIIsModeContinue_type _usDebugAPIIsModeContinue;
typedef bool (* _usDebugAPIGetSymbolAddress_type)(UnisimDebugAPI, const char *, uint64_t);
_usDebugAPIGetSymbolAddress_type _usDebugAPIGetSymbolAddress;
typedef bool (* _usDebugAPIGetFileSystemAddress_type)(UnisimDebugAPI, const char *, uint64_t);
_usDebugAPIGetFileSystemAddress_type _usDebugAPIGetFileSystemAddress;
typedef bool (* _usDebugAPIHasBreakpoint_type)(UnisimDebugAPI, uint64_t);
_usDebugAPIHasBreakpoint_type _usDebugAPIHasBreakpoint;
typedef bool (* _usDebugAPISetBreakpoint_type)(UnisimDebugAPI, uint64_t);
_usDebugAPISetBreakpoint_type _usDebugAPISetBreakpoint;
typedef bool (* _usDebugAPIDeleteBreakpoint_type)(UnisimDebugAPI, uint64_t);
_usDebugAPIDeleteBreakpoint_type _usDebugAPIDeleteBreakpoint;
typedef bool (* _usDebugAPIHasBreakpointSymbol_type)(UnisimDebugAPI, const char *);
_usDebugAPIHasBreakpointSymbol_type _usDebugAPIHasBreakpointSymbol;
typedef bool (* _usDebugAPISetBreakpointSymbol_type)(UnisimDebugAPI, const char *);
_usDebugAPISetBreakpointSymbol_type _usDebugAPISetBreakpointSymbol;
typedef bool (* _usDebugAPIDeleteBreakpointSymbol_type)(UnisimDebugAPI, const char *);
_usDebugAPIDeleteBreakpointSymbol_type _usDebugAPIDeleteBreakpointSymbol;
typedef bool (* _usDebugAPISetWatchpoint_type)(UnisimDebugAPI, uint64_t, uint32_t);
_usDebugAPISetWatchpoint_type _usDebugAPISetWatchpoint;
typedef bool (* _usDebugAPISetReadWatchpoint_type)(UnisimDebugAPI, uint64_t, uint32_t);
_usDebugAPISetReadWatchpoint_type _usDebugAPISetReadWatchpoint;
typedef bool (* _usDebugAPISetWriteWatchpoint_type)(UnisimDebugAPI, uint64_t, uint32_t);
_usDebugAPISetWriteWatchpoint_type _usDebugAPISetWriteWatchpoint;
typedef bool (* _usDebugAPIDeleteWatchpoint_type)(UnisimDebugAPI, uint64_t, uint32_t);
_usDebugAPIDeleteWatchpoint_type _usDebugAPIDeleteWatchpoint;
typedef bool (* _usDebugAPIDeleteReadWatchpoint_type)(UnisimDebugAPI, uint64_t, uint32_t);
_usDebugAPIDeleteReadWatchpoint_type _usDebugAPIDeleteReadWatchpoint;
typedef bool (* _usDebugAPIDeleteWriteWatchpoint_type)(UnisimDebugAPI, uint64_t, uint32_t);
_usDebugAPIDeleteWriteWatchpoint_type _usDebugAPIDeleteWriteWatchpoint;
typedef bool (* _usDebugAPISetWatchpointSymbol_type)(UnisimDebugAPI, const char *, uint32_t);
_usDebugAPISetWatchpointSymbol_type _usDebugAPISetWatchpointSymbol;
typedef bool (* _usDebugAPISetReadWatchpointSymbol_type)(UnisimDebugAPI, const char *, uint32_t);
_usDebugAPISetReadWatchpointSymbol_type _usDebugAPISetReadWatchpointSymbol;
typedef bool (* _usDebugAPISetWriteWatchpointSymbol_type)(UnisimDebugAPI, const char *, uint32_t);
_usDebugAPISetWriteWatchpointSymbol_type _usDebugAPISetWriteWatchpointSymbol;
typedef bool (* _usDebugAPIDeleteWatchpointSymbol_type)(UnisimDebugAPI, const char *, uint32_t);
_usDebugAPIDeleteWatchpointSymbol_type _usDebugAPIDeleteWatchpointSymbol;
typedef bool (* _usDebugAPIDeleteReadWatchpointSymbol_type)(UnisimDebugAPI, const char *, uint32_t);
_usDebugAPIDeleteReadWatchpointSymbol_type _usDebugAPIDeleteReadWatchpointSymbol;
typedef bool (* _usDebugAPIDeleteWriteWatchpointSymbol_type)(UnisimDebugAPI, const char *, uint32_t);
_usDebugAPIDeleteWriteWatchpointSymbol_type _usDebugAPIDeleteWriteWatchpointSymbol;

template <typename T>
bool load_sym(void *lib, const char *name, T &func)
{
	void *_func = dlsym(lib, name);
	if ( _func == NULL )
	{ 
		std::cerr << "Could not get '" << name << "'" << std::endl;
		return false;
	}
	func = (T)_func;

	return true;
}

bool close_lib()
{
	if ( dlclose(simlib) != 0 )
	{
		std::cerr << "Error while closing dynamic library" << std::endl;
		return false;
	}
	return true;
}

bool load_lib()
{
	simlib = dlopen(UNISIM_UAPI_LIB_PATH, RTLD_NOW || RTLD_GLOBAL);
	if(simlib == NULL)
	{
		std::cerr << "Could not load simulator dynamic library (" << UNISIM_UAPI_LIB_PATH << ")"
			<< std::endl;
		return false;
	}

	if ( !load_sym(simlib, "usCreateSimulator", _usCreateSimulator) )
		return false;
	if ( !load_sym(simlib, "usDestroySimulator", _usDestroySimulator) )
		return false;
	if ( !load_sym(simlib, "usSimulatorGetStatus", _usSimulatorGetStatus) )
		return false;
	if ( !load_sym(simlib, "usSimulatorSetup", _usSimulatorSetup) )
		return false;
	if ( !load_sym(simlib, "usSimulatorGetVersion", _usSimulatorGetVersion) )
		return false;
	if ( !load_sym(simlib, "usSimulatorGetExtendedAPIList", _usSimulatorGetExtendedAPIList) )
		return false;
	if ( !load_sym(simlib, "usSimulatorRun", _usSimulatorRun) )
		return false;
	if ( !load_sym(simlib, "usSimulatorStop", _usSimulatorStop) )
		return false;
	if ( !load_sym(simlib, "usSimulatorGetVariable", _usSimulatorGetVariable) )
		return false;
	if ( !load_sym(simlib, "usSimulatorGetVariableWithType", _usSimulatorGetVariableWithType) )
		return false;
	if ( !load_sym(simlib, "usSimulatorGetVariableList", _usSimulatorGetVariableList) )
		return false;
	if ( !load_sym(simlib, "usSimulatorGetVariableListWithType", _usSimulatorGetVariableListWithType) )
		return false;

	if ( !load_sym(simlib, "usVariableGetAssociatedSimulator", _usVariableGetAssociatedSimulator) )
		return false;
	if ( !load_sym(simlib, "usVariableGetName", _usVariableGetName) )
		return false;
	if ( !load_sym(simlib, "usVariableGetType", _usVariableGetType) )
		return false;
	if ( !load_sym(simlib, "usVariableVisible", _usVariableVisible) )
		return false;
	if ( !load_sym(simlib, "usVariableMutable", _usVariableMutable) )
		return false;
	if ( !load_sym(simlib, "usVariableSerializable", _usVariableSerializable) )
		return false;
	if ( !load_sym(simlib, "usVariableGetValueAsString", _usVariableGetValueAsString) )
		return false;
	if ( !load_sym(simlib, "usVariableSetValueFromString", _usVariableSetValueFromString) )
		return false;
	if ( !load_sym(simlib, "usVariableGetValueAsUnsignedLongLong", _usVariableGetValueAsUnsignedLongLong) )
		return false;
	if ( !load_sym(simlib, "usVariableSetValueFromUnsignedLongLong", _usVariableSetValueFromUnsignedLongLong) )
		return false;
	if ( !load_sym(simlib, "usVariableSetListener", _usVariableSetListener) )
		return false;
	if ( !load_sym(simlib, "usVariableRemoveListener", _usVariableRemoveListener) )
		return false;
	if ( !load_sym(simlib, "usDestroyVariable", _usDestroyVariable) )
		return false;

	if ( !load_sym(simlib, "usExtendedAPIGetName", _usExtendedAPIGetName) )
		return false;
	if ( !load_sym(simlib, "usExtendedAPIGetTypeName", _usExtendedAPIGetTypeName) )
		return false;
	if ( !load_sym(simlib, "usDestroyExtendedAPI", _usDestroyExtendedAPI) )
		return false;

	if ( !load_sym(simlib, "usCreateDebugAPI", _usCreateDebugAPI) )
		return false;
	if ( !load_sym(simlib, "usDestroyDebugAPI", _usDestroyDebugAPI) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetBreakpointHandler", _usDebugAPISetBreakpointHandler) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetStepMode", _usDebugAPISetStepMode) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetContinueMode", _usDebugAPISetContinueMode) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIIsModeStep", _usDebugAPIIsModeStep) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIIsModeContinue", _usDebugAPIIsModeContinue) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIGetSymbolAddress", _usDebugAPIGetSymbolAddress) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIGetFileSystemAddress", _usDebugAPIGetFileSystemAddress) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIHasBreakpoint", _usDebugAPIHasBreakpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetBreakpoint", _usDebugAPISetBreakpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteBreakpoint", _usDebugAPIDeleteBreakpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIHasBreakpointSymbol", _usDebugAPIHasBreakpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetBreakpointSymbol", _usDebugAPISetBreakpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteBreakpointSymbol", _usDebugAPIDeleteBreakpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetWatchpoint", _usDebugAPISetWatchpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetReadWatchpoint", _usDebugAPISetReadWatchpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetWriteWatchpoint", _usDebugAPISetWriteWatchpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteWatchpoint", _usDebugAPIDeleteWatchpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteReadWatchpoint", _usDebugAPIDeleteReadWatchpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteWriteWatchpoint", _usDebugAPIDeleteWriteWatchpoint) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetWatchpointSymbol", _usDebugAPISetWatchpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetReadWatchpointSymbol", _usDebugAPISetReadWatchpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPISetWriteWatchpointSymbol", _usDebugAPISetWriteWatchpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteWatchpointSymbol", _usDebugAPIDeleteWatchpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteReadWatchpointSymbol", _usDebugAPIDeleteReadWatchpointSymbol) )
		return false;
	if ( !load_sym(simlib, "usDebugAPIDeleteWriteWatchpointSymbol", _usDebugAPIDeleteWriteWatchpointSymbol) )
		return false;

	return true;
}

int CloseSimulator ( UnisimSimulator simulator )
{
	// destroying the simulator
	bool done = _usDestroySimulator(simulator);
	if ( !done )
		std::cerr << "Could not destroy simulator" << std::endl;

	if ( !close_lib() )
	{
		std::cerr << "Could not correctly close the library" << std::endl;
		return -1;
	}

	return 0;
}

void InstructionCounterListener ( UnisimVariable variable )
{
	unsigned long long instructionCounterValue = 
		_usVariableGetValueAsUnsignedLongLong(variable);
	if ( (instructionCounterValue % 1000) == 0 )
		std::cerr << ".";
	if ( (instructionCounterValue % 20000) == 0 )
		std::cerr << " " << instructionCounterValue << std::endl;
	if ( (instructionCounterValue % 100000UL) == 0)
	{
		std::cerr << "Calling stop" << std::endl;
		UnisimSimulator simulator = _usVariableGetAssociatedSimulator(variable);
		_usSimulatorStop(simulator);
	}
}

std::string SimulatorSetupStatusAsString(UnisimSimulatorSetupStatus status)
{
	std::stringstream str;
	switch ( status )
	{
		case UNISIM_SIMULATOR_SETUP_STATUS_OK:
			str << " (UNISIM_SIMULATOR_SETUP_STATUS_OK)";
			break;
		case UNISIM_SIMULATOR_SETUP_STATUS_WARNING:
			str << " (UNISIM_SIMULATOR_SETUP_STATUS_WARNING)";
			break;
		case UNISIM_SIMULATOR_SETUP_STATUS_ERROR:
			str << " (UNISIM_SIMULATOR_SETUP_STATUS_ERROR)";
			break;
		default:
			str << " (UNISIM_SIMULATOR_SETUP_STATUS_<unknown>)";
			break;
	}
	return str.str();
}

std::string SimulatorStatusAsString(UnisimSimulatorStatus status)
{
	std::stringstream str;
	switch ( status )
	{
		case UNISIM_SIMULATOR_STATUS_NONE:
			str << "UNISIM_SIMULATOR_STATUS_NONE";
			break;
		case UNISIM_SIMULATOR_STATUS_CREATED:
			str << "UNISIM_SIMULATOR_STATUS_CREATED";
			break;
		case UNISIM_SIMULATOR_STATUS_SETUP:
			str << "UNISIM_SIMULATOR_STATUS_SETUP";
			break;
		case UNISIM_SIMULATOR_STATUS_RUNNING:
			str << "UNISIM_SIMULATOR_STATUS_RUNNING";
			break;
		case UNISIM_SIMULATOR_STATUS_STOPPED:
			str << "UNISIM_SIMULATOR_STATUS_STOPPED";
			break;
		case UNISIM_SIMULATOR_STATUS_FINISHED:
			str << "UNISIM_SIMULATOR_STATUS_FINISHED";
			break;
		default:
			str << "UNISIM_SIMULATOR_STATUS_<unknown>";
			break;
	}
	return str.str();
}

std::map<uint64_t, uint64_t> insn_trace;
void BreakpointHandler(UnisimDebugAPI dapi, uint64_t addr)
{
	insn_trace[addr]++;
}

int test()
{
	if ( !load_lib() )
	{
		std::cerr << "Could not load symbols" << std::endl;
		return -1;
	}

	UnisimSimulator simulator;

	// creating the simulator
	simulator = _usCreateSimulator(0, 0);
	if ( simulator == 0 ) 
	{
		std::cerr << "Could not create simulator" << std::endl;
		return -1;
	}

	// obtaining its version
	const char *simulatorVersion = _usSimulatorGetVersion(simulator);
	if ( simulatorVersion == 0 )
		std::cerr << "Could not get the simulator version" << std::endl;
	else
		std::cerr << "Simulator version: " << simulatorVersion << std::endl;

	// getting a variable
	UnisimVariable variable;
	std::cerr << "Testing a single variable ('authors'):" << std::endl;
	variable = _usSimulatorGetVariable(simulator, "authors");
	if ( variable == 0 )
		std::cerr << "Could not get the authors variable" << std::endl;
	else
	{
		std::cerr << "The variable name is '" << _usVariableGetName(variable) << "'" <<  std::endl;
		std::cerr << "variableiable properties: " << std::endl
			<< " - visible      = " << _usVariableVisible(variable) << std::endl
			<< " - mutable      = " << _usVariableMutable(variable) << std::endl
			<< " - serializable = " << _usVariableSerializable(variable) << std::endl;
		std::cerr << "The variable value is '" << _usVariableGetValueAsString(variable) << "'" << std::endl;
	}
	_usDestroyVariable(variable);

	// getting multiple variables
	UnisimVariable *variableList;
	std::cerr << "Testing multiple variables:" << std::endl;
	variableList = _usSimulatorGetVariableList(simulator);
	int variableListIndex = 0;
	int variablesCounter = 0;
	while ( variableList[variableListIndex] != 0 )
	{
		_usDestroyVariable(variableList[variableListIndex]);
		variableListIndex++;
	}
	variablesCounter = variableListIndex;
	std::cerr << "Found " << variablesCounter << " variables" << std::endl;
	free(variableList);

	// getting variables list's of different types
	std::cerr << "Testing variables types:" << std::endl;
	variableList = _usSimulatorGetVariableListWithType(simulator, UNISIM_VARIABLE_TYPE_REGISTER);
	variableListIndex = 0;
	int registersCounter, parametersCounter, statisticsCounter, 
		formulasCounter, arraysCounter;
	while ( (variableList != 0 ) && (variableList[variableListIndex] != 0) )
	{
		_usDestroyVariable(variableList[variableListIndex]);
		variableListIndex++;
	}
	registersCounter = variableListIndex;
	std::cerr << "- found " << registersCounter << " registers" << std::endl;
	free(variableList);
	variableList = _usSimulatorGetVariableListWithType(simulator, UNISIM_VARIABLE_TYPE_PARAMETER);
	variableListIndex = 0;
	while ( (variableList != 0 ) && (variableList[variableListIndex] != 0) )
	{
		_usDestroyVariable(variableList[variableListIndex]);
		variableListIndex++;
	}
	parametersCounter = variableListIndex;
	std::cerr << "- found " << parametersCounter << " parameters" << std::endl;
	free(variableList);
	variableList = _usSimulatorGetVariableListWithType(simulator, UNISIM_VARIABLE_TYPE_STATISTIC);
	variableListIndex = 0;
	while ( (variableList != 0 ) && (variableList[variableListIndex] != 0) )
	{
		_usDestroyVariable(variableList[variableListIndex]);
		variableListIndex++;
	}
	statisticsCounter = variableListIndex;
	std::cerr << "- found " << statisticsCounter << " statistics" << std::endl;
	free(variableList);
	variableList = _usSimulatorGetVariableListWithType(simulator, UNISIM_VARIABLE_TYPE_FORMULA);
	variableListIndex = 0;
	while ( (variableList != 0 ) && (variableList[variableListIndex] != 0) )
	{
		_usDestroyVariable(variableList[variableListIndex]);
		variableListIndex++;
	}
	formulasCounter = variableListIndex;
	std::cerr << "- found " << formulasCounter << " formulas" << std::endl;
	free(variableList);
	variableList = _usSimulatorGetVariableListWithType(simulator, UNISIM_VARIABLE_TYPE_ARRAY);
	variableListIndex = 0;
	while ( (variableList != 0 ) && (variableList[variableListIndex] != 0) )
	{
		_usDestroyVariable(variableList[variableListIndex]);
	}
	arraysCounter = variableListIndex;
	std::cerr << "- found " << arraysCounter << " arrays" << std::endl;
	free(variableList);
	std::cerr << "- found a total of "
		<< (registersCounter + parametersCounter + statisticsCounter +
				formulasCounter + arraysCounter)
		<< " variables (from a total of "
		<< variablesCounter << " variables)"
		<< std::endl;

	// checking variables that are not registers, parameters, statistics or formulas
	std::cerr << "Checking non typed variables:" << std::endl;
	int voidsCounter = 0;
	variableList = _usSimulatorGetVariableList(simulator);
	variableListIndex = 0;
	while ( (variableList != 0) && (variableList[variableListIndex] != 0) )
	{
		UnisimVariable currentVariable = variableList[variableListIndex];
		UnisimVariable found;
		found = _usSimulatorGetVariableWithType(simulator, _usVariableGetName(currentVariable), UNISIM_VARIABLE_TYPE_REGISTER);
		if ( found == 0 )
		{
			found = _usSimulatorGetVariableWithType(simulator, _usVariableGetName(currentVariable), UNISIM_VARIABLE_TYPE_PARAMETER);
		}
		if ( found == 0 )
		{
			found = _usSimulatorGetVariableWithType(simulator, _usVariableGetName(currentVariable), UNISIM_VARIABLE_TYPE_STATISTIC);
		}
		if ( found == 0 )
		{
			found = _usSimulatorGetVariableWithType(simulator, _usVariableGetName(currentVariable), UNISIM_VARIABLE_TYPE_FORMULA);
		}
		if ( found == 0 )
		{
			found = _usSimulatorGetVariableWithType(simulator, _usVariableGetName(currentVariable), UNISIM_VARIABLE_TYPE_ARRAY);
		}
		if ( found == 0 )
		{
			voidsCounter++;
			std::cerr << " - " << voidsCounter << " "
				<< _usVariableGetName(currentVariable) << "("
				<< (_usVariableMutable(currentVariable) ? "M" : "")
				<< (_usVariableSerializable(currentVariable) ? "S" : "")
				<< (_usVariableVisible(currentVariable) ? "V" : "")
				<< "): " << _usVariableGetValueAsString(currentVariable) << std::endl;
		}
		else
		{
			_usDestroyVariable(found);
		}
		// here we should destroy the variable because it would never be used
		//   again, but we let it undestroyed to test that the simulator
		//   correctly deletes generated variables when destroyed
		// _usDestroyVariable(variableList[variableListIndex]); 
		variableListIndex++;
	}
	free(variableList);
	std::cerr << "- found a total of " << voidsCounter
		<< " non typed variables (from a total of "
		<< variablesCounter << " variables)" << std::endl;

	std::cerr << "Testing Extended API" << std::endl;
	UnisimDebugAPI dapi = 0;
	UnisimExtendedAPI *apiList = _usSimulatorGetExtendedAPIList(simulator);
	int apiListIndex = 0;
	while ( (apiList != 0) && (apiList[apiListIndex] != 0) )
	{
		UnisimExtendedAPI api = apiList[apiListIndex];
		std::cerr << " - " << _usExtendedAPIGetName(api) << " ("
			<< _usExtendedAPIGetTypeName(api) << ")" << std::endl;
		if ( strcmp("DebugAPI", _usExtendedAPIGetTypeName(api)) == 0 )
		{
			dapi = _usCreateDebugAPI(api);
		}
		_usDestroyExtendedAPI(api);
		apiListIndex++;
	}
	free(apiList);

	// run the simulator
	std::cerr << "Starting run test" << std::endl;

	// setting kernel_logger.std_err output
	std::cerr << " - setting logger";
	UnisimVariable loggerVariable =
		_usSimulatorGetVariable(simulator, "kernel_logger.std_err");
	if ( loggerVariable == 0 )
		std::cerr << std::endl << "WARNING: could not get logger output" << std::endl;
	else
	{
		std::cerr << " to true"; 
		_usVariableSetValueFromUnsignedLongLong(loggerVariable, 1);
		std::cerr << " (DONE)" << std::endl;
	}
	std::cerr << " - launching setup";
	UnisimSimulatorSetupStatus simulatorSetupStatus = _usSimulatorSetup(simulator);
	std::cerr << " " << SimulatorSetupStatusAsString(simulatorSetupStatus).c_str() << std::endl;
	std::cerr << " - simulator status ";
	UnisimSimulatorStatus simulatorStatus = _usSimulatorGetStatus(simulator); 
	std::cerr << " " << SimulatorStatusAsString(simulatorStatus).c_str() << std::endl;
	std::cerr << " - setting listener into instruction counter" << std::endl;
	UnisimVariable instructionCounterVariable =
		_usSimulatorGetVariable(simulator, "cpu.instruction-counter");
	if ( instructionCounterVariable == 0 )
	{
		std::cerr << "ERROR: could not get instruction counter variable" << std::endl;
		return CloseSimulator(simulator);
	}
	_usVariableSetListener(instructionCounterVariable, InstructionCounterListener);
	if ( dapi != 0 )
	{
		std::cerr << " - setting up debug step mode and breakpoint handler" << std::endl;
		if ( _usDebugAPISetBreakpointHandler(dapi, BreakpointHandler) )
		{
			if ( !_usDebugAPISetStepMode(dapi) )
			{
				std::cerr << "   - could not activate debug step mode" << std::endl;
			}
		}
		else
		{
			std::cerr << "   - could not set breakpoint handler" << std::endl;
		}
	}

	bool ok = _usSimulatorRun(simulator);
	if ( !ok )
		std::cerr << "Could not run the simulator" << std::endl;

	_usVariableRemoveListener(instructionCounterVariable);

	return CloseSimulator(simulator);
}

int main()
{
	if ( test() != 0 )
	{
		std::cerr << "Could not do more than once" << std::endl;
		return 0;
	}
	for ( std::map<uint64_t, uint64_t>::iterator it = insn_trace.begin();
			it != insn_trace.end();
			it++ )
	{
		std::cerr << "0x" << std::hex << (*it).first << std::dec << ": " << (*it).second << std::endl;
	}
	return test();
}
