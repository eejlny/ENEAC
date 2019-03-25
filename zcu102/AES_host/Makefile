
# HOTSPOT

include common_variables.mk

LIB_energy_arm = ../lib
LIB_AES = ./LibAES

CXX= g++-4.9

CFLAGS+= -O2 -I .
CFLAGS+= -Wno-unused-result -Wno-write-strings -Wno-deprecated-register
CFLAGSA = $(CFLAGS) -std=c++11
#XI_CFLAGS = $(CFLAGS) -DXILINX --std=gnu++11 -I $(INCLUDE_TBB_ARM)
#XI_CFLAGS = $(CFLAGS) -DXILINX --std=gnu++11 -march=armv7-a -I $(INCLUDE_TBB_ARM)
XI_CFLAGS = $(CFLAGS) -DXILINX --std=gnu++11 -I $(INCLUDE_TBB_ARM) -I LibAES   -I ./Schedulers
#XI_CFLAGS = $(CFLAGS) -DXILINX --std=gnu++11 --std=gnu++11 -mcpu=cortex-a53  -I LibkernelHotspot
XI_CFLAGS+= -DNEON  -march=armv8-a+crypto -funsafe-math-optimizations -ftree-vectorize  -ftree-vectorizer-verbose=2 #If NEON SIMD instructions are to be used
#XI_CFLAGS+= -mfloat-abi=soft


#CFALGS+= -DTRACE  # to get trace file output. Use ViTE to visualize

#LDFLAGS =  -ltbb -pthread -ltbbmalloc_proxy -ltbbmalloc # -lgopt


LDFLAGS =  -ltbb -pthread -ltbbmalloc_proxy -ltbbmalloc -z muldefs


#XI_LDFLAGS = $(LDFLAGS) -L $(LIB_TBB_ARM) -L $(LIB_energy_arm)
#XI_LDFLAGS = $(LDFLAGS) -L $(LIB_TBB_ARM) -L $(LIB_energy_arm) ./LibHotspot/libkernelnbody.a
XI_LDFLAGS = $(LDFLAGS) -L $(LIB_TBB_ARM) -L $(LIB_energy_arm) -L $(LIB_AES)
#XI_LDFLAGS = $(LDFLAGS) -L $(LIB_TBB_ARM) -L $(LIB_energy_arm)
XI_LDFLAGS+= -lenergy_meter_arm -lrt -lkernelAES # -lPVROCL
#XI_LDFLAGS+= -lenergy_meter_arm -lrt

XI_PROGs= AES_DSW
#NBODYV NBODY_FS

PROFILING= HOTSPOT_PROCL

SOURCE= aes.cpp kernelAES.h

.PHONY: all clean odroid libraries

all: odroid

libraries:
	$(MAKE) -C ../energy-meter

odroid: libraries $(XI_PROGs)


AES_DSW: $(SOURCE)
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DWRITERESULT #-DHOSTPRI_LINUX



#define GPU_THRESHOLD 10

mem_test: mem_test.cpp
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS)

clean:
	rm -f  $(XI_PROGs) AES_SS
	rm *.txt
