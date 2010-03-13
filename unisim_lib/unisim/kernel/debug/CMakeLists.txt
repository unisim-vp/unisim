message ( STATUS "Checking unisim::kernel::debug" )

include ( CheckIncludeFileCXX )

set ( CXX_INCLUDES iosfwd iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES string stdlib )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )

set ( kernel_debug_srcs
	${CMAKE_SOURCE_DIR}/unisim/kernel/debug/debug.hh
	${CMAKE_SOURCE_DIR}/unisim/kernel/debug/debug.cc
	)
add_library ( unisim__kernel__debug
	${kernel_debug_srcs}
	)

message ( STATUS "Checking unisim::kernel::debug - done" )

