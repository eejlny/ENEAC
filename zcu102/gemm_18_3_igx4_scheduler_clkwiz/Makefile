# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 16 Jul 2019
# Description - Makefile to generate interrupt-enabled gemm platform in sdsoc

PLATFORM = /home/programs/SDx/2018.3/platforms/zcu102_mig4/zcu102
LIBRARY = libkernelgemm.a

hpc: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw  gemm1_hpc gemm_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  gemm2_hpc gemm_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  gemm3_hpc gemm_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  gemm4_hpc gemm_hphpc.cpp  -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function
hpc:	CONFIG_FLAG = -DHPC
     
hp: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw  gemm1_hp gemm_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  gemm2_hp gemm_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  gemm3_hp gemm_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  gemm4_hp gemm_hphpc.cpp  -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function
hp: CONFIG_FLAG = -DHP

hpc hp: $(LIBRARY)

all: hpc hp

.PHONY: all

DEPS = gemm.h

CC = sds++ $(SDSFLAGS)

CFLAGS = -Wall -O3 -c $(CONFIG_FLAG)

gemm_hphpc.o: gemm_hphpc.cpp $(DEPS)
	${CC} ${CFLAGS} $< -o $@

kernelgemm.o: kernelgemm.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelgemm.linked: gemm_hphpc.o kernelgemm.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
$(LIBRARY): kernelgemm.linked
	cp _sds/swstubs/$(LIBRARY) .
	arm-linux-gnueabihf-ar rcs $(LIBRARY) _sds/swstubs/kernelgemm.o
	ranlib $(LIBRARY)
     
clean:
	${RM} *.a *.o *.linked

ultraclean: clean
	${RM}  *.linked.bit 
	${RM} -rf _sds sd_card