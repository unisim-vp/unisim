## UNISIM_CHECK_BOOST
## Checks if the boost library is installed
## Does not take parameters
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
## Does not take parameters
####################################################
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
## Does not take parameters
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
    AC_CHECK_LIB(ncurses, wgetch, broken_ncurses=no, broken_ncurses=yes)

    if test "$broken_ncurses" == "yes"; then
	AC_MSG_ERROR([installed ncurses library is broken.])
    else
	LIBS+=" -lncurses"
    fi
])

## UNISIM_CHECK_READLINE
## Checks if the realine library is installed
## Does not take parameters
#####################################################
AC_DEFUN_ONCE([UNISIM_CHECK_READLINE], [
    # Check if readline path has been overloaded
    AC_ARG_WITH(readline,
	AS_HELP_STRING([--with-readline=<path>], [readline library to use (will be completed with /include and /lib)]))
    if test "x$with_readline" != "x"; then
	AC_MSG_NOTICE([using readline at $with_readline])
	LDFLAGS+=" -L$with_readline/lib"
	CPPFLAGS+=" -I$with_readline/include"
    fi
	
    # Check for readline/readline.h
    AC_CHECK_HEADER(readline/readline.h,, AC_MSG_ERROR([readline/realine.h not found. Please install the readline development library. Use --with-readline=<path> to overload default includes search path.]))
	
    # Check for functions readline and add_history in libreadline.a
    AC_CHECK_LIB(readline, readline,
    AC_CHECK_LIB(readline, add_history, broken_readline=no,
    broken_readline=yes),
    broken_readline=yes)
	
    if test "$broken_readline" == "yes"; then
	AC_MSG_ERROR([installed readline library is broken.])
    else
	LIBS+=" -lreadline"
    fi
])

