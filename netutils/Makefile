-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-gcc
APPS-$(APP_IFCONFIG)=ifconfig
APPS-$(APP_TELNETD)=telnetd
APPS-$(APP_NETCAT)=nc

BIN:=$(patsubst %,out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)

out/%: %.o
	@mkdir -p out
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@mkdir -p out/gdb
	@mv out/*.gdb out/gdb

clean:
	@rm -rf out
	@rm -f $(OBJ)




