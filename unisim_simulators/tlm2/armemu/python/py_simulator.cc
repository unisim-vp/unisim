/*
 *  Copyright (c) 2010,
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

#include <Python.h>
#include <map>
#include "simulator.hh"
#define SIMULATOR_MODULE
#include "python/py_simulator.hh"
#include "python/py_variable.hh"
#include "python/py_debugger.hh"

static std::map<std::string, int> time_unit_map;
static std::map<std::string, sc_time_unit> sc_time_unit_map;

void TimeUnitMapInit()
{
	time_unit_map.insert(std::pair<std::string, int>("fs", 0));
	time_unit_map.insert(std::pair<std::string, int>("FS", 0));
	time_unit_map.insert(std::pair<std::string, int>("ps", 1));
	time_unit_map.insert(std::pair<std::string, int>("PS", 1));
	time_unit_map.insert(std::pair<std::string, int>("ns", 2));
	time_unit_map.insert(std::pair<std::string, int>("NS", 2));
	time_unit_map.insert(std::pair<std::string, int>("us", 3));
	time_unit_map.insert(std::pair<std::string, int>("US", 3));
	time_unit_map.insert(std::pair<std::string, int>("ms", 4));
	time_unit_map.insert(std::pair<std::string, int>("MS", 4));
	time_unit_map.insert(std::pair<std::string, int>("sec", 5));
	time_unit_map.insert(std::pair<std::string, int>("SEC", 5));

	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("fs", SC_FS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("FS", SC_FS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("ps", SC_PS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("PS", SC_PS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("ns", SC_NS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("NS", SC_NS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("us", SC_US));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("US", SC_US));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("ms", SC_MS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("MS", SC_MS));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("sec", SC_SEC));
	sc_time_unit_map.insert(std::pair<std::string, sc_time_unit>("SEC", SC_SEC));
}

//static int TimeUnit(const std::string& unit)
//{
//	const std::map<std::string, int>::iterator it = time_unit_map.find(unit);
//
//	if ( it == time_unit_map.end() ) return -1;
//	return (*it).second;
//}

static bool SCTimeUnit(const std::string& unit, sc_time_unit& sc_unit)
{
	const std::map<std::string, sc_time_unit>::iterator it = sc_time_unit_map.find(unit);

	if ( it == sc_time_unit_map.end() ) return false;
	sc_unit = (*it).second;

	return true;
}

extern "C" {
static Simulator *__current_simulator;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Simulator *sim;
    unisim::kernel::service::Simulator::SetupStatus setup;
    PyObject *trap_handler;
    PyObject *trap_context;
} armemu_SimulatorObject;

static Simulator *
create_simulator(char *xml_file, std::vector<std::string> *parms)
{
	Simulator *sim;
	char **argv;
	int argv_size = 4; // default number of parameters;
	if ( xml_file != NULL )
		argv_size += 2;
	if ( parms != NULL )
		argv_size += (parms->size() * 2);
	argv = (char **)malloc(sizeof(char *) * (argv_size + 1));
	int index = 0;
	argv[index++] = (char *)ARMEMU_EXEC_LOCATION;
	argv[index++] = (char *)"-p";
	argv[index++] = (char *)PYTHON_LIB_TO_SHARED_DATA_PATH;
	argv[index++] = (char *)"-w";
	if ( xml_file != NULL )
		argv[index++] = xml_file;
	if ( parms != NULL )
	{
		std::vector<std::string>::iterator it;
		for ( it = parms->begin(); it != parms->end(); it++ )
		{
			argv[index++] = (char *)"-s";
			argv[index++] = (char *)(*it).c_str();
		}
	}
	argv[index] = 0;

	sim = new Simulator(argv_size, argv);
	return sim;
}

static void
destroy_simulator(armemu_SimulatorObject *self)
{
	Simulator *sim = self->sim;
	if ( sim )
	{
		delete sim;
		if ( sc_curr_simcontext == sc_default_global_context )
		{
			delete sc_curr_simcontext;
		}
		else
		{
			delete sc_curr_simcontext;
			delete sc_default_global_context;
		}
		sc_curr_simcontext = 0;
		sc_default_global_context = 0;
	}
	self->sim = 0;
	sim = 0;
	if ( self->trap_handler )
		Py_DECREF(self->trap_handler);
	if ( self->trap_context )
		Py_DECREF(self->trap_context);
	self->trap_handler = 0;
	self->trap_context = 0;
}

static void
simulator_dealloc (armemu_SimulatorObject *self)
{
	destroy_simulator(self);
	__current_simulator = 0;

	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
simulator_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	armemu_SimulatorObject *self;
	self = (armemu_SimulatorObject *)type->tp_alloc(type, 0);
	self->sim = 0; // create_simulator();
	self->setup = unisim::kernel::service::Simulator::ST_ERROR; // self->sim->Setup();
	return (PyObject *)self;
}

static PyObject *
pydict_from_variable_list ( armemu_SimulatorObject *self,
		std::list<unisim::kernel::service::VariableBase*>& list)
{
	PyObject *result;
	result = PyDict_New();
	if ( result == NULL ) return NULL;

	for ( std::list<unisim::kernel::service::VariableBase *>::iterator it = list.begin();
			it != list.end();
			it++ )
	{
		PyObject *variable;
		variable = PyVariable_NewVariable((*it)->GetName());
		if ( variable == NULL )
		{
			PyDict_Clear(result);
			result = NULL;
			return result;
		}
		if ( PyDict_SetItemString(result, (*it)->GetName(), variable) == -1)
		{
			Py_DECREF(variable);
			PyDict_Clear(result);
			result = NULL;
			return result;
		}
		Py_DECREF(variable);
	}

	return result;
}

static void
simulator_trap_handler (void *_self, unsigned int id)
{
	PyObject *result;
	armemu_SimulatorObject *self = (armemu_SimulatorObject *)_self;
	if ( self->trap_handler == 0 )
	{
		return;
	}
	PyObject *arglist;
	arglist = Py_BuildValue("(Oi)", self->trap_context, id);
	result = PyObject_CallObject(self->trap_handler, arglist);
	Py_DECREF(arglist);
	if ( result != NULL ) Py_DECREF(result);
}

static int
simulator_init (armemu_SimulatorObject *self, PyObject *args, PyObject *kwds)
{
	char *xml_file = NULL;
	PyObject *parms = NULL;
	std::vector<std::string> *cparms = NULL;

	static char *kwlist[] = {"xml", "parms", NULL};
	if (! PyArg_ParseTupleAndKeywords(args, kwds, "|sO", kwlist,
			&xml_file, &parms))
		return -1;
	if ( (parms != NULL) )
	{
		if ( !PyDict_Check(parms) )
		{
			PyErr_SetString(PyExc_TypeError, "'parms' must be a dictionary of parameters");
			return -1;
		}
		PyObject *key, *value;
		Py_ssize_t pos = 0;
		while ( PyDict_Next(parms, &pos, &key, &value) )
		{
			if ( !PyUnicode_Check(key) )
			{
				PyErr_SetString(PyExc_TypeError,"parameter names (keys in the 'parms' dictionary must be unicode strings" );
				return -1;
			}
		}
		pos = 0;
		cparms = new std::vector<std::string>();
		while ( PyDict_Next(parms, &pos, &key, &value) )
		{
			char *ckey = 0;
			char *cvalue = 0;
			PyObject *utf8;

			utf8 = PyUnicode_AsEncodedString(key, NULL, NULL);
			ckey = PyBytes_AsString(utf8);
			Py_DECREF(utf8);
			if ( !PyUnicode_Check(value) )
			{
				PyObject *unicode;
				if ( PyBool_Check(value) )
				{
					if ( PyObject_IsTrue(value) )
						unicode = PyUnicode_FromFormat("true");
					else
						unicode = PyUnicode_FromFormat("false");
				}
				else
					unicode = PyUnicode_FromFormat("%A", value);
				utf8 = PyUnicode_AsEncodedString(unicode, NULL, NULL);
				Py_DECREF(unicode);
			}
			else
				utf8 = PyUnicode_AsEncodedString(value, NULL, NULL);
			cvalue = PyBytes_AsString(utf8);
			Py_DECREF(utf8);
			std::stringstream assign;
			assign << ckey << "=" << cvalue;
			cparms->push_back(assign.str());
		}
	}

	self->sim = create_simulator(xml_file, cparms);
	if ( xml_file )
		free(xml_file);

	if ( cparms )
		delete cparms;

	self->sim->SetTrapHandler(simulator_trap_handler, (void *)self);
	self->trap_handler = 0;
	self->trap_context = 0;
	__current_simulator = self->sim;

	return 0;
}

static PyObject *
simulator_setup (armemu_SimulatorObject *self)
{
	PyObject *result;
	self->setup = self->sim->Setup();
	result = PyLong_FromLong(self->setup);

	return result;
}

static PyObject *
simulator_version (armemu_SimulatorObject *self)
{
	PyObject *result;
	std::string version = (std::string)*self->sim->FindVariable("version");
	result = PyUnicode_FromString(version.c_str());
	return result;
}

static PyObject *
simulator_get_variable (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result = NULL;
	const char *var_name;

	if ( !PyArg_ParseTuple(args, "s", &var_name) )
		return result;
	unisim::kernel::service::VariableBase *var = self->sim->FindVariable(var_name);
	if ( var == 0 )
		PyErr_SetString(PyExc_ValueError, "could not find the given variable");
	else
		result = PyVariable_NewVariable(var->GetName());
	return result;
}

static PyObject *
simulator_get_variables (armemu_SimulatorObject *self)
{
	PyObject *result;

	std::list<unisim::kernel::service::VariableBase *> var_list;
	self->sim->GetVariables(var_list);
	result = pydict_from_variable_list(self, var_list);

	return result;
}

static PyObject *
simulator_get_parameters (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> parm_list;
	self->sim->GetParameters(parm_list);
	result = pydict_from_variable_list(self, parm_list);

	return result;
}

static PyObject *
simulator_get_statistics (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> stat_list;
	self->sim->GetStatistics(stat_list);
	result = pydict_from_variable_list(self, stat_list);

	return result;
}

static PyObject *
simulator_get_registers (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> reg_list;
	self->sim->GetRegisters(reg_list);
	result = pydict_from_variable_list(self, reg_list);

	return result;
}

static PyObject *
simulator_get_formulas ( armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> form_list;
	self->sim->GetFormulas(form_list);
	result = pydict_from_variable_list(self, form_list);

	return result;
}

//static void
//get_parameters_map (Simulator *sim, std::map<std::string, std::string> &parms)
//{
//	if ( sim == 0 ) return;
//	std::list<unisim::kernel::service::VariableBase *> parm_list;
//	sim->GetParameters(parm_list);
//	for ( std::list<unisim::kernel::service::VariableBase *>::iterator it = parm_list.begin();
//			it != parm_list.end();
//			it++ )
//		parms.insert(make_pair(std::string((*it)->GetName()),
//				(std::string)*(*it)));
//}
//
//static void
//set_parameters_map (Simulator *sim, std::map<std::string, std::string> &parms)
//{
//	if ( sim == 0 ) return;
//	for ( std::map<std::string, std::string>::iterator it = parms.begin();
//			it != parms.end();
//			it++ )
//		sim->SetVariable(it->first.c_str(), it->second.c_str());
//}

static PyObject *
simulator_is_running (armemu_SimulatorObject *self)
{
	PyObject *result = 0;

	if ( !self->setup )
	{
		result = PyUnicode_FromString("Simulation setup failed.");
	}

	if ( result == 0 )
	{
		if ( self->sim->IsRunning() )
			Py_RETURN_TRUE;
		Py_RETURN_FALSE;
	}
	return result;
}

static PyObject *
simulator_has_started (armemu_SimulatorObject *self)
{
	PyObject *result = 0;

	if ( !self->setup )
	{
		result = PyUnicode_FromString("Simulation setup failed.");
	}

	if ( result == 0 )
	{
		if ( self->sim->SimulationStarted() )
			Py_RETURN_TRUE;
		Py_RETURN_FALSE;
	}
	return result;
}

static PyObject *
simulator_has_finished(armemu_SimulatorObject *self)
{
	PyObject *result = 0;

	if ( !self->setup )
	{
		result = PyUnicode_FromString("Simulation setup failed.");
	}

	if ( result == 0 )
	{
		if ( self->sim->SimulationFinished() )
			Py_RETURN_TRUE;
		Py_RETURN_FALSE;
	}
	return result;
}

static PyObject *
simulator_run (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result = 0;
	double dtime = 0.0;
	char *cunit = "sec";
	std::string unit;
	sc_time_unit time_unit;
	if ( self->setup != unisim::kernel::service::Simulator::ST_OK_TO_START &&
			self->setup != unisim::kernel::service::Simulator::ST_WARNING )
	{
		result = PyUnicode_FromString("Simulation setup failed.");
	}

	if ( (result == 0) && PyArg_ParseTuple(args, "|ds", &dtime, cunit))
	{
		unit = cunit;
	}
	else
		return result;

	if ( result ) return result;
	if ( !SCTimeUnit(unit, time_unit) )
	{
		result = PyUnicode_FromFormat("ERROR: Unknown time_unit \"%s\"", unit.c_str());
		return result;
	}

	if ( dtime == 0.0 )
	{
		self->sim->Run();
		result = PyUnicode_FromString("Simulation finished.");
	}
	else
	{
		self->sim->Run(dtime, time_unit);
		PyObject *py_dtime = PyFloat_FromDouble(dtime);
		result = PyUnicode_FromFormat("Simulation of %S%s finished.",
				py_dtime, unit.c_str(), time_unit);
	}

	return result;
}

static PyObject *
simulator_stop (armemu_SimulatorObject *self)
{
	self->sim->Stop();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
simulator_set_trap_handler (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result = 0;

	PyObject *handler;
	PyObject *context;

	if ( !PyArg_ParseTuple(args, "OO", &context, &handler) )
	{
		PyErr_SetString(PyExc_TypeError, "parameters must be a context and a function/method");
		return NULL;
	}
    if ( !PyCallable_Check(handler) )
    {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    Py_XINCREF(handler);         /* Add a reference to new callback */
	if ( self->trap_handler )
	{
		result = self->trap_handler;
	    Py_DECREF(result);  /* Dispose of previous callback */
	}
	self->trap_handler = handler;
	if ( self->trap_context )
		Py_DECREF(self->trap_context);
	Py_INCREF(context);
	self->trap_context = context;
	if ( result )
		return result;
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
simulator_remove_trap_handler (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->trap_handler )
	{
		if ( self->trap_context )
		{
			result = Py_BuildValue("(OO)", self->trap_handler, self->trap_context);
			Py_DECREF(self->trap_context);
			self->trap_context = 0;
		}
		else
		{
			Py_INCREF(Py_None);
			result = Py_BuildValue("(OO)", self->trap_handler, Py_None);
		}
		Py_DECREF(self->trap_handler);
	}
	else
	{
		result = Py_None;
		Py_INCREF(result);
	}
	return result;
}

