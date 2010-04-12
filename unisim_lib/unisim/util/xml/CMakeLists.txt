message ( STATUS "Checking unisim::util::xml" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )

set ( C_INCLUDES stdarg )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES list string iosfwd )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__util__xml
		${CMAKE_SOURCE_DIR}/unisim/util/xml/xml.hh
		${CMAKE_SOURCE_DIR}/unisim/util/xml/xml.cc
	)
	
set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/util/xml/xml.hh
	PROPERTY
		HEADER_FILE_ONLY TRUE
	)
		
message ( STATUS "Checking unisim::util::xml - done" )

