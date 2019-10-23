#
#  Copyright (c) 2019,
#  Commissariat a l'Energie Atomique (CEA)
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
#
#   - Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#
#   - Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#   - Neither the name of CEA nor the names of its contributors may be used to
#     endorse or promote products derived from this software without specific prior
#     written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED.
#  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
#  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>

#################################
# PPC VLE Bindings a-la-unicorn #
#################################

import ctypes
import ctypes.util
import distutils.sysconfig
import pkg_resources
import inspect
import os.path
import sys
import weakref


# UNICORN architectures
UC_ARCH_ARM = 0
# UNICORN ARM modes
UC_MODE_ARM = 0
UC_MODE_THUMB = 1

UC_ERR_OK = 0

UC_ARM_REG_INVALID = 0
UC_ARM_REG_APSR = 1
UC_ARM_REG_APSR_NZCV = 2
UC_ARM_REG_CPSR = 3
UC_ARM_REG_FPEXC = 4
UC_ARM_REG_FPINST = 5
UC_ARM_REG_FPSCR = 6
UC_ARM_REG_FPSCR_NZCV = 7
UC_ARM_REG_FPSID = 8
UC_ARM_REG_ITSTATE = 9
UC_ARM_REG_LR = 10
UC_ARM_REG_PC = 11
UC_ARM_REG_SP = 12
UC_ARM_REG_SPSR = 13
UC_ARM_REG_D0 = 14
UC_ARM_REG_D1 = 15
UC_ARM_REG_D2 = 16
UC_ARM_REG_D3 = 17
UC_ARM_REG_D4 = 18
UC_ARM_REG_D5 = 19
UC_ARM_REG_D6 = 20
UC_ARM_REG_D7 = 21
UC_ARM_REG_D8 = 22
UC_ARM_REG_D9 = 23
UC_ARM_REG_D10 = 24
UC_ARM_REG_D11 = 25
UC_ARM_REG_D12 = 26
UC_ARM_REG_D13 = 27
UC_ARM_REG_D14 = 28
UC_ARM_REG_D15 = 29
UC_ARM_REG_D16 = 30
UC_ARM_REG_D17 = 31
UC_ARM_REG_D18 = 32
UC_ARM_REG_D19 = 33
UC_ARM_REG_D20 = 34
UC_ARM_REG_D21 = 35
UC_ARM_REG_D22 = 36
UC_ARM_REG_D23 = 37
UC_ARM_REG_D24 = 38
UC_ARM_REG_D25 = 39
UC_ARM_REG_D26 = 40
UC_ARM_REG_D27 = 41
UC_ARM_REG_D28 = 42
UC_ARM_REG_D29 = 43
UC_ARM_REG_D30 = 44
UC_ARM_REG_D31 = 45
UC_ARM_REG_FPINST2 = 46
UC_ARM_REG_MVFR0 = 47
UC_ARM_REG_MVFR1 = 48
UC_ARM_REG_MVFR2 = 49
UC_ARM_REG_Q0 = 50
UC_ARM_REG_Q1 = 51
UC_ARM_REG_Q2 = 52
UC_ARM_REG_Q3 = 53
UC_ARM_REG_Q4 = 54
UC_ARM_REG_Q5 = 55
UC_ARM_REG_Q6 = 56
UC_ARM_REG_Q7 = 57
UC_ARM_REG_Q8 = 58
UC_ARM_REG_Q9 = 59
UC_ARM_REG_Q10 = 60
UC_ARM_REG_Q11 = 61
UC_ARM_REG_Q12 = 62
UC_ARM_REG_Q13 = 63
UC_ARM_REG_Q14 = 64
UC_ARM_REG_Q15 = 65
UC_ARM_REG_R0 = 66
UC_ARM_REG_R1 = 67
UC_ARM_REG_R2 = 68
UC_ARM_REG_R3 = 69
UC_ARM_REG_R4 = 70
UC_ARM_REG_R5 = 71
UC_ARM_REG_R6 = 72
UC_ARM_REG_R7 = 73
UC_ARM_REG_R8 = 74
UC_ARM_REG_R9 = 75
UC_ARM_REG_R10 = 76
UC_ARM_REG_R11 = 77
UC_ARM_REG_R12 = 78
UC_ARM_REG_S0 = 79
UC_ARM_REG_S1 = 80
UC_ARM_REG_S2 = 81
UC_ARM_REG_S3 = 82
UC_ARM_REG_S4 = 83
UC_ARM_REG_S5 = 84
UC_ARM_REG_S6 = 85
UC_ARM_REG_S7 = 86
UC_ARM_REG_S8 = 87
UC_ARM_REG_S9 = 88
UC_ARM_REG_S10 = 89
UC_ARM_REG_S11 = 90
UC_ARM_REG_S12 = 91
UC_ARM_REG_S13 = 92
UC_ARM_REG_S14 = 93
UC_ARM_REG_S15 = 94
UC_ARM_REG_S16 = 95
UC_ARM_REG_S17 = 96
UC_ARM_REG_S18 = 97
UC_ARM_REG_S19 = 98
UC_ARM_REG_S20 = 99
UC_ARM_REG_S21 = 100
UC_ARM_REG_S22 = 101
UC_ARM_REG_S23 = 102
UC_ARM_REG_S24 = 103
UC_ARM_REG_S25 = 104
UC_ARM_REG_S26 = 105
UC_ARM_REG_S27 = 106
UC_ARM_REG_S28 = 107
UC_ARM_REG_S29 = 108
UC_ARM_REG_S30 = 109
UC_ARM_REG_S31 = 110
UC_ARM_REG_C1_C0_2 = 111
UC_ARM_REG_C13_C0_2 = 112
UC_ARM_REG_C13_C0_3 = 113
UC_ARM_REG_ENDING = 114

