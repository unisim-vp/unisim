## UNISIM_CHECK_GMP
## Checks if the gmp library is installed
#####################################################
AC_DEFUN([UNISIM_CHECK_GMP], [
  # Check if gmp path has been overloaded
  AC_ARG_WITH(gmp,
  AS_HELP_STRING([--with-gmp=<path>], [gmp library to use (will be completed with /include and /lib)]))
  if test "x$with_gmp" != "x"; then
    AC_MSG_NOTICE([using gmp at $with_gmp])
    LDFLAGS=${LDFLAGS}" -L$with_gmp/lib"
    CPPFLAGS=${CPPFLAGS}" -I$with_gmp/include"
  fi

  # Check for gmp.h
  AC_CHECK_HEADER(gmp.h,, gmp_not_found=yes)

  # Check for functions in gmp
  UNISIM_CHECK_LIB(gmp, __gmpz_init,,gmp_not_found=yes)

  if test "$gmp_not_found" == "yes"; then
    AC_MSG_ERROR([gmp not found])
  else
    LIBS="-lgmp ${LIBS}"
    AC_DEFINE([HAVE_GMP], [], [Whether gmp is available])
  fi
])
