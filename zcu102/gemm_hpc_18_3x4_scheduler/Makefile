LIBRARY = libkernelMatrixmult.a

#PLATFORM = /home/programs/Xilinx2/SDx/2017.1/platforms/zcu102_es2_ig/zcu102_es2

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
	 -sds-hw  mmult_top matrix_mult_accel.cpp  -clkid 3 -sds-end\
	-Wno-unused-label -Wno-unused-function



DEPS = matrix_mult.h


#CC = sdscc ${SDSFLAGS}
CC = sds++ ${SDSFLAGS}


CFLAGS = -Wall -O3 -c 
LFLAGS = -O3

.PHONY: all

all: ${LIBRARY}

matrix_mult_accel.o: matrix_mult_accel.cpp $(DEPS)
	${CC} ${CFLAGS} $< -o $@

kernelMatrixmult.o: kernelMatrixmult.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelMatrixmult.linked: matrix_mult_accel.o kernelMatrixmult.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
${LIBRARY}: kernelMatrixmult.linked
	cp _sds/swstubs/${LIBRARY} .
	arm-linux-gnueabihf-ar rcs ${LIBRARY} _sds/swstubs/kernelMatrixmult.o
	ranlib libkernelMatrixmult.a


clean:
	${RM} ${LIBRARY} matrix_mult_accel.o kernelMatrixmult.o  kernelMatrixmult.linked

ultraclean: clean
	${RM}  kernelMatrixmult.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
