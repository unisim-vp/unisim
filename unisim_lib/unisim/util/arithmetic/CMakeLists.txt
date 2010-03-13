get_property ( UNISIM__UTIL__ARITHMETIC_DEFINED GLOBAL PROPERTY UNISIM__UTIL__ARITHMETIC SET )
if ( NOT ${UNISIM__UTIL__ARITHMETIC_DEFINED} )
	message ( STATUS "Checking unisim::util::arithmetic" )

	set_property ( GLOBAL PROPERTY UNISIM__UTIL__ARITHMETIC TRUE )
	include ( CheckIncludeFile )

	set ( C_INCLUDES inttypes )
	foreach ( C_INCLUDE ${C_INCLUDES} )
		check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
		if ( NOT HAVE_${C_INCLUDE}_H )
			message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
		endif ( NOT HAVE_${C_INCLUDE}_H )
	endforeach ( C_INCLUDE )

	set_source_files_properties ( ${CMAKE_SOURCE_DIR}/unisim/util/arithmetic/arithmetic.hh
		PROPERTIES
			HEADER_FILE_ONLY TRUE )
	
	message ( STATUS "Checking unisim::util::arithmetic - done" )
endif ( NOT ${UNISIM__UTIL__ARITHMETIC_DEFINED} )

