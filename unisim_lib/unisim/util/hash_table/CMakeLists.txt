message ( STATUS "Checking unisim::util::hash_table" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )

set ( CXX_INCLUDES map )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )
	
set_source_files_properties ( ${CMAKE_SOURCE_DIR}/unisim/util/hash_table/hash_table.hh
	PROPERTIES
		HEADER_FILE_ONLY TRUE )

message ( STATUS "Checking unisim::util::hash_table - done" )

