cmake_minimum_required ( VERSION 2.8 )
project ( unisim_arm926ejs )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
set ( CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/build )
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
	)
foreach ( dir ${arm926ejs_emu_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${OSCI_SYSTEMC_INCLUDE_DIR} ${OSCI_TLM2_INCLUDE_DIR})

file ( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../bin )
file ( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../resources/gdb_server )
file ( 
	COPY 
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_armv4l.xml 
	DESTINATION ${CMAKE_BINARY_DIR}/../resources/gdb_server )

add_executable ( arm926ejs_emu 
	${CMAKE_SOURCE_DIR}/main.cc )
target_link_libraries ( arm926ejs_emu
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
set_target_properties ( arm926ejs_emu
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin )

add_custom_command (
	OUTPUT
		${CMAKE_BINARY_DIR}/../resources/default-config.xml
	COMMAND
		${CMAKE_BINARY_DIR}/../bin/arm926ejs_emu -g ${CMAKE_BINARY_DIR}/../resources/default-config.xml
	COMMENT
		"Creating simulator default configuration in \"resources/default-config.xml\""
	)

add_custom_target ( arm926ejs_emu__resources
	ALL
	DEPENDS
		${CMAKE_BINARY_DIR}/../resources/default-config.xml
	)

add_dependencies ( arm926ejs_emu__resources
	arm926ejs_emu )

# set_source_files_properties (
#	${CMAKE_SOURCE_DIR}/main.cc
#	PROPERTIES
#		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
#	)
include ( CPack )

