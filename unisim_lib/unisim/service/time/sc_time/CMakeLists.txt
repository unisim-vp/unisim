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

include_directories ( ${CMAKE_SOURCE_DIR} )

add_library ( unisim__service__time__sc_time
	STATIC
		${CMAKE_SOURCE_DIR}/unisim/service/time/sc_time/time.hh
		${CMAKE_SOURCE_DIR}/unisim/service/time/sc_time/time.cc
	)

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/service/time/sc_time/time.cc
	PROPERTIES
		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
	)

message ( STATUS "Checking unisim::service::time::sc_time - done" )



