pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libieee1666.pc systemc.pc tlm.pc
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I$(top_srcdir)/libieee1666 -I$(top_builddir)/libieee1666
AM_CXXFLAGS = -fPIC
sharedir = $(prefix)/share/libieee1666

nobase_include_HEADERS = \
libieee1666/systemc \
libieee1666/systemc.h \
libieee1666/tlm \
libieee1666/tlm.h \
libieee1666/core/sensitive.h \
libieee1666/core/port.h \
libieee1666/core/interface.h \
libieee1666/core/fwd.h \
libieee1666/core/export.h \
libieee1666/core/prim_channel.h \
libieee1666/core/event_finder.h \
libieee1666/core/kernel.h \
libieee1666/core/event.h \
libieee1666/core/module_name.h \
libieee1666/core/attribute.h \
libieee1666/core/module.h \
libieee1666/core/process_handle.h \
libieee1666/core/time.h \
libieee1666/core/object.h \
libieee1666/core/process.h \
libieee1666/core/thread_process.h \
libieee1666/core/cthread_process.h \
libieee1666/core/method_process.h \
libieee1666/core/kernel_event.h \
libieee1666/core/allocator.h \
libieee1666/core/spawn.h \
libieee1666/core/features.h \
libieee1666/core/reset.h \
libieee1666/core/coroutine.h \
libieee1666/core/stack.h \
libieee1666/core/simple_stack.h \
libieee1666/channels/fifo_in.h \
libieee1666/channels/clock.h \
libieee1666/channels/fifo_if.h \
libieee1666/channels/fwd.h \
libieee1666/channels/in.h \
libieee1666/channels/fifo_out.h \
libieee1666/channels/fifo.h \
libieee1666/channels/semaphore_if.h \
libieee1666/channels/inout.h \
libieee1666/channels/buffer.h \
libieee1666/channels/out.h \
libieee1666/channels/mutex_if.h \
libieee1666/channels/event_queue_if.h \
libieee1666/channels/event_queue.h \
libieee1666/channels/signal_if.h \
libieee1666/channels/mutex.h \
libieee1666/channels/signal.h \
libieee1666/channels/semaphore.h \
libieee1666/utilities/fwd.h \
libieee1666/utilities/trace_file.h \
libieee1666/utilities/report.h \
libieee1666/utilities/report_handler.h \
libieee1666/utilities/version.h \
libieee1666/utilities/vector.h \
libieee1666/tlm2/fwd.h \
libieee1666/tlm2/interface/blocking_transport.h \
libieee1666/tlm2/interface/non_blocking_transport.h \
libieee1666/tlm2/interface/debug_transport.h \
libieee1666/tlm2/interface/direct_memory.h \
libieee1666/tlm2/interface/combined.h \
libieee1666/tlm2/socket/target_socket.h \
libieee1666/tlm2/socket/initiator_socket.h \
libieee1666/tlm2/protocol/base_protocol.h \
libieee1666/tlm2/phase/base_phase.h \
libieee1666/tlm2/payload/generic_payload.h \
libieee1666/tlm2/global_quantum/global_quantum.h \
libieee1666/tlm_utils/simple_initiator_socket.h \
libieee1666/tlm_utils/simple_target_socket.h \
libieee1666/tlm_utils/passthrough_target_socket.h \
libieee1666/tlm_utils/peq_with_get.h \
libieee1666/tlm_utils/peq_with_cb_and_phase.h \
libieee1666/tlm_utils/tlm_quantumkeeper.h

noinst_HEADERS = \
libieee1666/core/sysdep/fcontext_coroutine.h \
libieee1666/core/sysdep/ucontext_coroutine.h \
libieee1666/core/sysdep/windows_fiber_coroutine.h \
libieee1666/core/sysdep/sjlj_except.h \
libieee1666/core/sysdep/pthread_coroutine.h \
libieee1666/core/sysdep/posix_guarded_stack.h \
libieee1666/core/sysdep/windows_guarded_stack.h \
libieee1666/utilities/backtrace.h \
libieee1666/utilities/sysdep/backtrace.h

