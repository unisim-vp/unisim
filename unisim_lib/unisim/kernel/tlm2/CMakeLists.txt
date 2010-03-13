message ( STATUS "Checking unisim::kernel::tlm2" )

include ( CheckIncludeFileCXX )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )

set ( CXX_INCLUDES queue iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

unisim_include ( unisim/kernel/debug )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__kernel__tlm2
	DEPENDS
		unisim__kernel__debug
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/kernel/tlm2/tlm.hh
	)

message ( STATUS "Checking unisim::kernel::tlm2 - done" )

