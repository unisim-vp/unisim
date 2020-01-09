###################################
# UNIPY UniSIM-VP python bindings #
# with support for                #
# - ARMv7 (arm32 and thumb)       #
# - PPC (vle)                     #
###################################

import ctypes

emuerr = ctypes.c_int
emu_engine = ctypes.c_void_p

_so = None

def bind( shared_object ):
    if not shared_object.endswith('.so'):
        raise Exception('bad cannot locate fuzr shared object (either define VLE4FUZR_SO or pass an argument)')
    global _so
    _so = ctypes.cdll.LoadLibrary(shared_object)
    if _so is None:
        raise ImportError("ERROR: fail to load the dynamic library.")

    # All functions return an error status as int, we just specify their argument types
    _so.emu_open_arm   .argtype = (ctypes.POINTER(emu_engine),ctypes.c_uint)
    _so.emu_open_vle   .argtype = (ctypes.POINTER(emu_engine))
    _so.emu_start      .argtype = (emu_engine, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint64)
    _so.emu_stop       .argtype = (emu_engine,)
    _so.emu_close      .argtype = (emu_engine,)
    _so.emu_reg_read   .argtype = (emu_engine, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_int, ctypes.POINTER(ctypes.c_uint64))
    _so.emu_reg_write  .argtype = (emu_engine, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_int, ctypes.c_uint64)
    _so.emu_mem_map    .argtype = (emu_engine, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p)
    _so.emu_mem_unmap  .argtype = (emu_engine, ctypes.c_uint64)
    _so.emu_mem_write  .argtype = (emu_engine, ctypes.c_uint64, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t)
    _so.emu_mem_read   .argtype = (emu_engine, ctypes.c_uint64, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t)
    _so.emu_mem_chprot .argtype = (emu_engine, ctypes.c_uint64, ctypes.c_uint)
    _so.emu_mem_chhook .argtype = (emu_engine, ctypes.c_uint64, ctypes.c_uint, ctypes.c_void_p)
    _so.emu_set_disasm .argtype = (emu_engine, ctypes.c_int)
    _so.emu_hook_add   .argtype = (emu_engine, ctypes.c_int, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64)
    _so.emu_page_info  .argtype = (emu_engine, ctypes.c_void_p, ctypes.c_uint64)
    _so.emu_pages_info .argtype = (emu_engine, ctypes.c_void_p)

EMU_ERR_OK = 0

class EmuError(Exception):
    def __init__(self, errno):
        self.errno = errno

    def __str__(self):
        return 'EmuError(%r)' % self.errno

def _EmuCheck(status):
    if status != EMU_ERR_OK:
        raise EmuError(status)

EMU_HOOK_POOL = {}

def _get_managed_c_hook( signature, function ):
    global EMU_HOOK_POOL
    key = (signature, function)
    hook = EMU_HOOK_POOL.get(key)
    if hook is None:
        if function is not None:
            function = signature(function)
        hook = ctypes.cast(function, signature)
        EMU_HOOK_POOL[key] = hook
    return hook

def EMU_close(ctx):
    # Close the emulator @ctx
    status = _so.emu_close(ctx)
    _EmuCheck(status)

def EMU_reg_read(ctx, reg_id):
    # Read a register
    reg = ctypes.c_uint64(0)
    rstr, rnum = reg_id
    status = _so.emu_reg_read(ctx, rstr.encode('ascii'), len(rstr), rnum, ctypes.byref(reg))
    _EmuCheck(status)
    return reg.value

def EMU_reg_write(ctx, reg_id , value):
    # Write a register
    rstr, rnum = reg_id
    status = _so.emu_reg_write(ctx, rstr.encode('ascii'), len(rstr), rnum, value)
    _EmuCheck(status)

# Hook type for memory hooks: hook( ctx, access_type, address, size, endianness, valref )
# The hook is called with:
# - @ctx:         the emulator
# - @access_type: the access type (0: read, 1: write: 2: fetch) 
# - @address:     the memory access address
# - @size:        the memory access size
# - @endianness:  the memory access endianness as a byte ordering mask
# - @valref:      the memory access value as a reference (from ctypes module)
# A return value of any non-zero number will abort the hooked memory access

