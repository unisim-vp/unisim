## UNISIM_CHECK_SYSTEMC
## Checks if the systemc library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
	# Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
	CXX_COMP=`basename $CXX`
	case "$host" in
		x386-apple-* | i386-apple-*)
			SYSTEMC_TARGET_ARCH="macosx386"
			;;
		powerpc-apple-macosx*)
			SYSTEMC_TARGET_ARCH="macosx"
			;;
		sparc-sun-solaris*)
			case "$CXX_COMP" in
				CC)
				SYSTEMC_TARGET_ARCH="sparcOS5"
				;;
				c++ | g++)
				SYSTEMC_TARGET_ARCH="gccsparcOS5"
				;;
			esac
			;;
		x86_64*linux*)
			SYSTEMC_TARGET_ARCH="linux64"
			;;
		*86*linux*)
			SYSTEMC_TARGET_ARCH="linux"
			;;
		*arm*linux*)
			SYSTEMC_TARGET_ARCH="linux-arm"
			;;
		powerpc*linux*)
			SYSTEMC_TARGET_ARCH="linux-powerpc"
			;;
		amd64*freebsd* | x86_64*freebsd*)
			SYSTEMC_TARGET_ARCH="freebsd64"
			;;
		*freebsd*)
			SYSTEMC_TARGET_ARCH="freebsd"
			;;
		*cygwin*)
			SYSTEMC_TARGET_ARCH="cygwin"
			;;
		amd64*mingw* | x86_64*mingw*)
			SYSTEMC_TARGET_ARCH="mingw64"
			;;
		*mingw*)
			SYSTEMC_TARGET_ARCH="mingw"
			;;
		*hpux11*)
			case "$CXX_COMP" in
			aCC)
				SYSTEMC_TARGET_ARCH="hpux11"
				;;
			c++ | g++)
				SYSTEMC_TARGET_ARCH="gcchpux11"
				;;
			esac
			;;
	esac

    # Check if SystemC path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [Overrides search path to SystemC library]))
    if test -n "$with_systemc"; then
		AC_MSG_NOTICE([using SystemC at $with_systemc])
		
		sep=':'

		case "${build}" in
			*mingw*)
				sep=';'
				;;
		esac
		export PKG_CONFIG_PATH="${with_systemc}/lib-${SYSTEMC_TARGET_ARCH}/pkgconfig${sep}${with_systemc}/lib/pkgconfig${sep}${PKG_CONFIG_PATH}"
    fi

	PKG_CHECK_MODULES(SystemC, systemc, AC_MSG_NOTICE([SystemC found]), AC_MSG_ERROR([SystemC not found]))
	
	CXXFLAGS="${CXXFLAGS} ${SystemC_CFLAGS}"
	LIBS="${LIBS} ${SystemC_LIBS}"
])
