all: xipfstool


xipfstool: xipfs.c
	gcc -o $@ $^ -I. -I../frosted-headers/include

clean:
	rm -f xipfstool
