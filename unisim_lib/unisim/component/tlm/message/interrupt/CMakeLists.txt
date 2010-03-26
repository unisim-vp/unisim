message ( STATUS "Checking unisim::component::tlm::message::interrupt" )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )

set ( unisim__component__tlm__message__interrupt_subdirs
	unisim/component/tlm/debug
	unisim/util/garbage_collector
	)

foreach ( dir ${unisim__component__tlm__message__interrupt_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( 
	${CMAKE_SOURCE_DIR} 
	${OSCI_SYSTEMC_INCLUDE_DIR}
	)

add_library ( unisim__component__tlm__message__interrupt
	STATIC
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/message/interrupt.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/message/interrupt.cc
	)

add_dependencies ( unisim__component__tlm__message__interrupt
	unisim__component__tlm__debug
	unisim__util__garbage_collector
	)

target_link_libraries ( unisim__component__tlm__message__interrupt
	${OSCI_SYSTEMC_LIB}
	unisim__util__garbage_collector
	)

message ( STATUS "Checking unisim::component::tlm::message::interrupt - done" )