MEM_HOOK_CBTYPE = ctypes.CFUNCTYPE(ctypes.c_void_p, emu_engine, ctypes.c_uint, ctypes.c_uint64, ctypes.c_uint, ctypes.c_uint, ctypes.POINTER(ctypes.c_uint64))

# class MemHook:
#     CBTYPE = 
#     NULL = ctypes.cast(None, CBTYPE)

#     def __init__(self, dev):
#         self.dev, self.callback = dev, ctypes.cast(self.CBTYPE(self.action), self.CBTYPE)
        
#     def action(self, ctx, access, address, size, endianness, value):
#         if access == 0:
#             res = self.dev.read(ctx, address, size, endianness)
#             if res is None: return value
#             return res
#         elif access == 1:
#             self.dev.write(ctx, address, size, endianness, value)
#         elif access == 2:
#             res = self.dev.fetch(ctx, address, size, endianness)
#             if res is None: return value
#             return res
#         return 0

# WARNING:
#  - a size argument has been added
#  - now using keyword arguments
#    - perms become a keyword arguments, defaulting to 7 (rwx)
#    - rhook, whook and xhook are respectively the read, write and fetch hooks (all defaulting to None)
# INFO:
#  - permissions: bit flags {1=read, 2=write, 4=execute}
#  - pages created with permissions 0 have no storage allocated, and won't accept any later permissions modification (EMU_mem_chprot)
#  - hooks are called whether the corresponding access is allowed or not (by given permissions).
#  - if an access is not allowed and the hook exits, it is called as exception handler (value reference is valid in case of a write).
#  - if a read or a fetch is allowed and the corresponding hooks exists, it is called after the value is read from memory
#  - if a write is allowed and the corresponding hooks exists, it is called before the value is written
#  - hooks have a value reference arguments to change the destination value (source is not affected).

def EMU_mem_init(ctx, address, size, **opts ):
    # Create a page of size @size starting at @address with
    # permissions @perms (defaulting to rwx) and hook (defaulting to None).
    perms = opts.get('perms',7)
    hooks = (_get_managed_c_hook(MEM_HOOK_CBTYPE, opts.get(atp + 'hook', None)) for atp in 'rwx')
    status = _so.emu_mem_map(ctx, address, size, perms, *hooks)
    _EmuCheck(status)

def EMU_mem_prot(ctx, address, new_prot):
    # Change permissions of page at @addr to @ne_prot
    status = _so.emu_mem_chprot(ctx, address, new_prot)
    _EmuCheck(status)

def EMU_mem_hook(ctx, address, new_hook):
    # change page hook
    status = _so.emu_mem_chhook(ctx, address, _get_managed_c_hook(MEM_HOOK_CBTYPE, new_hook))
    _EmuCheck(status)

def EMU_mem_erase(ctx, address):
    # Erase the page containing the byte at @address
    status = _so.emu_mem_unmap(ctx, address)
    _EmuCheck(status)

def EMU_mem_write(ctx, address, data, size=None):
    # Write @size bytes of @data to memory starting at @address. If
    # size is not specified or is None, all bytes of @data are copied.
    # Note: data should be a bytearray or equivalent (b'...'; no
    # encoded string)
    if size is None:
        size = len(data)
    status = _so.emu_mem_write(ctx, address, data, size)
    _EmuCheck(status)

def EMU_mem_read(ctx, address, size):
    # read data from memory
    data = ctypes.create_string_buffer(size)
    status = _so.emu_mem_read(ctx, address, data, size)
    _EmuCheck(status)
    return bytearray(data)

def EMU_start(ctx, begin, until, count=0):
    # Emulate from @begin, and stop when reaching address @until or after @count instructions
    status = _so.emu_start(ctx, begin, until, count)
    _EmuCheck(status)

def EMU_stop(ctx):
    # Stop emulation ASAP (callable from inside hook).
    status = _so.emu_stop(ctx)
    _EmuCheck(status)

