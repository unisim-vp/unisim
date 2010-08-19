cmake_minimum_required ( VERSION 2.8 )

set ( SIM_VERSION_MAJOR 0 )
set ( SIM_VERSION_MINOR 4 )
set ( SIM_VERSION_PATCH 2 )
set ( SIM_VERSION 
	"${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH}" )
set ( SIM_SOVERSION ${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR} ) 
set ( SIM_VERSION_CODENAME "Pegaso" )
set ( SIM_AUTHOR "Daniel Gracia Perez (daniel.gracia-perez@cea.fr)" )
set ( SIM_PROGRAM_NAME "UNISIM ARMEmu" )
set ( SIM_COPYRIGHT "Copyright (C) 2007-2010, Commissariat a l'Energie Atomique" )
set ( SIM_LICENSE "BSD (See file COPYING)" )
set ( SIM_DESCRIPTION "UNISIM ARMv5 User Level Simulator" )
set ( SIM_PYTHON_MODULE_NAME "armemu" )
set ( SIM_PYTHON_IMPORT_NAME "armemu${SIM_VERSION_MAJOR}${SIM_VERSION_MINOR}${SIM_VERSION_PATCH}")

if ( UNIX AND NOT APPLE )
	find_file ( LSB_RELEASE_FILE lsb-release /etc )
	if ( LSB_RELEASE_FILE )
		file ( STRINGS /etc/lsb-release DISTRIB_ID REGEX "DISTRIB_ID=(.*)$" )
		if ( DISTRIB_ID )
			string ( REGEX REPLACE "DISTRIB_ID=" "" DISTRIB_ID ${DISTRIB_ID} )
		endif ( DISTRIB_ID )
		file ( STRINGS /etc/lsb-release DISTRIB_RELEASE REGEX "DISTRIB_RELEASE=(.*)$" )
		if ( DISTRIB_RELEASE )
			string ( REGEX REPLACE "DISTRIB_RELEASE=" "" DISTRIB_RELEASE ${DISTRIB_RELEASE} )
		endif ( DISTRIB_RELEASE )
	endif ( LSB_RELEASE_FILE )
	message ( "**** Release = ${DISTRIB_ID}${DISTRIB_RELEASE}" )
endif ( UNIX AND NOT APPLE )

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
# set ( CMAKE_CXX_FLAGS "-fPIC" )
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
	unisim/service/debug/sim_debugger
	unisim/service/loader/elf_loader
	unisim/service/loader/linux_loader
	unisim/service/os/linux_os/linux_os_32
	unisim/service/trap_handler
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

add_custom_target ( armemu__share_license
	ALL
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/AUTHORS.txt
			${SHARE_DIRECTORY}/AUTHORS.txt
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/COPYING.txt
			${SHARE_DIRECTORY}/COPYING.txt
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/INSTALL.txt
			${SHARE_DIRECTORY}/INSTALL.txt
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/NEWS.txt
			${SHARE_DIRECTORY}/NEWS.txt
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/README.txt
			${SHARE_DIRECTORY}/README.txt
	COMMENT
		"Copying license files."
	)

add_custom_target ( armemu__share_gdb
	ALL
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/gdb_server
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
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
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/test/src/main.c
			${SHARE_DIRECTORY}/test/src/main.c
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/test/src/CMakeLists.txt
			${SHARE_DIRECTORY}/test/src/CMakeLists.txt
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/test/src/toolchain-armv5l.cmake 
			${SHARE_DIRECTORY}/test/src/toolchain-armv5l.cmake
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/test/test.armv5l
			${SHARE_DIRECTORY}/test/install/test.armv5l
	COMMENT
		"Setting test program source files"
	)

add_custom_target ( armemu__share_logo
	ALL
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/logo
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/logo/logo_unisim.icns
			${SHARE_DIRECTORY}/logo/logo_unisim.icns
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/logo/logo_unisim.png
			${SHARE_DIRECTORY}/logo/logo_unisim.png
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/logo/logo_unisim16.png
			${SHARE_DIRECTORY}/logo/logo_unisim16.png
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/logo/logo_unisim32.png
			${SHARE_DIRECTORY}/logo/logo_unisim32.png
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/logo/logo_unisim128.png
			${SHARE_DIRECTORY}/logo/logo_unisim128.png
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/logo/logo_unisim256.png
			${SHARE_DIRECTORY}/logo/logo_unisim256.png
	COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_SOURCE_DIR}/share/logo/unisim.ico
			${SHARE_DIRECTORY}/logo/unisim.ico
	COMMENT
		"Copying icon files"
	)
			