## UNISIM_CHECK_SDL
## Checks if the SDL library is installed
## Does not take parameters
#####################################################
AC_DEFUN_ONCE([UNISIM_CHECK_SDL], [
    # Check if SDL path has been overloaded
    AC_ARG_WITH(sdl,
	AS_HELP_STRING([--with-sdl=<path>], [sdl library to use (will be completed with /include and /lib)]))
    if test "x$with_sdl" != "x"; then
	AC_MSG_NOTICE([using SDL at $with_sdl])
	LDFLAGS+=" -L$with_sdl/lib"
	CPPFLAGS+=" -I$with_sdl/include"
    fi

    # Check for the main SDL header
    AC_CHECK_HEADER(SDL/SDL.h,, AC_MSG_ERROR([SDL/SDL.h not found. Please install the SDL development library (version >= 1.2.0).]))
	
    # Check for SDL functions
    AC_CHECK_LIB(SDL, SDL_WaitThread,
    AC_CHECK_LIB(SDL, SDL_FreeSurface,
    AC_CHECK_LIB(SDL, SDL_DestroyMutex,
    AC_CHECK_LIB(SDL, SDL_Quit,
    AC_CHECK_LIB(SDL, SDL_Init,
    AC_CHECK_LIB(SDL, SDL_CreateMutex,
    AC_CHECK_LIB(SDL, SDL_CreateThread,
    AC_CHECK_LIB(SDL, SDL_mutexP,
    AC_CHECK_LIB(SDL, SDL_mutexV,
    AC_CHECK_LIB(SDL, SDL_GetKeyName,
    AC_CHECK_LIB(SDL, SDL_Delay,
    AC_CHECK_LIB(SDL, SDL_PollEvent,
    AC_CHECK_LIB(SDL, SDL_SetVideoMode,
    AC_CHECK_LIB(SDL, SDL_WM_SetCaption,
    AC_CHECK_LIB(SDL, SDL_CreateRGBSurface,
    AC_CHECK_LIB(SDL, SDL_LockSurface,
    AC_CHECK_LIB(SDL, SDL_UnlockSurface,
    AC_CHECK_LIB(SDL, SDL_UpperBlit,
    AC_CHECK_LIB(SDL, SDL_UpdateRect,
    AC_CHECK_LIB(SDL, SDL_SaveBMP_RW, broken_sdl=no,
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes)

    if test "$broken_sdl" == "yes"; then
	AC_MSG_ERROR([installed SDL Library is broken.])
    else
	LIBS+=" -lSDL"
    fi
])

## UNISIM_CHECK_LIBXML2
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN_ONCE([UNISIM_CHECK_LIBXML2], [
    # Check if libxml2 path has been overloaded
    AC_ARG_WITH(libxml2,
	AS_HELP_STRING([--with-libxml2=<path>], [path to the libxml2 library]))
	
    if test "x$with_libxml2" != "x"; then
		AC_MSG_NOTICE([using libxml2 at $with_libxml2])
		LDFLAGS+=" -L$with_libxml2/lib"
		CPPFLAGS+=" -I$with_libxml2/include/libxml2"
    fi
	
    # Check for some libxml2 headers
    AC_CHECK_HEADER(libxml/encoding.h, broken_incxml2=no, broken_incxml2=yes)
    AC_CHECK_HEADER(libxml/xmlwriter.h, , broken_incxml2=yes)
	
    if test "$broken_incxml2" == "yes"; then
		AC_MSG_ERROR([libxml2 includes not found (/libxml2/libxml/*.hh). Please use --with-libxml2=<path>])
    else
		LIBS+=" -lxml2"
    fi

    # Check for SDL functions
    AC_CHECK_LIB(xml2, xmlNewTextWriterFilename,
    AC_CHECK_LIB(xml2, xmlTextWriterSetIndent,
    AC_CHECK_LIB(xml2, xmlTextWriterStartDocument,
    AC_CHECK_LIB(xml2, xmlTextWriterStartElement, 
    AC_CHECK_LIB(xml2, xmlTextWriterEndElement, 
    AC_CHECK_LIB(xml2, xmlFreeTextWriter, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteAttribute, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteFormatString, broken_libxml2=no,
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes)
	
    if test "$broken_libxml2" == "yes"; then
		AC_MSG_ERROR([installed xml2 Library is broken.])
    else
		LIBS+=" -lxml2"
    fi
])

## UNISIM_CHECK_SYSTEMC
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN_ONCE([UNISIM_CHECK_SYSTEMC], [
    # Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
    CXX_COMP=`basename $CXX`
    case "$host" in
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
	*linux*)
    	    SYSTEMC_TARGET_ARCH="linux"
    	    ;;
        *cygwin*)
    	    SYSTEMC_TARGET_ARCH="cygwin"
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
	*mingw32*)
    	    SYSTEMC_TARGET_ARCH="mingw32"
    	    ;;
    esac

    # Check if SystemC path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [systemc library to use (will be completed with /include and /lib-linux)]))
    if test "x$with_systemc" != "x"; then
	AC_MSG_NOTICE([using SystemC at $with_systemc])
	LDFLAGS+=" -L$with_systemc/lib-$SYSTEMC_TARGET_ARCH"
	CPPFLAGS+=" -I$with_systemc/include"
    fi

    # Check for systemc.h
    AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default includes search path.]))

    # Check for function 'main' in libsystemc.a
    AC_CHECK_LIB(systemc,main,broken_systemc=no,broken_system=yes)

    if test "$broken_systemc" == "yes"; then
	AC_MSG_ERROR([installed SystemC is broken. Please install the SystemC library (version > 2.1). Use --with-systemc=<path> to overload default includes search path.])
    else
	LIBS+=" -lsystemc"
    fi
])

## UNISIM_CONFIG_SUBDIR
## 1st param: friendly name (like unisim-bla-bla)
## 2nd param: subdirectory name to enter if enabled (define COND_<subdirectory> automake macro)
#####################################################
AC_DEFUN([UNISIM_CONFIG_SUBDIR], [
AC_ARG_ENABLE($1,
	AS_HELP_STRING([--enable-$1], [enable compiling $1 (default)])
	AS_HELP_STRING([--disable-$1], [disable compiling $1]),
	[case "${enableval}" in
	yes) unisim_enabled=true ;;
	no) unisim_enabled=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-$1) ;;
	esac], [unisim_enabled=true])
	AM_CONDITIONAL(COND_$2, test $unisim_enabled == true)
	if test $unisim_enabled == true; then
		AC_CONFIG_SUBDIRS([$2])
	fi
])

## UNISIM_LINK
AC_DEFUN([UNISIM_LINK], [
	LIBS+=" $1/lib$2.a"
])
