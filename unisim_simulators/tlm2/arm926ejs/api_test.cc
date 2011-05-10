#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "unisim/uapi/uapi.h"

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

int main()
{
	UniSimulator sim;

	// creating the simulator
	sim = CreateSimulator(0, 0);
	if ( sim == 0 ) 
	{
		std::cerr << "Could not create simulator" << std::endl;
		return 0;
	}

	// obtaining its version
	const char *version = Version(sim);
	if ( version == 0 )
		std::cerr << "Could not get the simulator version" << std::endl;
	else
		std::cerr << "Simulator version: " << version << std::endl;

	// getting a variable
	UniVariable var;
	std::cerr << "Testing a single variable ('authors'):" << std::endl;
	var = GetVariable(sim, "authors");
	if ( var == 0 )
		std::cerr << "Could not get the authors variable" << std::endl;
	else
	{
		std::cerr << "The variable name is '" << GetName(var) << "'" <<  std::endl;
		std::cerr << "Variable properties: " << std::endl
			<< " - visible      = " << IsVisible(var) << std::endl
			<< " - mutable      = " << IsMutable(var) << std::endl
			<< " - serializable = " << IsSerializable(var) << std::endl;
		std::cerr << "The variable value is '" << GetValueAsString(var) << "'" << std::endl;
	}
	DestroyVariable(var);

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

	// destroying the simulator
	bool st = DestroySimulator(sim);
	if ( !st )
		std::cerr << "Could not destroy simulator" << std::endl;
	return 0;
}