def EMU_set_disasm(ctx, disasm):
    # Activate (or deactivate) instruction disassembly according to
    # @disasm
    status = _so.emu_set_disasm(ctx, disasm)
    _EmuCheck(status)

# Hook type for code hooks: hook( ctx, addr, info ):
# The hook is called with:
# - @ctx:         the emulator
# - @address:     the current instruction address
# - @info:        context-dependent info (0 for BLOCK, instruction size for CODE and interrupt number for INTR)
# No return value

CODE_HOOK_CBTYPE = ctypes.CFUNCTYPE(None, emu_engine, ctypes.c_uint64, ctypes.c_uint)

EMU_HOOK_INTR = 1
EMU_HOOK_CODE = 4
EMU_HOOK_BLOCK = 8

class _CodeHook:
    def __init__(self, cb, cbargs):
        self.cb, self.cbargs = cb, cbargs

    def action(self, ctx, address, size):
        self.cb( ctx, address, size, **self.cbargs )

def _get_code_hook(cb, cbargs):
    return _get_managed_c_hook(CODE_HOOK_CBTYPE, _CodeHook(cb, cbargs).action)

def EMU_hook_code(ctx, callback, begin=1, end=0, **cbargs):
    status = _so.emu_hook_add(ctx, EMU_HOOK_CODE, _get_code_hook(callback, cbargs), begin, end)
    _EmuCheck(status)

def EMU_hook_BB(ctx, callback, begin=1, end=0, **cbargs):
    status = _so.emu_hook_add(ctx, EMU_HOOK_BLOCK, _get_code_hook(callback, cbargs), begin, end)
    _EmuCheck(status)

def EMU_hook_excpt(ctx, callback, begin=1, end=0, **cbargs):
    c_callback = _get_managed_c_hook(CodeHook(callback, cbargs).action)
    status = _so.emu_hook_add(ctx, EMU_HOOK_INTR, _get_code_hook(callback, cbargs), begin, end)
    _EmuCheck(status)

# Hook type for page info hooks: hook( first, last, perms, hooks ):
# The hook is called with:
# - @first:    the page first byte address
# - @last:     the page last byte address
# - @perms:    the page permissions (rwx bitset)
# - @hooks:    the present hooks (rwx bitset)
# No return value

EMU_PAGE_INFO_CBTYPE = ctypes.CFUNCTYPE(None, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint, ctypes.c_uint)

def EMU_page_info(ctx, addr, callback):
    cb = ctypes.cast(EMU_PAGE_INFO_CBTYPE(callback), EMU_PAGE_INFO_CBTYPE)
    return _so.emu_page_info(ctx, cb, addr) == 0

def EMU_pages_info(ctx, callback):
    cb = ctypes.cast(EMU_PAGE_INFO_CBTYPE(callback), EMU_PAGE_INFO_CBTYPE)
    status = _so.emu_pages_info(ctx, cb)
    _EmuCheck(status)

######################
# ARM specific stuff #
######################

def EMU_open_arm(is_thumb=False):
    # Create an arm emulator in arm32 mode when @is_thumb is False or in thumb2 mode when @is_thumb is True.
    ctx = emu_engine()
    status = _so.emu_open_arm(ctypes.byref(ctx), int(is_thumb))
    _EmuCheck(status)
    return ctx

EMU_ARM_REG_APSR = ('apsr',0)
def EMU_ARM_REG_R(idx):
    return ('gpr',idx)
EMU_ARM_REG_SP = ('gpr',13)
EMU_ARM_REG_LR = ('gpr',14)
EMU_ARM_REG_PC = ('gpr',15)

######################
# VLE specific stuff #
######################

def EMU_open_vle():
    # Create a vle emulator
    ctx = emu_engine()
    status = _so.emu_open_vle(ctypes.byref(ctx))
    _EmuCheck(status)
    return ctx

def EMU_VLE_REG_R(idx):
    return ('gpr',idx)

EMU_VLE_REG_LR = ('lr',0)
