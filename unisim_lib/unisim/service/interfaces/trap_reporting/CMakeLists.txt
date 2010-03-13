message ( STATUS "Checking unisim::service::interfaces::trap_reporting" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/kernel/service )

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/service/interfaces/trap_reporting.hh
	PROPERTIES
		HEADER_FILE_ONLY TRUE )

add_custom_target ( unisim__service__interfaces__trap_reporting
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/trap_reporting.hh
	)

message ( STATUS "Checking unisim::service::interfaces::trap_reporting - done" )
