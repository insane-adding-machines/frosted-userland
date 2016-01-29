-include ../kconfig/.config
CFLAGS?=
CC?=arm-frosted-eabi-cc
APPS-y:=
APPS-$(APP_INIT)+=init
APPS-$(APP_IDLING)+=idling
APPS-$(APP_FRESH)+=fresh
APPS-$(APP_CAT)+=cat
APPS-$(APP_ECHO)+=echo
APPS-$(APP_LS)+=ls
APPS-$(APP_TOUCH)+=touch
APPS-$(APP_RM)+=rm
APPS-$(APP_KILL)+=kill
APPS-$(APP_TRUE)+=true
APPS-$(APP_FALSE)+=false
APPS-$(APP_LN)+=ln
APPS-$(APP_MKDIR)+=mkdir
APPS-later:=cut dirname wc tee 
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




