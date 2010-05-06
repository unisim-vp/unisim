message ( STATUS "Checking unisim::kernel::service" )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/kernel )

add_custom_target ( unisim__kernel__service
	DEPENDS
		unisim__kernel )

message ( STATUS "Checking unisim::kernel::service - done" )

