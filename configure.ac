AC_INIT([mokowm], [1.0.99], [daniele.athome@gmail.com], [mokowm])
AC_CONFIG_SRCDIR(Makefile.am)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_SUBST(ACLOCAL_AMFLAGS, "$ACLOCAL_FLAGS -I m4")
AC_PROG_CC

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

EDJE_REQUIRED=0.0.0
EINA_REQUIRED=0.0.0
EVAS_REQUIRED=0.0.0
ECORE_REQUIRED=0.0.0
GLIB_REQUIRED=2.18.0

PKG_CHECK_MODULES(GLIB,
  glib-2.0 >= $GLIB_REQUIRED
  gobject-2.0 >= $GLIB_REQUIRED
)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(EDJE,
  edje >= $EDJE_REQUIRED
)
AC_SUBST(EDJE_CFLAGS)
AC_SUBST(EDJE_LIBS)

PKG_CHECK_MODULES(EINA,
  eina >= $EINA_REQUIRED
)
AC_SUBST(EINA_CFLAGS)
AC_SUBST(EINA_LIBS)

PKG_CHECK_MODULES(EVAS,
  evas >= $EVAS_REQUIRED
)
AC_SUBST(EVAS_CFLAGS)
AC_SUBST(EVAS_LIBS)

PKG_CHECK_MODULES(ECORE,
  ecore >= $ECORE_REQUIRED
  ecore-x >= $ECORE_REQUIRED
  ecore-evas >= $ECORE_REQUIRED
  ecore-input >= $ECORE_REQUIRED
  eet >= $ECORE_REQUIRED
)
AC_SUBST(ECORE_CFLAGS)
AC_SUBST(ECORE_LIBS)

PKG_CHECK_MODULES(X11, x11)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

PKG_CHECK_MODULES(FAKEKEY, libfakekey)
AC_SUBST(FAKEKEY_CFLAGS)
AC_SUBST(FAKEKEY_LIBS)

AC_ARG_WITH(edje-cc,
[  --with-edje-cc=PATH	specify a specific path to edje_cc],
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
])
AC_SUBST(edje_cc)

AC_ARG_ENABLE(qvga,
    AC_HELP_STRING([--enable-qvga], [Enable compiling for QVGA (default: disabled)]),
    enable_qvga=$enableval,
    enable_qvga=no)

if test "x$enable_qvga" != "xno"; then
    SCALE_FACTOR=1.0
    AC_DEFINE(QVGA, 1, [Define to enable QVGA build])
else
    SCALE_FACTOR=2.0
fi

AM_CONDITIONAL(ENABLE_QVGA, test "x$enable_qvga" = "xyes")
AC_SUBST(SCALE_FACTOR)

AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug], [Enable debug build (default: disabled)]),
    enable_debug=$enableval,
    enable_debug=no)

if test "x$enable_debug" != "xno"; then
    AC_DEFINE(DEBUG, 1, [Define to enable debug build])
fi


AC_OUTPUT([
  Makefile
  data/mokosession
  data/Makefile
  src/Makefile])
