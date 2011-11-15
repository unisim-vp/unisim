cmake_minimum_required ( VERSION 2.8 )
project ( unisim_lib_unisim_util_os_linux )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
# include ( ${CMAKE_SOURCE_DIR}/../../../../cmake/UnisimInclude.cmake ) 

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS *.cc *.tcc *.hh 
  CACHE
  STRING "Extensions of the source files" )

set ( CXX_INCLUDES map string vector iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
  check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
  if ( NOT HAVE_${CXX_INCLUDE} )
    message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
  endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES string stdlib inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
  check_include_file( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
  if ( NOT HAVE_${C_INCLUDE}_H )
    message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}\"." )
  endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( UNISIM_LIB_PATH
  ${CMAKE_SOURCE_DIR}/../../../.. )
include_directories ( ${UNISIM_LIB_PATH} ${CMAKE_BINARY_DIR} )

set ( ENDIAN_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/endian )
set ( ENDIAN_SRCS
  ${ENDIAN_SRCS_PATH}/endian.cc )
set ( BLOB_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/debug/blob )
set ( BLOB_SRCS
  ${BLOB_SRCS_PATH}/blob32.cc
  ${BLOB_SRCS_PATH}/blob64.cc
  ${BLOB_SRCS_PATH}/section32.cc
  ${BLOB_SRCS_PATH}/section64.cc
  ${BLOB_SRCS_PATH}/segment32.cc
  ${BLOB_SRCS_PATH}/segment64.cc )
set ( LINUX_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/os/linux )
set ( LINUX_SRCS
  ${LINUX_SRCS_PATH}/linux.cc )
add_library ( unisim_util_os_linux
  ${LINUX_SRCS}
  ${ENDIAN_SRCS}
  ${BLOB_SRCS} )
