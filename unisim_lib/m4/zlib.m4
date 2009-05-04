## UNISIM_CHECK_ZLIB
## Checks if the zlib library is installed
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_ZLIB], [
    # Check if zlib path has been overloaded
    AC_ARG_WITH(zlib,
	AS_HELP_STRING([--with-zlib=<path>], [zlib library to use (will be completed with /include and /lib)]))
    if test "x$with_zlib" != "x"; then
	AC_MSG_NOTICE([using zlib at $with_zlib])
	LDFLAGS=${LDFLAGS}" -L$with_zlib/lib"
	CPPFLAGS=${CPPFLAGS}" -I$with_zlib/include"
    fi
	
    # Check for zlib.h
    AC_CHECK_HEADER(zlib.h,, AC_MSG_ERROR([zlib.h not found. Please install the zlib development library (version >= 1.2.3).]))

    # Check for functions gzopen, gzclose and gzprintf in libz.a
    AC_CHECK_LIB(z, gzopen,
    AC_CHECK_LIB(z, gzdopen,
    AC_CHECK_LIB(z, gzread,
    AC_CHECK_LIB(z, gzrewind,
    AC_CHECK_LIB(z, deflateEnd,
    AC_CHECK_LIB(z, gzclose,
    AC_CHECK_LIB(z, gzwrite,
    AC_CHECK_LIB(z, deflate,
    AC_CHECK_LIB(z, crc32,
    AC_CHECK_LIB(z, deflateInit2_,
    AC_CHECK_LIB(z, inflateEnd,
    AC_CHECK_LIB(z, inflate,
    AC_CHECK_LIB(z, inflateInit2_,
    AC_CHECK_LIB(z, gzprintf, broken_zlib=no,
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes)

    if test "$broken_zlib" == "yes"; then
	AC_MSG_ERROR([installed zlib is broken.])
    else
	LIBS="-lz ${LIBS}"
    fi
])
