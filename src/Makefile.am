## Process this file with automake to produce Makefile.in
bin_PROGRAMS = ledcap


ledcap_SOURCES = \
	ledcap.c version.c capture.c cap_x11.c cap_imlib.c

ledcap_CFLAGS = \
	-Wall $(IMLIB_CFLAGS) $(X_CFLAGS) $(niftyled_CFLAGS)

ledcap_LDFLAGS = \
	$(IMLIB_LIBS) $(X_LIBS) $(niftyled_LIBS)
