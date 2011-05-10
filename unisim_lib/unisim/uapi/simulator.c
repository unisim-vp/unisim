/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : UNISIM C API                                                **
** filename  : simulator.c                                                 **
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

#define __UNISIM__UAPI__SIMULATOR__C_SRC

/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

#include <string>
#include "simulator.hh"

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

struct _UniSimulator
{
	Simulator *sim;
	UniSimulatorStatus status;
};

/****************************************************************************/
/**                                                                        **/
/**                     PROTOTYPES OF LOCAL FUNCTIONS                      **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
bool VariableTypeToVariableBaseType(VariableType type,
		unisim::kernel::service::VariableBase::Type &vartype);
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
UniSimulator CreateSimulator(char *xml_file, char **options)
/****************************************************************************/
{
	_UniSimulator *usim;
	char **argv;
	int argv_size = 4; // default number of parameters

	usim = (_UniSimulator *)malloc(sizeof(_UniSimulator));
	if ( usim == 0 ) return 0;
	usim->sim = 0;
	usim->status = NONE;

	if ( xml_file != 0 )
	{
		argv_size += 2;
	}

	int options_size;
	options_size = 0;
	while ( (options != 0) && (options[options_size] != 0) ) options_size++;
	argv_size += options_size * 2;

	int index;
	index = 0;
	argv = (char **)malloc(sizeof(char *) * (argv_size + 1));
	if ( argv == 0 ) 
	{
		free(usim);
		return 0;
	}
	argv[index++] = (char *)API__SIM_EXEC_LOCATION;
	argv[index++] = (char *)"-p";
	argv[index++] = (char *)API__LIB_TO_SHARED_DATA_PATH;
	argv[index++] = (char *)"-w";
	if ( xml_file != 0 )
		argv[index++] = xml_file;
	if ( options_size != 0 )
	{
		while ( options_size != 0 )
		{
			argv[index++] = (char *)"-s";
			argv[index++] = options[options_size - 1];
			options_size--;
		}
	}
	argv[index] = 0;

	usim->sim = new Simulator(argv_size, argv);
	if ( usim->sim == 0 ) 
	{
		free(argv);
		free(usim);
		return 0;
	}

	usim->status = CREATED;
	return usim;
}

/****************************************************************************/
bool DestroySimulator(UniSimulator sim)
/****************************************************************************/
{
	if ( sim == 0 ) return true;

	if ( sim->status == RUNNING ) return false;

	delete(sim->sim);
	sim->sim = 0;
	free(sim);
	sim = 0;
	return true;
}

/****************************************************************************/
UniSimulatorStatus GetSimulatorStatus(UniSimulator sim)
/****************************************************************************/
{
	UniSimulatorStatus status;

	if ( sim == 0 ) status = NONE;
	else status = sim->status;

	return status;
}

/****************************************************************************/
UniSimulatorSetupStatus Setup(UniSimulator sim)
/****************************************************************************/
{
	if ( sim == 0 ) return ERROR;

	// this constraint maybe relaxed in the future, but for the moment it
	// is safer to just allow the execution of setup once in the simulator
	// lifetime
	if ( sim->status != CREATED )
	{
		return ERROR;
	}

	unisim::kernel::service::Simulator::SetupStatus status;
	UniSimulatorSetupStatus ret_status;
	status = sim->sim->Setup();
	switch ( status )
	{
		case unisim::kernel::service::Simulator::ST_OK_TO_START:
			sim->status = SETUP;
			ret_status = OK;
			break;
		case unisim::kernel::service::Simulator::ST_OK_DONT_START:
			sim->status = FINISHED;
			ret_status = OK;
			break;
		case unisim::kernel::service::Simulator::ST_WARNING:
			sim->status = CREATED;
			ret_status = WARNING;
			break;
		case unisim::kernel::service::Simulator::ST_ERROR:
			sim->status = CREATED;
			ret_status = ERROR;
			break;
		default:
			sim->status = FINISHED;
			ret_status = ERROR;
			break;
	}

	return ret_status;
}

/****************************************************************************/
const char *Version(UniSimulator sim)
/****************************************************************************/
{
	if ( sim == 0 ) return 0;

	std::string version = (std::string)*sim->sim->FindVariable("version");
	return version.c_str();
}

/****************************************************************************/
bool Run(UniSimulator sim)
/****************************************************************************/
{
	return false;
}

/****************************************************************************/
void Stop(UniSimulator sim)
/****************************************************************************/
{
}