# alias registers
UC_ARM_REG_R13 = 12
UC_ARM_REG_R14 = 10
UC_ARM_REG_R15 = 11
UC_ARM_REG_SB = 75
UC_ARM_REG_SL = 76
UC_ARM_REG_FP = 77
UC_ARM_REG_IP = 78

UC_HOOK_INTR = 1
UC_HOOK_INSN = 2
UC_HOOK_CODE = 4
UC_HOOK_BLOCK = 8
UC_HOOK_MEM_READ_UNMAPPED = 16
UC_HOOK_MEM_WRITE_UNMAPPED = 32
UC_HOOK_MEM_FETCH_UNMAPPED = 64
UC_HOOK_MEM_READ_PROT = 128
UC_HOOK_MEM_WRITE_PROT = 256
UC_HOOK_MEM_FETCH_PROT = 512
UC_HOOK_MEM_READ = 1024
UC_HOOK_MEM_WRITE = 2048
UC_HOOK_MEM_FETCH = 4096
UC_HOOK_MEM_READ_AFTER = 8192
UC_HOOK_MEM_UNMAPPED = 112
UC_HOOK_MEM_PROT = 896
UC_HOOK_MEM_READ_INVALID = 144
UC_HOOK_MEM_WRITE_INVALID = 288
UC_HOOK_MEM_FETCH_INVALID = 576
UC_HOOK_MEM_INVALID = 1008
UC_HOOK_MEM_VALID = 7168

# from . import x86_const, arm64_const, unicorn_const as uc

# if not hasattr(sys.modules[__name__], "__file__"):
#     __file__ = inspect.getfile(inspect.currentframe())

# _python2 = sys.version_info[0] < 3
# if _python2:
#     range = xrange

# _lib = { 'darwin': 'libunicorn.dylib',
#          'win32': 'unicorn.dll',
#          'cygwin': 'cygunicorn.dll',
#          'linux': 'libunicorn.so',
#          'linux2': 'libunicorn.so' }


# # Windows DLL in dependency order
# _all_windows_dlls = (
#     "libwinpthread-1.dll",
#     "libgcc_s_seh-1.dll",
#     "libgcc_s_dw2-1.dll",
# )

# _loaded_windows_dlls = set()

# def _load_win_support(path):
#     for dll in _all_windows_dlls:
#         if dll in _loaded_windows_dlls:
#             continue

