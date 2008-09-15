# Process this file with autoconf to produce a configure script.
AC_INIT(bestfit, 0.2.0)
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
AC_CONFIG_SRCDIR([src/bestfit.h])

# Properties for this package
AC_DEFINE(BUG_EMAIL, ["oskar@osk.mine.nu"])
AC_DEFINE(AUTHORS, ["Oskar Liljeblad <oskar@osk.mine.nu>"])
AC_DEFINE(COPYRIGHT, ["2001 Oskar Liljeblad"])

# Stuff for the config.h.in file
AM_CONFIG_HEADER(config.h)
AH_TEMPLATE([BUG_EMAIL], [E-mail address for bug reporting])
AH_TEMPLATE([AUTHORS], [Authors of the program])
AH_TEMPLATE([COPYRIGHT], [Copyright information for the program])
AH_TEMPLATE([rl_completion_matches], [Define if rl_completion_matches not available])
AH_VERBATIM([_GNU_SOURCE],
[/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif])

# Enable maintainer mode
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([errno.h libintl.h limits.h stdint.h stdlib.h string.h strings.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_LSTAT
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset strerror strstr strtol])

# Handle --enable/disable-debug (disable debugging symbols by default)
AC_ARG_ENABLE(debug, [  --enable-debug          Add debugging symbols to executables], [
  if test "x$enable_debug" != "xyes"; then
    CFLAGS=`echo $CFLAGS|sed 's/-g//g'`
  else
    CFLAGS=`echo $CFLAGS|sed 's/-O2//g'`
  fi
],[CFLAGS=`echo $CFLAGS|sed 's/-g//g'`])

# Add -Wall to CFLAGS and clean it up
CFLAGS=`echo $CFLAGS -Wall|sed 's/^\ *//g;s/\ *$//g'`

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
