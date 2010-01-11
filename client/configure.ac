# Process this file with autoconf to produce a configure script.

# Initialization
AC_INIT([STI client], [0.0.1], [mandel@stanford.edu], [client])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src])

# Local definitions

# Checks for programs.
AI_CHECK_ORB
if test "x${ORB}" = "xno"; then
    AC_MSG_ERROR([an ORB is required])
fi

# Checks for libraries.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Output the results
AC_CONFIG_FILES([GNUmakefile])
AC_OUTPUT