static PyObject *
simulator_get_debugger (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result = 0;

	const char *debugger_name = 0;
	if ( !PyArg_ParseTuple(args, "s", &debugger_name) )
	{
		PyErr_SetString(PyExc_TypeError, "parameters must be the name of the desired debugger");
		return NULL;
	}
	// the name of the debugger should be checked before its creation
	result = PyDebugger_NewDebugger(debugger_name);
	return result;
}

static PyMethodDef simulator_methods[] =
{
	{"setup", (PyCFunction)simulator_setup, METH_NOARGS,
			"Setup the simulator."},
	{"version", (PyCFunction)simulator_version, METH_NOARGS,
			"Return the simulator version."},
	{"get_variables", (PyCFunction)simulator_get_variables, METH_NOARGS,
			"Return a dictionary with the complete list of the simulator variables."},
	{"get_parameters", (PyCFunction)simulator_get_parameters, METH_NOARGS,
			"Return a dictionary with the complete list of the simulator parameters."},
	{"get_statistics", (PyCFunction)simulator_get_statistics, METH_NOARGS,
			"Return a dictionary with the complete list of the simulator statistics."},
	{"get_registers", (PyCFunction)simulator_get_registers, METH_NOARGS,
			"Return a dictionary with the complete list of the simulator registers."},
	{"get_formulas", (PyCFunction)simulator_get_formulas, METH_NOARGS,
			"Return a dictionary with the complete list of the simulator formulas."},
	{"get_variable", (PyCFunction)simulator_get_variable, METH_VARARGS,
			"Get a simulator variable if existent."},
	{"run", (PyCFunction)simulator_run, METH_VARARGS,
			"Run the simulator."},
	{"stop", (PyCFunction)simulator_stop, METH_NOARGS,
			"Stop the simulator."},
	{"is_running", (PyCFunction)simulator_is_running, METH_NOARGS,
			"Check if the simulator is running or not."},
	{"has_started", (PyCFunction)simulator_has_started, METH_NOARGS,
			"Checks if the simulation has been started or not"},
	{"has_finished", (PyCFunction)simulator_has_finished, METH_NOARGS,
			"Checks if the simulation has been finished or not"},
	{"set_trap_handler", (PyCFunction)simulator_set_trap_handler, METH_VARARGS,
			"Set the external trap handler"},
	{"remove_trap_handler", (PyCFunction)simulator_remove_trap_handler, METH_NOARGS,
			"Remove the external trap handler"},
	{"get_debugger", (PyCFunction)simulator_get_debugger, METH_VARARGS,
					"Get a handler of the given name debugger"},
	{NULL}
};

