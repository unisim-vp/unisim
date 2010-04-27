cmake_minimum_required ( VERSION 2.8 )

set ( SIM_VERSION_MAJOR 0 )
set ( SIM_VERSION_MINOR 2 )
set ( SIM_VERSION_PATCH 0 )
set ( SIM_VERSION "\"${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH}\"" )
set ( SIM_VERSION_CODENAME "\"Pacifico\"" )
set ( SIM_AUTHOR "\"Daniel Gracia Perez (daniel.gracia-perez@cea.fr)\"" )
set ( SIM_PROGRAM_NAME "\"UNISIM armemu\"" )
set ( SIM_COPYRIGHT "\"Copyright (C) 2007-2010, Comissariat a l'Energie Atomique\"")
set ( SIM_LICENSE "\"BSD (See file COPYING)\"" )
set ( SIM_DESCRIPTION "\"UNISIM ARMv5 User Level Simulator\"" )
set ( SIM_PYTHON_MODULE_NAME "\"armemu\"" )
set ( SIM_PYTHON_IMPORT_NAME "armemu${SIM_VERSION_MAJOR}${SIM_VERSION_MINOR}${SIM_VERSION_PATCH}")

project ( unisim_armemu_v${SIM_VERSION_MAJOR}_${SIM_VERSION_MINOR}_${SIM_VERSION_PATCH} )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( FindPythonInterp )
# set ( PYTHON_DEBUG_LIBRARY "/opt/local/Library/Frameworks/Python.framework/Versions/3.1/lib/libpython3.1.dylib" )                                                                                                    
# set ( PYTHON_EXECUTABLE "/opt/local/bin/python" )                                                                                                
# set ( PYTHON_INCLUDE_DIR "/opt/local/Library/Frameworks/Python.framework/Headers" )                                                               
# set ( PYTHON_LIBRARY "/opt/local/Library/Frameworks/Python.framework/Versions/3.1/lib/libpython3.1.dylib")
message ( "Python executable: ${PYTHON_EXECUTABLE}" )
# message ( "Python includes path: ${PYTHON_INCLUDE_PATH}" )
# message ( "Python lib path: ${PYTHON_LIBRARIES}" )

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS *.cc *.tcc *.hh CACHE STRING "Extensions of the source files" )
set ( CMAKE_CXX_FLAGS "-fPIC -g" )
# set ( BUILD_SHARED_LIBS ON )
# set ( CMAKE_INSTALL_RPATH_USE_LINK_PATH true )
	
set ( CXX_INCLUDES iostream list string )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES getopt stdlib signal )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( ARMEMU_EXEC_NAME armemu-${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH} )
set ( ARMEMU_LIB_NAME armemu-${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH} )
set ( ARMEMU_EXEC_TARGET ${ARMEMU_EXEC_NAME}-exe )
set ( ARMEMU_LIB_TARGET ${ARMEMU_LIB_NAME}-lib )
set ( RELATIVE_SHARE_DIRECTORY "share/${ARMEMU_EXEC_NAME}" )
set ( SHARE_DIRECTORY "${CMAKE_BINARY_DIR}/${RELATIVE_SHARE_DIRECTORY}" )
set_property ( GLOBAL 
	PROPERTY
		UNISIM_KERNEL_BIN_TO_SHARED_DATA_PATH "../share/${ARMEMU_EXEC_NAME}" )

set ( armemu_subdirs
	unisim/kernel
	unisim/component/tlm2/processor/arm/arm926ejs
	unisim/component/tlm2/interrupt
	unisim/component/tlm2/memory/ram
	unisim/service/time/sc_time
	unisim/service/time/host_time
	unisim/service/debug/gdb_server
	unisim/service/debug/inline_debugger
	unisim/service/loader/elf_loader
	unisim/service/loader/linux_loader
	unisim/service/os/linux_os/linux_os_32
	unisim/service/power
	)
