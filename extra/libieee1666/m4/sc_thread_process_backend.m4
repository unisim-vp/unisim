AC_DEFUN([LIBIEEE1666_SC_THREAD_PROCESS_BACKEND], [
    # Check if pthreads are enabled for SystemC thread processes
    AC_ARG_ENABLE(pthreads,
	AS_HELP_STRING([--enable-pthreads], [use POSIX threads for SystemC thread processes]))
    if test "$enable_pthreads" = "yes"; then
		SC_THREAD_PROCESSES_USE_PTHREADS=1
		AC_MSG_NOTICE([using POSIX threads for SystemC thread processes])
		AX_PTHREAD(AC_MSG_NOTICE([POSIX thread support found.]),AC_MSG_ERROR([POSIX thread support not found.]))
    else
		SC_THREAD_PROCESSES_USE_PTHREADS=0
		AC_MSG_NOTICE([using boost coroutine for SystemC processes])
		AX_BOOST_BASE([1.53.0], AC_MSG_NOTICE([boost >= 1.53.0 found.]), AC_MSG_ERROR([boost >= 1.53.0 not found.]))
		AX_BOOST_SYSTEM
		AX_BOOST_CONTEXT
		AX_BOOST_COROUTINE
	fi
	AC_SUBST(SC_THREAD_PROCESSES_USE_PTHREADS)
])
