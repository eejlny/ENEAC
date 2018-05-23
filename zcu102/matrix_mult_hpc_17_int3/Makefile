LIBRARY = libkernelMatrixmult.a

PLATFORM = /home/programs/Xilinx2/SDx/2017.1/platforms/zcu102_es2_ig/zcu102_es2
SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	 -sds-hw mmult_top matrix_mult_accel.cpp -clkid 1 -sds-end\
	-Wno-unused-label -Wno-unused-function

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
	vivado -mode batch  -source vivado_int.tcl
	bootgen -arch zynqmp -image boot_bin.bif -w -process_bitstream bin
	#scp /home/eejlny/projects/sdsoc/matrix_mult_hpc_17_int2/_sds/p0/ipi/zcu102_es2.runs/impl_1/zcu102_es2_wrapper.bit.bin root@192.168.0.197:/root

clean:
	${RM} ${LIBRARY} matrix_mult_accel.o kernelMatrixmult.o  kernelMatrixmult.linked

ultraclean: clean
	${RM}  kernelMatrixmult.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
