AC_DEFUN([UNISIM_CHECK_GET_EXECUTABLE_PATH], [
	case ${host} in
		*linux*)  # other hosts: linux and other unixes
			AC_CHECK_HEADER([unistd.h],, AC_MSG_ERROR([unistd.h is missing.]))
			UNISIM_CHECK_LIB(c, readlink,, AC_MSG_ERROR([readlink support is missing.]))
			;;
		*mingw32*)  # win32 host
			AC_CHECK_HEADER([windows.h],, AC_MSG_ERROR([windows.h is missing.]))
			;;
		*darwin*)  # darwin host
			AC_CHECK_HEADER([mach-o/dyld.h],, AC_MSG_ERROR([mach-o/dyld.h is missing.]))
			UNISIM_CHECK_LIB(c, _NSGetExecutablePath,, AC_MSG_ERROR([_NSGetExecutablePath support is missing.]))
			;;
	esac
])