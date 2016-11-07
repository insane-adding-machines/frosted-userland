-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-cc
APPS-y:=

APPS-$(APP_DICE)+=dice
APPS-$(APP_BYTEBEAT)+=bytebeat
APPS-$(APP_GROOVY)+=groovy
BIN:=$(patsubst %,../out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

DOOM-$(APP_DOOM):=../out/fdoom ../../doom1.wad

all: $(BIN) $(DOOM-y)

../out/fdoom: DOOM/frosted-doom/fdoom
	mv $^ $@

DOOM/frosted-doom/fdoom:
	make -C DOOM/frosted-doom
	@echo "*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***"
	@echo "***                                                                             ***"
	@echo "***   Please copy doom1.wad to an SD card and insert it in your Frosted device  ***"
	@echo "***                                                                             ***"
	@echo "*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***"

../../doom1.wad:
	wget http://distro.ibiblio.org/pub/linux/distributions/slitaz/sources/packages/d/doom1.wad  -O $@


../out/%: %.o
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f $(OBJ)
