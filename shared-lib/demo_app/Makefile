
GCC = ./frosted-bflt-gcc
LD = ./frosted-bflt-ld
AR = arm-frosted-eabi-ar
NM = arm-frosted-eabi-nm
OBJCOPY = arm-frosted-eabi-objcopy

GCCFLAGS = -Wall -W
LDFLAGS = -Wl,-Map=$(EXE).Map

EXE := test.bflt
OBJS := main.o
LIBS := lib3.so
LIBPATH := ../demo_lib/bin

LIBLINKOBJ := $(EXE:.bflt=.bflt.o)
WRAPFLAGS :=

vpath lib%.so $(LIBPATH)
vpath lib%.so.gdb $(LIBPATH)
#vpath lib%_wraps.o $(LIBPATH)

# Everything below here probably doesn't need to be touched

#all: wrapper_flags $(EXE)
all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

lib%.so.gdb:
	$(error Missing symbol file $@, build the library first)

#lib%_wraps.o:
#	$(error Missing wrap file $@)

# Generate list of exported symbols from shared libraries and convert them to linker flags
wrapper_flags: $(LIBS:.so=.so.gdb)
	$(eval WRAPFLAGS := $(shell $(NM) -g $(^) | awk '/.+ .+ .+/ {printf "-Wl,--wrap,%s ", $$3}' ))

# Group all shared library symbols into one file
$(LIBLINKOBJ): $(LIBS:%.so=%.so.gdb)
	$(GCC) $(^:%=-Wl,-R,%) -r -o $@ $(LDFLAGS)

# Link everything together
#$(EXE): $(LIBLINKOBJ) $(OBJS) $(LIBS:.so=_wraps.o)
$(EXE): $(LIBLINKOBJ) $(OBJS)
	$(LD) $^ -o $@ $(WRAPFLAGS) $(LDFLAGS)

clean:
	rm -f *.o $(EXE) $(EXE).Map $(EXE).gdb
