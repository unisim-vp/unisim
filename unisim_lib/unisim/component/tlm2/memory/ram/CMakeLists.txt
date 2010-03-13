message ( STATUS "Checking unisim::component::tlm2::memory::ram" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( ram_subdirs
	unisim/kernel/service
	unisim/kernel/logger
	unisim/kernel/tlm2
	unisim/component/cxx/memory/ram
	)
foreach ( dir ${ram_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__component__tlm2__memory__ram
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/memory/ram/memory.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/memory/ram/memory.tcc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/memory/ram/memory.cc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/memory/ram/memory_debug.cc
	)

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/memory/ram/memory.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/memory/ram/memory_debug.cc
	PROPERTIES
		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
	)

target_link_libraries ( unisim__component__tlm2__memory__ram
	${OSCI_SYSTEMC_LIBRARIES}
	unisim__kernel__service
	unisim__kernel__logger
	unisim__component__cxx__memory__ram
	)

add_dependencies ( unisim__component__tlm2__memory__ram
	unisim__kernel__tlm2
	)

message ( STATUS "Checking unisim::component::tlm2::memory::ram - done" )

