cmake_minimum_required ( VERSION 2.8 )

set ( SIM_VERSION_MAJOR 0 )
set ( SIM_VERSION_MINOR 2 )
set ( SIM_VERSION_PATCH 0 )
set ( SIM_VERSION 
	${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH} )
set ( SIM_SOVERSION ${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR} ) 
set ( SIM_VERSION_CODENAME "\"Pacifico\"" )
set ( SIM_AUTHOR "\"Daniel Gracia Perez (daniel.gracia-perez@cea.fr)\"" )
set ( SIM_PROGRAM_NAME "\"UNISIM armemu\"" )
set ( SIM_COPYRIGHT "\"Copyright (C) 2007-2010, Comissariat a l'Energie Atomique\"")
set ( SIM_LICENSE "\"BSD (See file COPYING)\"" )
set ( SIM_DESCRIPTION "\"UNISIM ARMv5 User Level Simulator\"" )
set ( SIM_PYTHON_MODULE_NAME "\"armemu\"" )
set ( SIM_PYTHON_IMPORT_NAME "armemu${SIM_VERSION_MAJOR}${SIM_VERSION_MINOR}${SIM_VERSION_PATCH}")

option ( BUILD_ARMEMU_PYTHON_MODULE "Set this option if you want to compile a module of the unisim armemu simulator. Note that it has been developed under python3.1." )
project ( unisim_armemu )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( FindPythonInterp )
message ( "The following python executable `${PYTHON_EXECUTABLE}' will be used
	to create a python module of the UNISIM armemu simulator. If you want to use
	another python version please launch cmake with the following option:
	-DPYTHON_EXECUTABLE=<path to the python interpreter>" )

set ( ARMEMU_EXEC_NAME unisim-armemu-${SIM_VERSION} )
set ( ARMEMU_LIB_NAME unisim-armemu )
set ( ARMEMU_EXEC_TARGET ${ARMEMU_EXEC_NAME}-exe )
set ( ARMEMU_LIB_TARGET ${ARMEMU_LIB_NAME}-lib )
set ( RELATIVE_SHARE_DIRECTORY "share/${ARMEMU_EXEC_NAME}" )
set ( SHARE_DIRECTORY "${CMAKE_BINARY_DIR}/${RELATIVE_SHARE_DIRECTORY}" )
set_property ( GLOBAL 
	PROPERTY
		UNISIM_KERNEL_BIN_TO_SHARED_DATA_PATH "../share/${ARMEMU_EXEC_NAME}" )

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS *.cc *.tcc *.hh CACHE STRING "Extensions of the source files" )
set ( CMAKE_CXX_FLAGS "-fPIC" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${ARMEMU_LIB_NAME}-${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH} )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
	
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

set ( simulator__sources
	${CMAKE_SOURCE_DIR}/simulator.cc
	${CMAKE_SOURCE_DIR}/simulator.hh
	${CMAKE_BINARY_DIR}/config.hh
	)
	
