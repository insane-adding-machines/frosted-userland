-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-cc
APPS-y:=

APPS-$(APP_DICE)+=dice
BIN:=$(patsubst %,out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)


out/%: %.o
	@mkdir -p out
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@rm -f out/*.gdb

clean:
	@rm -rf out
	@rm -f $(OBJ)
