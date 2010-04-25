message ( STATUS "Checking unisim::service::loader::linux_loader" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES stdlib )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( linux_loader_subdirs
	unisim/kernel
	unisim/service/interfaces/memory
	unisim/service/interfaces/loader
	unisim/util/endian
	)
foreach ( dir ${linux_loader_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__loader__linux_loader
		${CMAKE_SOURCE_DIR}/unisim/service/loader/linux_loader/linux_loader.hh
		${CMAKE_SOURCE_DIR}/unisim/service/loader/linux_loader/linux_loader.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/linux_loader/linux32_loader.cc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/linux_loader/linux64_loader.cc
	)

target_link_libraries ( unisim__service__loader__linux_loader
	unisim__kernel
	unisim__util__endian
	)

add_dependencies ( unisim__service__loader__linux_loader
	unisim__service__interfaces__memory
	unisim__service__interfaces__loader
	)
message ( STATUS "Checking unisim::service::loader::linux_loader - done" )

