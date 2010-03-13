message ( STATUS "Checking unisim::kernel::logger" )
include ( CheckIncludeFileCXX )

set ( CXX_INCLUDES sstream iostream string fstream vector )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

include ( FindLibXml2 )
if ( NOT LIBXML2_FOUND )
	message ( SEND_ERROR "Could not find the libxml2 library." )
endif ( NOT LIBXML2_FOUND )

include_directories ( ${CMAKE_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR} )
add_library ( unisim__kernel__logger 
	STATIC 
		logger.cc
		logger.hh
		logger_server.cc
		logger_server.hh
	)

message ( STATUS "Checking unisim::kernel::logger - done" )
