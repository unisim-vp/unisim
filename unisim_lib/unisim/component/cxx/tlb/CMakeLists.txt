message ( STATUS "Checking unisim::component::cxx::tlb" )

include ( CheckIncludeFile )

set ( C_INCLUDES inttypes string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__component__cxx__tlb
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/tlb/tlb.hh
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/tlb/tlb.tcc
	)

message ( STATUS "Checking unisim::component::cxx::tlb - done " )
