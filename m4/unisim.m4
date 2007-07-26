## UNISIM_CHECK_BOOST
## Checks if the boost library is installed
## Does not need parameters
#####################################################
AC_DEFUN_ONCE([UNISIM_CHECK_BOOST], [
# Check if boost path has been overloaded
AC_ARG_WITH(boost,
	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
if test "x$with_boost" != "x"; then
	AC_MSG_NOTICE([using boost at $with_boost])
	LIBS+=" -lboost"
	LDFLAGS+=" -L$with_boost/lib"
	CPPFLAGS+=" -I$with_boost"
fi
	
# Check for some boost graph headers
AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp boost/graph/topological_sort.hpp boost/graph/visitors.hpp],,\
	AC_MSG_ERROR([boost graph headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
])


## UNISIM_CHECK_ZLIB
## Checks if the zlib library is installed
## Does not need parameters
#####################################################
AC_DEFUN_ONCE([UNISIM_CHECK_ZLIB], [
# Check if zlib path has been overloaded
AC_ARG_WITH(zlib,
	AS_HELP_STRING([--with-zlib=<path>], [zlib library to use (will be completed with /include and /lib)]))
if test "x$with_zlib" != "x"; then
	AC_MSG_NOTICE([using zlib at $with_zlib])
	LDFLAGS+=" -L$with_zlib/lib"
	CPPFLAGS+=" -I$with_zlib/include"
fi
	
# Check for zlib.h
AC_CHECK_HEADER(zlib.h,, AC_MSG_ERROR([zlib.h not found. Please install the zlib development library. Use --with-zlib=<path> to overload default includes search path.]))

# Check for functions gzopen, gzclose and gzprintf in libz.a
AC_CHECK_LIB(z, gzopen,
AC_CHECK_LIB(z, gzclose,
AC_CHECK_LIB(z, gzprintf, broken_zlib=no,
broken_zlib=yes),
broken_zlib=yes),
broken_zlib=yes)

if test "$broken_zlib" == "yes"; then
	AC_MSG_ERROR([installed zlib is broken.])
else
	LIBS+=" -lz"
fi
])

## UNISIM_CHECK_CURSES
## Checks if the curses library is installed
#####################################################
AC_DEFUN_ONCE([UNISIM_CHECK_CURSES], [
# Check if curses path has been overloaded
AC_ARG_WITH(ncurses,
	AS_HELP_STRING([--with-ncurses=<path>], [ncurses library to use (will be completed with /include and /lib)]))
if test "x$with_ncurses" != "x"; then
	AC_MSG_NOTICE([using ncurses at $with_ncurses])
	LDFLAGS+=" -L$with_ncurses/lib"
	CPPFLAGS+=" -I$with_ncurses/include"
fi
	
# Check for ncurses.h
AC_CHECK_HEADER(ncurses.h,, AC_MSG_ERROR([ncurses.h not found. Please install the readline development library. Use --with-ncurses=<path> to overload default includes search path.]))

# Check for functions wgetch in libncurses.a
AC_CHECK_LIB(ncurses, wgetch,
broken_ncurses=no, broken_ncurses=yes)

if test "$broken_ncurses" == "yes"; then
	AC_MSG_ERROR([installed ncurses library is broken.])
else
	LIBS+=" -lncurses"
fi
])

