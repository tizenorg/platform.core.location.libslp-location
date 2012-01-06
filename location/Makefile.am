lib_LTLIBRARIES = libSLP-location.la

libSLP_location_la_SOURCES = \
							 location-marshal.c   \
							 location-marshal.h   \
							 location-ielement.c  \
							 location-ielement.h  \
							 location-setting.c  \
							 location-setting.h  \
							 location-module.h \
							 location-module-internal.c\
							 location-module-internal.h\
							 location.c           \
							 location.h           \
							 location-position.c  \
							 location-position.h  \
							 location-velocity.c  \
							 location-velocity.h  \
							 location-accuracy.c  \
							 location-accuracy.h  \
							 location-address.c  \
							 location-address.h  \
							 location-boundary.c  \
							 location-boundary.h  \
							 location-satellite.c  \
							 location-satellite.h  \
							 location-poi-info.c  \
							 location-poi-info.h  \
							 location-signaling-util.c \
							 location-signaling-util.h \
							 location-common-util.c \
							 location-common-util.h \
							 location-gps.c       \
							 location-gps.h       \
							 location-wps.c       \
							 location-wps.h       \
							 location-ips.c       \
							 location-ips.h       \
							 location-cps.c       \
							 location-cps.h       \
							 location-sps.c       \
							 location-sps.h       \
							 location-hybrid.c \
							 location-hybrid.h \
							 location-geocode.c \
							 location-geocode.h \
							 location-poi.c   \
							 location-poi.h

libSLP_location_la_CFLAGS = \
							-fPIC\
							$(LOCATION_CFLAGS)

libSLP_location_la_LIBADD = \
							-lm\
							$(LOCATION_LIBS)

EXTRA_DIST = \
			 location-marshal.list

location-marshal.h: location-marshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --header --prefix=location > $@
location-marshal.c: location-marshal.list location-marshal.h $(GLIB_GENMARSHAL)
	echo "#include \"location-marshal.h\"" > $@ && $(GLIB_GENMARSHAL) location-marshal.list --body --prefix=location >> $@

libSLP_location_includedir = $(includedir)/location
libSLP_location_include_HEADERS = \
								  location.h     \
								  location-types.h \
								  location-position.h  \
								  location-velocity.h  \
								  location-accuracy.h  \
								  location-address.h  \
								  location-boundary.h  \
								  location-satellite.h  \
								  location-poi-info.h \
								  location-module.h
