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

static int PySimulatorType_Ready();
static int PyModule_AddSimulatorObject(PyObject *module);

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Simulator *sim;
    unisim::kernel::service::Simulator::SetupStatus setup;
} armemu_SimulatorObject;

static void simulator_dealloc (armemu_SimulatorObject *self);
static PyObject *simulator_new (PyTypeObject *type,
		PyObject *args,
		PyObject *kwds);
static int simulator_init (armemu_SimulatorObject *self,
		PyObject *args,
		PyObject *kwds);
static PyObject *simulator_version (armemu_SimulatorObject *self);
static PyObject *simulator_get_variable (armemu_SimulatorObject *self,
		PyObject *args);
static PyObject *simulator_get_variable_object (armemu_SimulatorObject *self,
		PyObject *args);
static PyObject *simulator_get_variables (armemu_SimulatorObject *self);
static PyObject *simulator_get_parameters (armemu_SimulatorObject *self);
static PyObject *simulator_get_statistics (armemu_SimulatorObject *self);
static PyObject *simulator_get_registers (armemu_SimulatorObject *self);
static PyObject *simulator_get_formulas (armemu_SimulatorObject *self);
static PyObject *simulator_set_variable (armemu_SimulatorObject *self,
		PyObject *args);
static PyObject *simulator_is_running (armemu_SimulatorObject *self);
static PyObject *simulator_has_started (armemu_SimulatorObject *self);
static PyObject *simulator_has_finished(armemu_SimulatorObject *self);
static PyObject *simulator_run (armemu_SimulatorObject *self,
		PyObject *args);

}

#endif /* __PYTHON_PY_SIMULATOR_HH__ */
