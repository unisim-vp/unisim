/*
 * py_simulator.hh
 *
 *  Created on: May 31, 2010
 *      Author: gracia
 */

#ifndef __PYTHON_PY_SIMULATOR_HH__
#define __PYTHON_PY_SIMULATOR_HH__

#include <Python.h>

extern "C" {

int PySimulatorType_Ready();
int PyModule_AddSimulatorObject(PyObject *module);

}

#endif /* __PYTHON_PY_SIMULATOR_HH__ */
