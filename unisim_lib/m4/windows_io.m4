AC_DEFUN([UNISIM_CHECK_WINDOWS_IO], [

	case $host in
		*mingw*)  # windows host
			AC_CHECK_HEADER([io.h],, AC_MSG_ERROR([io.h is missing.]))
			;;
	esac

])
