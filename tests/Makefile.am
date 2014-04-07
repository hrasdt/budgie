-include $(top_srcdir)/common.mk

if BUILD_TESTS
# Build a test program for budgie-db speed, etc.
bin_PROGRAMS = budgie-db-test

budgie_db_test_SOURCES = \
	budgie-db-test.c \
	../src/util.c

budgie_db_test_CFLAGS = \
	$(GIO_CFLAGS) \
	$(GTK3_CFLAGS) \
	$(AM_CFLAGS)

budgie_db_test_LDADD = \
	$(GIO_LIBS) \
	$(GTK3_LIBS) \
	-lid3 \
	../src/libbudgiedb.la
endif