#         lib_file = os.path.join(path, dll)
#         if ('/' not in path and '\\' not in path) or os.path.exists(lib_file):
#             try:
#                 #print('Trying to load Windows library', lib_file)
#                 ctypes.cdll.LoadLibrary(lib_file)
#                 #print('SUCCESS')
#                 _loaded_windows_dlls.add(dll)
#             except OSError as e:
#                 #print('FAIL to load %s' %lib_file, e)
#                 continue

_uc = None

VLE4FUZR_SO = os.getenv("VLE4FUZR_SO")
if not VLE4FUZR_SO.endswith('.so'):
    raise exception('VLE4FUZR_SO undefined')

_uc = ctypes.cdll.LoadLibrary(VLE4FUZR_SO)
if _uc is None:
    raise ImportError("ERROR: fail to load the dynamic library.")



# __version__ = "%u.%u.%u" % (uc.UC_VERSION_MAJOR, uc.UC_VERSION_MINOR, uc.UC_VERSION_EXTRA)

# setup all the function prototype
def _setup_prototype(lib, fname, restype, *argtypes):
    getattr(lib, fname).restype = restype
    getattr(lib, fname).argtypes = argtypes

ucerr = ctypes.c_int
uc_engine = ctypes.c_void_p
# uc_context = ctypes.c_void_p
uc_hook_h = ctypes.c_size_t

# class _uc_mem_region(ctypes.Structure):
#     _fields_ = [
#         ("begin", ctypes.c_uint64),
#         ("end",   ctypes.c_uint64),
#         ("perms", ctypes.c_uint32),
#     ]


# _setup_prototype(_uc, "uc_version", ctypes.c_uint, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int))
# _setup_prototype(_uc, "uc_arch_supported", ctypes.c_bool, ctypes.c_int)
_setup_prototype(_uc, "uc_open", ucerr, ctypes.c_uint, ctypes.c_uint, ctypes.POINTER(uc_engine))
# _setup_prototype(_uc, "uc_close", ucerr, uc_engine)
# _setup_prototype(_uc, "uc_strerror", ctypes.c_char_p, ucerr)
# _setup_prototype(_uc, "uc_errno", ucerr, uc_engine)
_setup_prototype(_uc, "uc_reg_read", ucerr, uc_engine, ctypes.c_int, ctypes.c_void_p)
_setup_prototype(_uc, "uc_reg_write", ucerr, uc_engine, ctypes.c_int, ctypes.c_void_p)
# _setup_prototype(_uc, "uc_mem_read", ucerr, uc_engine, ctypes.c_uint64, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t)
_setup_prototype(_uc, "uc_mem_write", ucerr, uc_engine, ctypes.c_uint64, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t)
# _setup_prototype(_uc, "uc_emu_start", ucerr, uc_engine, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_size_t)
# _setup_prototype(_uc, "uc_emu_stop", ucerr, uc_engine)
# _setup_prototype(_uc, "uc_hook_del", ucerr, uc_engine, uc_hook_h)
_setup_prototype(_uc, "uc_mem_map", ucerr, uc_engine, ctypes.c_uint64, ctypes.c_size_t, ctypes.c_uint32)
# _setup_prototype(_uc, "uc_mem_map_ptr", ucerr, uc_engine, ctypes.c_uint64, ctypes.c_size_t, ctypes.c_uint32, ctypes.c_void_p)
# _setup_prototype(_uc, "uc_mem_unmap", ucerr, uc_engine, ctypes.c_uint64, ctypes.c_size_t)
# _setup_prototype(_uc, "uc_mem_protect", ucerr, uc_engine, ctypes.c_uint64, ctypes.c_size_t, ctypes.c_uint32)
# _setup_prototype(_uc, "uc_query", ucerr, uc_engine, ctypes.c_uint32, ctypes.POINTER(ctypes.c_size_t))
# _setup_prototype(_uc, "uc_context_alloc", ucerr, uc_engine, ctypes.POINTER(uc_context))
# _setup_prototype(_uc, "uc_free", ucerr, ctypes.c_void_p)
# _setup_prototype(_uc, "uc_context_save", ucerr, uc_engine, uc_context)
# _setup_prototype(_uc, "uc_context_restore", ucerr, uc_engine, uc_context)
# _setup_prototype(_uc, "uc_mem_regions", ucerr, uc_engine, ctypes.POINTER(ctypes.POINTER(_uc_mem_region)), ctypes.POINTER(ctypes.c_uint32))

