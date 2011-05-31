# First some verification

set ( BUILD_PYTHON_MODULE__ERROR FALSE )
if ( NOT PYTHON__SIM__IMPORT_NAME )
	set ( BUILD_PYTHON_MODULE__ERROR TRUE )
	message ( SEND_ERROR "PYTHON__SIM__IMPORT_NAME not defined." )
endif ( NOT PYTHON__SIM__IMPORT_NAME )
if ( NOT PYTHON__SIM__MODULE_NAME )
	set ( BUILD_PYTHON_MODULE__ERROR TRUE )
	message ( SEND_ERROR "PYTHON__SIM__MODULE_NAME not defined." )
endif ( NOT PYTHON__SIM__MODULE_NAME )
if ( NOT PYTHON__SIM__INCLUDE_DIRS )
	set ( BUILD_PYTHON_MODULE__ERROR TRUE )
	message ( SEND_ERROR "PYTHON__SIM__INCLUDE_DIRS not defined." )
endif ( NOT PYTHON__SIM__INCLUDE_DIRS )
if ( NOT PYTHON__SIM__LIB_DIRS )
	set ( BUILD_PYTHON_MODULE__ERROR TRUE )
	message ( SEND_ERROR "PYTHON__SIM__LIB_DIRS not defined." )
endif ( NOT PYTHON__SIM__LIB_DIRS )
if ( NOT PYTHON__SIM__LIBS )
	set ( BUILD_PYTHON_MODULE__ERROR TRUE )
	message ( SEND_ERROR "PYTHON__SIM__LIBS not defined." )
endif ( NOT PYTHON__SIM__LIBS )
if ( NOT PYTHON__SIM__SIM_EXEC_NAME )
	set ( BUILD_PYTHON_MODULE__ERROR TRUE )
	message ( SEND_ERROR "PYTHON__SIM__SIM_EXEC_NAME not defined." )
endif ( NOT PYTHON__SIM__SIM_EXEC_NAME )
if ( ${BUILD_PYTHON_MODULE__ERROR} )
	message ( FATAL_ERROR 
"The usage python library generator requires the definition of the following 
variables:
- PYTHON__SIM__IMPORT_NAME = ${PYTHON__SIM__IMPORT_NAME}
- PYTHON__SIM__MODULE_NAME = ${PYTHON__SIM__MODULE_NAME}
- PYTHON__SIM__INCLUDE_DIRS = ${PYTHON__SIM__INCLUDE_DIRS}
- PYTHON__SIM__LIB_DIRS = ${PYTHON__SIM__LIB_DIRS}
- PYTHON__SIM__LIBS = ${PYTHON__SIM__LIBS}
- PYTHON__SIM__SIM_EXEC_NAME = ${PYTHON__SIM__SIM_EXEC_NAME}
Additionally you can set the following variables:
- PYTHON__SIM__EXTRA_COMPILE_FLAGS = ${PYTHON__SIM__EXTRA_COMPILE_FLAGS}" )
endif ( ${BUILD_PYTHON_MODULE__ERROR} )
message ( 
"The usage python library generator requires the definition of the following 
variables:
- PYTHON__SIM__IMPORT_NAME = ${PYTHON__SIM__IMPORT_NAME}
- PYTHON__SIM__MODULE_NAME = ${PYTHON__SIM__MODULE_NAME}
- PYTHON__SIM__INCLUDE_DIRS = ${PYTHON__SIM__INCLUDE_DIRS}
- PYTHON__SIM__LIB_DIRS = ${PYTHON__SIM__LIB_DIRS}
- PYTHON__SIM__LIBS = ${PYTHON__SIM__LIBS}
- PYTHON__SIM__SIM_EXEC_NAME = ${SIM_EXEC_NAME}
Additionally you can set the following variables:
- PYTHON__SIM__EXTRA_COMPILE_FLAGS = ${PYTHON__SIM__EXTRA_COMPILE_FLAGS}" )

include ( FindPythonInterp )
message ( "The following python executable `${PYTHON_EXECUTABLE}' will be used
	to create a python module of the UNISIM simulator. If you want to use
	another python version please launch cmake with the following option:
	-DPYTHON_EXECUTABLE=<path to the python interpreter>" )

if ( ${PYTHONINTERP_FOUND} )
	configure_file (
		${CMAKE_SOURCE_DIR}/python/setup.py.in
		${CMAKE_BINARY_DIR}/python/setup.py )
	configure_file (
		${CMAKE_SOURCE_DIR}/python/python_config.hh.in
		${CMAKE_BINARY_DIR}/python/python_config.hh )

	add_custom_command (
		OUTPUT
			lib/${PYTHON__SIM__IMPORT_NAME}/simulator.so
			lib/${PYTHON__SIM__IMPORT_NAME}/variable.so
			lib/${PYTHON__SIM__IMPORT_NAME}/debugger.so
		COMMAND
			${CMAKE_COMMAND} -E remove lib/${PYTHON__SIM__IMPORT_NAME}/simulator.so
		COMMAND
			${CMAKE_COMMAND} -E remove lib/${PYTHON__SIM__IMPORT_NAME}/variable.so
		COMMAND
			${CMAKE_COMMAND} -E remove lib/${PYTHON__SIM__IMPORT_NAME}/debugger.so
		COMMAND
			${CMAKE_COMMAND} -E make_directory python
		COMMAND
			${PYTHON_EXECUTABLE} python/setup.py build -t python --build-lib lib
		DEPENDS
			${CMAKE_BINARY_DIR}/config.hh
			${CMAKE_BINARY_DIR}/python/setup.py
			${CMAKE_BINARY_DIR}/python/python_config.hh
			${CMAKE_SOURCE_DIR}/python/py_simulator.hh
			${CMAKE_SOURCE_DIR}/python/py_simulator.cc
			${CMAKE_SOURCE_DIR}/python/py_variable.hh
			${CMAKE_SOURCE_DIR}/python/py_variable.cc
			${CMAKE_SOURCE_DIR}/python/py_debugger.hh
			${CMAKE_SOURCE_DIR}/python/py_debugger.cc
			simulatorlib
			${sim__deps}
		COMMENT
			"Building python extension."
		)

	add_custom_target ( ${SIM_LIB_TARGET}-python
		ALL
		DEPENDS
			lib/${PYTHON__SIM__IMPORT_NAME}/simulator.so
			lib/${PYTHON__SIM__IMPORT_NAME}/variable.so
			lib/${PYTHON__SIM__IMPORT_NAME}/debugger.so
		COMMENT
		"Python target."
	)

	install (
		FILES
			${CMAKE_BINARY_DIR}/lib/${PYTHON__SIM__IMPORT_NAME}.so
		DESTINATION
			lib
		)
endif ( ${PYTHONINTERP_FOUND} )

