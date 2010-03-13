message ( STATUS "Checking unisim::component::tlm2::interrupt" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( interrupt_subdirs
	unisim/kernel/service
	unisim/kernel/tlm2
	unisim/component/cxx/memory/ram
	)
foreach ( dir ${interrupt_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__component__tlm2__interrupt
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/interrupt/types.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/interrupt/master_stub.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/interrupt/master_stub.cc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/interrupt/slave_stub.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/interrupt/slave_stub.cc
	)

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/interrupt/master_stub.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/interrupt/slave_stub.cc
	PROPERTIES
		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
	)

target_link_libraries ( unisim__component__tlm2__interrupt
	${OSCI_SYSTEMC_LIBRARIES} )

message ( STATUS "Checking unisim::component::tlm2::interrupt - done" )


