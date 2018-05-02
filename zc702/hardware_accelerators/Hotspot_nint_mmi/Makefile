LIBRARY = libkernelHotspot.a

PLATFORM = /space/eejlny/projects/sdsoc/framework/platform/zc702_led
SDSFLAGS = -sds-pf ${PLATFORM} \
	 -sds-hw sobel_filter edge_detect.c -sds-end\
	-Wno-unused-label -Wno-unused-function

#SDSFLAGS = -sds-pf ${PLATFORM} \
#	-Wno-unused-label -Wno-unused-function


#CC = sdscc ${SDSFLAGS}
CC = sds++ ${SDSFLAGS}

CFLAGS = -Wall -O3 -c 
LFLAGS = -O3

.PHONY: all

all: ${LIBRARY}

edge_detect.o: edge_detect.c
	${CC} ${CFLAGS} $< -o $@

kernelHotspot.o: kernelHotspot.c
	${CC} ${CFLAGS} $< -o $@

# Create dummy linked file to build the hardware
kernelHotspot.linked: edge_detect.o kernelHotspot.o
	${CC} $^ -nostartfiles -o $@ 

# Add the interface functions to the library
${LIBRARY}: kernelHotspot.linked
	cp _sds/swstubs/${LIBRARY} .
	arm-linux-gnueabihf-ar rcs ${LIBRARY} _sds/swstubs/kernelHotspot.o

clean:
	${RM} ${LIBRARY} edge_detect.o kernelHotspot.o kernelHotspot.linked

ultraclean: clean
	${RM} kernelHotspot.linked.bit 
	${RM} -rf _sds sd_card

# LocalWords:  cpp
