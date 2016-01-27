CFLAGS?=
CC?=arm-frosted-eabi-cc
APPS-y:=cat echo ls touch
APPS-later:= cat echo false kill ln ls mkdir rm touch true 
APPS-later+=cut dirname wc tee 
BIN:=$(patsubst %,out/%,$(APPS-y))
OBJ:=$(patsubst %,%.o,$(APPS-y))

all: $(BIN)



out/%: %.o
	mkdir -p out
	arm-frosted-eabi-gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)
	rm -f out/*.gdb

clean:
	rm -rf out
	rm -f $(OBJ)




