# this makefile is to compile apps as a separate bFLT executable
#
#
# PATH CONFIG
FROSTED?=..
PREFIX:=$(PWD)/build

-include kconfig/.config
# TOOLCHAIN CONFIG
CROSS_COMPILE?=arm-frosted-eabi-
CC:=$(CROSS_COMPILE)gcc
AS:=$(CROSS_COMPILE)as
AR:=$(CROSS_COMPILE)ar

ifneq ($(V),1)
   Q:=@
   #Do not print "Entering directory ...".
   MAKEFLAGS += --no-print-directory
endif


#Applications selection
DIR-y+=lib binutils hw-utils netutils games


# COMPILER FLAGS -- Target CPU
CFLAGS+=-mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -mcpu=cortex-m3
CFLAGS+=-DCORE_M3 -D__frosted__
# COMPILER FLAGS -- No gcc libraries
CFLAGS+=-nostartfiles
# COMPILER FLAGS -- GOT/PIC
CFLAGS+=-fPIC -mlong-calls -fno-common -msingle-pic-base -mno-pic-data-is-text-relative -Wstack-usage=1024
# Debugging
CFLAGS+=-ggdb
CFLAGS+=-I../lib/include -I../lib/wolfssl

CFLAGS-$(LIB_WOLFSSL)+=-DENABLE_SSL -DMG_ENABLE_SSL -DWOLFSSL_FROSTED
CFLAGS+=$(CFLAGS-y)

# LINKER FLAGS
LDFLAGS+=-fPIC -mlong-calls -fno-common -Wl,-elf2flt -lgloss

all: apps.img
	cp apps.img $(FROSTED)/

xipfstool: xipfs
	make -C $^
	mv $^/xipfstool .

apps.img: $(APPS-y) $(DIR-y) sh xipfstool
	mv out/*.gdb gdb/ 2>/dev/null || true
	./xipfstool $@ $(APPS-y) out/*

binutils: FORCE
	mkdir -p out
	mkdir -p gdb
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

lib: FORCE
	mkdir -p out
	mkdir -p gdb
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

hw-utils: FORCE
	mkdir -p out
	mkdir -p gdb
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

netutils: FORCE
	mkdir -p out
	mkdir -p gdb
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS) -D__USE_MISC" CC=$(CC)

games: FORCE
	mkdir -p out
	mkdir -p gdb
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

sh: FORCE
	mkdir -p out
	cp $@/* out/


FORCE:

menuconfig:
	@$(MAKE) -C kconfig/ menuconfig -f Makefile.frosted

clean:
	$(foreach d,$(DIR-y),make -C $(d) clean;)
	@rm -f *.img
	@rm -f *.o
	@rm -rf gdb out
	@make -C xipfs clean
	@rm -f xipfstool