lib_LTLIBRARIES = libieee1666.la
libieee1666_la_SOURCES = \
libieee1666/core/attribute.cc \
libieee1666/core/event.cc \
libieee1666/core/event_finder.cc \
libieee1666/core/export.cc \
libieee1666/core/interface.cc \
libieee1666/core/kernel.cc \
libieee1666/core/module.cc \
libieee1666/core/module_name.cc \
libieee1666/core/object.cc \
libieee1666/core/port.cc \
libieee1666/core/prim_channel.cc \
libieee1666/core/process_handle.cc \
libieee1666/core/sensitive.cc \
libieee1666/core/time.cc \
libieee1666/core/process.cc \
libieee1666/core/thread_process.cc \
libieee1666/core/cthread_process.cc \
libieee1666/core/method_process.cc \
libieee1666/core/spawn.cc \
libieee1666/core/reset.cc \
libieee1666/core/simple_stack.cc \
libieee1666/channels/buffer.cc \
libieee1666/channels/clock.cc \
libieee1666/channels/event_queue.cc \
libieee1666/channels/event_queue_if.cc \
libieee1666/channels/fifo.cc \
libieee1666/channels/fifo_if.cc \
libieee1666/channels/fifo_in.cc \
libieee1666/channels/fifo_out.cc \
libieee1666/channels/in.cc \
libieee1666/channels/inout.cc \
libieee1666/channels/mutex.cc \
libieee1666/channels/mutex_if.cc \
libieee1666/channels/out.cc \
libieee1666/channels/semaphore.cc \
libieee1666/channels/semaphore_if.cc \
libieee1666/channels/signal.cc \
libieee1666/channels/signal_if.cc \
libieee1666/utilities/trace_file.cc \
libieee1666/utilities/report.cc \
libieee1666/utilities/report_handler.cc \
libieee1666/utilities/version.cc \
libieee1666/tlm2/interface/direct_memory.cc \
libieee1666/tlm2/phase/base_phase.cc \
libieee1666/tlm2/payload/generic_payload.cc \
libieee1666/tlm2/global_quantum/global_quantum.cc \
libieee1666/tlm_utils/tlm_quantumkeeper.cc

EXTRA_libieee1666_la_SOURCES = \
libieee1666/core/sysdep/fcontext_coroutine.cc \
libieee1666/core/sysdep/ucontext_coroutine.cc \
libieee1666/core/sysdep/windows_fiber_coroutine.cc \
libieee1666/core/sysdep/pthread_coroutine.cc \
libieee1666/core/sysdep/posix_guarded_stack.cc \
libieee1666/core/sysdep/windows_guarded_stack.cc \
libieee1666/utilities/sysdep/backtrace.cc

libieee1666_la_LIBADD = $(LIBIEEE1666_SYSDEPS)
libieee1666_la_DEPENDENCIES = $(LIBIEEE1666_SYSDEPS)

libieee1666_la_LDFLAGS = -release $(PACKAGE_VERSION)

nobase_dist_share_DATA = \
COPYING

EXTRA_DIST = \
bootstrap.sh \
m4/libtool.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4 \
m4/ltsugar.m4 \
m4/ltversion.m4 \
m4/ax_boost_base.m4 \
m4/ax_boost_context.m4 \
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

if LIBIEEE1666_ENABLE_ACCELLERA_INSTALL_LAYOUT
install-exec-hook:
	(test -e $(libdir)/libsystemc.a || (cd $(libdir) && $(LN_S) libieee1666.a libsystemc.a))
	(test -e $(libdir)/libsystemc.so || (cd $(libdir) && $(LN_S) libieee1666.so libsystemc.so))
	(test -z $(TARGET_ARCH) || test -e $(prefix)/lib-$(TARGET_ARCH) || (cd $(prefix) && $(LN_S) lib lib-$(TARGET_ARCH)))

install-data-hook:
	(test -e $(includedir)/systemc || (cd $(includedir) && $(LN_S) libieee1666/systemc systemc))
	(test -e $(includedir)/systemc.h || (cd $(includedir) && $(LN_S) libieee1666/systemc.h systemc.h))
	(test -e $(includedir)/tlm || (cd $(includedir) && $(LN_S) libieee1666/tlm tlm))
	(test -e $(includedir)/tlm.h || (cd $(includedir) && $(LN_S) libieee1666/tlm.h tlm.h))
	(test -e $(includedir)/tlm_utils || (cd $(includedir) && $(LN_S) libieee1666/tlm_utils tlm_utils))
	(test -e $(includedir)/core || (cd $(includedir) && $(LN_S) libieee1666/core core))
	(test -e $(includedir)/channels || (cd $(includedir) && $(LN_S) libieee1666/channels channels))
	(test -e $(includedir)/utilities || (cd $(includedir) && $(LN_S) libieee1666/utilities utilities))
	(test -e $(includedir)/tlm2 || (cd $(includedir) && $(LN_S) libieee1666/tlm2 tlm2))
endif
