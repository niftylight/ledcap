#############
# ledcap configure.ac
# v0.5 - (c) Daniel Hiepler (2010-2014)


# --------------------------------
#    initialization
# --------------------------------

# package info
AC_INIT([ledcap], 
		m4_esyscmd([./version --short]),
		[http://github.com/niftylight/ledcap/issues], 
		[ledcap], 
		[http://wiki.niftylight.de/AC_PACKAGE_NAME])
AC_COPYRIGHT([Copyright (C) 2006-2014 Daniel Hiepler])

# for cross compiling
AC_CANONICAL_SYSTEM

# autotools initialization
AC_LANG(C)
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([color-tests dist-bzip2 parallel-tests silent-rules -Wall -Wno-portability])



# --------------------------------
#    further info on the package
# --------------------------------
# infos
PACKAGE_DESCRIPTION="niftyled screen capture tool"
PACKAGE_DESCRIPTION_LONG="send screen capture to LED hardware using niftyled"

AC_SUBST([PACKAGE_DESCRIPTION], [${PACKAGE_DESCRIPTION}], [package description])
AC_SUBST([PACKAGE_DESCRIPTION_LONG], [${PACKAGE_DESCRIPTION_LONG}], [long package description])
AC_DEFINE_UNQUOTED(PACKAGE_DESCRIPTION, "${PACKAGE_DESCRIPTION}", [package description])
AC_DEFINE_UNQUOTED(PACKAGE_DESCRIPTION_LONG, "${PACKAGE_DESCRIPTION_LONG}", [long package description])

# versioning (s. version script)
AC_SUBST([PACKAGE_MAJOR_VERSION], m4_esyscmd([./version --major]), [major version])
AC_SUBST([PACKAGE_MINOR_VERSION], m4_esyscmd([./version --minor]), [minor version])
AC_SUBST([PACKAGE_MICRO_VERSION], m4_esyscmd([./version --micro]), [micro version])
AC_SUBST([PACKAGE_GIT_VERSION], m4_esyscmd([./version --git]), [git version])
AC_SUBST([DEBIAN_REVISION], m4_esyscmd([./version --debian]), [debian revision])


# --------------------------------
#    checks for programs
# --------------------------------
dnl change this to 0.28 as soon as it's widely used. Then also delete m4/pkg.m4
PKG_PROG_PKG_CONFIG([0.25])
if test -z "${PKG_CONFIG}"; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/ or check your distribution.])
fi


# --------------------------------
#    checks for libraries
# --------------------------------

AC_SEARCH_LIBS([cposix])

# Test for libniftyled
PKG_CHECK_MODULES(niftyled, [niftyled], [], [AC_MSG_ERROR([You need libniftyled + development headers installed])])
AC_SUBST(niftyled_CFLAGS)
AC_SUBST(niftyled_LIBS)

# Test for X
PKG_CHECK_MODULES(X, [x11], [HAVE_X=1], [HAVE_X=0])
AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)
AM_CONDITIONAL([USE_X], [test $HAVE_X -eq 1])

# Test for imlib2
PKG_CHECK_MODULES(IMLIB, imlib2, [HAVE_IMLIB=1], [HAVE_IMLIB=0])
AC_SUBST(IMLIB_CFLAGS)
AC_SUBST(IMLIB_LIBS)


# --------------------------------
#    checks for header files
# --------------------------------
AC_HEADER_STDC


# --------------------------------
#    checks for types
# --------------------------------


# --------------------------------
#    checks for structures
# --------------------------------


# --------------------------------
#    checks for compiler characteristics
# --------------------------------


# --------------------------------
#    checks for library functions
# --------------------------------


# --------------------------------
#    checks for system services
# --------------------------------


# --------------------------------
#   compile process
# --------------------------------
AC_PROG_CC_C99
AM_PROG_CC_C_O
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


# --------------------------------
# Arguments for configure-script
# --------------------------------
AC_ARG_ENABLE(
        debug,
		AS_HELP_STRING([--enable-debug], [enable debugging, default: no]),
		[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
		esac],
		[debug=false])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)


# use Imlib to capture
AC_ARG_ENABLE(
	imlib-capture,
	AS_HELP_STRING([--enable-imlib-capture], [Enable capturing with imlib]),
	[ if test x$enableval = xno ; then WANT_IMLIB=false ; else if test $HAVE_IMLIB -eq 1 ; then WANT_IMLIB=true ; else AC_MSG_ERROR([imlib capture requested but imlib not found]) ; fi ; fi ],
	[ WANT_IMLIB=true ])
AM_CONDITIONAL([USE_IMLIB], [test x$WANT_IMLIB = xtrue && test $HAVE_IMLIB -eq 1])


# use libX to capture
AC_ARG_ENABLE(
	x-capture,
	AS_HELP_STRING([--enable-x-capture], [Enable capturing with libX11]),
	[ if test x$enableval = xno ; then WANT_X=false ; else if test $HAVE_X -eq 1 ; then WANT_X=true ; else AC_MSG_ERROR([X capture requested but X not found]) ; fi ; fi ],
	[ WANT_X=true ])
AM_CONDITIONAL([USE_X], [test x$WANT_X = xtrue && test $HAVE_X -eq 1])


# we need at least one capture mechanism
if (test "x$WANT_X" = "xfalse" || test $HAVE_X -ne 1) &&
	(test "x$WANT_IMLIB" = "xfalse" || test $HAVE_IMLIB -ne 1) ; then
  AC_MSG_ERROR([No supported capture mechanism found. Please install one first. (e.g. libX11 or imlib)])
fi


# build string with capture mechanisms to build
if test x$WANT_IMLIB = xtrue && test $HAVE_IMLIB -eq 1 ; then CAPTURE="imlib $CAPTURE" ; fi
if test x$WANT_X = xtrue && test $HAVE_X -eq 1 ; then CAPTURE="X11 $CAPTURE" ; fi


# --------------------------------
# Output
# --------------------------------
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
[
        Makefile
        src/version.c
        src/Makefile
        gentoo/media-gfx/ledcap/ledcap-9999.ebuild
        dist/debian/changelog
        dist/debian/control.tmpl
        dist/debian/rules.tmpl
        dist/debian/watch
])
AC_OUTPUT



echo -e "
Configure report:

\tPackage.....................:  ${PACKAGE_STRING}
\tDescription.................:  ${PACKAGE_DESCRIPTION}
\tRevision....................:  ${PACKAGE_GIT_VERSION}
\tURL.........................:  ${PACKAGE_URL}
\tBugreports..................:  ${PACKAGE_BUGREPORT}
\tCapture mechanisms..........:  ${CAPTURE}

\tInstall prefix..............:  ${prefix}
\tC compiler..................:  ${CC}
\tSystem CFLAGS...............:  ${CFLAGS}
\tSystem CXXFLAGS.............:  ${CXXFLAGS}
\tSystem LDFLAGS..............:  ${LDFLAGS}
"
