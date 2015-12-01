pkgconfig_DATA = systemc.pc
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I$(top_srcdir) -I$(top_builddir)
AM_CXXFLAGS = -fPIC
includedir = $(prefix)/include
sharedir = $(prefix)/share
libdir= $(prefix)/lib
nobase_include_HEADERS = \
systemc \
systemc.h \
tlm \
tlm.h \
ieee1666/kernel/sensitive.h \
ieee1666/kernel/port.h \
ieee1666/kernel/interface.h \
ieee1666/kernel/fwd.h \
ieee1666/kernel/export.h \
ieee1666/kernel/prim_channel.h \
ieee1666/kernel/event_finder.h \
ieee1666/kernel/kernel.h \
ieee1666/kernel/event.h \
ieee1666/kernel/module_name.h \
ieee1666/kernel/attribute.h \
ieee1666/kernel/module.h \
ieee1666/kernel/process_handle.h \
ieee1666/kernel/time.h \
ieee1666/kernel/object.h \
ieee1666/kernel/process.h \
ieee1666/kernel/thread_process.h \
ieee1666/kernel/cthread_process.h \
ieee1666/kernel/method_process.h \
ieee1666/kernel/kernel_event.h \
ieee1666/kernel/allocator.h \
ieee1666/kernel/spawn.h \
ieee1666/kernel/features.h \
ieee1666/base/fifo_in.h \
ieee1666/base/clock.h \
ieee1666/base/fifo_if.h \
ieee1666/base/fwd.h \
ieee1666/base/in.h \
ieee1666/base/fifo_out.h \
ieee1666/base/fifo.h \
ieee1666/base/semaphore_if.h \
ieee1666/base/inout.h \
ieee1666/base/buffer.h \
ieee1666/base/out.h \
ieee1666/base/mutex_if.h \
ieee1666/base/event_queue_if.h \
ieee1666/base/event_queue.h \
ieee1666/base/signal_if.h \
ieee1666/base/mutex.h \
ieee1666/base/signal.h \
ieee1666/base/semaphore.h \
ieee1666/util/fwd.h \
ieee1666/util/trace_file.h \
ieee1666/util/report.h \
ieee1666/util/report_handler.h \
ieee1666/util/version.h \
ieee1666/util/backtrace.h \
ieee1666/util/vector.h \
ieee1666/tlm2/interoperability_layer/fwd.h \
ieee1666/tlm2/interoperability_layer/interface/blocking_transport.h \
ieee1666/tlm2/interoperability_layer/interface/non_blocking_transport.h \
ieee1666/tlm2/interoperability_layer/interface/debug_transport.h \
ieee1666/tlm2/interoperability_layer/interface/direct_memory.h \
ieee1666/tlm2/interoperability_layer/interface/combined.h \
ieee1666/tlm2/interoperability_layer/socket/target_socket.h \
ieee1666/tlm2/interoperability_layer/socket/initiator_socket.h \
ieee1666/tlm2/interoperability_layer/protocol/base_protocol.h \
ieee1666/tlm2/interoperability_layer/phase/base_phase.h \
ieee1666/tlm2/interoperability_layer/payload/generic_payload.h \
ieee1666/tlm2/interoperability_layer/global_quantum/global_quantum.h \
tlm_utils/simple_initiator_socket.h \
tlm_utils/simple_target_socket.h \
tlm_utils/passthrough_target_socket.h \
tlm_utils/peq_with_get.h \
tlm_utils/peq_with_cb_and_phase.h \
tlm_utils/tlm_quantumkeeper.h

BUILT_SOURCES = ieee1666/kernel/features.h

CLEANFILES = ieee1666/kernel/features.h

lib_LTLIBRARIES = libsystemc.la
libsystemc_la_SOURCES = \
ieee1666/kernel/attribute.cc \
ieee1666/kernel/event.cc \
ieee1666/kernel/event_finder.cc \
ieee1666/kernel/export.cc \
ieee1666/kernel/interface.cc \
ieee1666/kernel/kernel.cc \
ieee1666/kernel/module.cc \
ieee1666/kernel/module_name.cc \
ieee1666/kernel/object.cc \
ieee1666/kernel/port.cc \
ieee1666/kernel/prim_channel.cc \
ieee1666/kernel/process_handle.cc \
ieee1666/kernel/sensitive.cc \
ieee1666/kernel/time.cc \
ieee1666/kernel/process.cc \
ieee1666/kernel/thread_process.cc \
ieee1666/kernel/cthread_process.cc \
ieee1666/kernel/method_process.cc \
ieee1666/kernel/kernel_event.cc \
ieee1666/kernel/spawn.cc \
ieee1666/base/buffer.cc \
ieee1666/base/clock.cc \
ieee1666/base/event_queue.cc \
ieee1666/base/event_queue_if.cc \
ieee1666/base/fifo.cc \
ieee1666/base/fifo_if.cc \
ieee1666/base/fifo_in.cc \
ieee1666/base/fifo_out.cc \
ieee1666/base/in.cc \
ieee1666/base/inout.cc \
ieee1666/base/mutex.cc \
ieee1666/base/mutex_if.cc \
ieee1666/base/out.cc \
ieee1666/base/semaphore.cc \
ieee1666/base/semaphore_if.cc \
ieee1666/base/signal.cc \
ieee1666/base/signal_if.cc \
ieee1666/util/trace_file.cc \
ieee1666/util/report.cc \
ieee1666/util/report_handler.cc \
ieee1666/util/version.cc \
ieee1666/util/backtrace.cc \
ieee1666/tlm2/interoperability_layer/interface/direct_memory.cc \
ieee1666/tlm2/interoperability_layer/phase/base_phase.cc \
ieee1666/tlm2/interoperability_layer/payload/generic_payload.cc \
ieee1666/tlm2/interoperability_layer/global_quantum/global_quantum.cc \
tlm_utils/tlm_quantumkeeper.cc

libsystemc_la_LIBADD=$(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_SYSTEM_LIB) $(PTHREAD_LIBS) $(LIBS)

nobase_dist_share_DATA = \
COPYING

EXTRA_DIST = \
m4/cxxabi.m4 \
m4/check_lib.m4 \
m4/sc_thread_process_backend.m4

ieee1666/kernel/features.h: Makefile
	echo "// Do not modify: this file is generated while building process." > $@; \
	echo "#ifndef __IEEE1666_KERNEL_FEATURES_H__" >> $@; \
	echo "#define __IEEE1666_KERNEL_FEATURES_H__" >> $@; \
	echo "#ifndef SC_THREAD_PROCESSES_USE_PTHREADS" >> $@; \
	if test ${SC_THREAD_PROCESSES_USE_PTHREADS} -ne 0; then \
		echo "// using POSIX threads for SystemC thread processes" >> $@; \
	else \
		echo "// using boost coroutine for SystemC thread processes" >> $@; \
	fi; \
	echo "#define SC_THREAD_PROCESSES_USE_PTHREADS $(SC_THREAD_PROCESSES_USE_PTHREADS)" >> $@; \
	echo "#endif" >> $@; \
	echo "#endif" >> $@

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
	(rmdir $(libdir) || true)
	
install-exec-hook:
	(cd $(prefix) && ( [ -d lib-$(IEEE1666_TARGET_ARCH) ] || $(LN_S) lib lib-$(IEEE1666_TARGET_ARCH) ) )
