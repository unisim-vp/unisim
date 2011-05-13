#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <dlfcn.h>
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
typedef void (* _usDestroyVariable_type)(UnisimVariable);
_usDestroyVariable_type _usDestroyVariable;

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
	simlib = dlopen("/Users/gracia/Developer/unisim/arm926ejs/build/lib/liblibuapi.dylib", RTLD_LAZY);
	if(simlib == NULL)
	{
		std::cerr << "Could not load simulator dynamic library"
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
	if ( !load_sym(simlib, "usDestroyVariable", _usDestroyVariable) )
		return false;

	return true;
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
		std::cerr << "The variableiable name is '" << _usVariableGetName(variable) << "'" <<  std::endl;
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
		currentVariable = 0;
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

int main()
{
	if ( test() != 0 )
	{
		std::cerr << "Could not do more than once" << std::endl;
		return 0;
	}
	return test();
}
