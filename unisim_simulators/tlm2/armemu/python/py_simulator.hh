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

#ifndef __PYTHON_PY_SIMULATOR_HH__
#define __PYTHON_PY_SIMULATOR_HH__

#include <Python.h>
#include "simulator.hh"
#include "python/python_config.hh"

extern "C" {

/* Variable full capsule name */
#define PySimulator_Module_Name PACKAGE_NAME".simulator"
// "armemu041"
#define PySimulator_Capsule_Name PACKAGE_NAME".simulator._C_API"
// "armemu041._C_API"

/* C API functions */
#define PySimulator_GetSimRef_NUM 0
#define PySimulator_GetSimRef_RETURN Simulator *
#define PySimulator_GetSimRef_PROTO ()

/* Total number of C API pointers */
#define PySimulator_API_pointers 1

#ifdef SIMULATOR_MODULE
/* This section is used when compiling py_variable.cc */

static PySimulator_GetSimRef_RETURN PySimulator_GetSimRef PySimulator_GetSimRef_PROTO;

#else // SIMULATOR_MODULE

/* This section is used in modules that use simulator module's API */
static void **PySimulator_API;

#define PySimulator_GetSimRef \
	(*(PySimulator_GetSimRef_RETURN (*)PySimulator_GetSimRef_PROTO) PySimulator_API[PySimulator_GetSimRef_NUM])

/* Ensures that the initial PySimulator_API is NULL
 */
static void import_simulator_api_init(void)
{
	PySimulator_API = NULL;
}

/* Return -1 on error, 0 on success.
 * PyCapsule_Import will set an exception if there's an error.
 */
static int import_simulator_api(void)
{
	if ( PySimulator_API != NULL ) return 0;
	if ( PyImport_ImportModule(PySimulator_Module_Name) == NULL ) return -1;
	PySimulator_API = (void **)PyCapsule_Import(PySimulator_Capsule_Name, 0);
	return ( PySimulator_API != NULL ) ? 0 : -1;
}

#endif // SIMULATOR_MODULE

}

#endif /* __PYTHON_PY_SIMULATOR_HH__ */
