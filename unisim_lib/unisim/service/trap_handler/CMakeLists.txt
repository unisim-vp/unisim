message ( STATUS "Checking unisim::service::trap_handler" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( trap_handler_subdirs 
	unisim/service/interfaces/trap_reporting
	unisim/kernel
	)

foreach ( dir ${host_time_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_library ( unisim__service__trap_handler
		${CMAKE_SOURCE_DIR}/unisim/service/trap_handler/trap_handler.hh
		${CMAKE_SOURCE_DIR}/unisim/service/trap_handler/trap_handler.cc
	)
	
target_link_libraries ( unisim__service__trap_handler
	unisim__kernel
	)

message ( STATUS "Checking unisim::service::trap_handler - done" )



