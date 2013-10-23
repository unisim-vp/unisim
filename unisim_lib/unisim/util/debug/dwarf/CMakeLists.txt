message ( STATUS "Checking unisim::util::debug::dwarf" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES iostream sstream limits iosfwd vector map string )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( dwarf_subdirs 
	unisim/util/debug
	unisim/util/endian
	unisim/util/arithmetic
	unisim/util/debug/blob
	)

foreach ( dir ${dwarf_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__util__debug__dwarf
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/abbrev.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/abbrev.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/addr_range.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/addr_range.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/attr.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/attr.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/attr.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/call_frame_prog.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/call_frame_prog.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/call_frame_vm.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/call_frame_vm.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/cie.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/cie.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/class.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/class.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/cu.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/cu.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/die.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/die.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/dwarf.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/dwarf.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/dwarf32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/dwarf64.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/encoding.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/encoding.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/expr_vm.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/expr_vm.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/fde.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/fde.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/filename.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/filename.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/fmt.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/fwd.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/leb128.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/leb128.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/loc.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/loc.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/macinfo.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/macinfo.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/ml.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/ml.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/pub.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/pub.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/range.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/range.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/stmt_prog.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/stmt_prog.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/stmt_vm.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/dwarf/stmt_vm.tcc
	)

target_link_libraries ( unisim__util__debug__dwarf
	unisim__util__endian
	unisim__util__debug
	unisim__util__debug__blob
	)

add_dependencies ( unisim__util__debug__dwarf
	unisim__util__endian
	unisim__util__debug
	unisim__util__arithmetic
	unisim__util__debug__blob
	)
	
message ( STATUS "Checking unisim::service::debug::dwarf - done" )
