get_property ( UNISIM__UTIL__HASH_TABLE_DEFINED 
	GLOBAL 
	PROPERTY UNISIM__UTIL__HASH_TABLE SET )
if ( NOT ${UNISIM__UTIL__HASH_TABLE_DEFINED} )
	message ( STATUS "Checking unisim::util::hash_table" )
	set_property ( GLOBAL PROPERTY UNISIM__UTIL__HASH_TABLE TRUE )

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
	
	set_source_files_properties ( ${CMAKE_SOURCE_DIR}/unisim/util/hash_table/hash_table.hh
		PROPERTIES
			HEADER_FILE_ONLY TRUE )

	message ( STATUS "Checking unisim::util::hash_table - done" )
endif ( NOT ${UNISIM__UTIL__HASH_TABLE_DEFINED} )

