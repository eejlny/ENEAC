LIBRARY = libnbody.a

PLATFORM = zc702
SDSFLAGS = -sds-pf ${PLATFORM} \
	-sds-hw ComputeChunkFPGA NBodyHW.cpp -sds-end\
	-Wno-unused-label -Wno-unused-function

CC = sds++ ${SDSFLAGS}

CFLAGS = -Wall -O3 -c
LFLAGS = -O3

.PHONY: all

all: ${LIBRARY}

NBodyHW.o: NBodyHW.cpp
	${CC} ${CFLAGS} $< -o $@

kernelNBody.o: kernelNBody.cpp
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelnbody.linked: NBodyHW.o kernelNBody.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
${LIBRARY}: kernelnbody.linked
	cp _sds/swstubs/${LIBRARY} .
	arm-linux-gnueabihf-ar rcs ${LIBRARY} _sds/swstubs/kernelNBody.o

clean:
	${RM} ${LIBRARY} NBodyHW.o kernelNBody.o kernelnbody.linked

ultraclean: clean
	${RM} kernelnbody.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
