pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libieee1666.pc systemc.pc tlm.pc
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I$(top_srcdir)/ieee1666 -I$(top_builddir)/ieee1666
AM_CXXFLAGS = -fPIC
sharedir = $(prefix)/share/libieee1666
nobase_include_HEADERS = \
ieee1666/systemc \
ieee1666/systemc.h \
ieee1666/tlm \
ieee1666/tlm.h \
ieee1666/core/sensitive.h \
ieee1666/core/port.h \
ieee1666/core/interface.h \
ieee1666/core/fwd.h \
ieee1666/core/export.h \
ieee1666/core/prim_channel.h \
ieee1666/core/event_finder.h \
ieee1666/core/kernel.h \
ieee1666/core/event.h \
ieee1666/core/module_name.h \
ieee1666/core/attribute.h \
ieee1666/core/module.h \
ieee1666/core/process_handle.h \
ieee1666/core/time.h \
ieee1666/core/object.h \
ieee1666/core/process.h \
ieee1666/core/thread_process.h \
ieee1666/core/cthread_process.h \
ieee1666/core/method_process.h \
ieee1666/core/kernel_event.h \
ieee1666/core/allocator.h \
ieee1666/core/spawn.h \
ieee1666/core/features.h \
ieee1666/core/reset.h \
ieee1666/channels/fifo_in.h \
ieee1666/channels/clock.h \
ieee1666/channels/fifo_if.h \
ieee1666/channels/fwd.h \
ieee1666/channels/in.h \
ieee1666/channels/fifo_out.h \
ieee1666/channels/fifo.h \
ieee1666/channels/semaphore_if.h \
ieee1666/channels/inout.h \
ieee1666/channels/buffer.h \
ieee1666/channels/out.h \
ieee1666/channels/mutex_if.h \
ieee1666/channels/event_queue_if.h \
ieee1666/channels/event_queue.h \
ieee1666/channels/signal_if.h \
ieee1666/channels/mutex.h \
ieee1666/channels/signal.h \
ieee1666/channels/semaphore.h \
ieee1666/utilities/fwd.h \
ieee1666/utilities/trace_file.h \
ieee1666/utilities/report.h \
ieee1666/utilities/report_handler.h \
ieee1666/utilities/version.h \
ieee1666/utilities/backtrace.h \
ieee1666/utilities/vector.h \
ieee1666/tlm2/fwd.h \
ieee1666/tlm2/interface/blocking_transport.h \
ieee1666/tlm2/interface/non_blocking_transport.h \
ieee1666/tlm2/interface/debug_transport.h \
ieee1666/tlm2/interface/direct_memory.h \
ieee1666/tlm2/interface/combined.h \
ieee1666/tlm2/socket/target_socket.h \
ieee1666/tlm2/socket/initiator_socket.h \
ieee1666/tlm2/protocol/base_protocol.h \
ieee1666/tlm2/phase/base_phase.h \
ieee1666/tlm2/payload/generic_payload.h \
ieee1666/tlm2/global_quantum/global_quantum.h \
ieee1666/tlm_utils/simple_initiator_socket.h \
ieee1666/tlm_utils/simple_target_socket.h \
ieee1666/tlm_utils/passthrough_target_socket.h \
ieee1666/tlm_utils/peq_with_get.h \
ieee1666/tlm_utils/peq_with_cb_and_phase.h \
ieee1666/tlm_utils/tlm_quantumkeeper.h

BUILT_SOURCES = ieee1666/core/features.h

CLEANFILES = \
$(top_builddir)/ieee1666/core/features.h

