AC_DEFUN([UNISIM_CHECK_WINDOWS], [

	case $host in
		*mingw*)  # windows host
			AC_CHECK_HEADER([windows.h],, AC_MSG_ERROR([windows.h is missing.]))
			;;
	esac

])
