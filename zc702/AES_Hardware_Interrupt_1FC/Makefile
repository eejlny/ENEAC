LIBRARY = libkernelAES.a

PLATFORM = /space/ma17215/Projects/framework/platform/zc702_led
SDSFLAGS = -sds-pf ${PLATFORM} \
	 -sds-hw aes_enc_hw aes_enc.cpp -sds-end\
	-Wno-unused-label -Wno-unused-function

#SDSFLAGS = -sds-pf ${PLATFORM} \
#	-Wno-unused-label -Wno-unused-function

DEPS = aes_enc.h sbox.h

#SDSFLAGS = -sds-pf ${PLATFORM} \
#	-Wno-unused-label -Wno-unused-function


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

clean:
	${RM} ${LIBRARY} aes_enc.o kernelAES.o  kernelAES.linked

ultraclean: clean
	${RM}  kernelAES.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
