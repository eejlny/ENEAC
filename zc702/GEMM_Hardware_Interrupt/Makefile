LIBRARY = libkernelMatrixmult.a

PLATFORM = /space/eejlny/projects/sdsoc/framework/platform/zc702_led
SDSFLAGS = -sds-pf ${PLATFORM} \
	 -sds-hw mmult_top matrix_mult_accel.cpp -sds-end\
	-Wno-unused-label -Wno-unused-function

#SDSFLAGS = -sds-pf ${PLATFORM} \
#	-Wno-unused-label -Wno-unused-function

DEPS = matrix_mult.h

#SDSFLAGS = -sds-pf ${PLATFORM} \
#	-Wno-unused-label -Wno-unused-function


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

clean:
	${RM} ${LIBRARY} matrix_mult_accel.o kernelMatrixmult.o  kernelMatrixmult.linked

ultraclean: clean
	${RM}  kernelMatrixmult.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
