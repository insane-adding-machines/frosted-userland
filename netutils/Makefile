-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-gcc
APPS-$(APP_IFCONFIG)+=ifconfig
APPS-$(APP_ROUTE)+=route
APPS-$(APP_TELNETD)+=telnetd
APPS-$(APP_NETCAT)+=nc
APPS-$(APP_HOST)+=host
APPS-$(APP_HTTPD)+=httpd
APPS-$(APP_STARWARS)+=starwars
APPS-$(APP_NTPC)+=ntpc
APPS-$(APP_SSHD)+=sshd
APPS-$(APP_PING)+=ping

LIBS-$(LIB_MONGOOSE)+=../lib/libmongoose.a
LIBS-$(LIB_WOLFSSL)+=../lib/libssl.a ../lib/libcrypto.a

BIN:=$(patsubst %,../out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)

../out/sshd:
	make -C wolfssh -f ../Makefile.wolfssh
	cp sshd ../out
	cp sshd.gdb ../gdb

../out/%: %.o
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(CFLAGS-y) $(LDFLAGS) $(LIBS-y) -lm


clean:
	@rm -f $(OBJ)
	@make -C wolfssh -f ../Makefile.wolfssh clean




