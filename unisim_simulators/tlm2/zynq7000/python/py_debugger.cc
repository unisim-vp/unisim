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
// #include "simulator.hh"
#include "unisim/uapi/uapi.h"
#include <string>
#define DEBUGGER_MODULE
#include "python/py_debugger.hh"
#include "python/py_simulator.hh"

extern "C" {

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
	UnisimDebugAPI debugger;
//    std::string *name;
//	Simulator **simulator;
//	unisim::api::debug::DebugAPI *debugger;
	PyObject *debug_context;
    PyObject *debug_breakpoint_handler;
    PyObject *debug_watchpoint_handler;
} debugger_DebuggerObject;

void debugger_breakpoint_handler(void *_self, uint64_t addr)
{
	PyObject *result;
	debugger_DebuggerObject *self = (debugger_DebuggerObject *)_self;
	if ( self->debug_breakpoint_handler == 0 )
	{
		return;
	}
	PyObject *arglist;
	arglist = Py_BuildValue("(OK)",
			(self->debug_context != 0 ?
				self->debug_context : Py_None),
			addr);
	result = PyObject_CallObject(self->debug_breakpoint_handler, arglist);
	Py_DECREF(arglist);
	if ( result != NULL ) Py_DECREF(result);
}

void debugger_watchpoint_handler(void *_self, uint64_t addr, bool read)
{
	PyObject *result;
	debugger_DebuggerObject *self = (debugger_DebuggerObject *)_self;
	if ( self->debug_watchpoint_handler == 0 )
	{
		return;
	}
	PyObject *arglist;
	arglist = Py_BuildValue("(OKO)",
			(self->debug_context != 0 ?
					self->debug_context : Py_None),
			addr,
			(read ? Py_True : Py_False));
	result = PyObject_CallObject(self->debug_watchpoint_handler, arglist);
	Py_DECREF(arglist);
	if ( result != NULL ) Py_DECREF(result);
}

static void
debugger_dealloc (debugger_DebuggerObject *self)
{
	// delete self->name;
	// self->name = 0;
	// self->simulator = 0;
	usDestroyDebugAPI(self->debugger);
	self->debugger = 0;
	Py_DECREF(self->debug_context);
	Py_DECREF(self->debug_breakpoint_handler);
	Py_DECREF(self->debug_watchpoint_handler);
	self->debug_context = 0;
	self->debug_breakpoint_handler = 0;
	self->debug_watchpoint_handler = 0;

	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
debugger_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	// a debugger should not be created alone, it should be obtained
	//  from a simulator
	return NULL;
}

static PyObject *
debugger_init (debugger_DebuggerObject *self, PyObject *args, PyObject *kwds)
{
	// self->name = 0;
	// self->simulator = 0;
	self->debugger = 0;
	self->debug_context = 0;
	self->debug_breakpoint_handler = 0;
	self->debug_watchpoint_handler = 0;
	return (PyObject *)self;
}

static PyObject *
debugger_get_name (debugger_DebuggerObject *self)
{
	PyObject *result = NULL;
	result = PyUnicode_FromString(usExtendedAPIGetName((UnisimExtendedAPI)self->debugger));
	return result;
}

