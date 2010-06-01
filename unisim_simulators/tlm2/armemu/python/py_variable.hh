/*
 * variable.hh
 *
 *  Created on: May 31, 2010
 *      Author: gracia
 */

#ifndef __PYTHON_PY_VARIABLE_HH__
#define __PYTHON_PY_VARIABLE_HH__

#include <Python.h>

extern "C" {

static int PyVariableType_Ready();
static int PyModule_AddVariableObject(PyObject *module);

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    unisim::kernel::service::VariableBase *var;
} variable_VariableObject;

static void variable_dealloc (variable_VariableObject *self);
static PyObject *variable_new (PyTypeObject *type,
		PyObject *args,
		PyObject *kwds);
static PyObject *variable_init (variable_VariableObject *self,
		PyObject *args,
		PyObject *kwds);
static PyObject * variable_get_name (variable_VariableObject *self);
static PyObject *variable_get_value (variable_VariableObject *self);

}

#endif /* __PYTHON_VARIABLE_HH__ */
