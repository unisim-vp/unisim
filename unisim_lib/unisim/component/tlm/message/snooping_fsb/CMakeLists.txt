message ( STATUS "Checking unisim::component::tlm::message::snooping_fsb" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( unisim__component__tlm__message__snooping_fsb_subdirs
	unisim/kernel/logger
	unisim/component/tlm/debug
	unisim/util/garbage_collector
	)

foreach ( dir ${unisim__component__tlm__message__snooping_fsb_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__component__tlm__message__snooping_fsb
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/message/snooping_fsb.hh
	)

add_dependencies ( unisim__component__tlm__message__snooping_fsb
	unisim__kernel__logger
	unisim__component__tlm__debug
	unisim__util__garbage_collector
	)

message ( STATUS "Checking unisim::component::tlm::message::snooping_fsb- done" )