static PyObject *
debugger_set_context (debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *context = NULL;

	if ( !PyArg_ParseTuple(args, "O", &context) )
	{
		PyErr_SetString(PyExc_TypeError, "A context must be given");
		return NULL;
	}

	if ( self->debug_context )
		Py_DECREF(self->debug_context);
	self->debug_context = context;
	Py_INCREF(self->debug_context);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
debugger_set_breakpoint_callback(debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *handler = NULL;

	if ( !PyArg_ParseTuple(args, "O", &handler) )
	{
		PyErr_SetString(PyExc_TypeError, "A callback method must be given");
		return NULL;
	}

	if ( !PyCallable_Check(handler) )
	{
		PyErr_SetString(PyExc_TypeError, "The parameter must be callable");
		return NULL;
	}

	if ( self->debug_breakpoint_handler )
		Py_DECREF(self->debug_breakpoint_handler);
	self->debug_breakpoint_handler = handler;
	Py_INCREF(self->debug_breakpoint_handler);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
debugger_set_watchpoint_callback(debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *handler = NULL;

	if ( !PyArg_ParseTuple(args, "O", &handler) )
	{
		PyErr_SetString(PyExc_TypeError, "A callback method must be given");
		return NULL;
	}

	if ( !PyCallable_Check(handler) )
	{
		PyErr_SetString(PyExc_TypeError, "The parameter must be callable");
		return NULL;
	}

	if ( self->debug_watchpoint_handler )
		Py_DECREF(self->debug_watchpoint_handler);
	self->debug_watchpoint_handler = handler;
	Py_INCREF(self->debug_watchpoint_handler);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
debugger_set_step_mode (debugger_DebuggerObject *self)
{
	PyObject *result = NULL;
	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( usDebugAPISetStepMode(self->debugger) )
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_is_mode_step (debugger_DebuggerObject *self)
{
	PyObject *result = NULL;
	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( usDebugAPIIsModeStep(self->debugger) )
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_set_continue_mode (debugger_DebuggerObject *self)
{
	PyObject *result = NULL;
	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( usDebugAPISetContinueMode(self->debugger) )
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_is_mode_continue (debugger_DebuggerObject *self)
{
	PyObject *result = NULL;
	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( usDebugAPIIsModeContinue(self->debugger) )
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_has_breakpoint (debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *result = NULL;
	PyObject *parm = NULL;
	unsigned long long int addr = 0;
	PyObject *str_bytes = NULL;
	char *c_str = 0;
	bool parm_ok = false;

	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger no longer available");
		return result;
	}
	if ( PyArg_ParseTuple(args, "O", &parm) )
	{
		if ( PyLong_Check(parm) )
		{
			// the parameter is a number
			addr = PyLong_AsUnsignedLongLong(parm);
			parm_ok = true;
		}
		else if ( PyUnicode_Check(parm) )
		{
			// the parameter is a string
			if ( PyUnicode_FSConverter( parm, &str_bytes) )
			{
				c_str = PyBytes_AsString(str_bytes);
				parm_ok = true;
			}
			Py_DECREF(str_bytes);
		}
	}
	if ( !parm_ok )
	{
		PyErr_SetString(PyExc_TypeError,
				"An address/symbol/filename:lien is expected");
		return NULL;
	}

	if ( c_str )
	{
		if ( usDebugAPIHasBreakpointSymbol(self->debugger, c_str) )
			Py_RETURN_TRUE;
	}
	else
		if ( usDebugAPIHasBreakpoint(self->debugger, addr) )
			Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_set_breakpoint (debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *result = NULL;
	PyObject *parm = NULL;
	unsigned long long int addr = 0;
	PyObject *str_bytes = NULL;
	char *c_str = 0;
	bool parm_ok = false;

	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( PyArg_ParseTuple(args, "O", &parm) )
	{
		if ( PyLong_Check(parm) )
		{
			// the parameter is a number
			addr = PyLong_AsUnsignedLongLong(parm);
			parm_ok = true;
		}
		else if ( PyUnicode_Check(parm) )
		{
			// the parameter is a string
			if ( PyUnicode_FSConverter( parm, &str_bytes) )
			{
				c_str = PyBytes_AsString(str_bytes);
				parm_ok = true;
			}
			Py_DECREF(str_bytes);
		}
	}
	if ( !parm_ok )
	{
		PyErr_SetString(PyExc_TypeError,
				"An address/symbol/filename:line is expected");
		return NULL;
	}

	if ( c_str )
	{
		if ( usDebugAPISetBreakpointSymbol(self->debugger, c_str) )
			Py_RETURN_TRUE;
	}
	else
		if ( usDebugAPISetBreakpoint(self->debugger, addr) )
			Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_delete_breakpoint (debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *result = NULL;
	PyObject *parm = NULL;
	unsigned long long int addr = 0;
	PyObject *str_bytes = NULL;
	char *c_str = 0;
	bool parm_ok = false;

	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( PyArg_ParseTuple(args, "O", &parm) )
	{
		if ( PyLong_Check(parm) )
		{
			// the parameter is a number
			addr = PyLong_AsUnsignedLongLong(parm);
			parm_ok = true;
		}
		else if ( PyUnicode_Check(parm) )
		{
			// the parameter is a string
			if ( PyUnicode_FSConverter( parm, &str_bytes) )
			{
				c_str = PyBytes_AsString(str_bytes);
				parm_ok = true;
			}
			Py_DECREF(str_bytes);
		}
	}
	if ( !parm_ok )
	{
		PyErr_SetString(PyExc_TypeError,
				"An address/symbol/filename:lien is expected");
		return NULL;
	}

	if ( c_str )
	{
		if ( usDebugAPIDeleteBreakpointSymbol(self->debugger, c_str) )
			Py_RETURN_TRUE;

	}
	else
	{
		if ( usDebugAPIDeleteBreakpoint(self->debugger, addr) )
			Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

typedef enum WatchpointType_t { READ, WRITE, RW} WatchpointType_t;

static PyObject *
debugger_generic_set_wathcpoint ( debugger_DebuggerObject *self, PyObject *args,
		WatchpointType_t type)
{
	PyObject *result = NULL;
	PyObject *parm = NULL;
	unsigned long long int addr = 0;
	PyObject *str_bytes = NULL;
	char *c_str = 0;
	unsigned int size = 0;
	bool parm_ok = false;

	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( PyArg_ParseTuple(args, "OI", &parm, &size) )
	{
		if ( PyLong_Check(parm) )
		{
			// the parameter is a number
			addr = PyLong_AsUnsignedLongLong(parm);
			parm_ok = true;
		}
		else if ( PyUnicode_Check(parm) )
		{
			// the parameter is a string
			if ( PyUnicode_FSConverter( parm, &str_bytes) )
			{
				c_str = PyBytes_AsString(str_bytes);
				parm_ok = true;
			}
			Py_DECREF(str_bytes);
		}
	}
	if ( !parm_ok )
	{
		PyErr_SetString(PyExc_TypeError,
				"An address/symbol/filename:line and a size are expected");
		return NULL;
	}

	bool watchpoint_result = false;
	switch ( type )
	{
	case READ: // read watchpoint
		watchpoint_result = ( c_str ?
				usDebugAPISetReadWatchpointSymbol(self->debugger, c_str, size):
				usDebugAPISetReadWatchpoint(self->debugger, addr, size));
		break;
	case WRITE: // write watchpoint
		watchpoint_result = ( c_str ?
				usDebugAPISetWriteWatchpointSymbol(self->debugger, c_str, size):
				usDebugAPISetWriteWatchpoint(self->debugger, addr, size));
		break;
	default: // read/write watchpoint
		watchpoint_result = ( c_str ?
				usDebugAPISetWatchpointSymbol(self->debugger, c_str, size):
				usDebugAPISetWatchpoint(self->debugger, addr, size));
		break;
	}

	if ( watchpoint_result )
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_set_watchpoint (debugger_DebuggerObject *self, PyObject *args)
{
	return debugger_generic_set_wathcpoint(self, args, RW);
}

static PyObject *
debugger_set_read_watchpoint (debugger_DebuggerObject *self, PyObject *args)
{
	return debugger_generic_set_wathcpoint(self, args, READ);
}

static PyObject *
debugger_set_write_watchpoint (debugger_DebuggerObject *self, PyObject *args)
{
	return debugger_generic_set_wathcpoint(self, args, (WatchpointType_t)WRITE);
}

static PyObject *
debugger_generic_delete_wathcpoint ( debugger_DebuggerObject *self, PyObject *args,
		WatchpointType_t type)
{
	PyObject *result = NULL;
	PyObject *parm = NULL;
	unsigned long long int addr = 0;
	PyObject *str_bytes = NULL;
	char *c_str = 0;
	unsigned int size = 0;
	bool parm_ok = false;

	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( PyArg_ParseTuple(args, "OI", &parm, &size) )
	{
		if ( PyLong_Check(parm) )
		{
			// the parameter is a number
			addr = PyLong_AsUnsignedLongLong(parm);
			parm_ok = true;
		}
		else if ( PyUnicode_Check(parm) )
		{
			// the parameter is a string
			if ( PyUnicode_FSConverter( parm, &str_bytes) )
			{
				c_str = PyBytes_AsString(str_bytes);
				parm_ok = true;
			}
			Py_DECREF(str_bytes);
		}
	}
	if ( !parm_ok )
	{
		PyErr_SetString(PyExc_TypeError,
				"An address/symbol/filename:line and a size are expected");
		return NULL;
	}

	bool watchpoint_result = false;
	switch ( type )
	{
	case READ: // read watchpoint
		watchpoint_result = ( c_str ?
				usDebugAPIDeleteReadWatchpointSymbol(self->debugger, c_str, size) :
				usDebugAPIDeleteReadWatchpoint(self->debugger, addr, size) );
		break;
	case WRITE: // write watchpoint
		watchpoint_result = ( c_str ?
				usDebugAPIDeleteWriteWatchpointSymbol(self->debugger, c_str, size) :
				usDebugAPIDeleteWriteWatchpoint(self->debugger, addr, size) );
		break;
	default: // read/write watchpoint
		watchpoint_result = ( c_str ?
				usDebugAPIDeleteWatchpointSymbol(self->debugger, c_str, size) :
				usDebugAPIDeleteWatchpoint(self->debugger, addr, size) );
		break;
	}

	if ( watchpoint_result )
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *
debugger_delete_watchpoint (debugger_DebuggerObject *self, PyObject *args)
{
	return debugger_generic_delete_wathcpoint(self, args, RW);
}

static PyObject *
debugger_delete_read_watchpoint (debugger_DebuggerObject *self, PyObject *args)
{
	return debugger_generic_delete_wathcpoint(self, args, READ);
}

static PyObject *
debugger_delete_write_watchpoint (debugger_DebuggerObject *self, PyObject *args)
{
	return debugger_generic_delete_wathcpoint(self, args, WRITE);
}

static PyObject *
debugger_get_symbol_address (debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *result = NULL;
	PyObject *parm = NULL;
  uint64_t addr = 0;
	// unsigned long long int addr = 0;
	PyObject *str_bytes = NULL;
	char *c_str = 0;
	bool parm_ok = false;

	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( PyArg_ParseTuple(args, "O", &parm) )
	{
		if ( PyUnicode_Check(parm) )
		{
			// the parameter is a string
			if ( PyUnicode_FSConverter( parm, &str_bytes) )
			{
				c_str = PyBytes_AsString(str_bytes);
				parm_ok = true;
			}
			Py_DECREF(str_bytes);
		}
	}
	if ( !parm_ok )
	{
		PyErr_SetString(PyExc_TypeError,
				"An symbol/filename:line is expected");
		Py_RETURN_NONE;
	}

	if ( usDebugAPIGetSymbolAddress(self->debugger, c_str, &addr) )
    return PyLong_FromUnsignedLongLong(addr);
	Py_RETURN_NONE;
}

static PyObject *
debugger_get_fs_address (debugger_DebuggerObject *self, PyObject *args)
{
	PyObject *result = NULL;
	PyObject *parm = NULL;
  uint64_t addr = 0;
	// unsigned long long int addr = 0;
	PyObject *str_bytes = NULL;
	char *c_str = 0;
	bool parm_ok = false;

	if ( self->debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"Unisim debugger is no longer available");
		return result;
	}
	if ( PyArg_ParseTuple(args, "O", &parm) )
	{
		if ( PyUnicode_Check(parm) )
		{
			// the parameter is a string
			if ( PyUnicode_FSConverter( parm, &str_bytes) )
			{
				c_str = PyBytes_AsString(str_bytes);
				parm_ok = true;
			}
			Py_DECREF(str_bytes);
		}
	}
	if ( !parm_ok )
	{
		PyErr_SetString(PyExc_TypeError,
				"An symbol/filename:line is expected");
		Py_RETURN_NONE;
	}

	if ( usDebugAPIGetFileSystemAddress(self->debugger, c_str, &addr) )
    return PyLong_FromUnsignedLongLong(addr);
	Py_RETURN_NONE;
}

static PyObject *
debugger_read_memory (debugger_DebuggerObject *self, PyObject *args)
{
  PyObject *result = NULL;
  unsigned long long int addr;
  uint8_t value;

  if ( self->debugger == 0 )
  {
    PyErr_Format(PyExc_RuntimeError,
                 "Unisim debugger is no longer available");
    return result;
  }
  if ( PyArg_ParseTuple(args, "K", &addr) )
  {
    if ( usDebugAPIReadMemory(self->debugger, addr, &value) )
      return PyLong_FromUnsignedLong(value);
    Py_RETURN_NONE;
  }
  Py_RETURN_NONE;
}

static PyMethodDef debugger_methods[] =
{
		{"get_name", (PyCFunction)debugger_get_name, METH_NOARGS,
				"Return the debugger name."},
		{"set_context", (PyCFunction)debugger_set_context, METH_VARARGS,
				"Set a context that will be given to the breakpoint and"
				" watchpoint callbacks."},
		{"set_breakpoint_callback",
				(PyCFunction)debugger_set_breakpoint_callback,
				METH_VARARGS,
				"Set a callback for breakpoint events."},
		{"set_wathpoint_callback",
				(PyCFunction)debugger_set_watchpoint_callback,
				METH_VARARGS,
				"Set a callback for watchpoint events."},
		{"set_step_mode",
				(PyCFunction)debugger_set_step_mode,
				METH_NOARGS,
				"Set the debugger in step mode."},
		{"is_mode_step",
				(PyCFunction)debugger_is_mode_step,
				METH_NOARGS,
				"True if the debugger is in step mode."},
		{"set_continue_mode",
				(PyCFunction)debugger_set_continue_mode,
				METH_NOARGS,
				"Set the debugger in continue mode."},
		{"is_mode_continue",
				(PyCFunction)debugger_is_mode_continue,
				METH_NOARGS,
				"True if the debugger is in continue mode."},
		{"has_breakpoint",
				(PyCFunction)debugger_has_breakpoint,
				METH_VARARGS,
				"True if a breakpoint to the given address/symbol/file exists."},
		{"set_breakpoint",
				(PyCFunction)debugger_set_breakpoint,
				METH_VARARGS,
				"Set a breakpoint."},
		{"delete_breakpoint",
				(PyCFunction)debugger_delete_breakpoint,
				METH_VARARGS,
				"Remove a breakpoint."},
		{"set_watchpoint",
				(PyCFunction)debugger_set_watchpoint,
				METH_VARARGS,
				"Set a watchpoint."},
		{"set_read_watchpoint",
				(PyCFunction)debugger_set_read_watchpoint,
				METH_VARARGS,
				"Set a read watchpoint."},
		{"set_write_watchpoint",
				(PyCFunction)debugger_set_write_watchpoint,
				METH_VARARGS,
				"Set a write watchpoint."},
		{"delete_watchpoint",
				(PyCFunction)debugger_delete_watchpoint,
				METH_VARARGS,
				"Remove a watchpoint."},
		{"delete_read_watchpoint",
				(PyCFunction)debugger_delete_read_watchpoint,
				METH_VARARGS,
				"Remove a read watchpoint."},
		{"delete_write_watchpoint",
				(PyCFunction)debugger_delete_write_watchpoint,
				METH_VARARGS,
				"Remove a write watchpoint."},
    {"get_symbol_address",
      (PyCFunction)debugger_get_symbol_address,
      METH_VARARGS,
      "Get the address of a given symbol."},
    {"get_fs_address",
      (PyCFunction)debugger_get_fs_address,
      METH_VARARGS,
      "Get the address of a source file in the binary. Format is <source_file_name>#<line>."},
    {"read_memory",
      (PyCFunction)debugger_read_memory,
      METH_VARARGS,
      "Get the data stored in the given memory location (from the cpu view)."},
		{NULL}
};

static PyTypeObject debugger_DebuggerType = {
	    PyVarObject_HEAD_INIT(NULL, 0)
	    PACKAGE_NAME".debugger.Debugger",					/* tp_name */
	    sizeof(debugger_DebuggerObject),		/* tp_basicsize */
	    0,									/* tp_itemsize */
	    (destructor)debugger_dealloc,		/* tp_dealloc */
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
	    "UNISIM debugger objects",	/* tp_doc */
	    0,									/* tp_traverse */
	    0,									/* tp_clear */
	    0,									/* tp_richcompare */
	    0,									/* tp_weaklistoffset */
	    0,									/* tp_iter */
	    0,									/* tp_iternext */
	    debugger_methods,									/* tp_methods */
	    0,									/* tp_members */
	    0,									/* tp_getset */
	    0,									/* tp_base */
	    0,									/* tp_dict */
	    0,									/* tp_descr_get */
	    0,									/* tp_descr_set */
	    0,									/* tp_dictoffset */
	    (initproc)debugger_init,			/* tp_init */
	    0,									/* tp_alloc */
	    debugger_new,						/* tp_new */
};

static PyModuleDef debuggermodule = {
    PyModuleDef_HEAD_INIT,
    "debugger",
	//SIM_PYTHON_MODULE_NAME,
    "UNISIM python debugger module.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_debugger(void)
{
    PyObject* m;
    static void *PyDebugger_API[PyDebugger_API_pointers];
    PyObject *c_api_object;

    if ( PyType_Ready(&debugger_DebuggerType) < 0 )
    	return NULL;

    m = PyModule_Create(&debuggermodule);
    if (m == NULL)
        return NULL;

	Py_INCREF(&debugger_DebuggerType);
	PyModule_AddObject(m, "Debugger", (PyObject *)&debugger_DebuggerType);

	/* Initialize the C API pointer array */
	PyDebugger_API[PyDebugger_NewDebugger_NUM] = (void *)PyDebugger_NewDebugger;

	/* Create a Capsule containing the API pointer array's address */
	c_api_object = PyCapsule_New((void *)PyDebugger_API, PyDebugger_Capsule_Name, NULL);

	if ( c_api_object != NULL )
		PyModule_AddObject(m, "_C_API", c_api_object);
	else
		return NULL;

	import_simulator_api_init();

	return m;
}

static PyObject *
PyDebugger_NewDebugger (UnisimDebugAPI debugger)
{
	if ( import_simulator_api() < 0 )
		return NULL;

//	Simulator **sim = PySimulator_GetSimulatorRef(sim_obj);
//	if ( *sim == 0 )
//	{
//		PyErr_Format(PyExc_RuntimeError,
//				"No reference found to the simulator when creating debugger.");
//		return NULL;
//	}

	if ( debugger == 0 )
	{
		PyErr_Format(PyExc_RuntimeError,
				"No debugger available when creating PyDebugger.");
		return NULL;
	}

	debugger_DebuggerObject *self;
	self = (debugger_DebuggerObject *)debugger_DebuggerType.tp_alloc(
			&debugger_DebuggerType, 0);
	// unisim::kernel::service::Object *obj = debugger->GetParentObject();
	// self->name = new std::string(debugger->GetName());
	// self->simulator = sim;
	self->debugger = debugger;
	self->debug_context = 0;
	self->debug_breakpoint_handler = 0;
	self->debug_watchpoint_handler = 0;

	usDebugAPISetContext(self->debugger, (void *)self);
	usDebugAPISetBreakpointHandler(self->debugger, debugger_breakpoint_handler);
	usDebugAPISetWatchpointHandler(self->debugger, debugger_watchpoint_handler);
	return (PyObject *)self;
}

}
