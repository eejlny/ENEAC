# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 16 Jul 2019
# Description - Makefile to generate interrupt-enabled hotspot platform in sdsoc

PLATFORM = /home/programs/SDx/2018.3/platforms/zcu102_mig4/zcu102
LIBRARY = libkernelhotspot.a

hpc: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw  sobel_filter1_hpc hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  sobel_filter2_hpc hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  sobel_filter3_hpc hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  sobel_filter4_hpc hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function
hpc:	CONFIG_FLAG = -DHPC
     
hp: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw  sobel_filter1_hp hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  sobel_filter2_hp hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  sobel_filter3_hp hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  sobel_filter4_hp hotspot_hphpc.cpp  -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function
hp: CONFIG_FLAG = -DHP

hpc hp: $(LIBRARY)

all: hpc hp

.PHONY: all

DEPS = hotspot.h

CC = sds++ $(SDSFLAGS)

CFLAGS = -Wall -O3 -c $(CONFIG_FLAG)

hotspot_hphpc.o: hotspot_hphpc.cpp $(DEPS)
	${CC} ${CFLAGS} $< -o $@

kernelhotspot.o: kernelhotspot.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelhotspot.linked: hotspot_hphpc.o kernelhotspot.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
$(LIBRARY): kernelhotspot.linked
	cp _sds/swstubs/$(LIBRARY) .
	arm-linux-gnueabihf-ar rcs $(LIBRARY) _sds/swstubs/kernelhotspot.o
	ranlib $(LIBRARY)
     
clean:
	${RM} *.a *.o *.linked

ultraclean: clean
	${RM}  *.linked.bit 
	${RM} -rf _sds sd_card