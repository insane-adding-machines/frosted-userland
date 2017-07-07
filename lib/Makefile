-include ../kconfig/.config
LIBS-y:=
LIBS-$(LIB_WOLFSSL)+=libssl.a libcrypto.a
LIBS-$(LIB_MONGOOSE)+=libmongoose.a



all: $(LIBS-y)
	echo "Building $(LIBS-y)"


libssl.a: 
	make -C wolfssl -f ../Makefile.wolfssl

libcrypto.a:
	make -C wolfssl -f ../Makefile.wolfssl crypto

mongoose.o: mongoose/mongoose.c
	arm-frosted-eabi-gcc -c -o $@ $^ -D__unix__ -DMG_DISABLE_POPEN -g -ggdb -O0 -DCS_DISABLE_STDIO -DFROSTED -DMG_DISABLE_SOCKETPAIR -DMG_DISABLE_WS_RANDOM_MASK -DMG_DISABLE_PFS $(CFLAGS) 

libmongoose.a: mongoose.o
	arm-frosted-eabi-ar cr $@ $^
	arm-frosted-eabi-ranlib $@
	mkdir -p include
	cp -a mongoose/mongoose.h include/


clean: 
	make -C wolfssl -f ../Makefile.wolfssl clean
	rm -f *.a *.o
	rm -rf include/*
