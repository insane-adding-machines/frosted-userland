-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-cc
APPS-y:=

APPS-$(APP_RANDOM)+=random
APPS-$(APP_SENSOR)+=sensor
APPS-$(APP_MORSE)+=morse
APPS-$(APP_FBTEST)+=fbtest
APPS-$(APP_BUSYLOOP)+=busyloop
APPS-$(APP_GPIOCTL)+=gpioctl
APPS-$(APP_LATENCY)+=latency
BIN:=$(patsubst %,../out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)

../out/%: %.o
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f $(OBJ)
