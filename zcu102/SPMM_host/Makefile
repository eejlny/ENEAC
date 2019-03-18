
# SPMM

LIB_spmm = ./lib_spmm

CXX= g++

CFLAGS+=  -O2 -I . 

CFLAGS+= -Wno-unused-result -Wno-write-strings -Wno-deprecated-register 

XI_CFLAGS = $(CFLAGS) -DXILINX -DOFFLOAD -mcpu=cortex-a53 -I $(LIB_spmm)  -I ${CURDIR}  -I ./Schedulers

XI_CFLAGS+= -DNEON  -funsafe-math-optimizations -ftree-vectorize -ftree-vectorizer-verbose=2 #If NEON SIMD instructions are to be used


XI_LDFLAGS = -L $(LIB_spmm)

LDFLAGS = -pthread -lpthread -ltbb -z muldefs

XI_LDFLAGS+= -lrt -lkernelspmm 

XI_PROGs= SPMM

SOURCE= spmm-top.cpp   kernelspmm.h

.PHONY: all clean odroid

all: odroid

odroid: $(XI_PROGs)


SPMM: $(SOURCE)
	$(CXX) -o $@ $<  $(XI_CFLAGS) $(XI_LDFLAGS)  $(LDFLAGS)
	

clean:
	rm -f  $(XI_PROGs)

