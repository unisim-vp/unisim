message ( STATUS "Checking unisim::component::tlm2::chipset::arm926ejs_pxp" )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( tlm2_pxp_subdirs
	unisim/kernel
	unisim/util/endian
	unisim/util/generic_peripheral_register
	)

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES iostream sstream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

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
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/uart/uart.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/uart/uart.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_int_source_identifier.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/util/time_signal_splitter.hh
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/util/time_signal_splitter.cc
	)

add_library ( unisim__component__tlm2__chipset__arm926ejs_pxp
	${tlm2_pxp_srcs}
	)

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/uart/uart.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.cc
	${CMAKE_SOURCE_DIR}/unisim/component/tlm2/chipset/arm926ejs_pxp/util/time_signal_splitter.cc
	PROPERTIES
		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
	)

target_link_libraries ( unisim__component__tlm2__chipset__arm926ejs_pxp
	${OSCI_SYSTEMC_LIBRARIES} 
	unisim__kernel
	unisim__util__endian
	unisim__util__generic_peripheral_register
	)

add_dependencies ( unisim__component__tlm2__chipset__arm926ejs_pxp
	unisim__kernel
	unisim__util__endian
	unisim__util__generic_peripheral_register
	)

message ( STATUS "Checking unisim::component::tlm2::chipset::arm926ejs_pxp - done" )
