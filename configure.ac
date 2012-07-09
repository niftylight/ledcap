# configure.ac 
# v0.3 - Daniel Hiepler (2010-2012)

# --------------------------------
#    initialization
# --------------------------------

AC_INIT([ledcap], m4_esyscmd([./version.sh --short]),[http://github.com/niftylight/ledcap/issues], [ledcap], [http://wiki.niftylight.de/AC_PACKAGE_NAME])
AC_COPYRIGHT([Copyright (C) 2006-2012 Daniel Hiepler])

AM_INIT_AUTOMAKE([color-tests dist-bzip2 parallel-tests silent-rules -Wall])
AC_CONFIG_MACRO_DIR([m4])


# --------------------------------
#    information on the package
# --------------------------------
# infos
PACKAGE_DESCRIPTION="niftyled screen capture tool"
AC_SUBST([PACKAGE_DESCRIPTION], [${PACKAGE_DESCRIPTION}], [package description])
AC_DEFINE_UNQUOTED(PACKAGE_DESCRIPTION, "${PACKAGE_DESCRIPTION}", [package description])

# versioning (s. version.sh)
AC_SUBST([PACKAGE_MAJOR_VERSION], m4_esyscmd([./version.sh --major]), [major version])
AC_SUBST([PACKAGE_MINOR_VERSION], m4_esyscmd([./version.sh --minor]), [minor version])
AC_SUBST([PACKAGE_MICRO_VERSION], m4_esyscmd([./version.sh --micro]), [micro version])
AC_SUBST([PACKAGE_GIT_VERSION], m4_esyscmd([./version.sh --git]), [git version])


# --------------------------------
#    checks for programs
# --------------------------------
AC_PROG_CC
AM_PROG_CC_C_O

PKG_PROG_PKG_CONFIG([0.14])
if test -z "$PKG_CONFIG"; then
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
PKG_CHECK_MODULES(X, [x11], [AC_DEFINE([HAVE_X],[],[Define if X is installed])], [AC_MSG_ERROR([You need X11 + development headers installed])])
AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)

# Test for imlib2
PKG_CHECK_MODULES(IMLIB, imlib2, [AC_DEFINE([HAVE_IMLIB],[],[Define if imlib2 is installed])], [AC_MSG_ERROR([You need imlib2 + development headers installed])])
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
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


# --------------------------------
# Arguments for configure-script
# --------------------------------


# --------------------------------
# Output
# --------------------------------
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
[
	Makefile
	src/version.c
	src/Makefile 
])
AC_OUTPUT


echo "
Configure report:

    Package.....................:  ${PACKAGE_STRING}
    Description.................:  ${PACKAGE_DESCRIPTION}
    Revision....................:  ${PACKAGE_GIT_VERSION}   
    URL.........................:  ${PACKAGE_URL}
    Bugreports..................:  ${PACKAGE_BUGREPORT}

    Install prefix..............:  ${prefix}
    C compiler..................:  ${CC}
    C++ compiler................:  ${CPP}
    System CFLAGS...............:  ${CFLAGS}
    System CXXFLAGS.............:  ${CXXFLAGS}
    System LDFLAGS..............:  ${LDFLAGS}
"