# # uc_hook_add is special due to variable number of arguments
# _uc.uc_hook_add = _uc.uc_hook_add
# _uc.uc_hook_add.restype = ucerr

UC_HOOK_CODE_CB = ctypes.CFUNCTYPE(None, uc_engine, ctypes.c_uint64, ctypes.c_size_t, ctypes.c_void_p)
# UC_HOOK_MEM_INVALID_CB = ctypes.CFUNCTYPE(
#     ctypes.c_bool, uc_engine, ctypes.c_int,
#     ctypes.c_uint64, ctypes.c_int, ctypes.c_int64, ctypes.c_void_p
# )
# UC_HOOK_MEM_ACCESS_CB = ctypes.CFUNCTYPE(
#     None, uc_engine, ctypes.c_int,
#     ctypes.c_uint64, ctypes.c_int, ctypes.c_int64, ctypes.c_void_p
# )
# UC_HOOK_INTR_CB = ctypes.CFUNCTYPE(
#     None, uc_engine, ctypes.c_uint32, ctypes.c_void_p
# )
# UC_HOOK_INSN_IN_CB = ctypes.CFUNCTYPE(
#     ctypes.c_uint32, uc_engine, ctypes.c_uint32, ctypes.c_int, ctypes.c_void_p
# )
# UC_HOOK_INSN_OUT_CB = ctypes.CFUNCTYPE(
#     None, uc_engine, ctypes.c_uint32,
#     ctypes.c_int, ctypes.c_uint32, ctypes.c_void_p
# )
# UC_HOOK_INSN_SYSCALL_CB = ctypes.CFUNCTYPE(None, uc_engine, ctypes.c_void_p)


# access to error code via @errno of UcError
class UcError(Exception):
    def __init__(self, errno):
        self.errno = errno

    def __str__(self):
        #return _uc.uc_strerror(self.errno).decode('ascii')
        return 'strerrno(%r)' % self.errno


# # return the core's version
# def uc_version():
#     major = ctypes.c_int()
#     minor = ctypes.c_int()
#     combined = _uc.uc_version(ctypes.byref(major), ctypes.byref(minor))
#     return (major.value, minor.value, combined)


# # return the binding's version
# def version_bind():
#     return (
#         uc.UC_API_MAJOR, uc.UC_API_MINOR,
#         (uc.UC_API_MAJOR << 8) + uc.UC_API_MINOR,
#     )


# # check to see if this engine supports a particular arch
# def uc_arch_supported(query):
#     return _uc.uc_arch_supported(query)


# class uc_x86_mmr(ctypes.Structure):
#     """Memory-Management Register for instructions IDTR, GDTR, LDTR, TR."""
#     _fields_ = [
#         ("selector", ctypes.c_uint16),  # not used by GDTR and IDTR
#         ("base", ctypes.c_uint64),      # handle 32 or 64 bit CPUs
#         ("limit", ctypes.c_uint32),
#         ("flags", ctypes.c_uint32),     # not used by GDTR and IDTR
#     ]

# class uc_x86_msr(ctypes.Structure):
#     _fields_ = [
#         ("rid", ctypes.c_uint32),
#         ("value", ctypes.c_uint64),
#     ]

# class uc_x86_float80(ctypes.Structure):
#     """Float80"""
#     _fields_ = [
#         ("mantissa", ctypes.c_uint64),
#         ("exponent", ctypes.c_uint16),
#     ]


# class uc_x86_xmm(ctypes.Structure):
#     """128-bit xmm register"""
#     _fields_ = [
#         ("low_qword", ctypes.c_uint64),
#         ("high_qword", ctypes.c_uint64),
#     ]

