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
} armemu_SimulatorObject;

static void
simulator_dealloc (armemu_SimulatorObject *self)
{
	if ( self->sim != 0 )
		delete self->sim;
	self->sim = 0;
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
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
simulator_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	armemu_SimulatorObject *self;
	char *argv[2] =
	{
			(char *)"/Users/gracia/Developer/unisim/armemu/build/armemu-0.2.0",
			(char *)0
	};

	self = (armemu_SimulatorObject *)type->tp_alloc(type, 0);
	self->sim = new Simulator(1, argv);
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
	std::string version = (std::string)*self->sim->GetVariable("version");
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
	std::string var = (std::string)*self->sim->GetVariable(var_name);
	result = PyUnicode_FromString(var.c_str());
	return result;
}

static PyObject *
simulator_set_variable (armemu_SimulatorObject *self, PyObject *args)
{
	PyObject *result;
	const char *var_name, *value;
	if ( !PyArg_ParseTuple(args, "ss", &var_name, &value) )
		return NULL;
	std::cerr << "var_name = " << var_name << "; value = " << value << std::endl;
	self->sim->SetVariable(var_name, value);
	std::string var = (std::string)*self->sim->GetVariable(var_name);
	result = PyUnicode_FromString(var.c_str());
	return result;
}

static PyObject *
simulator_run (armemu_SimulatorObject *self)
{
	PyObject *result;
	bool setup;

	setup = self->sim->Setup();
	if ( setup )
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
