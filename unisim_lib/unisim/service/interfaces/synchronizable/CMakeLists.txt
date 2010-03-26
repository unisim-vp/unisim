message ( STATUS "Checking unisim::service::interfaces::synchronizable" )

include ( CheckIncludeFile )

set ( C_INCLUDES inttypes ) 
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories (
	${CMAKE_SOURCE_DIR}
	)

add_custom_target ( unisim__service__interfaces__synchronizable
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/synchronizable.hh
	)

message ( STATUS "Checking unisim::service::interfaces::synchronizable - done" )

