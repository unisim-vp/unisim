message ( STATUS "Checking unisim::component::tlm2::chipset::arm926ejs_pxp" )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( tlm2_pxp_subdirs
	unisim/kernel
	)

foreach ( dir ${tlm2_pxp_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	)

set ( tlm2_pxp_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.cc
	)

add_library ( unisim__component__tlm2__chipset__arm926ejs_pxp
	${tlm2_pxp_srcs}
	)

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.cc
	PROPERTIES
		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
	)

target_link_libraries ( unisim__component__tlm2__chipset__arm926ejs_pxp
	${OSCI_SYSTEMC_LIBRARIES} 
	unisim__kernel
	)

add_dependencies ( unisim__component__tlm2__chipset__arm926ejs_pxp
	unisim__kernel
	)

message ( STATUS "Checking unisim::component::tlm2::chipset::arm926ejs_pxp - done" )
