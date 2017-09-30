# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

CC = arm-frosted-eabi-gcc
LD = arm-frosted-eabi-gcc
AR = arm-frosted-eabi-ar
NM = arm-frosted-eabi-nm
OBJCOPY = arm-frosted-eabi-objcopy

GCCFLAGS = -Wall -W
GCCFLAGS+=-mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -mcpu=cortex-m3
GCCFLAGS+=-DCORE_M3 -D__frosted__
# COMPILER FLAGS -- No gcc libraries
GCCFLAGS+=-nostartfiles
# COMPILER FLAGS -- GOT/PIC
GCCFLAGS+=-fPIC -mlong-calls -fno-common -msingle-pic-base -mno-pic-data-is-text-relative -Wstack-usage=1024

LDFLAGS = -nostartfiles -static -fPIC -mlong-calls -fno-common -Wl,-elf2flt -lgloss

# Select an ID for your library here
LIBID = 3
LIBOBJS := lib.o

# Everything below here probably doesn't need to be touched
LIB := lib$(LIBID).so
LIBBASE := $(shell printf "%\#.2x000000" $(LIBID))

EXPORTLISTFILE := exports.sym
EXPORTSYMBOLS := $(shell awk '{printf "-G %s ", $$0}' "$(EXPORTLISTFILE)")

WRAPFILE := lib$(LIBID)_wraps.S

all: bin

%.o: %.c
	$(CC) $(GCCFLAGS) -DLIB_ID=$(LIBID) -c $<

$(LIB): $(LIBOBJS) $(EXPORTLISTFILE)
	$(AR) -r $(LIB:.so=.a) $(LIBOBJS)

# We use an interesting hack here to stop a huge file from being produced
# Since we're linking at $(LIBBASE) as the base, everything between 0x00000000 and $(LIBBASE)
# will be useless padding that can take up to 4064MB (yes, that's right).

# First we link the library at base 0x00000000 to produce our library (no padding will be generated)
# This will make $(LIB) (our actual library that we send to the calculator)
# and $(LIB).gdb (the file containing the addresses of symbols)
	#$(LD) $(LDFLAGS) -mid-shared-library -Wl,-shared-lib-id,0 -Wl,--whole-archive,$(LIB:.so=.a),--no-whole-archive -o $@
	$(LD) $(LDFLAGS) -Wl,-shared-lib-id,0 -Wl,--whole-archive,$(LIB:.so=.a),--no-whole-archive -o $@

# Then we change the base addresses in $(LIB).gdb to the proper library base address $(LIBBASE)
# This is okay because our code is PIC anyway
	$(OBJCOPY) --change-addresses $(LIBBASE) $(LIB).gdb

# Whitelist symbols to export
	$(OBJCOPY) $(EXPORTSYMBOLS) $(LIB).gdb
	@echo =======
	@echo The following symbols will be exported
	@echo
	@$(NM) $(LIB).gdb -g | sort
	@echo =======

$(LIB).gdb: $(LIB)

$(WRAPFILE:.S=.o): $(WRAPFILE)
	$(CC) $(GCCFLAGS) -DLIB_ID=$(LIBID) -c $<

$(WRAPFILE):
# Generate wrappers to wrap shared library calls so they don't crash due to relocation problems
# They are basically custom code veneers since the automatically generated ones suck and
# don't get relocated properly

# These wrapper object files must be linked to all executabled using this shared library

	echo .section .shlb >> $(WRAPFILE)
	awk '{printf "__wrap_%s_addr: .word __real_%s\n", $$0, $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)
	awk '{printf "__wrap_%s: .global __wrap_%s\n    ldr pc,__wrap_%s_addr\n\n", $$0, $$0, $$0}' "$(EXPORTLISTFILE)" >> $(WRAPFILE)

#bin: $(LIB) $(LIB).gdb $(WRAPFILE:.S=.o)
bin: $(LIB) $(LIB).gdb
	mkdir -p bin
	cp $^ bin

clean:
	rm -rf *.o *.a
	rm -rf $(LIB) $(LIB).gdb $(WRAPFILE)
	rm -rf bin/$(LIB) bin/$(WRAPFILE:.S=.o) bin/$(LIB).gdb
	-rm -rf bin
