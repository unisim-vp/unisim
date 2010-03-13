message ( STATUS "Checking unisim::service::interfaces::time" )

include ( CheckIncludeFile )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could C \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/service/interfaces/time.hh
	PROPERTIES
		HEADER_FILE_ONLY TRUE )

add_custom_target ( unisim__service__interfaces__time
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/time.hh
	)

message ( STATUS "Checking unisim::service::interfaces::time - done" )
