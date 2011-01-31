message ( STATUS "Checking unisim::component::tlm2::processor::arm::armemu" )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( tlm2_armemu_subdirs
	unisim/kernel
	unisim/component/cxx/processor/arm/armemu
	)

foreach ( dir ${tlm2_armemu_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	)

set ( tlm2_armemu_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/armemu/armemu.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/armemu/armemu.cc
	)

add_library ( unisim__component__tlm2__processor__arm__armemu
	${tlm2_armemu_srcs}
	)

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/armemu/armemu.cc
	PROPERTIES
		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
	)

target_link_libraries ( unisim__component__tlm2__processor__arm__armemu
	${OSCI_SYSTEMC_LIBRARIES} 
	unisim__kernel
	unisim__component__cxx__processor__arm__armemu
	)

add_dependencies ( unisim__component__tlm2__processor__arm__armemu
	unisim__kernel
	unisim__component__cxx__processor__arm__armemu
	)

message ( STATUS "Checking unisim::component::tlm2::processor::arm::armemu - done" )
