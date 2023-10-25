## UNISIM_CHECK_SOFTFLOAT_EMU
## Checks if an softfloat-emu library is installed
## Does not take parameters
## unisim_cv_softfloat_emu is set to either 'yes' or 'no'
## HAVE_SOFTFLOAT_EMU is defined to 1 if found
#####################################################
AC_DEFUN([UNISIM_CHECK_SOFTFLOAT_EMU], [
    # Check if softfloat-emu library path has been overloaded
    AC_ARG_WITH(softfloat-emu,
	AS_HELP_STRING([--with-softfloat-emu=<path>], [Overrides search path to softfloat-emu library]))
    if test -n "${with_softfloat_emu}"; then
		AC_MSG_NOTICE([using softfloat-emu library at $with_softfloat_emu])
		
		sep=':'

		case "${build}" in
			*mingw*)
				sep=';'    # separator on a windows build machine is ';', not ':'
				;;
		esac
		export PKG_CONFIG_PATH="${with_softfloat_emu}/lib/pkgconfig${sep}${with_softfloat_emu}/lib64/pkgconfig${sep}${PKG_CONFIG_PATH}"
    fi
    
	PKG_CHECK_MODULES(SOFTFLOAT_EMU, softfloat-emu, unisim_cv_softfloat_emu=yes, unisim_cv_softfloat_emu=no)

	if test "x$unisim_cv_softfloat_emu" = "xyes"; then
		AC_MSG_NOTICE([softfloat-emu library found])
		CXXFLAGS="${CXXFLAGS} ${SOFTFLOAT_EMU_CFLAGS}"
		LIBS="${LIBS} ${SOFTFLOAT_EMU_LIBS}"
		AC_DEFINE([HAVE_SOFTFLOAT_EMU], [1], [Define to 1 if you have an softfloat-emu library])
	else
		AC_MSG_NOTICE([softfloat-emu library not found])
	fi
])