foreach ( dir ${armemu_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${OSCI_SYSTEMC_INCLUDE_DIR} ${OSCI_TLM2_INCLUDE_DIR})

if ( UNISIM__SERVICE__POWER__SUPPORT )
	set ( SIM_POWER_ESTIMATOR_SUPPORT TRUE )
else ( UNISIM__SERVICE__POWER__SUPPORT )
	set ( SIM_POWER_ESTIMATOR_SUPPORT FALSE )
endif ( UNISIM__SERVICE__POWER__SUPPORT )
configure_file ( 
	${CMAKE_SOURCE_DIR}/config.hh.in 
	${CMAKE_BINARY_DIR}/config.hh )
configure_file (
	${CMAKE_SOURCE_DIR}/setup.py.in
	${CMAKE_BINARY_DIR}/setup.py )

add_custom_target ( armemu__share_gdb
	ALL
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/gdb_server
	COMMAND
		${CMAKE_COMMAND} -E create_symlink
			${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_armv5l.xml
			${SHARE_DIRECTORY}/gdb_server/gdb_armv5l.xml
	COMMENT
		"Setting GDB resource files"
	)

add_custom_target ( armemu__share_test
	ALL
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/test/src
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/test/build
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/test/install
	COMMAND
		${CMAKE_COMMAND} -E create_symlink
			${CMAKE_SOURCE_DIR}/share/test/src/main.c
			${SHARE_DIRECTORY}/test/src/main.c
	COMMAND
		${CMAKE_COMMAND} -E create_symlink
			${CMAKE_SOURCE_DIR}/share/test/src/CMakeLists.txt
			${SHARE_DIRECTORY}/test/src/CMakeLists.txt
	COMMAND
		${CMAKE_COMMAND} -E create_symlink 
			${CMAKE_SOURCE_DIR}/share/test/src/toolchain-armv5l.cmake 
			${SHARE_DIRECTORY}/test/src/toolchain-armv5l.cmake
	COMMENT
		"Setting test program source files"
	)

set ( armemu__sources
	${CMAKE_SOURCE_DIR}/simulator.cc
	${CMAKE_SOURCE_DIR}/simulator.hh
	${CMAKE_BINARY_DIR}/config.hh
	)
	
add_library ( ${ARMEMU_LIB_TARGET}
	SHARED
		${armemu__sources}
	)
add_executable ( ${ARMEMU_EXEC_TARGET}
	${CMAKE_SOURCE_DIR}/main.cc
	)
set_source_files_properties ( ${CMAKE_SOURCE_DIR}/python_lib.cc
	PROPERTIES
		COMPILE_FLAGS -I${PYTHON_INCLUDE_PATH}
	)
set ( armemu__libraries
	${OSCI_SYSTEMC_LIBRARIES}
	unisim__kernel
	unisim__component__cxx__processor__arm
	unisim__component__tlm2__processor__arm__arm926ejs
	unisim__component__tlm2__interrupt
	unisim__component__tlm2__memory__ram
	unisim__service__time__host_time
	unisim__service__time__sc_time
	unisim__service__loader__elf_loader
	unisim__service__loader__linux_loader
	unisim__service__os__linux_os__linux_os_32
	unisim__service__debug__gdb_server
	unisim__service__debug__inline_debugger
	)
if ( UNISIM__SERVICE__POWER__SUPPORT )
	list ( APPEND armemu__libraries
		unisim__service__power )
endif ( UNISIM__SERVICE__POWER__SUPPORT )

target_link_libraries ( ${ARMEMU_LIB_TARGET}
	${armemu__libraries}
	)
target_link_libraries ( ${ARMEMU_EXEC_TARGET}
	${ARMEMU_LIB_TARGET}
	)

set ( armemu__deps
	unisim__kernel
	unisim__component__tlm2__processor__arm__arm926ejs
	unisim__component__tlm2__interrupt
	unisim__component__tlm2__memory__ram
	unisim__service__time__sc_time
	unisim__service__time__host_time
	unisim__service__debug__gdb_server
	unisim__service__debug__inline_debugger
	unisim__service__loader__elf_loader
	unisim__service__loader__linux_loader
	unisim__service__os__linux_os__linux_os_32
	)
if ( UNISIM__SERVICE__POWER__SUPPORT )
	list ( APPEND armemu__deps
		unisim__service__power
		)
endif ( UNISIM__SERVICE__POWER__SUPPORT )
add_dependencies ( ${ARMEMU_LIB_TARGET}
	${armemu__deps}
	)
add_dependencies ( ${ARMEMU_EXEC_TARGET}
	${ARMEMU_LIB_TARGET}
	)

set_target_properties ( ${ARMEMU_EXEC_TARGET}
	PROPERTIES
		RUNTIME_OUTPUT_NAME ${ARMEMU_EXEC_NAME}
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set_target_properties ( ${ARMEMU_LIB_TARGET}
	PROPERTIES
		ARCHIVE_OUTPUT_NAME ${ARMEMU_LIB_NAME}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
		LIBRARY_OUTPUT_NAME ${ARMEMU_LIB_NAME}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

add_custom_command (
	OUTPUT lib/${SIM_PYTHON_IMPORT_NAME}.so
	COMMAND
		${CMAKE_COMMAND} -E make_directory python
	COMMAND
		${PYTHON_EXECUTABLE} setup.py build -t python --build-lib lib
	DEPENDS
		${ARMEMU_LIB_TARGET}
		${CMAKE_BINARY_DIR}/setup.py
		${CMAKE_BINARY_DIR}/config.hh
		${CMAKE_SOURCE_DIR}/python_lib.cc
		${ARMEMU_LIB_TARGET}
	COMMENT
		"Building python extension."
	)
if ( ${PYTHONINTERP_FOUND} )
	add_custom_target ( ${ARMEMU_LIB_TARGET}-python
		ALL
		DEPENDS
			lib/${SIM_PYTHON_IMPORT_NAME}.so
		COMMENT
			"Python target."
		)
endif ( ${PYTHONINTERP_FOUND} )

add_custom_command (
	OUTPUT
		${SHARE_DIRECTORY}/template-default-config.xml
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}
	COMMAND
		${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME} -g ${SHARE_DIRECTORY}/template-default-config.xml
	DEPENDS
		${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME}
	COMMENT
		"Creating simulator default configuration in \"${RELATIVE_SHARE_DIRECTORY}/template-default-config.xml\""
	)

if ( ${PYTHONINTERP_FOUND} )
#   WARNING!!!
#	uncomment the following if you want to install the python library
#   however it has not been tested for installation (still under development)

#	install (
#		FILES
#			${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
#		DESTINATION
#			lib
#		)
endif ( ${PYTHONINTERP_FOUND} )
install ( TARGETS ${ARMEMU_EXEC_TARGET} ${ARMEMU_LIB_TARGET}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)
install (
	FILES
		${CMAKE_SOURCE_DIR}/AUTHORS
		${CMAKE_SOURCE_DIR}/COPYING
		${CMAKE_SOURCE_DIR}/INSTALL
		${CMAKE_SOURCE_DIR}/NEWS
		${CMAKE_SOURCE_DIR}/README
	DESTINATION
		${RELATIVE_SHARE_DIRECTORY}
	)
install ( 
	FILES
		${SHARE_DIRECTORY}/template-default-config.xml
	DESTINATION
		${RELATIVE_SHARE_DIRECTORY}
	)
install ( 
	FILES
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_armv5l.xml
	DESTINATION
		${RELATIVE_SHARE_DIRECTORY}/gdb_server
	)
install ( 
	FILES 
		${CMAKE_SOURCE_DIR}/share/test/src/main.c
		${CMAKE_SOURCE_DIR}/share/test/src/CMakeLists.txt
		${CMAKE_SOURCE_DIR}/share/test/src/toolchain-armv5l.cmake
	DESTINATION
		${RELATIVE_SHARE_DIRECTORY}/test/src
	)
# the test programs will be installed if they are compiled
install (
	PROGRAMS
		${SHARE_DIRECTORY}/test/install/test.armv5l
	DESTINATION
		${RELATIVE_SHARE_DIRECTORY}/test/install
	OPTIONAL
	)	
	
add_custom_target ( armemu__resources
	ALL
	DEPENDS
		${SHARE_DIRECTORY}/template-default-config.xml
	)

add_dependencies ( armemu__resources
	${ARMEMU_EXEC_TARGET} )

add_test ( working_test
	bin/${ARMEMU_EXEC_NAME}
	)
	
add_custom_target ( prepare_check
	COMMENT
		"Preparing tests"
	)
	
add_custom_target ( calling_ctest
	COMMAND
		${CMAKE_CTEST_COMMAND}
	DEPENDS
		${ARMEMU_EXEC_TARGET} )

add_custom_target ( check
	COMMENT
		"Tests finished"
	)

add_dependencies ( check
	prepare_check
	calling_ctest
	)
	
enable_testing()

set ( CPACK_PACKAGE_VERSION_MAJOR ${SIM_VERSION_MAJOR} )
set ( CPACK_PACKAGE_VERSION_MINOR ${SIM_VERSION_MINOR} )
set ( CPACK_PACKAGE_VERSION_PATCH ${SIM_VERSION_PATCH} )
set ( CPACK_SOURCE_GENERATOR "TBZ2" )
set ( CPACK_GENERATOR "TBZ2" )
include ( CPack )
