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
#define VARIABLE_MODULE
#include "python/py_variable.hh"

extern "C" {

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    unisim::kernel::service::VariableBase *var;
} variable_VariableObject;

static void
variable_dealloc (variable_VariableObject *self)
{
	self->var = 0;

	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
variable_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	return NULL;
	//return PyVariable_NewVariable();
	variable_VariableObject *self;
	self = (variable_VariableObject *)type->tp_alloc(type, 0);
	self->var = 0;
	return (PyObject *)self;
}

static PyObject *
variable_init (variable_VariableObject *self, PyObject *args, PyObject *kwds)
{
	self->var = 0;
	return (PyObject *)self;
}

static PyObject *
variable_get_name (variable_VariableObject *self)
{
	PyObject *result;
	const char *name = self->var->GetName();
	result = PyUnicode_FromString(name);
	return result;
}

static PyObject *
variable_getname (variable_VariableObject *self, void *closure)
{
	PyObject *result;
	const char *name = self->var->GetName();
	result = PyUnicode_FromString(name);
	return result;
}

static PyObject *
variable_getvalue (variable_VariableObject *self, void *closure)
{
	PyObject *result;
	std::string value = (std::string)*self->var;
	result = PyUnicode_FromString(value.c_str());
	return result;
}

static int
variable_setvalue (variable_VariableObject *self, PyObject *value, void *closure)
{
	if ( value == NULL )
	{
		PyErr_SetString(PyExc_TypeError, "Cannot set value to NULL");
		return -1;
	}
	if ( !PyUnicode_Check(value) )
	{
		PyErr_SetString(PyExc_TypeError, "The value must be a string");
		return -1;
	}
	// PyObject *ascii = PyUnicode_EncodeASCII(PyUnicode_AS_UNICODE(value), PyUnicode_GET_SIZE(value), NULL);
	// printf ("%s\n", PyByteArray_AsString(ascii));
	PyObject *utf8 = PyUnicode_AsEncodedString(value, NULL, NULL);
	if ( !PyBytes_Check(utf8) )
	{
		PyErr_SetString(PyExc_TypeError, "not a python bytes");
		return -1;
	}
	*self->var = PyBytes_AsString(utf8);
	return 0;
}
static PyObject *
variable_get_value ( variable_VariableObject *self )
{
	PyObject *result;
	std::string value = (std::string)*self->var;
	result = PyUnicode_FromString(value.c_str());
	return result;
}

static PyObject *
variable_set_value ( variable_VariableObject *self, PyObject *args)
{
	const char *value;

	if ( !PyArg_ParseTuple(args, "s", &value) )
		return NULL;
	*self->var = value;
	return (PyObject *)self;
}

static PyMethodDef variable_methods[] =
{
		{"get_name", (PyCFunction)variable_get_name, METH_NOARGS,
				"Return the variable name."},
		{"get_value", (PyCFunction)variable_get_value, METH_NOARGS,
				"Return the variable value as a string."},
		{"set_value", (PyCFunction)variable_set_value, METH_VARARGS,
				"Set the variable value."},
		{NULL}
};

static PyGetSetDef variable_getseters[] =
{
		{"name",
				(getter)variable_getname, NULL,
				"variable name",
				NULL,
		},
		{"value",
				(getter)variable_getvalue, (setter)variable_setvalue,
				"variable value",
				NULL,
		},
		{NULL},
};

static PyTypeObject variable_VariableType = {
	    PyVarObject_HEAD_INIT(NULL, 0)
	    "variable.Variable",					/* tp_name */
	    sizeof(variable_VariableObject),		/* tp_basicsize */
	    0,									/* tp_itemsize */
	    (destructor)variable_dealloc,		/* tp_dealloc */
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
	    "UNISIM variable objects",	/* tp_doc */
	    0,									/* tp_traverse */
	    0,									/* tp_clear */
	    0,									/* tp_richcompare */
	    0,									/* tp_weaklistoffset */
	    0,									/* tp_iter */
	    0,									/* tp_iternext */
	    variable_methods,									/* tp_methods */
	    0,									/* tp_members */
	    variable_getseters,									/* tp_getset */
	    0,									/* tp_base */
	    0,									/* tp_dict */
	    0,									/* tp_descr_get */
	    0,									/* tp_descr_set */
	    0,									/* tp_dictoffset */
	    (initproc)variable_init,			/* tp_init */
	    0,									/* tp_alloc */
	    0, //variable_new,						/* tp_new */
};

static PyModuleDef variablemodule = {
    PyModuleDef_HEAD_INIT,
    "variable",
	//SIM_PYTHON_MODULE_NAME,
    "UNISIM python variable module.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_variable(void)
{
    PyObject* m;
    static void *PyVariable_API[PyVariable_API_pointers];
    PyObject *c_api_object;

    if ( PyType_Ready(&variable_VariableType) < 0 )
    	return NULL;

    m = PyModule_Create(&variablemodule);
    if (m == NULL)
        return NULL;

	Py_INCREF(&variable_VariableType);
	PyModule_AddObject(m, "Variable", (PyObject *)&variable_VariableType);

	/* Initialize the C API pointer array */
	PyVariable_API[PyVariable_NewVariable_NUM] = (void *)PyVariable_NewVariable;

	/* Create a Capsule containing the API pointer array's address */
	c_api_object = PyCapsule_New((void *)PyVariable_API, PyVariable_Capsule_Name, NULL);

	if ( c_api_object != NULL )
		PyModule_AddObject(m, "_C_API", c_api_object);
	else
		return NULL;
	return m;
}

static PyObject *
PyVariable_NewVariable (unisim::kernel::service::VariableBase *var)
{
	variable_VariableObject *self;
	self = (variable_VariableObject *)variable_VariableType.tp_alloc(
			&variable_VariableType, 0);
	self->var = var;
	return (PyObject *)self;
}

}
