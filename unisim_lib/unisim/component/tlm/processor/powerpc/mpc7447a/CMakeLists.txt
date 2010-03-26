message ( STATUS "Checking unisim::component::tlm::processor::powerpc::mpc7447a" )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( tlm_mpc7447a_subdirs
	unisim/kernel/service
	unisim/component/cxx/processor/powerpc/mpc7447a
	unisim/component/tlm/processor/powerpc
	)

foreach ( dir ${tlm_mpc7447a_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories (
	${CMAKE_SOURCE_DIR}
	${OSCI_SYSTEMC_INCLUDE_DIR}
	${CMAKE_BINARY_DIR}
	)

set ( tlm_mpc7447a_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/tlm/processor/powerpc/mpc7447a.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm/processor/powerpc/mpc7447a_debug.cc
	)

add_library ( unisim__component__tlm__processor__powerpc__mpc7447a
	${tlm_mpc7447a_srcs}
	)

target_link_libraries ( unisim__component__tlm__processor__powerpc__mpc7447a
	${OSCI_SYSTEMC_LIBRARIES} 
	unisim__kernel__service
	unisim__component__cxx__processor__powerpc__mpc7447a
	)

add_dependencies ( unisim__component__tlm__processor__powerpc__mpc7447a
	unisim__component__cxx__processor__powerpc__mpc7447a
	unisim__component__tlm__processor__powerpc
	)

message ( STATUS "Checking unisim::component::tlm::processor::powerpc::mpc7447a - done" )

