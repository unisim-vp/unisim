S32DS         = /opt/NXP/S32DS_Power_v1.2/S32DS
CROSS_COMPILE = powerpc-eabivle-
CC            = $(CROSS_COMPILE)gcc
RM            = rm -f
CP            = cp

CFLAGS    = -I.
OPTFLAGS ?= -O1
CFLAGS    = -g3 $(OPTFLAGS) -fdebug-prefix-map=$(PWD)=../../ # -Wall
LDFLAGS   = -static

DRV_SRC        = src/drv/intc.c src/drv/stm.c src/drv/swt.c src/drv/pit.c

Z4_2_LIBS      =
Z4_2_SRCS      = src/Z4_2/flashrchw.c src/Z4_2/main_Z4_2.c $(DRV_SRC)
Z4_2_ASMS      = src/Z4_2/startup.S src/Z4_2/intc_sw_handlers.S src/Z4_2/vector.S
Z4_2_EXE_RAM   = bin/Z4_2/ram_boot.elf
Z4_2_EXE_FLASH = bin/Z4_2/flash_boot.elf

Z7_0_LIBS      =
Z7_0_SRCS      = src/Z7_0/main_Z7_0.c $(DRV_SRC)
Z7_0_ASMS      = src/Z7_0/startup.S src/Z7_0/intc_sw_handlers.S src/Z7_0/vector.S
Z7_0_EXE_RAM   = bin/Z7_0/ram_boot.elf
Z7_0_EXE_FLASH = bin/Z7_0/flash_boot.elf

Z7_1_LIBS      =
Z7_1_SRCS      = src/Z7_1/main_Z7_1.c $(DRV_SRC)
Z7_1_ASMS      = src/Z7_1/startup.S src/Z7_1/intc_sw_handlers.S src/Z7_1/vector.S
Z7_1_EXE_RAM   = bin/Z7_1/ram_boot.elf
Z7_1_EXE_FLASH = bin/Z7_1/flash_boot.elf


Z4_2_COBJS     = $(patsubst %.c,obj/Z4_2/%.o,$(Z4_2_SRCS))
Z4_2_AOBJS     = $(patsubst %.S,obj/Z4_2/%.o,$(Z4_2_ASMS))
Z4_2_OBJS      = $(Z4_2_COBJS) $(Z4_2_AOBJS)
Z4_2_DEPS      = $(patsubst %.o,%.d,$(Z4_2_OBJS))

Z7_0_COBJS     = $(patsubst %.c,obj/Z7_0/%.o,$(Z7_0_SRCS))
Z7_0_AOBJS     = $(patsubst %.S,obj/Z7_0/%.o,$(Z7_0_ASMS))
Z7_0_OBJS      = $(Z7_0_COBJS) $(Z7_0_AOBJS)
Z7_0_DEPS      = $(patsubst %.o,%.d,$(Z7_0_OBJS))

Z7_1_COBJS     = $(patsubst %.c,obj/Z7_1/%.o,$(Z7_1_SRCS))
Z7_1_AOBJS     = $(patsubst %.S,obj/Z7_1/%.o,$(Z7_1_ASMS))
Z7_1_OBJS      = $(Z7_1_COBJS) $(Z7_1_AOBJS)
Z7_1_DEPS      = $(patsubst %.o,%.d,$(Z7_1_OBJS))

EXECUTABLES =
OBJS        =
DEPS        =

Z4_2_RAM_LINKER_ARGS= \
-T src/Z4_2/57xx_ram.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z4_2_FLASH_LINKER_ARGS= \
-T src/Z4_2/57xx_flash.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z4_2_COMPILER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-fmessage-length=0 \
-fsigned-char \
-ffunction-sections \
-fdata-sections \
-fstrict-volatile-bitfields \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z4_2_ASSEMBLER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-DHW_INIT \
-DI_CACHE \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
--sysroot="$(S32DS)/e200_ewl2"

Z7_0_RAM_LINKER_ARGS= \
-T src/Z7_0/57xx_ram.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z7_0_FLASH_LINKER_ARGS= \
-T src/Z7_0/57xx_flash.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z7_0_COMPILER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-fmessage-length=0 \
-fsigned-char \
-ffunction-sections \
-fdata-sections \
-fstrict-volatile-bitfields \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z7_0_ASSEMBLER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-DI_CACHE \
-DD_CACHE \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
--sysroot="$(S32DS)/e200_ewl2"

