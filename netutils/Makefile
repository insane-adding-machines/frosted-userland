-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-gcc
APPS-$(APP_IFCONFIG)+=ifconfig
APPS-$(APP_ROUTE)+=route
APPS-$(APP_TELNETD)+=telnetd
APPS-$(APP_NETCAT)+=nc
APPS-$(APP_HOST)+=host
#APPS-$(APP_HTTPD)+=https
APPS-$(APP_STARWARS)+=starwars
APPS-$(APP_NTPC)+=ntpc
APPS-$(APP_SSHD)+=sshd
APPS-$(APP_PING)+=ping

LIBS-$(LIB_MONGOOSE)+=../lib/libmongoose.a
LIBS-$(LIB_WOLFSSL)+= ../lib/libssl.a
CFLAGS-$(LIB_WOLFSSL)+=-I../lib/include/ -DDEBUG_WOLFSSH

BIN:=$(patsubst %,../out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)

../out/sshd:
	make -C wolfssh -f ../Makefile.wolfssh
	cp sshd ../out
	cp sshd.gdb ../gdb

../out/https: https.o
	mkdir -p ../out
	mkdir -p ../gdb
	echo $(LDFLAGS)
	@arm-frosted-eabi-gcc -o $@ $(LDFLAGS) $(CFLAGS) $(CFLAGS-y) ../lib/wolfssl/src/*.o ../lib/wolfssl/wolfcrypt/src/*.o $^
	cp https ../out
	cp https.gdb ../gdb

../out/%: %.o
	echo $(LDFLAGS)
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(CFLAGS-y) $(LDFLAGS) $(LIBS-y) -lm


clean:
	@rm -f $(OBJ)
	@make -C wolfssh -f ../Makefile.wolfssh clean




