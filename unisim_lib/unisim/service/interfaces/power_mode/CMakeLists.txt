message ( STATUS "Checking unisim::service::interfaces::power_mode" )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__power_mode
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/power_mode.hh
	)

message ( STATUS "Checking unisim::service::interfaces::power_mode- done" )


