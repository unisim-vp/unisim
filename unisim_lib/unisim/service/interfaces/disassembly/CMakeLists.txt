message ( STATUS "Checking unisim::service::interfaces::disassembly" )

include ( CheckIncludeFileCXX )

set ( CXX_INCLUDES string ) 
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/service/interfaces/disassembly.hh
	PROPERTIES
		HEADER_FILE_ONLY TRUE )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__disassembly
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/disassembly.hh
	)

message ( STATUS "Checking unisim::service::interfaces::disassembly - done" )
