message ( STATUS "Checking unisim::util::arithmetic" )

include ( CheckIncludeFile )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__util__arithmetic
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/util/arithmetic/arithmetic.hh
	)

message ( STATUS "Checking unisim::util::arithmetic - done" )