Z7_1_RAM_LINKER_ARGS= \
-T src/Z7_1/57xx_ram.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z7_1_FLASH_LINKER_ARGS= \
-T src/Z7_1/57xx_flash.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z7_1_COMPILER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-fmessage-length=0 \
-fsigned-char \
-ffunction-sections \
-fdata-sections \
-fstrict-volatile-bitfields \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

Z7_1_ASSEMBLER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-DI_CACHE \
-DD_CACHE \
-mcpu=e200z7 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
--sysroot="$(S32DS)/e200_ewl2"


# -DDISABLE_SWT2

EXECUTABLES += $(Z4_2_EXE_RAM) $(Z4_2_EXE_FLASH)
EXECUTABLES += $(Z7_0_EXE_RAM) $(Z7_0_EXE_FLASH)
EXECUTABLES += $(Z7_1_EXE_RAM) $(Z7_1_EXE_FLASH)
OBJS += $(Z4_2_OBJS)
OBJS += $(Z7_0_OBJS)
OBJS += $(Z7_1_OBJS)
DEPS += $(Z4_2_DEPS)
DEPS += $(Z7_0_DEPS)
DEPS += $(Z7_1_DEPS)

.PHONY: all
all: $(EXECUTABLES)

$(Z4_2_EXE_RAM): $(Z4_2_OBJS) src/Z4_2/57xx_ram.ld
	$(CC) $(Z4_2_RAM_LINKER_ARGS) $(LDFLAGS) -o $@ $(Z4_2_OBJS) $(Z4_2_LIBS)

$(Z7_0_EXE_RAM): $(Z7_0_OBJS) src/Z7_0/57xx_ram.ld
	$(CC) $(Z7_0_RAM_LINKER_ARGS) $(LDFLAGS) -o $@ $(Z7_0_OBJS) $(Z7_0_LIBS)

$(Z7_1_EXE_RAM): $(Z7_1_OBJS) src/Z7_1/57xx_ram.ld
	$(CC) $(Z7_1_RAM_LINKER_ARGS) $(LDFLAGS) -o $@ $(Z7_1_OBJS) $(Z7_1_LIBS)

$(Z4_2_EXE_FLASH): $(Z4_2_OBJS) src/Z4_2/57xx_flash.ld
	$(CC) $(Z4_2_FLASH_LINKER_ARGS) $(LDFLAGS) -o $@ $(Z4_2_OBJS) $(Z4_2_LIBS)

$(Z7_0_EXE_FLASH): $(Z7_0_OBJS) src/Z7_0/57xx_flash.ld
	$(CC) $(Z7_0_FLASH_LINKER_ARGS) $(LDFLAGS) -o $@ $(Z7_0_OBJS) $(Z7_0_LIBS)

$(Z7_1_EXE_FLASH): $(Z7_1_OBJS) src/Z7_1/57xx_flash.ld
	$(CC) $(Z7_1_FLASH_LINKER_ARGS) $(LDFLAGS) -o $@ $(Z7_1_OBJS) $(Z7_1_LIBS)

$(Z4_2_COBJS): obj/Z4_2/%.o: %.c
	@mkdir -p `dirname $@`
	$(CC) $(Z4_2_COMPILER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(Z7_0_COBJS): obj/Z7_0/%.o: %.c
	@mkdir -p `dirname $@`
	$(CC) $(Z7_0_COMPILER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(Z7_1_COBJS): obj/Z7_1/%.o: %.c
	@mkdir -p `dirname $@`
	$(CC) $(Z7_1_COMPILER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(Z4_2_AOBJS): obj/Z4_2/%.o: %.S
	@mkdir -p `dirname $@`
	$(CC) $(Z4_2_ASSEMBLER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(Z7_0_AOBJS): obj/Z7_0/%.o: %.S
	@mkdir -p `dirname $@`
	$(CC) $(Z7_0_ASSEMBLER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(Z7_1_AOBJS): obj/Z7_1/%.o: %.S
	@mkdir -p `dirname $@`
	$(CC) $(Z7_1_ASSEMBLER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) $(EXECUTABLES)
	$(RM) $(DEPS)

-include $(DEPS)

