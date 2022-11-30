## UNISIM_CHECK_HLA_RTI1516E
## Checks if an HLA RTI 1516e library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_HLA_RTI1516E], [
    # Check if HLA RTI 1516e library path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-hla-rti1516e=<path>], [Overrides search path to HLA RTI 1516e library (e.g. OpenRTI)]))
    if test -n "${with_hla_rti1516e}"; then
		AC_MSG_NOTICE([using HLA RTI 1516e library at $with_hla_rti1516e])
		
		sep=':'

		case "${build}" in
			*mingw*)
				sep=';'    # separator on a windows build machine is ';', not ':'
				;;
		esac
		export PKG_CONFIG_PATH="${with_hla_rti1516e}/lib/pkgconfig${sep}${with_hla_rti1516e}/lib64/pkgconfig${sep}${PKG_CONFIG_PATH}"
    fi
    
	PKG_CHECK_MODULES(HLA_RTI1516E, hla-rti1516e, found_with_hla_rti1516e=yes, found_with_hla_rti1516e=no)

	if test "x$found_with_hla_rti1516e" = "xyes"; then
		AC_MSG_NOTICE([HLA RTI 1516e library found])
		CXXFLAGS="${CXXFLAGS} ${HLA_RTI1516E_CFLAGS}"
		LIBS="${LIBS} ${HLA_RTI1516E_LIBS}"
		AC_DEFINE([HAVE_HLA_RTI1516E], [1], [Define to 1 if you have an HLA RTI 1516e library])
	else
		AC_MSG_NOTICE([HLA RTI 1516e library not found (optional). Please install OpenRTI (version >= 0.9.0) or equivalent. Use --with-hla-rti1516e=<path> to overload default search path.])
	fi
])
