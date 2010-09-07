message ( STATUS "Checking unisim::util::debug::debugger_handler" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__util__debug__debugger_handler
		${CMAKE_SOURCE_DIR}/unisim/util/debug/debugger_handler/debugger_handler.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/debugger_handler/debugger_handler.cc
	)

message ( STATUS "Checking unisim::service::debug::sim_debugger - done" )



