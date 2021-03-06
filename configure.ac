AC_PREREQ([2.68])
AC_INIT([mal-gtk], [0.3.25], [agpotter@gmail.com])
AX_IS_RELEASE([git-directory])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects])
#AC_CONFIG_HEADERS([config.h])

AX_CHECK_ENABLE_DEBUG([info])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_14([noext])
AX_COMPILER_FLAGS([WARN_CFLAGS],[WARN_LDFLAGS])

AX_PTHREAD( [
            AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.])
            CLIBS="$PTHREAD_LIBS $LIBS"
            CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
            LDFLAGS="$LDFLAGS $PTHREAD_CFLAGS"
            CC="$PTHREAD_CC"],[])

AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])

# Checks for libraries.

PKG_CHECK_MODULES([GOBJECT], [gobject-2.0 >= 2.44.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.44.0])
PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 >= 2.44.0])
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.4.0])
PKG_CHECK_MODULES([CURL], [libcurl >= 7.24.0])
PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.7.8])
PKG_CHECK_MODULES([LIBSECRET], [libsecret-1 >= 0.12])

# To use C++11 lambdas with sigc++:
PKG_CHECK_MODULES([SIGCPP], [sigc++-2.0 >= 2.2.11])

AC_SUBST([GBOJECT_LIBS])
AC_SUBST([GOBJECT_CFLAGS])
AC_SUBST([GLIB_LIBS])
AC_SUBST([GLIB_CFLAGS])
AC_SUBST([GLIBMM_LIBS])
AC_SUBST([GLIBMM_CFLAGS])
AC_SUBST([GTKMM_LIBS])
AC_SUBST([GTKMM_CFLAGS])
AC_SUBST([CURL_LIBS])
AC_SUBST([CURL_CFLAGS])
AC_SUBST([LIBXML_LIBS])
AC_SUBST([LIBXML_CFLAGS])
AC_SUBST([LIBSECRET_CFLAGS])
AC_SUBST([LIBSECRET_LIBS])

#GLIB_GSETTINGS
GLIB_TESTS

# Checks for header files.
AC_CHECK_HEADERS([locale.h stdio.h stdarg.h])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
