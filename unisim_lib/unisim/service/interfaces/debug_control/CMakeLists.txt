message ( STATUS "Checking unisim::service::interfaces::debug_yielding" )

include ( CheckIncludeFileCXX )

set ( CXX_INCLUDES string ) 
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/service/interfaces/debug_yielding.hh
	PROPERTIES
		HEADER_FILE_ONLY TRUE )

add_custom_target ( unisim__service__interfaces__debug_yielding
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/debug_yielding.hh
	)

message ( STATUS "Checking unisim::service::interfaces::debug_yielding - done" )

