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

}

#endif /* __PYTHON_VARIABLE_HH__ */
