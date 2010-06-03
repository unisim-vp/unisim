/*
 * py_simulator.cc
 *
 *  Created on: May 31, 2010
 *      Author: gracia
 */

#include <Python.h>
#include "simulator.hh"
#include "python/py_simulator.hh"
#include "python/py_variable.hh"

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

static int TimeUnit(const std::string& unit)
{
	const std::map<std::string, int>::iterator it = time_unit_map.find(unit);

	if ( it == time_unit_map.end() ) return -1;
	return (*it).second;
}

static bool SCTimeUnit(const std::string& unit, sc_time_unit& sc_unit)
{
	const std::map<std::string, sc_time_unit>::iterator it = sc_time_unit_map.find(unit);

	if ( it == sc_time_unit_map.end() ) return false;
	sc_unit = (*it).second;

	return true;
}

extern "C" {

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Simulator *sim;
    unisim::kernel::service::Simulator::SetupStatus setup;
} armemu_SimulatorObject;

static Simulator *
create_simulator()
{
	Simulator *sim;
	char *argv[3] =
	{
			(char *)"/Users/gracia/Developer/unisim/armemu/build/bin/unisim-armemu-0.3.1",
			(char *)"-w",
			0
	};

	sim = new Simulator(2, argv);
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
}

static void
simulator_dealloc (armemu_SimulatorObject *self)
{
	destroy_simulator(self);

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

static int
simulator_init (armemu_SimulatorObject *self, PyObject *args, PyObject *kwds)
{
	self->sim = create_simulator();
	self->setup = self->sim->Setup();
	return 0;
}

static PyObject *
simulator_version (armemu_SimulatorObject *self)
{
	PyObject *result;
	std::string version = (std::string)*self->sim->FindVariable("version");
	result = PyUnicode_FromString(version.c_str());
	return result;
}

//static PyObject *
//simulator_get_variable (armemu_SimulatorObject *self, PyObject *args)
//{
//	PyObject *result;
//	const char *var_name;
//
//	if ( !PyArg_ParseTuple(args, "s", &var_name) )
//		return NULL;
//	std::string var = (std::string)*self->sim->FindVariable(var_name);
//	result = PyUnicode_FromString(var.c_str());
//	return result;
//}

static PyObject *
simulator_get_variable (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result;
	const char *var_name;

	if ( !PyArg_ParseTuple(args, "s", &var_name) )
		return NULL;
	unisim::kernel::service::VariableBase *var = self->sim->FindVariable(var_name);
	if ( var != 0 )
	{
		result = PyVariable_NewVariable(var);
	}
	return result;
}

static PyObject *
pydict_from_variable_list ( std::list<unisim::kernel::service::VariableBase*>& list)
{
	PyObject *result;
	result = PyDict_New();
	if ( result == NULL ) return NULL;

	for ( std::list<unisim::kernel::service::VariableBase *>::iterator it = list.begin();
			it != list.end();
			it++ )
	{
		PyObject *name;
		PyObject *value;
		name = PyUnicode_FromString((*it)->GetName());
		value = PyUnicode_FromString(((std::string)*(*it)).c_str());
		if ( PyDict_SetItem(result, name, value) == -1)
		{
			Py_DECREF(name);
			Py_DECREF(value);
			PyDict_Clear(result);
			result = NULL;
			return result;
		}
		Py_DECREF(name);
		Py_DECREF(value);
	}

	return result;
}

static PyObject *
simulator_get_variables (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> var_list;
	self->sim->GetParameters(var_list);
	result = pydict_from_variable_list(var_list);

	return result;
}

static PyObject *
simulator_get_parameters (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> parm_list;
	self->sim->GetParameters(parm_list);
	result = pydict_from_variable_list(parm_list);

	return result;
}

static PyObject *
simulator_get_statistics (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> stat_list;
	self->sim->GetStatistics(stat_list);
	result = pydict_from_variable_list(stat_list);

	return result;
}

static PyObject *
simulator_get_registers (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> reg_list;
	self->sim->GetRegisters(reg_list);
	result = pydict_from_variable_list(reg_list);

	return result;
}

static PyObject *
simulator_get_formulas ( armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->sim == 0 ) return NULL;
	std::list<unisim::kernel::service::VariableBase *> form_list;
	self->sim->GetFormulas(form_list);
	result = pydict_from_variable_list(form_list);

	return result;
}

static void
get_parameters_map (Simulator *sim, std::map<std::string, std::string> &parms)
{
	if ( sim == 0 ) return;
	std::list<unisim::kernel::service::VariableBase *> parm_list;
	sim->GetParameters(parm_list);
	for ( std::list<unisim::kernel::service::VariableBase *>::iterator it = parm_list.begin();
			it != parm_list.end();
			it++ )
		parms.insert(make_pair(std::string((*it)->GetName()),
				(std::string)*(*it)));
}

static void
set_parameters_map (Simulator *sim, std::map<std::string, std::string> &parms)
{
	if ( sim == 0 ) return;
	for ( std::map<std::string, std::string>::iterator it = parms.begin();
			it != parms.end();
			it++ )
		sim->SetVariable(it->first.c_str(), it->second.c_str());
}

//static PyObject *
//simulator_set_variable (armemu_SimulatorObject *self, PyObject *args)
//{
//	PyObject *result;
//	std::map<std::string, std::string> parms;
//	const char *var_name, *value;
//	get_parameters_map(self->sim, parms);
//	destroy_simulator(self);
//	self->sim = create_simulator();
//	if ( !PyArg_ParseTuple(args, "ss", &var_name, &value) )
//		return NULL;
//	set_parameters_map(self->sim, parms);
//	self->sim->SetVariable(var_name, value);
//	self->setup = self->sim->Setup();
//	std::string var = (std::string)*self->sim->FindVariable(var_name);
//	result = PyUnicode_FromString(var.c_str());
//	return result;
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
	if ( self->setup != unisim::kernel::service::Simulator::ST_OK_TO_START ||
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

static PyMethodDef simulator_methods[] =
{
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
	//{"set_variable", (PyCFunction)simulator_set_variable, METH_VARARGS,
	//		"Set the value of a simulator variable."},
	{"run", (PyCFunction)simulator_run, METH_VARARGS,
			"Run the simulator."},
	{"is_running", (PyCFunction)simulator_is_running, METH_NOARGS,
			"Check if the simulator is running or not."},
	{"has_started", (PyCFunction)simulator_has_started, METH_NOARGS,
			"Checks if the simulation has been started or not"},
	{"has_finished", (PyCFunction)simulator_has_finished, METH_NOARGS,
			"Checks if the simulation has been finished or not"},
	{NULL}
};

static PyTypeObject armemu_SimulatorType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "armemu.ARMEmu",					/* tp_name */
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
    Py_TPFLAGS_DEFAULT,					/* tp_flags */
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
// PyInit_simulator(void)
PyInit_ARMEMU_DECLARATION
{
    PyObject* m;

    if ( PyType_Ready(&armemu_SimulatorType) < 0 )
    	return NULL;

    m = PyModule_Create(&simulatormodule);
    if (m == NULL)
        return NULL;

    TimeUnitMapInit();
	Py_INCREF(&armemu_SimulatorType);
	PyModule_AddObject(m, "Simulator", (PyObject *)&armemu_SimulatorType);

	if ( import_variable_module() < 0 )
	{
		printf ("ERROR: could not import '"PyVariable_Module_Name"'.\n");
		return NULL;
	}

	if ( import_variable_api() < 0 )
	{
		printf ("ERROR: could not import '"PyVariable_Capsule_Name"'.\n");
		return NULL;
	}

	return m;
}


}
