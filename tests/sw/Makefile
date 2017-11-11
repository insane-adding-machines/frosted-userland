-include ../../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-cc
APPS-y:= fstest

APPS-$(APP_PTHREADS)+=pthreads
APPS-$(APP_SEM)+=sem
BIN:=$(patsubst %,../../out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)

../../out/%: %.o
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f $(OBJ)
