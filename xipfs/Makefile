all: xipfstool xipfs-ls


xipfstool: xipfs.c
	gcc -o $@ $^ -I. -I../frosted-headers/include/sys/fs

xipfs-ls: xipfs-ls.c
	gcc -o $@ $^ -I. -I../frosted-headers/include/sys/fs

clean:
	rm -f xipfstool xipfs-ls
