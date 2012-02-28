ACLOCAL_AMFLAGS=-I m4
INCLUDES = -I$(top_srcdir)
includedir = $(prefix)/include
sharedir = $(prefix)/share/libshmidcat
include_HEADERS = shmidcat.h
noinst_HEADERS = NativeShmidcat.h

lib_LTLIBRARIES = libshmidcat.la
libshmidcat_la_SOURCES = shmidcat.c NativeShmidcat.c

dist_share_DATA = COPYING

EXTRA_DIST= NativeShmidcat.java sample.vcd launch.sh

all-local: NativeShmidcat.class
clean-local:
	rm -rf $(top_builddir)/org

install-exec-local: $(top_builddir)/NativeShmidcat.class
	mkdir -p $(DIST_DIR)$(bindir)
	cp -fr $(top_builddir)/org $(DIST_DIR)$(bindir)

$(top_builddir)/NativeShmidcat.class: $(top_srcdir)/NativeShmidcat.java
	$(JDK_PATH)/bin/javac -d ./ $(top_srcdir)/NativeShmidcat.java

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
	
