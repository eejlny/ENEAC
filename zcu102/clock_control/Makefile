
LIB_AES = ./LibAES

CXX= g++-4.9

#CXX= g++

CFLAGS+= -O3 -I .
CFLAGS+= -Wno-unused-result -Wno-write-strings -Wno-deprecated-register
CFLAGSA = $(CFLAGS) -std=c++11
XI_CFLAGS = $(CFLAGS) -DXILINX --std=gnu++11 
XI_CFLAGS+= -DNEON  -march=armv8-a+crypto -funsafe-math-optimizations -ftree-vectorize  -ftree-vectorizer-verbose=2 #If NEON SIMD instructions are to be used
#XI_CFLAGS = $(CFLAGS)

LDFLAGS =  -ltbb -pthread -ltbbmalloc_proxy -ltbbmalloc -z muldefs
#LDFLAGS =  -pthread  -z muldefs

XI_LDFLAGS = $(LDFLAGS) -L $(LIB_AES)
XI_LDFLAGS+= -lrt -lkernelAES  # -lPVROCL

#XI_LDFLAGS+= -lkernelAES  # -lPVROCL



XI_PROGs= AES_DSW
#NBODYV NBODY_FS

SOURCE= main.cpp  kernelAES.h

.PHONY: all clean odroid libraries

all: odroid

odroid: libraries $(XI_PROGs)

aes_enc.o: aes_enc.cpp aes_enc.h
	$(CXX) -c aes_enc.cpp $(XI_CFLAGS)



AES_DSW: $(SOURCE) aes_enc.o
	$(CXX) -o $@ $< aes_enc.o $(XI_CFLAGS) $(XI_LDFLAGS) -DWRITERESULT #-DHOSTPRI_LINUX


clean:
	rm -f  $(XI_PROGs) AES_DSW
	rm *.o
