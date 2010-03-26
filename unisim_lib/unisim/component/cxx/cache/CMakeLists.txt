message ( STATUS "Checking unisim::component::cxx::cache" )

include ( CheckIncludeFile )

set ( C_INCLUDES inttypes string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__component__cxx__cache
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/cache/cache.hh
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/cache/cache.tcc
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/cache/cache_interface.hh
	)

message ( STATUS "Checking unisim::component::cxx::cache - done " )

