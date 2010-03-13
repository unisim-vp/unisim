message ( STATUS "Checking unisim::service::interfaces::loader" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__loader
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/loader.hh
	)

message ( STATUS "Checking unisim::service::interfaces::loader - done" )
