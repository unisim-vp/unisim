AC_DEFUN([UNISIM_CHECK_LIBC_IEEE754], [

	case $host in
		*mingw*)  # windows host
			;;
		*)  # un*x hosts
			AC_CHECK_HEADER([ieee754.h],, AC_MSG_ERROR([ieee754.h is missing.]))
			;;
	esac

])
