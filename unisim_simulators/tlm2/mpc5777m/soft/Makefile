S32DS         = /opt/NXP/S32DS_Power_v1.2/S32DS
CROSS_COMPILE = powerpc-eabivle-
CC            = $(CROSS_COMPILE)gcc
RM            = rm -f
CP            = cp

CFLAGS    = -I.
OPTFLAGS ?= -O1
CFLAGS    = -g3 $(OPTFLAGS) -fdebug-prefix-map=$(PWD)=../ # -Wall
LDFLAGS   = -static

LIBS      =
SRCS      = src/flashrchw.c src/main_Z4_2.c src/intc.c src/stm.c
ASMS      = src/startup.S src/intc_sw_handlers.S src/vector.S

COBJS     = $(patsubst %.c,%.o,$(SRCS))
AOBJS     = $(patsubst %.S,%.o,$(ASMS))
OBJS      = $(COBJS) $(AOBJS)
DEPS      = $(patsubst %.o,%.d,$(OBJS))

EXE       = bin/boot.elf

FPRODS    =

LINKER_ARGS= \
-T src/57xx_ram.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

COMPILER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-fmessage-length=0 \
-fsigned-char \
-ffunction-sections \
-fdata-sections \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

ASSEMBLER_ARGS= \
-Iinclude \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-DHW_INIT \
-DI_CACHE \
-DDISABLE_SWT2 \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
--sysroot="$(S32DS)/e200_ewl2"

#-DDISABLE_SWT2 \


.PHONY: all
all: $(EXE)

FPRODS += $(EXE)
$(EXE): $(OBJS)
	$(CC) $(LINKER_ARGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(COBJS): %.o: %.c
	@mkdir -p `dirname $@`
	$(CC) $(COMPILER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(AOBJS): %.o: %.S
	@mkdir -p `dirname $@`
	$(CC) $(ASSEMBLER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<


.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) $(FPRODS)
	$(RM) $(DEPS)

-include $(DEPS)