lib_LTLIBRARIES = libieee1666.la
libieee1666_la_SOURCES = \
ieee1666/core/attribute.cc \
ieee1666/core/event.cc \
ieee1666/core/event_finder.cc \
ieee1666/core/export.cc \
ieee1666/core/interface.cc \
ieee1666/core/kernel.cc \
ieee1666/core/module.cc \
ieee1666/core/module_name.cc \
ieee1666/core/object.cc \
ieee1666/core/port.cc \
ieee1666/core/prim_channel.cc \
ieee1666/core/process_handle.cc \
ieee1666/core/sensitive.cc \
ieee1666/core/time.cc \
ieee1666/core/process.cc \
ieee1666/core/thread_process.cc \
ieee1666/core/cthread_process.cc \
ieee1666/core/method_process.cc \
ieee1666/core/spawn.cc \
ieee1666/core/reset.cc \
ieee1666/channels/buffer.cc \
ieee1666/channels/clock.cc \
ieee1666/channels/event_queue.cc \
ieee1666/channels/event_queue_if.cc \
ieee1666/channels/fifo.cc \
ieee1666/channels/fifo_if.cc \
ieee1666/channels/fifo_in.cc \
ieee1666/channels/fifo_out.cc \
ieee1666/channels/in.cc \
ieee1666/channels/inout.cc \
ieee1666/channels/mutex.cc \
ieee1666/channels/mutex_if.cc \
ieee1666/channels/out.cc \
ieee1666/channels/semaphore.cc \
ieee1666/channels/semaphore_if.cc \
ieee1666/channels/signal.cc \
ieee1666/channels/signal_if.cc \
ieee1666/utilities/trace_file.cc \
ieee1666/utilities/report.cc \
ieee1666/utilities/report_handler.cc \
ieee1666/utilities/version.cc \
ieee1666/utilities/backtrace.cc \
ieee1666/tlm2/interface/direct_memory.cc \
ieee1666/tlm2/phase/base_phase.cc \
ieee1666/tlm2/payload/generic_payload.cc \
ieee1666/tlm2/global_quantum/global_quantum.cc \
ieee1666/tlm_utils/tlm_quantumkeeper.cc

libieee1666_la_LDFLAGS = -release $(PACKAGE_VERSION)

nobase_dist_share_DATA = \
COPYING

EXTRA_DIST = \
m4/cxxabi.m4 \
m4/check_lib.m4 \
m4/sc_thread_process_backend.m4 \
pkg/pkgconfig/libieee1666.pc.in \
pkg/pkgconfig/systemc.pc.in \
pkg/pkgconfig/tlm.pc.in \
pkg/rpm/libieee1666.spec.in \
pkg/deb/changelog.in \
pkg/deb/compat.in \
pkg/deb/copyright.in \
pkg/deb/rules.in \
pkg/deb/control.in \
pkg/deb/libieee1666.install.in \
pkg/deb/libieee1666-dev.install.in \
pkg/deb/libieee1666-static-dev.install.in \
pkg/deb/source/format.in

ieee1666/core/features.h: Makefile
	@echo "Generating $@"; \
	echo "// Do not modify: this file is generated while building process." > $@; \
	echo "#ifndef __IEEE1666_CORE_FEATURES_H__" >> $@; \
	echo "#define __IEEE1666_CORE_FEATURES_H__" >> $@; \
	echo "#ifndef SC_THREAD_PROCESSES_USE_PTHREADS" >> $@; \
	if test ${SC_THREAD_PROCESSES_USE_PTHREADS} -ne 0; then \
		echo "// using POSIX threads for SystemC thread processes" >> $@; \
	else \
		echo "// using boost coroutine for SystemC thread processes" >> $@; \
	fi; \
	echo "#define SC_THREAD_PROCESSES_USE_PTHREADS $(SC_THREAD_PROCESSES_USE_PTHREADS)" >> $@; \
	echo "#endif" >> $@; \
	echo "#define PACKAGE_VERSION_MAJOR $(PACKAGE_VERSION_MAJOR)" >> $@; \
	echo "#define PACKAGE_VERSION_MINOR $(PACKAGE_VERSION_MINOR)" >> $@; \
	echo "#define PACKAGE_VERSION_PATCH $(PACKAGE_VERSION_PATCH)" >> $@; \
	echo "#define PACKAGE_VERSION \"$(PACKAGE_VERSION)\"" >> $@; \
	echo "#endif" >> $@

rpm-source: $(top_builddir)/$(PACKAGE_NAME).spec dist-gzip
	mkdir -p $(HOME)/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	cp -f $(top_builddir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz $(HOME)/rpmbuild/SOURCES
	rpmbuild -bs $(top_builddir)/$(PACKAGE_NAME).spec

rpm-binary: rpm-source
	rpmbuild --rebuild $(HOME)/rpmbuild/SRPMS/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-${PACKAGE_RELEASE}.src.rpm

deb-source: debian/control debian/changelog
	dpkg-source -b .

# First try debuild without signing the source package nor the .changes file.
# If debuild is not available, then try dpkg-buildpackage
deb-binary: deb-source
	debuild -us -uc || dpkg-buildpackage 

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
	(rmdir $(libdir) || true)
