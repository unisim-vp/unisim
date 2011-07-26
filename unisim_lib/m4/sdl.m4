## UNISIM_CHECK_SDL
## Checks if the SDL library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SDL], [
    # Check if SDL path has been overloaded
    AC_ARG_WITH(sdl,
	AS_HELP_STRING([--with-sdl=<path>], [sdl library to use (will be completed with /include and /lib)]))

    if test "x$with_sdl" != "x"; then
		AC_MSG_NOTICE([using SDL at $with_sdl])
		AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no, $with_sdl/bin)
		if test "x$sdl_config_installed" != "xyes"; then
				AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
		fi
		AC_MSG_NOTICE([sdl-config found])
		sdl_cflags="`$with_sdl/bin/sdl-config --cflags`"
		sdl_libs="`$with_sdl/bin/sdl-config --libs`"
		AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
		CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
		LIBS=${LIBS}" ${sdl_libs}"
    else
		if test $host = $build; then
			# We are not crosscompiling so we can execute sdl-config in the PATH on the 'build' machine
			AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no)
			if test "x$sdl_config_installed" != "xyes"; then
					AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
			fi
			AC_MSG_NOTICE([sdl-config found])
			sdl_cflags="`sdl-config --cflags`"
			sdl_libs="`sdl-config --libs`"
			AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LIBS=${LIBS}" ${sdl_libs}"
		else
			# We are crosscompiling and we can't use sdl-config of the 'build' machine.
			sdl_cflags="-I/usr/include"
			sdl_libs="-L/usr/lib"
			AC_MSG_NOTICE([Trying with compiler option ${sdl_cflags} ${sdl_libs} to compile and link with SDL.])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LIBS=${LIBS}" ${sdl_libs}"
		fi
	fi

    # Check for the main SDL header
    AC_CHECK_HEADER(SDL.h, broken_sdl=no, broken_sdl=yes)
	
    # Check for SDL functions
    UNISIM_CHECK_LIB(SDL, SDL_WaitThread,
    UNISIM_CHECK_LIB(SDL, SDL_FreeSurface,
    UNISIM_CHECK_LIB(SDL, SDL_DestroyMutex,
    UNISIM_CHECK_LIB(SDL, SDL_Quit,
    UNISIM_CHECK_LIB(SDL, SDL_Init,
    UNISIM_CHECK_LIB(SDL, SDL_CreateMutex,
    UNISIM_CHECK_LIB(SDL, SDL_CreateThread,
    UNISIM_CHECK_LIB(SDL, SDL_mutexP,
    UNISIM_CHECK_LIB(SDL, SDL_mutexV,
    UNISIM_CHECK_LIB(SDL, SDL_GetKeyName,
    UNISIM_CHECK_LIB(SDL, SDL_Delay,
    UNISIM_CHECK_LIB(SDL, SDL_PollEvent,
    UNISIM_CHECK_LIB(SDL, SDL_SetVideoMode,
    UNISIM_CHECK_LIB(SDL, SDL_WM_SetCaption,
    UNISIM_CHECK_LIB(SDL, SDL_CreateRGBSurface,
    UNISIM_CHECK_LIB(SDL, SDL_LockSurface,
    UNISIM_CHECK_LIB(SDL, SDL_UnlockSurface,
    UNISIM_CHECK_LIB(SDL, SDL_UpperBlit,
    UNISIM_CHECK_LIB(SDL, SDL_UpdateRect,
    UNISIM_CHECK_LIB(SDL, SDL_SaveBMP_RW, broken_sdl=no,
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
		AC_MSG_NOTICE([SDL not found. No video frame buffer or input devices will be available.])
    else
		AC_DEFINE([HAVE_SDL], [], [Whether SDL is available])
    fi
])
