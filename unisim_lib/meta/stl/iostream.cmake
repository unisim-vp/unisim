function (stl_iostream)
	include (CheckIncludeFileCXX)

	check_include_file_cxx (iostream HAVE_STL_IOSTREAM)
	if (NOT ${HAVE_STL_IOSTREAM})
		message (SEND_ERROR "Could not find c++ stl library \"iostream\".")
	endif (NOT ${HAVE_STL_IOSTREAM})
endfunction (stl_iostream)

