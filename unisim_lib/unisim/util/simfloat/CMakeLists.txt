message ( STATUS "Checking unisim::util::simfloat" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )

set ( CXX_INCLUDES iosfwd iostream)
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

if ( WIN32 )
	set ( C_INCLUDES limits float math stdio assert fenv )
else ( WIN32 )
	set ( C_INCLUDES limits float math stdio assert )
endif ( WIN32 )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( unisim__util__simfloat_subdirs
	unisim/util/endian
	)

foreach ( dir ${unisim__util__simfloat_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
	
add_custom_target ( unisim__util__simfloat
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/util/simfloat/floating.hh
		${CMAKE_SOURCE_DIR}/unisim/util/simfloat/floating.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/simfloat/host_floating.hh
		${CMAKE_SOURCE_DIR}/unisim/util/simfloat/host_floating.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/simfloat/integer.hh
		${CMAKE_SOURCE_DIR}/unisim/util/simfloat/integer.tcc
	)

add_dependencies ( unisim__util__simfloat
	unisim__util__endian
	)

message ( STATUS "Checking unisim::util::simfloat- done" )


