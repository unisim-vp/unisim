message ( STATUS "Checking unisim::kernel::tlm" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )

set ( CXX_INCLUDES list vector stack string )
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

set ( kernel__tlm_subdirs
	unisim/kernel/service
	unisim/util/garbage_collector
	)

foreach ( dir ${kernel__tlm_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( 
	${CMAKE_SOURCE_DIR} 
	${OSCI_SYSTEMC_INCLUDE_DIR}
	)

add_library ( unisim__kernel__tlm
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/kernel/tlm/tlm.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/tlm/tlm.cc
	)

add_dependencies ( unisim__kernel__tlm
	unisim__kernel__service
	unisim__util__garbage_collector
	)

target_link_libraries ( unisim__kernel__tlm
	${OSCI_SYSTEMC_LIBRARIES}
	)

message ( STATUS "Checking unisim::kernel::tlm - done" )


