-include ../kconfig/.config
LIBS-y:=
LIBS-$(LIB_MONGOOSE)+=libmongoose.a
LIBS-$(LIB_WOLFSSL)+=libwolfssl.a



all: $(LIBS-y)
	echo "Building $(LIBS-y)"


libwolfssl.a: 
	make -C wolfssl -f ../Makefile.wolfssl

mongoose.o: mongoose/mongoose.c
	arm-frosted-eabi-gcc -c -o $@ $^ -D__unix__ -DMG_DISABLE_POPEN

libmongoose.a: mongoose.o
	arm-frosted-eabi-ar cr $@ $^
	arm-frosted-eabi-ranlib $@
	cp -a mongoose/mongoose.h include/


clean: 
	make -C wolfssl -f ../Makefile.wolfssl clean
	rm -f *.a *.o
	rm -rf include/*
