message ( STATUS "Checking unisim::service::interfaces::os" )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__os
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/os.hh
	)

message ( STATUS "Checking unisim::service::interfaces::os - done" )

