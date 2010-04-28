message ( STATUS "Checking unisim::kernel::logger" )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/kernel )

add_custom_target ( unisim__kernel__logger
	DEPENDS
		unisim__kernel )

message ( STATUS "Checking unisim::kernel::logger - done" )
