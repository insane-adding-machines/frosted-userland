CROSS_COMPILE?=arm-frosted-eabi-
CC:=$(CROSS_COMPILE)gcc
LDFLAGS+=-fPIC -mlong-calls -fno-common -Wl,-elf2flt -lgloss

CFLAGS+=-mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -mcpu=cortex-m3
CFLAGS+=-DCORE_M3 -D__frosted__
# COMPILER FLAGS -- No gcc libraries
CFLAGS+=-nostartfiles
# COMPILER FLAGS -- GOT/PIC
CFLAGS+=-fPIC -mlong-calls -fno-common -msingle-pic-base -mno-pic-data-is-text-relative -Wstack-usage=1024


OBJS:=src/chdir.o src/dirname.o src/mem.o src/pthreads.o  src/tests.o

ARCH?=stm32

CFLAGS-y:=-I. $(EXTRA_CFLAGS) -I./include

all: tests

tests: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS-y) -o $@ $< $(EXTRA_CFLAGS) $(PLATFORM_CFLAGS)

clean:
	rm -f $(OBJS)
