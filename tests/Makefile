-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-cc

all: 
	make -C sw/ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)
	make -C hw/ LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

#clean:
#	make -C sw/ clean LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)
#	make -C hw/ clean LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" CC=$(CC)

