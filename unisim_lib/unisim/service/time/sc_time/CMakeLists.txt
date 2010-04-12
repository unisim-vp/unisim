message ( STATUS "Checking unisim::service::time::sc_time" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( sc_time_subdirs
	unisim/service/interfaces/time
	unisim/kernel/service
	)

foreach ( dir ${sc_time_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( 
	${CMAKE_SOURCE_DIR} 
	${OSCI_SYSTEMC_INCLUDE_DIR} 
	)

add_library ( unisim__service__time__sc_time
		${CMAKE_SOURCE_DIR}/unisim/service/time/sc_time/time.hh
		${CMAKE_SOURCE_DIR}/unisim/service/time/sc_time/time.cc
	)

target_link_libraries ( unisim__service__time__sc_time
	${OSCI_SYSTEMC_LIB}
	unisim__kernel__service
	)

add_dependencies ( unisim__service__time__sc_time
	unisim__service__interfaces__time
	unisim__kernel__service
	)

message ( STATUS "Checking unisim::service::time::sc_time - done" )



