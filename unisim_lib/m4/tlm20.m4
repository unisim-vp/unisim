## UNISIM_CHECK_TLM20
## Checks if the OSCI TLM2.0 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_TLM20], [
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
    AC_ARG_WITH(tlm20,
	AS_HELP_STRING([--with-tlm20=<path>], [Overrides search path to SystemC TLM 2.0 library]))
    if test -n "$with_tlm20"; then
		AC_MSG_NOTICE([using SystemC TLM 2.0 at $with_tlm20])
		export PKG_CONFIG_PATH="${with_tlm20}/lib-${SYSTEMC_TARGET_ARCH}/pkgconfig"
    fi

	PKG_CHECK_MODULES(TLM20, tlm, AC_MSG_NOTICE([SystemC TLM 2.0 found]), AC_MSG_ERROR([SystemC TLM 2.0 not found]))
	
	CXXFLAGS="${CXXFLAGS} ${TLM20_CFLAGS}"
	LIBS="${LIBS} ${TLM20_LIBS}"
])