# class uc_arm64_neon128(ctypes.Structure):
#     """128-bit neon register"""
#     _fields_ = [
#         ("low_qword", ctypes.c_uint64),
#         ("high_qword", ctypes.c_uint64),
#     ]

# # Subclassing ref to allow property assignment.
# class UcRef(weakref.ref):
#     pass

# This class tracks Uc instance destruction and releases handles.
# class UcCleanupManager(object):
#     def __init__(self):
#         self._refs = {}

#     def register(self, uc):
#         ref = UcRef(uc, self._finalizer)
#         ref._uch = uc._uch
#         self._refs[id(ref)] = ref

#     def _finalizer(self, ref):
#         del self._refs[id(ref)]
#         Uc.release_handle(ref._uch)

class Uc(object):
    # _cleanup = UcCleanupManager()

    def __init__(self, arch, mode):
        # # verify version compatibility with the core before doing anything
        # (major, minor, _combined) = uc_version()
        # if major != uc.UC_API_MAJOR or minor != uc.UC_API_MINOR:
        #     self._uch = None
        #     # our binding version is different from the core's API version
        #     raise UcError(uc.UC_ERR_VERSION)

        self._arch, self._mode = arch, mode
        self._uch = ctypes.c_void_p()
        status = _uc.uc_open(arch, mode, ctypes.byref(self._uch))
        if status != UC_ERR_OK:
            self._uch = None
            raise UcError(status)
        # internal mapping table to save callback & userdata
        self._callbacks = {}
        self._ctype_cbs = {}
        self._callback_count = 0
