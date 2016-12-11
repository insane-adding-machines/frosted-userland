all: xipfstool xipfs-ls


xipfstool: xipfs.c
	gcc -o $@ $^ -I. -I../frosted-headers/include

xipfs-ls: xipfs-ls.c
	gcc -o $@ $^ -I. -I../frosted-headers/include

clean:
	rm -f xipfstool xipfs-ls
