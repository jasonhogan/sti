# Process this file with autoconf to produce a configure script.

# Initialization
AC_INIT([Slow Analog Out Device], [0.0.1], [hogan@stanford.edu], [slowAnalogOut])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src])

# Local definitions

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_CPP
AI_CHECK_ORB
if test "x${ORB}" = "xno"; then
    AC_MSG_ERROR([an ORB is required])
fi

# Checks for libraries.
AC_CHECK_LIB([bus], [bus_space_tag])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h bus_space.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.

# Output the results
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([GNUmakefile
    src/GNUmakefile])
AC_OUTPUT
