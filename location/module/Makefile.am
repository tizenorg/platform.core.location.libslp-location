noinst_LTLIBRARIES = liblocation-module.la

COMMON_HEADER_DIR = include
MANAGER_DIR = manager
MAP_SERVICE_DIR = map-service
MODULE_DIR = module

liblocation_module_la_SOURCES = \
							 module-internal.c

liblocation_module_la_CFLAGS = \
							-fPIC\
							-I${srcdir} \
							-I${srcdir}/.. \
							-I${srcdir}/../include \
							-I${srcdir}/../${MANAGER_DIR} \
							-I${srcdir}/../${MODULE_DIR} \
							-I${srcdir}/../${MAP_SERVICE_DIR} \
							$(LOCATION_CFLAGS)

