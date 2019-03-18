
# HOTSPOT

include common_variables.mk

LIB_energy_arm = ../lib
LIB_hotspot = ./LibHotspot

CXX= g++-4.9

CFLAGS+= -O3 -I . 
CFLAGS+= -Wno-unused-result -Wno-write-strings -Wno-deprecated-register
CFLAGSA = $(CFLAGS) -std=c++11
XI_CFLAGS = $(CFLAGS) -DXILINX --std=gnu++11 -I -mcpu=cortex-a53 -I ./LibHotspot -I ./Schedulers
XI_CFLAGS+= -DNEON -funsafe-math-optimizations -ftree-vectorize -ftree-vectorizer-verbose=2 #If NEON SIMD instructions are to be used




LDFLAGS =  -ltbb -pthread -ltbbmalloc_proxy -ltbbmalloc 


XI_LDFLAGS = $(LDFLAGS) -L $(LIB_energy_arm) -L $(LIB_hotspot)
XI_LDFLAGS+= -lenergy_meter_arm -lrt -lkernelHotspot # -lPVROCL





XI_PROGs= HOTSPOT_DSW 
#NBODYV NBODY_FS

PROFILING= HOTSPOT_PROCL 

SOURCE= hotspot.cpp  kernelHotspot.h

.PHONY: all clean odroid libraries

all: odroid

libraries:
	$(MAKE) -C ../energy-meter

odroid: libraries $(XI_PROGs)



HOTSPOT_DSW: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DWRITERESULT 
	#-DHOSTPRI_LINUX


#define GPU_THRESHOLD 10 

mem_test: mem_test.cpp
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) 

clean:
	rm -f  $(XI_PROGs) HOTSPOT_SS 

