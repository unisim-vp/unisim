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
#include "simulator.hh"
#include "config.hh"

extern "C" {
typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Simulator *sim;
    bool setup;
} armemu_SimulatorObject;

static Simulator *
create_simulator()
{
	Simulator *sim;
	char *argv[3] =
	{
			(char *)"/Users/gracia/Developer/unisim/armemu/build/armemu-0.2.0",
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
	self->sim = create_simulator();
	self->setup = self->sim->Setup();
	return (PyObject *)self;
}

static int
simulator_init (armemu_SimulatorObject *self, PyObject *args, PyObject *kwds)
{
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

static PyObject *
simulator_get_variable (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result;
	const char *var_name;

	if ( !PyArg_ParseTuple(args, "s", &var_name) )
		return NULL;
	std::string var = (std::string)*self->sim->FindVariable(var_name);
	result = PyUnicode_FromString(var.c_str());
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

static PyObject *
simulator_get_parameters (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result;
	std::map<std::string, std::string> parms;

	if ( self->sim == 0 ) return NULL;
	result = PyDict_New();
	if ( result == NULL ) return NULL;
	get_parameters_map(self->sim, parms);
	for ( std::map<std::string, std::string>::iterator it = parms.begin();
			it != parms.end();
			it++ )
	{
		PyObject *name;
		PyObject *value;
		name = PyUnicode_FromString(it->first.c_str());
		value = PyUnicode_FromString(it->second.c_str());
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
simulator_set_variable (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result;
	std::map<std::string, std::string> parms;
	const char *var_name, *value;
	get_parameters_map(self->sim, parms);
	destroy_simulator(self);
	self->sim = create_simulator();
	if ( !PyArg_ParseTuple(args, "ss", &var_name, &value) )
		return NULL;
	set_parameters_map(self->sim, parms);
	self->sim->SetVariable(var_name, value);
	self->setup = self->sim->Setup();
	std::string var = (std::string)*self->sim->FindVariable(var_name);
	result = PyUnicode_FromString(var.c_str());
	return result;
}

static PyObject *
simulator_run (armemu_SimulatorObject *self)
{
	PyObject *result;

	if ( self->setup )
	{
		self->sim->Run();
		result = PyUnicode_FromString("Simulation finished.");
	}
	else
		result = PyUnicode_FromString("Simulation setup failed.");
	return result;
}

static PyMethodDef simulator_methods[] =
{
	{"version", (PyCFunction)simulator_version, METH_NOARGS,
			"Return the simulator version."},
	{"get_parameters", (PyCFunction)simulator_get_parameters, METH_NOARGS,
			"Return a dictionary with the complete list of the simulator parameters."},
	{"get_variable", (PyCFunction)simulator_get_variable, METH_VARARGS,
			"Get the value of a simulator variable."},
	{"set_variable", (PyCFunction)simulator_set_variable, METH_VARARGS,
			"Set the value of a simulator variable."},
	{"run", (PyCFunction)simulator_run, METH_NOARGS,
			"Run the simulator."},
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

static PyModuleDef simulatormodule = {
    PyModuleDef_HEAD_INIT,
	SIM_PYTHON_MODULE_NAME,
    "UNISIM ARMEmu python extension.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_armemu020(void)
{
    PyObject* m;

    if (PyType_Ready(&armemu_SimulatorType) < 0)
        return NULL;

    m = PyModule_Create(&simulatormodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&armemu_SimulatorType);
    PyModule_AddObject(m, SIM_PYTHON_MODULE_NAME, (PyObject *)&armemu_SimulatorType);
    return m;
}

}