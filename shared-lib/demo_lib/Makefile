# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

CC = arm-frosted-eabi-gcc
LD = arm-frosted-eabi-gcc
AR = arm-frosted-eabi-ar
NM = arm-frosted-eabi-nm
OBJCOPY = arm-frosted-eabi-objcopy

GCCFLAGS = -Wall -W -ggdb
GCCFLAGS+=-mthumb -mlittle-endian -mthumb-interwork -mcpu=cortex-m3
GCCFLAGS+=-DCORE_M3 -D__frosted__
# COMPILER FLAGS -- No gcc libraries
GCCFLAGS+=-nostartfiles
# COMPILER FLAGS -- GOT/PIC
GCCFLAGS+=-fPIC -mlong-calls -fno-common -mno-pic-data-is-text-relative -Wstack-usage=1024 -msingle-pic-base
# make smaller
#GCCFLAGS+=-ffunction-sections -fdata-sections

LDFLAGS = -nostartfiles -static -fPIC -msingle-pic-base -mlong-calls -mno-pic-data-is-text-relative -fno-common -Wl,-elf2flt -Wl,-Map=$(LIB).Map
#-lgloss 

# Select an ID for your library here
LIBID = 3
LIBOBJS := lib.o

# Everything below here probably doesn't need to be touched
LIB := lib$(LIBID).so
LIBBASE := $(shell printf "%\#.2x000000" $(LIBID))

EXPORTLISTFILE := exports.sym
EXPORTSYMBOLS = $(shell awk '{printf "-G %s ", $$0}' "$(EXPORTLISTFILE)")
EXPORTSYMBOLS_WRAPPED = $(shell awk '{printf "-G __wrap_%s ", $$0}' "$(EXPORTLISTFILE)")

WRAPFILE := lib$(LIBID)_wraps.S
WRAPOBJ  := $(WRAPFILE:.S=.o)
WRAPFLAGS = $(shell awk '{printf "-Wl,--wrap,%s ", $$0}' "$(EXPORTLISTFILE)")

all: bin

print:
	@echo EXPORTSYMBOLS = $(EXPORTSYMBOLS)
	@echo EXPORTSYMBOLS_WRAPPED = $(EXPORTSYMBOLS_WRAPPED)
	@echo WRAPFLAGS = $(WRAPFLAGS)

%.o: %.c
	$(CC) $(GCCFLAGS) $(WRAPFLAGS) -DLIB_ID=$(LIBID) -c $<

$(EXPORTLISTFILE): $(LIBOBJS)
	$(NM) $(LIBOBJS) -g --defined-only | cut -d" " -f 3 | grep -v "_start" > $@_temp
	@awk NF $@_temp > $@_temp2
	@grep -v ":" $@_temp2 > $@
	@rm $@_temp
	@rm $@_temp2

$(LIB): $(LIBOBJS) $(EXPORTLISTFILE) $(WRAPOBJ)
	$(AR) -r $(LIB:.so=.a) $(LIBOBJS) $(WRAPOBJ)
# We use an interesting hack here to stop a huge file from being produced
# Since we're linking at $(LIBBASE) as the base, everything between 0x00000000 and $(LIBBASE)
# will be useless padding that can take up to 4064MB (yes, that's right).
# First we link the library at base 0x00000000 to produce our library (no padding will be generated)
# This will make $(LIB) (our actual library that we send to the calculator)
# and $(LIB).gdb (the file containing the addresses of symbols)
	$(LD) $(LDFLAGS) $(WRAPFLAGS) -Wl,-shared-lib-id,0 -Wl,--whole-archive,$(LIB:.so=.a),--no-whole-archive -o $@

$(LIB).gdb: $(LIB)
# Then we change the base addresses in $(LIB).gdb to the proper library base address $(LIBBASE), this is okay because our code is PIC anyway
	$(OBJCOPY) --change-addresses $(LIBBASE) $(LIB).gdb
# Whitelist symbols to export
	#$(OBJCOPY) $(EXPORTSYMBOLS) $(LIB).gdb
	$(OBJCOPY) $(EXPORTSYMBOLS_WRAPPED) $(LIB).gdb
	@echo =======
	@echo The following symbols will be exported
	@echo
	@$(NM) $(LIB).gdb -g | sort
	@echo =======
	@arm-frosted-eabi-size $(LIB).gdb

$(WRAPOBJ): $(WRAPFILE)
	$(CC) $(GCCFLAGS) $(WRAPFLAGS) -DLIB_ID=$(LIBID) -c $<
	#$(OBJCOPY) --change-addresses $(LIBBASE) $(WRAPOBJ)

$(WRAPFILE):
# Generate wrappers to wrap shared library calls so they don't crash due to relocation problems
# They are basically custom code veneers since the automatically generated ones suck and
# don't get relocated properly
# These wrapper object files must be linked to all executabled using this shared library
	echo ".thumb" > $(WRAPFILE)
	printf ".syntax unified" >> $(WRAPFILE)
#printf ".syntax unified\n.section .text\n" >> $(WRAPFILE)
#	awk '{printf "\n__wrap_%s_addr:\n    .extern %s\n", $$0, $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
#	awk '{printf "\n.extern __real_%s\n", $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
#	awk '{printf "\n__wrap_%s_addr:\n    .word __real_%s\n", $$0, $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
#	awk '{printf "\n__wrap_%s_addr:\n    .word %s\n", $$0, $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
#	awk '{printf "\n    .extern %s\n", $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
	awk '{printf "\n    .extern __real_%s\n", $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
# This will fetch the libs R9 base lib for from the PDT
# and restore the main app's R9 base lib from the PDT
	#awk '{printf ".global __wrap_%s\n__wrap_%s:\n    push {r3}\n    sub r9, #4\n    ldr r9,[r9]\n    mov r3, #%d\n    ldr r9, [r9, r3, LSL #2]\n    ldr r3, =%s\n    mov r12,r3\n    pop {r3}\n    blx r12\n    sub r9, #4\n    ldr r9,[r9]\n    ldr r9, [r9, #0]\n    bx lr\n\n", $$0, $$0, $(LIBID), $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
	awk '{printf ".global __wrap_%s\n__wrap_%s:\n    push {r3}\n    sub r9, #4\n    ldr r12,[r9]\n    mov r3, #%d\n    lsl r3, r3, #4\n    ldr r9, [r12, r3]\n    add r3, #8\n    str lr, [r12, r3]\n    sub r3, #4\n    ldr r12, [r12, r3]\n    ldr r3, =__real_%s\n    add r12, r12, r3\n    pop {r3}\n    blx r12\n    push {r3}\n    sub r9, #4\n    ldr r9,[r9]\n    mov r3,#%d\n    lsl r3,r3, #4\n    add r3, #8\n    ldr lr, [r9, r3]\n    ldr r9, [r9, #0]\n    pop {r3}\n    bx lr\n\n", $$0, $$0, $(LIBID), $$0, $(LIBID)}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)

bin: $(LIB) $(LIB).gdb  $(WRAPOBJ)
	mkdir -p bin
	cp $^ bin

clean:
	rm $(EXPORTLISTFILE)
	rm -rf *.o *.a
	rm -rf $(LIB) $(LIB).gdb $(WRAPFILE)
	rm -rf bin/$(LIB) bin/$(WRAPOBJ) bin/$(LIB).gdb
	-rm -rf bin
