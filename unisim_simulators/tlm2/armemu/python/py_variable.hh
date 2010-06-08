/*
 * variable.hh
 *
 *  Created on: May 31, 2010
 *      Author: gracia
 */

#ifndef __PYTHON_PY_VARIABLE_HH__
#define __PYTHON_PY_VARIABLE_HH__

#include <Python.h>
#include "simulator.hh"
#include "python/python_config.hh"
extern "C" {

/* Variable full capsule name */
#define PyVariable_Module_Name PACKAGE_NAME".variable"
#define PyVariable_Capsule_Name PACKAGE_NAME".variable._C_API"

/* C API functions */
#define PyVariable_NewVariable_NUM 0
#define PyVariable_NewVariable_RETURN PyObject *
#define PyVariable_NewVariable_PROTO (unisim::kernel::service::VariableBase *)

/* Total number of C API pointers */
#define PyVariable_API_pointers 1

#ifdef VARIABLE_MODULE
/* This section is used when compiling py_variable.cc */

static PyVariable_NewVariable_RETURN PyVariable_NewVariable PyVariable_NewVariable_PROTO;

#else // VARIABLE_MODULE
/* This section is used in modules that use variablemodule's API */
static void **PyVariable_API;

#define PyVariable_NewVariable \
	(*(PyVariable_NewVariable_RETURN (*)PyVariable_NewVariable_PROTO) PyVariable_API[PyVariable_NewVariable_NUM])

/* Ensures that the initial PyVariable_API is NULL
 */
static void import_variable_init(void)
{
	PyVariable_API = NULL;
}

/* Return -1 on error, 0 on success.
 */
static int import_variable_module(void)
{
	if ( PyImport_ImportModule(PyVariable_Module_Name) != NULL )
	{
		return 0;
	}
	return -1;
}

/* Return -1 on error, 0 on success.
 * PyCapsule_Import will set an exception if there's an error.
 */
static int import_variable_api(void)
{
	if ( PyVariable_API != NULL ) return 0;
	PyVariable_API = (void **)PyCapsule_Import(PyVariable_Capsule_Name, 0);
	return ( PyVariable_API != NULL ) ? 0 : -1;
}

#endif // VARIABLE_MODULE

}

#endif /* __PYTHON_VARIABLE_HH__ */
