#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <dlfcn.h>
#include "unisim/uapi/uapi.h"

#if 0

std::string VariableString(UniVariable var)
{
	std::stringstream str;
	str << GetName(var) << " (";
	switch ( GetType(var) )
	{
		case UNIVAR_VOID:
			str << "VOID";
			break;
		case UNIVAR_ARRAY:
			str << "ARRAY";
			break;
		case UNIVAR_PARAMETER:
			str << "PARAMETER";
			break;
		case UNIVAR_STATISTIC:
			str << "STATISTIC";
			break;
		case UNIVAR_REGISTER:
			str << "REGISTER";
			break;
		case UNIVAR_FORMULA:
			str << "FORMULA";
			break;
		default:
			str << "NONE";
			break;
	}
	str << ","
		<< (IsVisible(var) ? "V" : "")
		<< (IsMutable(var) ? "M" : "")
		<< (IsSerializable(var) ? "S" : "")
		<< ") = "
		<< GetValueAsString(var);

	return str.str();
}
#endif

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
typedef UnisimVariable *(* _usSimulatorGetVariableList_type)(UnisimSimulator, const char *);
_usSimulatorGetVariableList_type _usSimulatorGetVariableList;
typedef UnisimVariable *(* _usSimulatorGetVariableListWithType_type)(UnisimSimulator, const char *, UnisimVariableType);
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

#if 0
	// getting multiple variables
	UniVariable *var_list;
	std::cerr << "Testing multiple variables:" << std::endl;
	var_list = GetVariables(sim);
	int index = 0;
	int num_vars;
	while ( var_list[index] != 0 )
	{
		DestroyVariable(var_list[index]);
		index++;
	}
	num_vars = index;
	std::cerr << "Found " << (index - 1) << " variables" << std::endl;
	free(var_list);

	// getting multiple registers
	std::cerr << "Testing multiple registers:" << std::endl;
	var_list = GetVariablesWithType(sim, UNIVAR_REGISTER);
	index = 0;
	int num_regs, num_parms, num_stats, num_forms, num_arrays;
	while ( (var_list != 0 ) && (var_list[index] != 0) )
	{
		DestroyVariable(var_list[index]);
		index++;
	}
	num_regs = index;
	std::cerr << "- found " << num_regs << " registers" << std::endl;
	free(var_list);
	var_list = GetVariablesWithType(sim, UNIVAR_PARAMETER);
	index = 0;
	while ( (var_list != 0 ) && (var_list[index] != 0) )
	{
		DestroyVariable(var_list[index]);
		index++;
	}
	num_parms = index;
	std::cerr << "- found " << num_parms << " parameters" << std::endl;
	free(var_list);
	var_list = GetVariablesWithType(sim, UNIVAR_STATISTIC);
	index = 0;
	while ( (var_list != 0 ) && (var_list[index] != 0) )
	{
		DestroyVariable(var_list[index]);
		index++;
	}
	num_stats = index;
	std::cerr << "- found " << num_stats << " statistics" << std::endl;
	free(var_list);
	var_list = GetVariablesWithType(sim, UNIVAR_FORMULA);
	index = 0;
	while ( (var_list != 0 ) && (var_list[index] != 0) )
	{
		DestroyVariable(var_list[index]);
		index++;
	}
	num_forms = index;
	std::cerr << "- found " << num_forms << " formulas" << std::endl;
	free(var_list);
	var_list = GetVariablesWithType(sim, UNIVAR_ARRAY);
	index = 0;
	while ( (var_list != 0 ) && (var_list[index] != 0) )
	{
		DestroyVariable(var_list[index]);
	}
	num_arrays = index;
	std::cerr << "- found " << num_arrays << " arrays" << std::endl;
	free(var_list);
	std::cerr << "- found a total of "
		<< (num_forms + num_stats + num_parms + num_regs + num_arrays)
		<< " variables (from a total of "
		<< num_vars << " variables)"
		<< std::endl;

	// checking variables that are not registers, parameters, statistics or formulas
	std::cerr << "Checking non typed variables:" << std::endl;
	int num_void = 0;
	var_list = GetVariables(sim);
	index = 0;
	while ( (var_list != 0) && (var_list[index] != 0) )
	{
		UniVariable cur = var_list[index];
		UniVariable found;
		found = GetVariableWithType(sim, GetName(cur), UNIVAR_REGISTER);
		if ( found == 0 )
		{
			found = GetVariableWithType(sim, GetName(cur), UNIVAR_PARAMETER);
		}
		if ( found == 0 )
		{
			found = GetVariableWithType(sim, GetName(cur), UNIVAR_STATISTIC);
		}
		if ( found == 0 )
		{
			found = GetVariableWithType(sim, GetName(cur), UNIVAR_FORMULA);
		}
		if ( found == 0 )
		{
			found = GetVariableWithType(sim, GetName(cur), UNIVAR_ARRAY);
		}
		if ( found == 0 )
		{
			num_void++;
			std::cerr << " - " << num_void << " "
				<< GetName(cur) << "("
				<< (IsMutable(cur) ? "M" : "")
				<< (IsSerializable(cur) ? "S" : "")
				<< (IsVisible(cur) ? "V" : "")
				<< "): " << GetValueAsString(cur) << std::endl;
		}
		else
		{
			DestroyVariable(found);
		}
		cur = 0;
		DestroyVariable(var_list[index]); 
		index++;
	}
	free(var_list);
	std::cerr << "- found a total of " << num_void
		<< " non typed variables (from a total of "
		<< num_vars << " variables)" << std::endl;

#endif 

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