if ( APPLE )
	configure_file ( 
		${CMAKE_SOURCE_DIR}/share/macosx/armemu-launcher.sh.in
		${SHARE_DIRECTORY}/macosx/armemu-launcher.sh )
	add_custom_target ( armemu__share_macosx
		ALL
		COMMAND
			${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/macosx
		COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${CMAKE_SOURCE_DIR}/share/macosx/Info.plist
				${SHARE_DIRECTORY}/macosx/Info.plist
		COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${CMAKE_SOURCE_DIR}/share/macosx/armemu-launcher.scpt
				${SHARE_DIRECTORY}/macosx/armemu-launcher.scpt
		COMMENT
			"Setting MacOSX application files."
		)
elseif ( UNIX )
	configure_file (
		${CMAKE_SOURCE_DIR}/share/linux/unisim-armemu.desktop.in
		${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop.in )
	add_custom_target ( armemu__share_linux
		ALL
		COMMAND
			${CMAKE_COMMAND} -E copy
				${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop.in
				${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop
		COMMAND
			desktop-file-install --dir=${SHARE_DIRECTORY}/linux ${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop
		DEPENDS
			${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop.in
		COMMENT
			"Setting Linux desktop application files."
		)
endif ( APPLE )

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
	unisim__service__debug__sim_debugger
	unisim__service__trap_handler
	)
if ( UNISIM__SERVICE__POWER__SUPPORT )
	list ( APPEND armemu__libraries
		unisim__service__power )
endif ( UNISIM__SERVICE__POWER__SUPPORT )

set ( UNISIM_KERNEL_PYTHON_TO_SHARED_DATA_PATH "../../share/${ARMEMU_EXEC_NAME}" )
set ( PYTHON__ARMEMU__INCLUDE_DIRS
	"'${CMAKE_SOURCE_DIR}',
	 '${CMAKE_BINARY_DIR}',
	 '${OSCI_SYSTEMC_INCLUDE_DIR}',
	 '${OSCI_TLM2_INCLUDE_DIR}'")
set ( PYTHON__ARMEMU__LIB_DIRS
	"'${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}'")
foreach ( lib ${OSCI_SYSTEMC_LIBRARIES} )
	get_filename_component( lib_path ${lib}
		PATH )
	set ( PYTHON__ARMEMU__LIB_DIRS
		"${PYTHON__ARMEMU__LIB_DIRS},
		 '${lib_path}'")
endforeach ( lib ${OSCI_SYSTEMC_LIBRARIES} )
set ( PYTHON__ARMEMU__LIBS
	"'simulatorlib',
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
	 'unisim__service__debug__sim_debugger',
	 'unisim__service__trap_handler',
	 'unisim__util__random',
	 'unisim__util__debug',
	 'unisim__util__xml',
	 'unisim__kernel',
	 'xml2',
	 'systemc'")
if ( UNISIM__SERVICE__POWER__SUPPORT )
	get_filename_component ( lib_path ${CACTI_4_2_LIBRARY}
		PATH )
	set ( PYTHON__ARMEMU__LIB_DIRS
		"${PYTHON__ARMEMU__LIB_DIRS},
		 '${lib_path}'")
	get_filename_component ( lib_name ${CACTI_4_2_LIBRARY}
		NAME_WE )
	string ( REGEX REPLACE "^lib" "" lib_name ${lib_name} )
	set ( PYTHON__ARMEMU__LIBS
		"${PYTHON__ARMEMU__LIBS},
		'unisim__service__power',
		'${lib_name}'" )
endif ( UNISIM__SERVICE__POWER__SUPPORT )
	 
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

# add_library ( simulator
# 	${simulator__sources}
# 	)
# target_link_libraries ( simulator
# 	${armemu__libraries}
# 	)
# set_target_properties ( simulator
# 	PROPERTIES
# 		COMPILE_DEFINITIONS SIM_EXECUTABLE
# 	)
add_library ( simulatorlib
	${simulator__sources}
	)
target_link_libraries ( simulatorlib
	${armemu__libraries}
	)
set_target_properties ( simulatorlib
	PROPERTIES
		COMPILE_DEFINITIONS SIM_LIBRARY
	)
add_executable ( ${ARMEMU_EXEC_TARGET}
	${CMAKE_SOURCE_DIR}/main.cc
	${simulator__sources}
	)
set_target_properties ( ${ARMEMU_EXEC_TARGET}
	PROPERTIES
		COMPILE_DEFINITIONS SIM_EXECUTABLE )
target_link_libraries ( ${ARMEMU_EXEC_TARGET}
	# simulator
	${armemu__libraries}
	)
# add_library ( ${ARMEMU_LIB_TARGET}
# 	SHARED
# 		${simulator__sources} )
# target_link_libraries ( ${ARMEMU_LIB_TARGET}
# 	${armemu__libraries} )

# set_target_properties ( simulator
# 	PROPERTIES
# 		VERSION ${SIM_VERSION}
# 		SOVERSION ${SIM_SOVERSION} )
set_target_properties ( ${ARMEMU_EXEC_TARGET}
	PROPERTIES
		RUNTIME_OUTPUT_NAME ${ARMEMU_EXEC_NAME} )
# set_target_properties ( ${ARMEMU_LIB_TARGET}
# 	PROPERTIES
# 		LIBRARY_OUTPUT_NAME ${ARMEMU_LIB_NAME} 
# 		VERSION ${SIM_VERSION}
# 		SOVERSION ${SIM_SOVERSION} )
	
set ( armemu__deps
	unisim__kernel
	unisim__component__tlm2__processor__arm__arm926ejs
	unisim__component__tlm2__interrupt
	unisim__component__tlm2__memory__ram
	unisim__service__time__sc_time
	unisim__service__time__host_time
	unisim__service__debug__gdb_server
	unisim__service__debug__inline_debugger
	unisim__service__debug__sim_debugger
	unisim__service__loader__elf_loader
	unisim__service__loader__linux_loader
	unisim__service__os__linux_os__linux_os_32
	unisim__service__trap_handler
	)
if ( UNISIM__SERVICE__POWER__SUPPORT )
	list ( APPEND armemu__deps
		unisim__service__power
		)
endif ( UNISIM__SERVICE__POWER__SUPPORT )
# add_dependencies ( simulator
# 	${armemu__deps}
# 	)
# add_dependencies ( ${ARMEMU_LIB_TARGET}
# 	${armemu__deps}
# 	)
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
	configure_file (
		${CMAKE_SOURCE_DIR}/python/setup.py.in
		${CMAKE_BINARY_DIR}/python/setup.py )
	configure_file (
		${CMAKE_SOURCE_DIR}/python/python_config.hh.in
		${CMAKE_BINARY_DIR}/python/python_config.hh )

	add_custom_command (
		OUTPUT lib/${SIM_PYTHON_IMPORT_NAME}.so
		COMMAND
			${CMAKE_COMMAND} -E remove lib/${SIM_PYTHON_IMPORT_NAME}.so
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

install ( TARGETS ${ARMEMU_EXEC_TARGET} # ${ARMEMU_LIB_TARGET}
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
		${SHARE_DIRECTORY}/AUTHORS.txt
		${SHARE_DIRECTORY}/COPYING.txt
		${SHARE_DIRECTORY}/INSTALL.txt
		${SHARE_DIRECTORY}/NEWS.txt
		${SHARE_DIRECTORY}/README.txt
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
		${SHARE_DIRECTORY}/test/src/main.c
		${SHARE_DIRECTORY}/test/src/CMakeLists.txt
		${SHARE_DIRECTORY}/test/src/toolchain-armv5l.cmake
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
install (
	FILES
		${SHARE_DIRECTORY}/logo/logo_unisim.icns
		${SHARE_DIRECTORY}/logo/logo_unisim.png
		${SHARE_DIRECTORY}/logo/logo_unisim256.png
		${SHARE_DIRECTORY}/logo/logo_unisim128.png
		${SHARE_DIRECTORY}/logo/logo_unisim32.png
		${SHARE_DIRECTORY}/logo/logo_unisim16.png
		${SHARE_DIRECTORY}/logo/unisim.ico
	DESTINATION
		${RELATIVE_SHARE_DIRECTORY}/logo
	)
if ( APPLE )	
	install (
		FILES
			${SHARE_DIRECTORY}/macosx/Info.plist
			${SHARE_DIRECTORY}/macosx/armemu-launcher.scpt
			${SHARE_DIRECTORY}/macosx/armemu-launcher.sh
		DESTINATION
			${RELATIVE_SHARE_DIRECTORY}/macosx
		)
elseif ( UNIX )
	#	add_custom_command ( TARGET ${ARMEMU_EXEC_TARGET}
	#		POST_BUILD
	#		COMMAND 
	#			${CMAKE_SOURCE_DIR}/share/linux/find_deb_dependencies.sh
	#				${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME}
	#				${CMAKE_BINARY_DIR}/lib/${ARMEMU_LIB_NAME}
	#				${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
	#				> ${CMAKE_BINARY_DIR}/deps.deb
	#		COMMENT
	#			"Computing ARMEmu Debian dependencies."
	#		)
	add_custom_command ( 
		OUTPUT ${CMAKE_BINARY_DIR}/deps.deb
		COMMAND
			${CMAKE_SOURCE_DIR}/share/linux/find_deb_dependencies.sh
				${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME}
				${CMAKE_BINARY_DIR}/lib/${ARMEMU_LIB_NAME}
				${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
				> ${CMAKE_BINARY_DIR}/deps.deb
		DEPENDS
			${ARMEMU_EXEC_TARGET}
			${ARMEMU_LIB_TARGET}
			${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
		COMMENT
			"Computing ARMEmu Debian dependencies."
		)
	add_custom_target ( debian_package
		COMMAND
			make
		COMMAND
			${CMAKE_COMMAND} .
		COMMAND
			make package
		DEPENDS ${CMAKE_BINARY_DIR}/deps.deb
		)
	if ( EXISTS ${CMAKE_BINARY_DIR}/deps.deb )
		file ( STRINGS ${CMAKE_BINARY_DIR}/deps.deb ARMEMU__DEPS_DEB
			LIMIT_COUNT 1 )
		message ( "ARMEMU__DEPS_DEB = ${ARMEMU__DEPS_DEB}" )
		set ( ARMEMU__DEPS_DEB ${ARMEMU__DEPS_DEB} CACHE STRING "Debian dependencies" FORCE )
	else ( EXISTS ${CMAKE_BINARY_DIR}/deps.deb )
		set ( ARMEMU__DEPS_DEB "" CACHE STRING "Debian dependencies" FORCE )
		message ( "Could not find debian dependencies file, if you "
			"are executing make package, make sure you have done a simple "
			"make or make install before (or a previous make package)." )
	endif ( EXISTS ${CMAKE_BINARY_DIR}/deps.deb )
	install (
		FILES
			${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop
		DESTINATION
			share/applications
		)
endif ( APPLE )

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

set ( CPACK_PACKAGE_NAME "unisim-armemu" )
set ( CPACK_PACKAGE_FILE_NAME "unisim-armemu-${SIM_VERSION}" )
if ( DISTRIB_ID AND DISTRIB_RELEASE )
	if ( ${DISTRIB_ID} MATCHES "Ubuntu" )
		set ( CPACK_PACKAGE_FILE_NAME "unisim-armemu_${SIM_VERSION}-${DISTRIB_ID}${DISTRIB_RELEASE}_${CMAKE_SYSTEM_PROCESSOR}" )
	endif ( ${DISTRIB_ID} MATCHES "Ubuntu" )
endif ( DISTRIB_ID AND DISTRIB_RELEASE )
set ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "UNISIM ArmEmu project." )
set ( CPACK_PACKAGE_VENDOR "CEA" )
set ( CPACK_PACKAGE_CONTACT "daniel.gracia-perez@cea.fr" )
set ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.txt" )
set ( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.txt" )
set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt" )
set ( CPACK_PACKAGE_VERSION_MAJOR ${SIM_VERSION_MAJOR} )
set ( CPACK_PACKAGE_VERSION_MINOR ${SIM_VERSION_MINOR} )
set ( CPACK_PACKAGE_VERSION_PATCH ${SIM_VERSION_PATCH} )
set ( CPACK_PACKAGE_INSTALL_DIRECTORY "UNISIM ArmEmu ${SIM_VERSION}" )
if ( APPLE )
	set ( CPACK_GENERATOR "Bundle" )
	set ( CPACK_BUNDLE_NAME "UNISIM ArmEmu" )
	set ( CPACK_BUNDLE_PLIST "${SHARE_DIRECTORY}/macosx/Info.plist" )
	set ( CPACK_BUNDLE_STARTUP_COMMAND "${SHARE_DIRECTORY}/macosx/armemu-launcher.sh" )
	set ( CPACK_BUNDLE_ICON "${SHARE_DIRECTORY}/logo/logo_unisim.icns" )
elseif ( UNIX )
	set ( CPACK_GENERATOR "DEB" )
	set ( CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386 )
	set ( CPACK_DEBIAN_PACKAGE_DEPENDS ${ARMEMU__DEPS_DEB} )
	set ( CPACK_DEBIAN_PACKAGE_SECTION "devel" )
	set ( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
	set ( CPACK_DEBIAN_PACKAGE_RECOMMENDS "" )
	set ( CPACK_DEBIAN_PACKAGE_SUGGESTS "" )
	set ( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "" )
endif ( APPLE )
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