set ( armemu__libraries
	${OSCI_SYSTEMC_LIBRARIES}
	unisim__kernel
	# unisim__component__cxx__processor__arm
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

set ( PYTHON__ARMEMU__LIBS
	"'simulator',
	 'unisim__component__tlm2__interrupt',
	 'unisim__component__tlm2__memory__ram',
	 'unisim__component__tlm2__processor__arm__arm926ejs',
	 'unisim__component__cxx__processor__arm',
	 'unisim__component__cxx__memory__ram',
	 'unisim__service__time__host_time',
	 'unisim__service__time__sc_time',
	 'unisim__service__loader__elf_loader',
	 'unisim__service__loader__linux_loader',
	 'unisim__service__os__linux_os__linux_os_32',
	 'unisim__service__os__linux_os',
	 'unisim__service__debug__gdb_server',
	 'unisim__service__debug__inline_debugger',
	 'unisim__util__debug',
	 'unisim__util__xml',
	 'unisim__kernel',
	 'xml2',
	 'systemc'")
	 
set ( CMAKE_REQUIRED_FLAGS "-include stdio.h" )
check_include_file ( editline/readline.h HAVE_editline_readline_H )
if ( HAVE_editline_readline_H )
	find_library ( LIBEDIT
		edit )
	if ( LIBEDIT )
		set ( PYTHON__ARMEMU__LIBS
			"${PYTHON__ARMEMU__LIBS},'edit'" )
	endif ( LIBEDIT )
endif ( HAVE_editline_readline_H )

configure_file ( 
	${CMAKE_SOURCE_DIR}/config.hh.in 
	${CMAKE_BINARY_DIR}/config.hh )
configure_file (
	${CMAKE_SOURCE_DIR}/setup.py.in
	${CMAKE_BINARY_DIR}/setup.py )

add_library ( simulator
		${simulator__sources}
	)
target_link_libraries ( simulator
	${armemu__libraries}
	)
add_executable ( ${ARMEMU_EXEC_TARGET}
	${CMAKE_SOURCE_DIR}/main.cc
	)
target_link_libraries ( ${ARMEMU_EXEC_TARGET}
	simulator
	${armemu__libraries}
	)
add_library ( ${ARMEMU_LIB_TARGET}
	SHARED
		${simulator__sources} )
target_link_libraries ( ${ARMEMU_LIB_TARGET}
	${armemu__libraries} )

set_target_properties ( simulator
	PROPERTIES
		VERSION ${SIM_VERSION}
		SOVERSION ${SIM_SOVERSION} )
set_target_properties ( ${ARMEMU_EXEC_TARGET}
	PROPERTIES
		RUNTIME_OUTPUT_NAME ${ARMEMU_EXEC_NAME} )
set_target_properties ( ${ARMEMU_LIB_TARGET}
	PROPERTIES
		LIBRARY_OUTPUT_NAME ${ARMEMU_LIB_NAME} 
		VERSION ${SIM_VERSION}
		SOVERSION ${SIM_SOVERSION} )
	
# if ( APPLE )
# 	add_executable ( ${ARMEMU_EXEC_TARGET}-apple
# 		MACOSX_BUNDLE
# 			${CMAKE_SOURCE_DIR}/main.cc
# 		)
# 	target_link_libraries ( ${ARMEMU_EXEC_TARGET}-apple
# 		simulator
# 		${armemu__libraries}
# 		)
# 	set_target_properties ( ${ARMEMU_EXEC_TARGET}-apple
# 		PROPERTIES
# 			RUNTIME_OUTPUT_NAME ${ARMEMU_EXEC_NAME}
# 			VERSION ${SIM_VERSION} ) 
# endif ( APPLE )

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
add_dependencies ( simulator
	${armemu__deps}
	)
add_dependencies ( ${ARMEMU_LIB_TARGET}
	${armemu__deps}
	)
add_dependencies ( ${ARMEMU_EXEC_TARGET}
	${armemu__deps}
	)

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

if ( BUILD_ARMEMU_PYTHON_MODULE )
	add_custom_command (
		OUTPUT lib/${SIM_PYTHON_IMPORT_NAME}.so
		COMMAND
			${CMAKE_COMMAND} -E remove_directory python
		COMMAND
			${CMAKE_COMMAND} -E remove lib/${SIM_PYTHON_IMPORT_NAME}.so
		COMMAND
			${CMAKE_COMMAND} -E make_directory python
		COMMAND
			${PYTHON_EXECUTABLE} setup.py build -t python --build-lib lib
		DEPENDS
			${CMAKE_BINARY_DIR}/setup.py
			${CMAKE_BINARY_DIR}/config.hh
			${CMAKE_SOURCE_DIR}/python_lib.cc
			simulator
			${armemu__deps}
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

	if ( ${PYTHONINTERP_FOUND} )
		install (
			FILES
				${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
			DESTINATION
				lib
			)
	endif ( ${PYTHONINTERP_FOUND} )
endif ( BUILD_ARMEMU_PYTHON_MODULE )

install ( TARGETS ${ARMEMU_EXEC_TARGET} ${ARMEMU_LIB_TARGET}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)
# if ( APPLE )
# 	install ( TARGETS ${ARMEMU_EXEC_TARGET}-apple
# 		BUNDLE DESTINATION applications )
# endif ( APPLE )
install (
	FILES
		${CMAKE_SOURCE_DIR}/AUTHORS.txt
		${CMAKE_SOURCE_DIR}/COPYING.txt
		${CMAKE_SOURCE_DIR}/INSTALL.txt
		${CMAKE_SOURCE_DIR}/NEWS.txt
		${CMAKE_SOURCE_DIR}/README.txt
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

if ( APPLE )
	set ( CPACK_GENERATOR "Bundle" )
	set ( CPACK_BUNDLE_NAME "unisim-armemu" )
	set ( CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/README.txt" )
	set ( CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/share/logo/logo_unisim.icns" )
endif ( APPLE )
set ( CPACK_PACKAGE_FILE_NAME "unisim-armemu-${SIM_VERSION}" )
set ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "UNISIM ArmEmu project." )
set ( CPACK_PACKAGE_VENDOR "CEA" )
set ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.txt" )
set ( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.txt" )
set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt" )
set ( CPACK_PACKAGE_VERSION_MAJOR ${SIM_VERSION_MAJOR} )
set ( CPACK_PACKAGE_VERSION_MINOR ${SIM_VERSION_MINOR} )
set ( CPACK_PACKAGE_VERSION_PATCH ${SIM_VERSION_PATCH} )
set ( CPACK_PACKAGE_INSTALL_DIRECTORY "UNISIM ArmEmu ${SIM_VERSION}" )
# set ( CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share/logo/logo_unisim.icns" )
IF(WIN32 AND NOT UNIX)
	# There is a bug in NSI that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backlasshes.
	SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
	SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
	SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
	SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
	SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
	SET(CPACK_NSIS_MODIFY_PATH ON)
# ELSE(WIN32 AND NOT UNIX)
# 	SET(CPACK_STRIP_FILES "bin/${ARMEMU_EXEC_NAME}")
	# SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "${ARMEMU_EXEC_NAME}" "${ARMEMU_EXEC_NAME}")
set ( CPACK_SOURCE_GENERATOR "TBZ2;TGZ;ZIP" )
set ( CPACK_SOURCE_PACKAGE_FILE_NAME "unisim-armemu-${SIM_VERSION}" )
INCLUDE(CPack)
