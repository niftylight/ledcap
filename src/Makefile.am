#############
# ledcap Makefile.am
# v0.4 - (c) Daniel Hiepler (2010-2014)


# compile for debugging ?
if DEBUG
    DEBUG_CFLAGS = -DDEBUG
endif


bin_PROGRAMS = ledcap

ledcap_SOURCES = \
	ledcap.c version.c capture.c

EXTRA_DIST = \
	capture.h \
	cap_imlib.h \
	cap_x11.h \
	version.h

ledcap_CFLAGS = \
	-Wall -Wextra -Werror -Wno-unused-parameter \
	$(niftyled_CFLAGS)

ledcap_LDADD = \
	 $(niftyled_LIBS)

if USE_X
ledcap_SOURCES += cap_x11.c
ledcap_CFLAGS += $(X_CFLAGS) -DHAVE_X
ledcap_LDADD += $(X_LIBS)
endif

if USE_IMLIB
ledcap_SOURCES += cap_imlib.c
ledcap_CFLAGS += $(IMLIB_CFLAGS) -DHAVE_IMLIB
ledcap_LDADD += $(IMLIB_LIBS)
endif

