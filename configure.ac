dnl Process this file with autoconf to produce a configure script.

dnl Initialise autoconf and automake
AC_INIT([cw], [1.0.17], [rrt@sc3d.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

dnl Checks for programs
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_LN_S

dnl Checks for C compiler flags
AC_MSG_CHECKING([for -pedantic (gcc) compiler flag])
ORIG_CFLAGS=""
CFLAGS="-pedantic"
AC_TRY_COMPILE(,[return 0;],AC_MSG_RESULT(yes),AC_MSG_RESULT(no);CFLAGS="")
AC_MSG_CHECKING([for -Wall (gcc) compiler flag])
ORIG_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_TRY_COMPILE(,[return 0;],AC_MSG_RESULT(yes),AC_MSG_RESULT(no);CFLAGS="$ORIG_CFLAGS")

dnl Checks for functions and headers
AC_HEADER_STDC
AC_CHECK_LIB(util, openpty)
AC_CHECK_FILE("/dev/ptmx", AC_DEFINE(HAVE_DEV_PTMX, 1, Have /dev/ptmx.))
AC_CHECK_FUNCS(getpt isatty isxdigit openpty putenv regcomp setenv setproctitle setsid sigaction sscanf uname unsetenv vsnprintf waitpid)
AC_CHECK_HEADERS(ctype.h fcntl.h libgen.h libutil.h pty.h regex.h signal.h string.h strings.h stropts.h unix.h util.h sys/ioctl.h sys/stat.h sys/time.h sys/types.h sys/utsname.h sys/wait.h)

dnl Generate output
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
