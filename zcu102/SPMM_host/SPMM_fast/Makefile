# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 08 Apr 2019
# Description: Makefile used to compile SPMM with energy monitoring and FPGA accelerator support for the ultrascale+ devboard

hpc: LIB_spmm = ./lib_spmm/hpc
hp: LIB_spmm = ./lib_spmm/hp

hpc hp: ultrascale

all: hp hpc

.PHONY: all

LIB_energy_arm = ../lib_energy

CXX= g++

CFLAGS+=  -O2 -I . 

CFLAGS+= -Wno-unused-result -Wno-write-strings -Wno-deprecated-register

XI_CFLAGS = $(CFLAGS) -DXILINX -DOFFLOAD -mcpu=cortex-a53 -DDEBUG -I $(LIB_spmm) -I $(LIB_energy_arm) -I ${CURDIR}  -I ./Schedulers

XI_CFLAGS+= -DNEON  -funsafe-math-optimizations -ftree-vectorize -ftree-vectorizer-verbose=2 #If NEON SIMD instructions are to be used


XI_LDFLAGS = -L $(LIB_spmm) -L $(LIB_energy_arm)

LDFLAGS = -pthread -lpthread -ltbb -z muldefs

XI_LDFLAGS+= -lrt -lkernelspmm -lenergy_meter_arm

XI_PROGs= SPMM

SOURCE= spmm-top.cpp kernelspmm.h

.PHONY: all clean ultrascale

all: ultrascale

ultrascale: $(XI_PROGs)

libraries:
	$(MAKE) -C ../energy-meter

SPMM: $(SOURCE)
	$(CXX) -o $@ $<  $(XI_CFLAGS) $(XI_LDFLAGS)  $(LDFLAGS)
	

clean:
	rm -f  $(XI_PROGs)

