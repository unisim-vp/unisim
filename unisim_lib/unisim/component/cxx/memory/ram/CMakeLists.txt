message ( STATUS "Checking unisim::component::cxx::memory::ram" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

unisim_include ( unisim/kernel/service )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__component__cxx__memory__ram
		memory.hh
		memory.tcc
		memory_32.cc
		memory_64.cc
	)

message ( STATUS "Checking unisim::component::cxx::memory::ram - done" )