/****************************************************************************/
UniVariable GetVariable(UniSimulator sim, const char *varname)
/****************************************************************************/
{
	return GetVariableWithType(sim, varname, UNIVAR_VOID);
}

/****************************************************************************/
UniVariable GetVariableWithType(UniSimulator sim, const char *varname,
		VariableType type)
/****************************************************************************/
{
	if ( sim == 0 ) return 0;

	unisim::kernel::service::VariableBase::Type vartype;
	if ( !VariableTypeToVariableBaseType(type, vartype) )
		return 0;

	unisim::kernel::service::VariableBase *var =
		sim->sim->FindVariable(varname, vartype);
	if ( var == 0 ) return 0;

	UniVariable univar = CreateVariable(var);
	if ( univar == 0 ) return 0;
	return univar;
}

/****************************************************************************/
UniVariable *GetVariables(UniSimulator sim)
/****************************************************************************/
{
	std::list<unisim::kernel::service::VariableBase *> var_list;
	UniVariable *ret_list;

	if ( sim == 0 ) return 0;

	sim->sim->GetVariables(var_list);

	if ( var_list.size() == 0 ) return 0;

	ret_list = (UniVariable *)malloc(sizeof(UniVariable) * var_list.size());
	
	if ( ret_list == 0 ) return 0;

	ret_list = (UniVariable *)memset(ret_list, 
			0, 
			sizeof(UniVariable) * var_list.size());

	std::list<unisim::kernel::service::VariableBase *>::iterator it;
	int index;
	bool error;
	index = 0;
	error = false;
	for ( it = var_list.begin(); 
			(!error) && (it != var_list.end());
			it++ )
	{
		UniVariable univar = CreateVariable(*it);
		if ( univar == 0 ) error = true;
		else ret_list[index] = univar;
		index++;
	}

	if ( error )
	{
		for ( index = 0; ret_list[index] != 0; index++ )
		{
			DestroyVariable(ret_list[index]);
			ret_list[index] = 0;
		}
		free(ret_list);
		return 0;
	}
	
	return ret_list;
}

/****************************************************************************/
UniVariable *GetVariablesWithType(UniSimulator sim, VariableType type)
/****************************************************************************/
{
	std::list<unisim::kernel::service::VariableBase *> var_list;
	UniVariable *ret_list;

	if ( sim == 0 ) return 0;

	unisim::kernel::service::VariableBase::Type vartype;
	if ( !VariableTypeToVariableBaseType(type, vartype) )
		return 0;

	sim->sim->GetVariables(var_list, vartype);

	if ( var_list.size() == 0 ) return 0;

	ret_list = (UniVariable *)malloc(sizeof(UniVariable) * var_list.size());
	
	if ( ret_list == 0 ) return 0;

	ret_list = (UniVariable *)memset(ret_list, 
			0, 
			sizeof(UniVariable) * var_list.size());

	std::list<unisim::kernel::service::VariableBase *>::iterator it;
	int index;
	bool error;
	index = 0;
	error = false;
	for ( it = var_list.begin(); 
			(!error) && (it != var_list.end());
			it++ )
	{
		UniVariable univar = CreateVariable(*it);
		if ( univar == 0 ) error = true;
		else ret_list[index] = univar;
		index++;
	}

	if ( error )
	{
		for ( index = 0; ret_list[index] != 0; index++ )
		{
			DestroyVariable(ret_list[index]);
			ret_list[index] = 0;
		}
		free(ret_list);
		return 0;
	}
	
	return ret_list;
}

/****************************************************************************/
/**                                                                        **/
/**                     LOCAL FUNCTIONS                                    **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
bool VariableTypeToVariableBaseType(VariableType type,
		unisim::kernel::service::VariableBase::Type &vartype)
/****************************************************************************/
{
	switch ( type )
	{
		case UNIVAR_VOID: 
			vartype = unisim::kernel::service::VariableBase::VAR_VOID; 
			break;
		case UNIVAR_ARRAY:
			vartype = unisim::kernel::service::VariableBase::VAR_ARRAY;
			break;
		case UNIVAR_PARAMETER:
			vartype = unisim::kernel::service::VariableBase::VAR_PARAMETER;
			break;
		case UNIVAR_STATISTIC:
			vartype = unisim::kernel::service::VariableBase::VAR_STATISTIC;
			break;
		case UNIVAR_REGISTER:
			vartype = unisim::kernel::service::VariableBase::VAR_REGISTER;
			break;
		case UNIVAR_FORMULA:
			vartype = unisim::kernel::service::VariableBase::VAR_FORMULA;
			break;
		default:
			return false;
			break;
	}
	return true;
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

