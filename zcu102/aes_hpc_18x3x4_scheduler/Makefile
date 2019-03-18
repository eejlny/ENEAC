LIBRARY = libkernelAES.a


PLATFORM = zcu102

#SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
#	-Wno-unused-label -Wno-unused-function

#SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
#	 -sds-hw  mmult_top1 matrix_mult_accel.cpp  -clkid 3 -sds-end\
#	 -sds-hw  mmult_top2 matrix_mult_accel.cpp  -clkid 3 -sds-end\
#	 -sds-hw  mmult_top3 matrix_mult_accel.cpp  -clkid 3 -sds-end\
#	 -sds-hw  mmult_top4 matrix_mult_accel.cpp  -clkid 3 -sds-end\
#	-Wno-unused-label -Wno-unused-function

SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	 -sds-hw  aes_enc aes_enc.cpp  -clkid 3 -sds-end\
	-Wno-unused-label -Wno-unused-function



DEPS = aes_enc.h


#CC = sdscc ${SDSFLAGS}
CC = sds++ ${SDSFLAGS}


CFLAGS = -Wall -O3 -c 
LFLAGS = -O3

.PHONY: all

all: ${LIBRARY}

aes_enc.o: aes_enc.cpp $(DEPS)
	${CC} ${CFLAGS} $< -o $@

kernelAES.o: kernelAES.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelAES.linked: aes_enc.o kernelAES.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
${LIBRARY}: kernelAES.linked
	cp _sds/swstubs/${LIBRARY} .
	arm-linux-gnueabihf-ar rcs ${LIBRARY} _sds/swstubs/kernelAES.o
	ranlib libkernelAES.a


clean:
	${RM} ${LIBRARY} aes_enc.o kernelAES.o  kernelAES.linked

ultraclean: clean
	${RM}  kernelAES.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
