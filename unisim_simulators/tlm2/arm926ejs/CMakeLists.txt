cmake_minimum_required ( VERSION 2.8 )
project ( unisim_arm926ejs )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS *.cc *.tcc *.hh CACHE STRING "Extensions of the source files" )
	
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

set ( SIM_VERSION_MAJOR 0 )
set ( SIM_VERSION_MINOR 1 )
set ( SIM_VERSION_PATCH 1 )
set ( SIM_VERSION_CODENAME "\"Sagrera\"" )
set ( SIM_AUTHOR "\"Daniel Gracia Perez (daniel.gracia-perez@cea.fr)\"" )

set ( arm926ejs_emu_subdirs
	unisim/kernel/service
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
foreach ( dir ${arm926ejs_emu_subdirs} )
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

set ( RELATIVE_SHARE_DIRECTORY "share/arm926ejs_emu" )
set ( SHARE_DIRECTORY "${CMAKE_BINARY_DIR}/${RELATIVE_SHARE_DIRECTORY}" )

add_custom_target ( arm926ejs_emu__share_gdb
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

add_custom_target ( arm926ejs_emu__share_test
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

add_executable ( arm926ejs_emu 
	${CMAKE_SOURCE_DIR}/main.cc 
	${CMAKE_BINARY_DIR}/config.hh
	)
set ( arm926ejs_emu_libraries
	${OSCI_SYSTEMC_LIBRARIES}
	unisim__kernel__service
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
	list ( APPEND arm926ejs_emu_libraries
		unisim__service__power )
endif ( UNISIM__SERVICE__POWER__SUPPORT )

target_link_libraries ( arm926ejs_emu
	${arm926ejs_emu_libraries}
	)

set ( arm926ejs_emu_deps
	unisim__kernel__service
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
	list ( APPEND arm926ejs_emu_deps
		unisim__service__power
		)
endif ( UNISIM__SERVICE__POWER__SUPPORT )
add_dependencies ( arm926ejs_emu
	${arm926ejs_emu_deps}
	)

set_target_properties ( arm926ejs_emu
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

add_custom_command (
	OUTPUT
		${SHARE_DIRECTORY}/default-config.xml
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}
	COMMAND
		${CMAKE_BINARY_DIR}/bin/arm926ejs_emu -g ${SHARE_DIRECTORY}/default-config.xml
	DEPENDS
		${CMAKE_BINARY_DIR}/bin/arm926ejs_emu
	COMMENT
		"Creating simulator default configuration in \"${RELATIVE_SHARE_DIRECTORY}/default-config.xml\""
	)

install ( TARGETS arm926ejs_emu
	RUNTIME DESTINATION bin
	)
install ( 
	FILES
		${SHARE_DIRECTORY}/default-config.xml
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
		${RELATIVE_SHARE_DIRECTORY}/test/
	OPTIONAL
	)	
	
add_custom_target ( arm926ejs_emu__resources
	ALL
	DEPENDS
		${SHARE_DIRECTORY}/default-config.xml
	)

add_dependencies ( arm926ejs_emu__resources
	arm926ejs_emu )

add_custom_target ( check
	COMMAND
		${CMAKE_CTEST_COMMAND}
	DEPENDS
		arm926ejs_emu )

add_custom_target ( prepare_check
	COMMENT
		"Preparing tests"
	)

add_dependencies ( check
	prepare_check )

enable_testing()

set ( CPACK_PACKAGE_VERSION_MAJOR ${SIM_VERSION_MAJOR} )
set ( CPACK_PACKAGE_VERSION_MINOR ${SIM_VERSION_MINOR} )
set ( CPACK_PACKAGE_VERSION_PATCH ${SIM_VERSION_PATCH} )
set ( CPACK_SOURCE_GENERATOR "TBZ2" )
set ( CPACK_GENERATOR "TBZ2" )
include ( CPack )

