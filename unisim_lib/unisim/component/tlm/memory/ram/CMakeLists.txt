message ( STATUS "Checking unisim::component::tlm::memory::ram" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )

set ( tlm_memory_ram_deps
	unisim/kernel/tlm
	unisim/kernel/logger
	unisim/component/tlm/message/memory
	unisim/component/cxx/memory/ram
	)

foreach ( dir ${tlm_memory_ram_deps} )
	unisim_include ( ${dir} )
endforeach ( dir ${tlm_memory_ram_deps} )

include_directories ( 
	${CMAKE_SOURCE_DIR}
	${OSCI_SYSTEMC_INCLUDE_DIR}
	)

add_library ( unisim__component__tlm__memory__ram
	STATIC
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/memory/ram/memory.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/memory/ram/memory.tcc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/memory/ram/memory_32.cc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/memory/ram/memory_32_debug.cc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/memory/ram/memory_64.cc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/memory/ram/memory_64_debug.cc
	)

add_dependencies ( unisim__component__tlm__memory__ram
	unisim__kernel__tlm
	unisim__kernel__logger
	unisim__component__tlm__message__memory
	unisim__component__cxx__memory__ram
	)

target_link_libraries ( unisim__component__tlm__memory__ram
	${OSCI_SYSTEMC_LIB}
	unisim__kernel__tlm
	unisim__kernel__logger
	unisim__component__cxx__memory__ram
	)

message ( STATUS "Checking unisim::component::tlm::memory::ram - done" )



