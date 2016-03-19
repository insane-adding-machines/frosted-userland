EXTRA-y:=mini.fatfs

TGT:=$(patsubst %,../out/%,$(EXTRA-y))

all: $(TGT)


../out/%: %
	cp $< ../out

clean:
	echo
