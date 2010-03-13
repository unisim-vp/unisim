message ( STATUS "Checking unisim::service::interfaces::memory_access_reporting" )

include ( CheckIncludeFileCXX )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )


set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/service/interfaces/memory_access_reporting.hh
	PROPERTIES
		HEADER_FILE_ONLY TRUE )

add_custom_target ( unisim__service__interfaces__memory_access_reporting
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/memory_access_reporting.hh
	)


message ( STATUS "Checking unisim::service::interfaces::memory_access_reporting - done" )


