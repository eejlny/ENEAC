LIBRARY = libkernelspmm.a

#PLATFORM =  /home/programs/SDx/2018.1/platforms/zcu102_mig2/zcu102

PLATFORM = zcu102

SDSFLAGS = -sds-proc a53 -sds-pf ${PLATFORM} \
	 -sds-hw spmm1  spmm.cpp -clkid 4 -sds-end\
	 -sds-hw spmm2  spmm.cpp -clkid 4 -sds-end\
	 -sds-hw spmm3  spmm.cpp -clkid 4 -sds-end\
	 -sds-hw spmm4  spmm.cpp -clkid 4 -sds-end\
	-Wno-unused-label -Wno-unused-function

#clkid 3 200MHz
#clkid 4 300mhz
#clkid 5 400mhz

DEPS = spmm.h

#SDSFLAGS = -sds-pf ${PLATFORM} \
#	-Wno-unused-label -Wno-unused-function


#CC = sdscc ${SDSFLAGS}
CC = sds++ ${SDSFLAGS}

CFLAGS = -Wall -O3 -c 
LFLAGS = -O3

.PHONY: all

all: ${LIBRARY}

spmm.o: spmm.cpp $(DEPS)
	${CC} ${CFLAGS} $< -o $@

kernelspmm.o: kernelspmm.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelspmm.linked: spmm.o kernelspmm.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
${LIBRARY}: kernelspmm.linked
	cp _sds/swstubs/${LIBRARY} .
	arm-linux-gnueabihf-ar rcs ${LIBRARY} _sds/swstubs/kernelspmm.o
	ranlib libkernelspmm.a

clean:
	${RM} ${LIBRARY} spmm.o kernelspmm.o  kernelspmm.linked

ultraclean: clean
	${RM}  kernelspmm.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