static PyTypeObject armemu_SimulatorType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    PACKAGE_NAME".simulator.Simulator",					/* tp_name */
    sizeof(armemu_SimulatorObject),		/* tp_basicsize */
    0,									/* tp_itemsize */
    (destructor)simulator_dealloc,		/* tp_dealloc */
    0,									/* tp_print */
    0,									/* tp_getattr */
    0,									/* tp_setattr */
    0,									/* tp_reserved */
    0,									/* tp_repr */
    0,									/* tp_as_number */
    0,									/* tp_as_sequence */
    0,									/* tp_as_mapping */
    0,									/* tp_hash  */
    0,									/* tp_call */
    0,									/* tp_str */
    0,									/* tp_getattro */
    0,									/* tp_setattro */
    0,									/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,					/* tp_flags */
    "UNISIM ARMEmu simulator objects",	/* tp_doc */
    0,									/* tp_traverse */
    0,									/* tp_clear */
    0,									/* tp_richcompare */
    0,									/* tp_weaklistoffset */
    0,									/* tp_iter */
    0,									/* tp_iternext */
    simulator_methods,									/* tp_methods */
    0,									/* tp_members */
    0,									/* tp_getset */
    0,									/* tp_base */
    0,									/* tp_dict */
    0,									/* tp_descr_get */
    0,									/* tp_descr_set */
    0,									/* tp_dictoffset */
    (initproc)simulator_init,			/* tp_init */
    0,									/* tp_alloc */
    simulator_new,						/* tp_new */
};

