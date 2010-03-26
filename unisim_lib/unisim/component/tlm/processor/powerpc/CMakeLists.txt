message ( STATUS "Checking unisim::component::tlm::processor::powerpc" )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( tlm_powerpc_subdirs
	unisim/kernel/service
	unisim/kernel/tlm
	unisim/component/cxx/processor/powerpc
	unisim/component/tlm/message/snooping_fsb
	unisim/component/tlm/message/interrupt
	)

foreach ( dir ${tlm_powerpc_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories (
	${CMAKE_SOURCE_DIR}
	${OSCI_SYSTEMC_INCLUDE_DIR}
	${CMAKE_BINARY_DIR}
	)

set ( tlm_powerpc_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/tlm/processor/powerpc/powerpc.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm/processor/powerpc/powerpc.tcc
	)

add_custom_target ( unisim__component__tlm__processor__powerpc
	SOURCES
		${tlm_powerpc_srcs}
	)

add_dependencies ( unisim__component__tlm__processor__powerpc
	unisim__kernel__service
	unisim__kernel__tlm
	unisim__component__cxx__processor__powerpc
	unisim__component__tlm__message__snooping_fsb
	unisim__component__tlm__message__interrupt
	)

message ( STATUS "Checking unisim::component::tlm::processor::powerpc - done" )


