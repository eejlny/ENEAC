# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 16 Jul 2019
# Description - Makefile to generate interrupt-enabled aes platform in sdsoc

PLATFORM = /home/programs/SDx/2018.3/platforms/zcu102_mig4/zcu102
LIBRARY = libkernelaes.a

hpc: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw  aes1_hpc aes_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  aes2_hpc aes_hphpc.cpp  -clkid 0 -sds-end\
     -sds-hw  aes3_hpc aes_hphpc.cpp  -clkid 0 -sds-end\
     -sds-hw  aes4_hpc aes_hphpc.cpp  -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function
hpc:	CONFIG_FLAG = -DHPC
     
hp: SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	-sds-hw  aes1_hp aes_hphpc.cpp  -clkid 0 -sds-end\
	-sds-hw  aes2_hp aes_hphpc.cpp  -clkid 0 -sds-end\
     -sds-hw  aes3_hp aes_hphpc.cpp  -clkid 0 -sds-end\
     -sds-hw  aes4_hp aes_hphpc.cpp  -clkid 0 -sds-end\
	-Wno-unused-label -Wno-unused-function
hp: CONFIG_FLAG = -DHP

hpc hp: $(LIBRARY)

all: hpc hp

.PHONY: all

DEPS = aes.h

CC = sds++ $(SDSFLAGS)

CFLAGS = -Wall -O3 -c $(CONFIG_FLAG)

aes_hphpc.o: aes_hphpc.cpp $(DEPS)
	${CC} ${CFLAGS} $< -o $@

kernelaes.o: kernelaes.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelaes.linked: aes_hphpc.o kernelaes.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
$(LIBRARY): kernelaes.linked
	cp _sds/swstubs/$(LIBRARY) .
	arm-linux-gnueabihf-ar rcs $(LIBRARY) _sds/swstubs/kernelaes.o
	ranlib $(LIBRARY)
     
clean:
	${RM} *.a *.o *.linked

ultraclean: clean
	${RM}  *.linked.bit 
	${RM} -rf _sds sd_card