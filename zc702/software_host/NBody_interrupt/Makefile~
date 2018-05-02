# LINUX Version Xilinx asenjo@uma.es 5/2016, based on andres@uma.es 11/2014
# NBODY

include common_variables.mk

LIB_energy_arm = ../lib

CXX= g++

CFLAGS+= -O3 -I . 
CFLAGS+= -Wno-unused-result -Wno-write-strings -Wno-deprecated-register
CFLAGSA = $(CFLAGS) -std=c++11
XI_CFLAGS = $(CFLAGS) -DXILINX --std=gnu++11 -march=armv7-a -I $(INCLUDE_TBB_ARM) -I LibNBody
XI_CFLAGS+= -DNEON -funsafe-math-optimizations -mfloat-abi=hard -mfpu=neon #If NEON SIMD instructions are to be used
#XI_CFLAGS+= -mfloat-abi=hard -mfpu=neon #If NEON SIMD instructions are to be used

#CFALGS+= -DTRACE  # to get trace file output. Use ViTE to visualize

LDFLAGS =  -ltbb -pthread -ltbbmalloc_proxy -ltbbmalloc # -lgopt 

XI_LDFLAGS = $(LDFLAGS) -L $(LIB_TBB_ARM) -L $(LIB_energy_arm) -L LibNBody -lkernelnbody 
XI_LDFLAGS+= -lenergy_meter_arm -lrt # -lPVROCL


XI_PROGs= NBODY_DSW 
#NBODYV NBODY_FS

PROFILING= NBODY_PROCL 

SOURCE= main.cpp  ../Schedulers/FixedScheduler.h ../Schedulers/DynamicScheduler.h ../Schedulers/StaticScheduler.h Body.h NBody.h

.PHONY: all clean odroid libraries

all: odroid

apple: nbodya

nbodya: $(SOURCE) 
#	$(CXX) -o $@ $< -I $(INCLUDE_TBB_APPLE) -L $(LIB_TBB_APPLE) $(CFLAGSA) $(LDFLAGS) -DSCHED=DYNAMIC -DNOENERGY -DONETIMESTEP=5
	$(CXX) -o $@ $< -I $(INCLUDE_TBB_APPLE) -L $(LIB_TBB_APPLE) $(CFLAGSA) $(LDFLAGS) -DSCHED=SS -DNOENERGY -DONETIMESTEP=5
#	$(CXX) -o $@ $< -I $(INCLUDE_TBB_APPLE) -L $(LIB_TBB_APPLE) $(CFLAGSA) $(LDFLAGS) -DSCHED=LS -DNOENERGY -DONETIMESTEP=5
#	$(CXX) -o $@ $< -I $(INCLUDE_TBB_APPLE) -L $(LIB_TBB_APPLE) $(CFLAGSA) $(LDFLAGS) -DSCHED=FIXS -DNOENERGY -DONETIMESTEP=5


libraries:
	$(MAKE) -C ../energy-meter

odroid: libraries $(XI_PROGs)

NBODY_FS: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DSCHED=FIXS 

NBODY_DS: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DSCHED=DYNAMIC -DONETIMESTEP=5

NBODY_DSW: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DSCHED=DYNAMIC -DONETIMESTEP=5 -DWRITERESULT #-DVERBOSE #-DONETIMESTEP
	#-DHOSTPRI_LINUX

NBODY_SS: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DSCHED=SS -DONETIMESTEP=5

NBODY_b: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DSCHED=FIXS -DONETIMESTEP=5

NBODYV: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DSCHED=FIXS -DVERBOSE=1

NBODY_NOENERGY: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DSCHED=FIXS -DONETIMESTEP=15 -DNOENERGY

NBODY_PRIO: $(SOURCE) 
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) -DHOSTPRI_LINUX -DSCHED=FIXS 

oracle: NBODY_SS

#define GPU_THRESHOLD 10 

mem_test: mem_test.cpp
	$(CXX) -o $@ $< $(XI_CFLAGS) $(XI_LDFLAGS) 

clean:
	rm -f  $(XI_PROGs) NBODY_SS nbodya

