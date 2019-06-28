# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 20 Jun 2019
# Description - Makefile to generate interrupt-enabled SPMM platform in sdsoc

LIBRARY = libkernelspmm.a
PLATFORM = /home/programs/SDx/2018.3/platforms/zcu102_mig4/zcu102

hpc: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw spmm_fast_hpc0 spmm_hphpc.cpp -clkid 0 -sds-end\
	-sds-hw spmm_fast_hpc1 spmm_hphpc.cpp -clkid 0 -sds-end\
	-sds-hw spmm_fast_hpc2 spmm_hphpc.cpp -clkid 0 -sds-end\
	-sds-hw spmm_fast_hpc3 spmm_hphpc.cpp -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function     
hp: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw spmm_fast_hp0 spmm_hphpc.cpp -clkid 0 -sds-end\
	-sds-hw spmm_fast_hp1 spmm_hphpc.cpp -clkid 0 -sds-end\
	-sds-hw spmm_fast_hp2 spmm_hphpc.cpp -clkid 0 -sds-end\
	-sds-hw spmm_fast_hp3 spmm_hphpc.cpp -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function

hpc hp: ${LIBRARY}

all: hp hpc

.PHONY: all


DEPS = spmm.h

CC = sds++ ${SDSFLAGS}

CFLAGS = -Wall -O3 -c 
LFLAGS = -O3
     
spmm_hphpc.o: spmm_hphpc.cpp $(DEPS)
	${CC} ${CFLAGS} $< -o $@  
     
kernelspmm.o: kernelspmm.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelspmm.linked: spmm_hphpc.o kernelspmm.o
	${CC} $^ -nostartfiles -o $@

# Add the interface functions to the library
${LIBRARY}: kernelspmm.linked
	cp _sds/swstubs/${LIBRARY} .
	arm-linux-gnueabihf-ar rcs ${LIBRARY} _sds/swstubs/kernelspmm.o
	ranlib libkernelspmm.a       
     
clean:
	${RM} *.a *.o *.linked

ultraclean: clean
	${RM}  kernelspmm.linked.bit 
	${RM} -rf _sds sd_card