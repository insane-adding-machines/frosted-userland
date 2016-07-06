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
APPS-$(APP_MOUNT)+=mount
APPS-$(APP_WC)+=wc
APPS-$(APP_HEAD)+=head
APPS-$(APP_TAIL)+=tail
APPS-$(APP_YES)+=yes
APPS-$(APP_SLEEP)+=sleep
APPS-$(APP_UPTIME)+=uptime
APPS-$(APP_KLOGD)+=klogd
APPS-$(APP_GDBSERVER)+=gdbserver
APPS-$(APP_PLAY)+=play
APPS-$(APP_REBOOT)+=reboot
APPS-$(APP_RENICE)+=renice
APPS-$(APP_CUT)+=cut
APPS-later:=dirname tee
BIN:=$(patsubst %,../out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)

../out/%: %.o
	@arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -rf out
	@rm -f $(OBJ)
