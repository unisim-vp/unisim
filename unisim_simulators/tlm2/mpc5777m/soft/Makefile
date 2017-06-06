S32DS         = /opt/Freescale/S32_Power_v1.1/S32DS
CROSS_COMPILE = powerpc-eabivle-
CC            = $(CROSS_COMPILE)gcc
RM            = rm -f
CP            = cp

CFLAGS    = -I.
OPTFLAGS ?= -O1
CFLAGS    = -g3 $(OPTFLAGS) # -Wall
LDFLAGS   = -static

LIBS      =
SRCS      = main.c
ASMS      = startup.S

COBJS     = $(patsubst %.c,%.o,$(SRCS))
AOBJS     = $(patsubst %.S,%.o,$(ASMS))
OBJS      = $(COBJS) $(AOBJS)
DEPS      = $(patsubst %.o,%.d,$(COBJS))

EXE       = boot.elf

FPRODS    =

LINKER_ARGS= \
-T 57xx_ram.ld \
-Xlinker \
--gc-sections \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mhard-float \
--sysroot="$(S32DS)/e200_ewl2"

COMPILER_ARGS= \
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
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
-I"$(S32DS)/e200_ewl2/EWL_C/include" \
-I"$(S32DS)/e200_ewl2/EWL_C/include/pa" \
-DMPC5777M \
-DDISABLE_SWT2 \
-DI_CACHE \
-mcpu=e200z4 \
-specs=ewl_c9x.specs \
-mbig \
-mvle \
-mregnames \
--sysroot="$(S32DS)/e200_ewl2"

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

-include $(DEPS)

