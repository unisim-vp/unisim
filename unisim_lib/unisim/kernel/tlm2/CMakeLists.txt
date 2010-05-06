message ( STATUS "Checking unisim::kernel::tlm2" )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/kernel )

add_custom_target ( unisim__kernel__tlm
	DEPENDS
		unisim__kernel )

message ( STATUS "Checking unisim::kernel::tlm2 - done" )

