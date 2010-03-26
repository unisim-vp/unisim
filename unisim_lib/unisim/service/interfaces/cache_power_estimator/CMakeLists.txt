message ( STATUS "Checking unisim::service::interfaces::cache_power_estimator" )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__cache_power_estimator
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/cache_power_estimator.hh
	)

message ( STATUS "Checking unisim::service::interfaces::cache_power_estimator- done" )

