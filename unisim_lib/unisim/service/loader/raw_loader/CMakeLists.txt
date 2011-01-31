message ( STATUS "Checking unisim::service::loader::raw_loader" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES fstream string )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( raw_loader_subdirs
	unisim/kernel
	unisim/service/interfaces/memory
	)
foreach ( dir ${raw_loader_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__loader__raw_loader
		${CMAKE_SOURCE_DIR}/unisim/service/loader/raw_loader/raw_loader.hh
		${CMAKE_SOURCE_DIR}/unisim/service/loader/raw_loader/raw_loader.cc
	)

target_link_libraries ( unisim__service__loader__raw_loader
	unisim__kernel
	)

add_dependencies ( unisim__service__loader__raw_loader
	unisim__kernel
	unisim__service__interfaces__memory
	)

message ( STATUS "Checking unisim::service::loader::raw_loader - done" )


