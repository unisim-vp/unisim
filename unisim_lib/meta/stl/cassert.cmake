function (stl_cassert)
	include (CheckIncludeFileCXX)

	check_include_file_cxx (cassert HAVE_STL_CASSERT)
	if (NOT ${HAVE_STL_CASSERT})
		message (SEND_ERROR "Could not find c++ stl library \"cassert\".")
	endif (NOT ${HAVE_STL_CASSERT})
endfunction (stl_cassert)
