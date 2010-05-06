message ( STATUS "Checking unisim::util::random" )

include ( CheckIncludeFile )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

add_library ( unisim__util__random
		${CMAKE_SOURCE_DIR}/unisim/util/random/random.hh
		${CMAKE_SOURCE_DIR}/unisim/util/random/random.cc
	)

include_directories ( ${CMAKE_SOURCE_DIR} )

message ( STATUS "Checking unisim::util::random - done" )
