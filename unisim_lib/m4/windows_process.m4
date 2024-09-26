AC_DEFUN([UNISIM_CHECK_WINDOWS_PROCESS], [

	case $host in
		*mingw*)  # windows host
			AC_CHECK_HEADER([process.h],, AC_MSG_ERROR([process.h is missing.]))
			;;
	esac

])
