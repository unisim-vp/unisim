message ( STATUS "Checking unisim::component::tlm2::processor::arm" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES string )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( tlm2_arm_subdirs
	unisim/kernel/service
	unisim/kernel/logger
	unisim/kernel/tlm2
	unisim/component/cxx/processor/arm
	unisim/component/tlm2/interrupt
	unisim/service/interfaces/trap_reporting
	)

foreach ( dir ${tlm2_arm_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${OSCI_SYSTEMC_INCLUDE_DIR}
	)

add_custom_target ( unisim__component__tlm2__processor__arm
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/arm.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm2/processor/arm/arm.tcc
	)

add_dependencies ( unisim__component__tlm2__processor__arm
	unisim__kernel__service
	unisim__kernel__logger
	unisim__kernel__tlm2
	unisim__component__cxx__processor__arm
	unisim__component__tlm2__interrupt
	unisim__service__interfaces__trap_reporting
	)

target_link_libraries ( unisim__component__tlm2__processor__arm
	${OSCI_SYSTEMC_LIBRARIES}
	)

message ( STATUS "Checking unisim::component::tlm2::processor::arm - done" )
