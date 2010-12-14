dnl  Copyright 2010 wucan<canbaby@21cn.com>
dnl Process this file with autoconf to create configure.
dnl base on xinput

AC_PREREQ([2.57])
AC_INIT(xmux,[0.0.1], [http://www.canus.org/xmux],xmux)
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_LIBTOOL

dnl build for real hardware or pc test running
AC_ARG_WITH([build-target],
		AC_HELP_STRING([--with-build-target=TARGET],
		[Set the build target system: i386 or arm (default is i386)]),
		[build_target=$withval],
		[build_target=i386])
AC_DEFINE_UNQUOTED(BUILD_TARGET, [$build_target], [build target])

if test x$build_target = xarm; then
	AC_DEFINE(BUILD_FOR_ARM, 1, [build for arm real hardware])
fi

AM_CONDITIONAL(BUILD_FOR_ARM, test x$build_target = xarm)

# dvbpsi support
PKG_CHECK_MODULES(DVBPSI, [libdvbpsi >= 0.1.7],
                  HAVE_DVBPSI="yes"; AC_DEFINE(HAVE_DVBPSI, 1, [dvbpsi available]),
                  HAVE_DVBPSI="no");
AM_CONDITIONAL(HAVE_DVBPSI, [ test "$HAVE_DVBPSI" = "yes" ])

AC_SUBST(DVBPSI_CFLAGS)
AC_SUBST(DVBPSI_LIBS)
AC_SUBST(HAVE_DVBPSI)

AC_SUBST(VERSION)

AC_OUTPUT([Makefile
           src/Makefile
           src/imp/Makefile
           src/imp/fpga/Makefile
           src/libwu/Makefile
           tools/Makefile
           doc/Makefile])
