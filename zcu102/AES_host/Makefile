# Author: Kris Nikov - kris.nikov@bris.ac.uk
# Date: 15 Jul 2019
# Description: Makefile used to compile AES with energy monitoring and FPGA accelerator support for the ultrascale+ devboard

hpc: LIB_aes = ./lib_aes/hpc
hpc:	CONFIG_FLAG = -DHPC

hp: LIB_aes = ./lib_aes/hp
hp: CONFIG_FLAG = -DHP

#Specify software scheduler type
SCHEDULER = -DMULTIDYNAMIC
#SCHEDULER = -DMULTIHAP

#Specify level of debug
DEBUG_FLAGS = -DHWDEBUG
#DEBUG_FLAGS = -DDEBUG 
#DEBUG_FLAGS += -DDEBUG_CHUNKS

LIB_energy_arm = ../lib_energy

CXX= g++

CFLAGS = -O2 -I . 

CFLAGS += -Wno-unused-result -Wno-write-strings -Wno-deprecated-register

XI_CFLAGS = $(CFLAGS) -DXILINX -DOFFLOAD --std=gnu++11 -mcpu=cortex-a53 $(CONFIG_FLAG) $(SCHEDULER) -I $(LIB_aes) -I $(LIB_energy_arm) -I ${CURDIR}  -I ./schedulers

XI_CFLAGS += -DNEON -march=armv8-a+crypto -funsafe-math-optimizations -ftree-vectorize -ftree-vectorizer-verbose=2 #If NEON SIMD instructions are to be used

#Comment out to disable debug information
XI_CFLAGS += $(DEBUG_FLAGS)

LDFLAGS = -ltbb -pthread -ltbbmalloc_proxy -ltbbmalloc -z muldefs

XI_LDFLAGS = $(LDFLAGS) -L $(LIB_aes) -L $(LIB_energy_arm)

XI_LDFLAGS += -lrt -lkernelaes -lenergy_meter_arm

XI_PROGs= AES

SOURCE= aes_top.cpp kernelaes.h

hpc hp: ultrascale

all: hp hpc

.PHONY: all

ultrascale: $(XI_PROGs)

AES: $(SOURCE)
	$(CXX) -o $@ $<  $(XI_CFLAGS) $(XI_LDFLAGS) $(LDFLAGS)
	
clean:
	${RM} -f $(XI_PROGs)
	${RM} *.txt
	${RM} *.dat