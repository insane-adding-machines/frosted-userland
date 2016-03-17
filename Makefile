# this makefile is to compile apps as a separate bFLT executable
#
#
-include ../kconfig/.config
-include ../config.mk

# PATH CONFIG
FROSTED?=..
PREFIX:=$(PWD)/build

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
DIR-y+=lib binutils hw-utils netutils extra games


# COMPILER FLAGS -- Target CPU
CFLAGS+=-mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -mcpu=cortex-m3
CFLAGS+=-DCORE_M3 -D__frosted__
# COMPILER FLAGS -- No gcc libraries
CFLAGS+=-nostartfiles
# COMPILER FLAGS -- GOT/PIC
CFLAGS+=-fPIC -mlong-calls -fno-common -msingle-pic-base -mno-pic-data-is-text-relative
# Debugging
CFLAGS+=-ggdb
CFLAGS+=-I../lib/include

# LINKER FLAGS
LDFLAGS+=-fPIC -mlong-calls -fno-common -Wl,-elf2flt -lgloss

all: apps.img
	cp apps.img $(FROSTED)/


apps.img: $(APPS-y) $(DIR-y)
	@mkdir -p gdb
	@mv binutils/out/*.gdb $(PWD)/gdb || true
	@mv hw-utils/out/*.gdb $(PWD)/gdb || true
	@mv netutils/out/*.gdb $(PWD)/gdb || true
	$(FROSTED)/tools/xipfstool $@ $(APPS-y) $(addsuffix ,$(DIR-y))

binutils: FORCE
	mkdir -p $@/out
	touch $@/out/dummy
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

lib: FORCE
	mkdir -p $@/out
	touch $@/out/dummy
	make -C $@

hw-utils: FORCE
	mkdir -p $@/out
	touch $@/out/dummy
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

netutils: FORCE
	mkdir -p $@/out
	touch $@/out/dummy
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

games: FORCE
	mkdir -p $@/out
	touch $@/out/dummy
	make -C $@ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

FORCE:

menuconfig:
	@$(MAKE) -C kconfig/ menuconfig -f Makefile.frosted

clean:
	@make -C binutils clean
	@make -C netutils clean
	@make -C hw-utils clean
	@make -C games clean
	@make -C lib clean
	@rm -f $(APPS-y)
	@rm -f *.img
	@rm -f *.o
	@rm -f *.gdb
