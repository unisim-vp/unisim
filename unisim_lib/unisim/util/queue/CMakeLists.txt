message ( STATUS "Checking unisim::util::queue" )

include ( CheckIncludeFile )
include ( CheckIncludeFileCXX )

set ( CXX_INCLUDES iosfwd stdexcept sstream iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES inttypes string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_library ( unisim__util__queue
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/util/queue/queue.hh
		${CMAKE_SOURCE_DIR}/unisim/util/queue/queue.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/queue/queue.cc
	)

message ( STATUS "Checking unisim::util::queue - done" )