#        self._cleanup.register(self)

    # @staticmethod
    # def release_handle(uch):
    #     if uch:
    #         try:
    #             status = _uc.uc_close(uch)
    #             if status != UC_ERR_OK:
    #                 raise UcError(status)
    #         except:  # _uc might be pulled from under our feet
    #             pass

    # emulate from @begin, and stop when reaching address @until
    def emu_start(self, begin, until, timeout=0, count=0):
        status = _uc.uc_emu_start(self._uch, begin, until, timeout, count)
        if status != UC_ERR_OK:
            raise UcError(status)

    # # stop emulation
    # def emu_stop(self):
    #     status = _uc.uc_emu_stop(self._uch)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    # return the value of a register
    def reg_read(self, reg_id):
        # read to 64bit number to be safe
        reg = ctypes.c_uint64(0)
        status = _uc.uc_reg_read(self._uch, reg_id, ctypes.byref(reg))
        if status != UC_ERR_OK:
            raise UcError(status)
        return reg.value

    # write to a register
    def reg_write(self, reg_id, value):
        # convert to 64bit number to be safe
        reg = ctypes.c_uint64(value)

        status = _uc.uc_reg_write(self._uch, reg_id, ctypes.byref(reg))
        if status != UC_ERR_OK:
            raise UcError(status)

    # # read from MSR - X86 only
    # def msr_read(self, msr_id):
    #     return self.reg_read(x86_const.UC_X86_REG_MSR, msr_id)

    # # write to MSR - X86 only
    # def msr_write(self, msr_id, value):
    #     return self.reg_write(x86_const.UC_X86_REG_MSR, (msr_id, value))

    # # read data from memory
    # def mem_read(self, address, size):
    #     data = ctypes.create_string_buffer(size)
    #     status = _uc.uc_mem_read(self._uch, address, data, size)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)
    #     return bytearray(data)

    # write to memory
    def mem_write(self, address, data):
        status = _uc.uc_mem_write(self._uch, address, data, len(data))
        if status != UC_ERR_OK:
            raise UcError(status)

    # map a range of memory
    def mem_map(self, address, size, perms=7):
        status = _uc.uc_mem_map(self._uch, address, size, perms)
        if status != UC_ERR_OK:
            raise UcError(status)

    # # map a range of memory from a raw host memory address
    # def mem_map_ptr(self, address, size, perms, ptr):
    #     status = _uc.uc_mem_map_ptr(self._uch, address, size, perms, ptr)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    # # unmap a range of memory
    # def mem_unmap(self, address, size):
    #     status = _uc.uc_mem_unmap(self._uch, address, size)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    # # protect a range of memory
    # def mem_protect(self, address, size, perms=uc.UC_PROT_ALL):
    #     status = _uc.uc_mem_protect(self._uch, address, size, perms)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    # # return CPU mode at runtime
    # def query(self, query_mode):
    #     result = ctypes.c_size_t(0)
    #     status = _uc.uc_query(self._uch, query_mode, ctypes.byref(result))
    #     if status != UC_ERR_OK:
    #         raise UcError(status)
    #     return result.value

    def _hookcode_cb(self, handle, address, size, user_data):
        # call user's callback with self object
        (cb, data) = self._callbacks[user_data]
        cb(self, address, size, data)

    # def _hook_mem_invalid_cb(self, handle, access, address, size, value, user_data):
    #     # call user's callback with self object
    #     (cb, data) = self._callbacks[user_data]
    #     return cb(self, access, address, size, value, data)

    # def _hook_mem_access_cb(self, handle, access, address, size, value, user_data):
    #     # call user's callback with self object
    #     (cb, data) = self._callbacks[user_data]
    #     cb(self, access, address, size, value, data)

    # def _hook_intr_cb(self, handle, intno, user_data):
    #     # call user's callback with self object
    #     (cb, data) = self._callbacks[user_data]
    #     cb(self, intno, data)

    # def _hook_insn_in_cb(self, handle, port, size, user_data):
    #     # call user's callback with self object
    #     (cb, data) = self._callbacks[user_data]
    #     return cb(self, port, size, data)

    # def _hook_insn_out_cb(self, handle, port, size, value, user_data):
    #     # call user's callback with self object
    #     (cb, data) = self._callbacks[user_data]
    #     cb(self, port, size, value, data)

    # def _hook_insn_syscall_cb(self, handle, user_data):
    #     # call user's callback with self object
    #     (cb, data) = self._callbacks[user_data]
    #     cb(self, data)

    # add a hook
    def hook_add(self, htype, callback, user_data=None, begin=1, end=0, arg1=0):
        _h2 = uc_hook_h()

        # save callback & user_data
        self._callback_count += 1
        self._callbacks[self._callback_count] = (callback, user_data)
        cb = None

        if htype == UC_HOOK_INSN:
            insn = ctypes.c_int(arg1)
            if arg1 == x86_const.UC_X86_INS_IN:  # IN instruction
                cb = ctypes.cast(UC_HOOK_INSN_IN_CB(self._hook_insn_in_cb), UC_HOOK_INSN_IN_CB)
            if arg1 == x86_const.UC_X86_INS_OUT:  # OUT instruction
                cb = ctypes.cast(UC_HOOK_INSN_OUT_CB(self._hook_insn_out_cb), UC_HOOK_INSN_OUT_CB)
            if arg1 in (x86_const.UC_X86_INS_SYSCALL, x86_const.UC_X86_INS_SYSENTER):  # SYSCALL/SYSENTER instruction
                cb = ctypes.cast(UC_HOOK_INSN_SYSCALL_CB(self._hook_insn_syscall_cb), UC_HOOK_INSN_SYSCALL_CB)
            status = _uc.uc_hook_add(
                self._uch, ctypes.byref(_h2), htype, cb,
                ctypes.cast(self._callback_count, ctypes.c_void_p),
                ctypes.c_uint64(begin), ctypes.c_uint64(end), insn
            )
        elif htype == UC_HOOK_INTR:
            cb = ctypes.cast(UC_HOOK_INTR_CB(self._hook_intr_cb), UC_HOOK_INTR_CB)
            status = _uc.uc_hook_add(
                self._uch, ctypes.byref(_h2), htype, cb,
                ctypes.cast(self._callback_count, ctypes.c_void_p),
                ctypes.c_uint64(begin), ctypes.c_uint64(end)
            )
        else:
            if htype in (UC_HOOK_BLOCK, UC_HOOK_CODE):
                # set callback with wrapper, so it can be called
                # with this object as param
                cb = ctypes.cast(UC_HOOK_CODE_CB(self._hookcode_cb), UC_HOOK_CODE_CB)
                status = _uc.uc_hook_add(
                    self._uch, ctypes.byref(_h2), htype, cb,
                    ctypes.cast(self._callback_count, ctypes.c_void_p),
                    ctypes.c_uint64(begin), ctypes.c_uint64(end)
                )
            elif htype & (UC_HOOK_MEM_READ_UNMAPPED |
                          UC_HOOK_MEM_WRITE_UNMAPPED |
                          UC_HOOK_MEM_FETCH_UNMAPPED |
                          UC_HOOK_MEM_READ_PROT |
                          UC_HOOK_MEM_WRITE_PROT |
                          UC_HOOK_MEM_FETCH_PROT):
                cb = ctypes.cast(UC_HOOK_MEM_INVALID_CB(self._hook_mem_invalid_cb), UC_HOOK_MEM_INVALID_CB)
                status = _uc.uc_hook_add(
                    self._uch, ctypes.byref(_h2), htype, cb,
                    ctypes.cast(self._callback_count, ctypes.c_void_p),
                    ctypes.c_uint64(begin), ctypes.c_uint64(end)
                )
            else:
                cb = ctypes.cast(UC_HOOK_MEM_ACCESS_CB(self._hook_mem_access_cb), UC_HOOK_MEM_ACCESS_CB)
                status = _uc.uc_hook_add(
                    self._uch, ctypes.byref(_h2), htype, cb,
                    ctypes.cast(self._callback_count, ctypes.c_void_p),
                    ctypes.c_uint64(begin), ctypes.c_uint64(end)
                )

        # save the ctype function so gc will leave it alone.
        self._ctype_cbs[self._callback_count] = cb

        if status != UC_ERR_OK:
            raise UcError(status)

        return _h2.value

    # # delete a hook
    # def hook_del(self, h):
    #     _h = uc_hook_h(h)
    #     status = _uc.uc_hook_del(self._uch, _h)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)
    #     h = 0

    # def context_save(self):
    #     ptr = ctypes.cast(0, ctypes.c_voidp)
    #     status = _uc.uc_context_alloc(self._uch, ctypes.byref(ptr))
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    #     status = _uc.uc_context_save(self._uch, ptr)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    #     return SavedContext(ptr)

    # def context_update(self, context):
    #     status = _uc.uc_context_save(self._uch, context.pointer)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    # def context_restore(self, context):
    #     status = _uc.uc_context_restore(self._uch, context.pointer)
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    # # this returns a generator of regions in the form (begin, end, perms)
    # def mem_regions(self):
    #     regions = ctypes.POINTER(_uc_mem_region)()
    #     count = ctypes.c_uint32()
    #     status = _uc.uc_mem_regions(self._uch, ctypes.byref(regions), ctypes.byref(count))
    #     if status != UC_ERR_OK:
    #         raise UcError(status)

    #     try:
    #         for i in range(count.value):
    #             yield (regions[i].begin, regions[i].end, regions[i].perms)
    #     finally:
    #         _uc.uc_free(regions)


# class SavedContext(object):
#     def __init__(self, pointer):
#         self.pointer = pointer

#     def __del__(self):
#         status = _uc.uc_free(self.pointer)
#         if status != UC_ERR_OK:
#             raise UcError(status)

# # print out debugging info
# def debug():
#     archs = {
#         "arm": uc.UC_ARCH_ARM,
#         "arm64": uc.UC_ARCH_ARM64,
#         "mips": uc.UC_ARCH_MIPS,
#         "sparc": uc.UC_ARCH_SPARC,
#         "m68k": uc.UC_ARCH_M68K,
#         "x86": uc.UC_ARCH_X86,
#     }

#     all_archs = ""
#     keys = archs.keys()
#     for k in sorted(keys):
#         if uc_arch_supported(archs[k]):
#             all_archs += "-%s" % k

#     major, minor, _combined = uc_version()

#     return "python-%s-c%u.%u-b%u.%u" % (
#         all_archs, major, minor, uc.UC_API_MAJOR, uc.UC_API_MINOR
#     )
