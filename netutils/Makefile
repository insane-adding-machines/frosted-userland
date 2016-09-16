-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-gcc
APPS-$(APP_IFCONFIG)+=ifconfig
APPS-$(APP_ROUTE)+=route
APPS-$(APP_TELNETD)+=telnetd
APPS-$(APP_NETCAT)+=nc
APPS-$(APP_HOST)+=host
APPS-$(APP_HTTPD)+=httpd

LIBS-$(LIB_MONGOOSE)+=../lib/libmongoose.a
LIBS-$(LIB_WOLFSSL)+=../lib/libwolfssl.a

BIN:=$(patsubst %,../out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)

../out/%: %.o
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(CFLAGS-y) $(LDFLAGS) $(LIBS-y) -lm

clean:
	@rm -f $(OBJ)