//int PySimulatorType_Ready()
//{
//    return PyType_Ready(&armemu_SimulatorType);
//}
//
//int PyModule_AddSimulatorObject(PyObject *m)
//{
//    TimeUnitMapInit();
//
//	Py_INCREF(&armemu_SimulatorType);
//	PyModule_AddObject(m, "Simulator", (PyObject *)&armemu_SimulatorType);
//	return 0;
//}

static PyModuleDef simulatormodule = {
    PyModuleDef_HEAD_INIT,
    "simulator",
	//SIM_PYTHON_MODULE_NAME,
    "UNISIM python simulator module.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_simulator(void)
// PyInit_ARMEMU_DECLARATION
{
    PyObject* m;
    static void *PySimulator_API[PySimulator_API_pointers];
    PyObject *c_api_object;

    __current_simulator = 0;

    if ( PyType_Ready(&armemu_SimulatorType) < 0 )
    	return NULL;

    m = PyModule_Create(&simulatormodule);
    if (m == NULL)
        return NULL;

    TimeUnitMapInit();
	Py_INCREF(&armemu_SimulatorType);
	PyModule_AddObject(m, "Simulator", (PyObject *)&armemu_SimulatorType);

	/* Initialize the C API pointer array */
	PySimulator_API[PySimulator_GetSimRef_NUM] = (void *)PySimulator_GetSimRef;

	/* Create a Capsule containing the API pointer array's address */
	c_api_object = PyCapsule_New((void *)PySimulator_API, PySimulator_Capsule_Name, NULL);

	if ( c_api_object != NULL )
		PyModule_AddObject(m, "_C_API", c_api_object);
	else
		return NULL;

	import_variable_api_init();

	if ( import_variable_api() < 0 )
		return NULL;

	import_debugger_api_init();

	if ( import_debugger_api() < 0 )
		return NULL;

	return m;
}

static Simulator *
PySimulator_GetSimRef()
{
	return __current_simulator;
}

}
