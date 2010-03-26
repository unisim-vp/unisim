message ( STATUS "Checking unisim::util::garbage_collector" )

include ( CheckIncludeFile )
include ( CheckIncludeFileCXX )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES cstddef iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES stdlib string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

unisim_include ( unisim/kernel/debug )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_library ( unisim__util__garbage_collector
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/util/garbage_collector/garbage_collector.hh
		${CMAKE_SOURCE_DIR}/unisim/util/garbage_collector/garbage_collector.cc
	)

target_link_libraries ( unisim__util__garbage_collector
	unisim__kernel__debug
	)

add_dependencies ( unisim__util__garbage_collector
	unisim__kernel__debug
	)

message ( STATUS "Checking unisim::util::garbage_collector - done" )




