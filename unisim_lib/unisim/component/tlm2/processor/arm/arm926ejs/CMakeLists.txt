message ( STATUS "Checking unisim::component::tlm2::processor::arm::arm926ejs" )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( tlm2_arm926ejs_subdirs
	unisim/kernel/service
	unisim/component/cxx/processor/arm
	unisim/component/tlm2/processor/arm
	)

foreach ( dir ${tlm2_arm926ejs_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	)

set ( tlm2_arm926ejs_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.cc
	)

add_library ( unisim__component__tlm2__processor__arm__arm926ejs
	${tlm2_arm926ejs_srcs}
	)

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.cc
	PROPERTIES
		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
	)

target_link_libraries ( unisim__component__tlm2__processor__arm__arm926ejs
	${OSCI_SYSTEMC_LIBRARIES} 
	unisim__kernel__service
	unisim__component__cxx__processor__arm
	)

add_dependencies ( unisim__component__tlm2__processor__arm__arm926ejs
	unisim__kernel__service
	unisim__component__cxx__processor__arm
	unisim__component__tlm2__processor__arm
	)

message ( STATUS "Checking unisim::component::tlm2::processor::arm::arm926ejs - done" )
