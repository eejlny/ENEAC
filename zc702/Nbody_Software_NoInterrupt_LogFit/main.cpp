//============================================================================
// Name        : barnesHut_OpenCL.cpp
// Author      : Antonio Vilches, Andres Rodrigez, Rafael Asenjo
// Version     : 1.0
// Copyright   : Department of  Computer Architecture at UMA (c)
// Description : NBody Simulation for an heterogeneous Computing System
//============================================================================

////#define MAP_SIZE 4096UL
////#define MAP_MASK (MAP_SIZE - 1)

#define LS 0  // Logarithmic Scheduler (Vilches)
#define HDSS 1 // Heterogeneous Dynamic Self-Schdeduler (Belviranli)
#define FIXS 2 // Fixed GPU anc CPU Chunk Scheduler
#define DYNAMIC 3 // Fixed GPU chunk - dynamic CPU chunk scheduler
#define SS 4 // Static Scheduler (only one GPU chunk)
#define CONCORD 5 // Paper Concord Intel
#define PROF 6 // Profiling scheduler
#define LOGFIT 7

// If add a new scheduler don't forget to add the name in SchedulerName variable.
//#define SCHED LS

//#define WRITERESULT 1   // write individual gpu throughpus to disk
//#define GPU_THRESHOLD 10  // minimun GPU execution time (ms) for GPU chunk stimation LS scheduler
//#define PJTRACER		// write paje.trace file
//#define PROFILING_CL // GPU time measured by device (openCL) for LS scheduler
//#define ONETIMESTEP 15 // Overwrite problem time-steps 
//#define HOSTPRI  // rise GPU host-thread priority
//#define PROFILING_CL2 // GPU time breakdown (openCL)

//#define PCM_EN   // enable PCM (energy)

int main_thread=0;


#ifdef PJTRACER
#include "pfortrace.h"
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/stat.h>
#include <assert.h>
#include "NBody.h"

#include <fcntl.h>
#include <sys/mman.h>

#include "tbb/task_scheduler_init.h"
#include "tbb/tbb.h"




#if SCHED == HDSS
	#include "HDSS.h"
    HSchedulerHDSS hs;
#elif SCHED == FIXS
	#include "../Schedulers/FixedScheduler.h"
	HSchedulerFixed hs;
	#define EXTRA 1
	typedef int extraType;
#elif SCHED == DYNAMIC
	#include "../Schedulers/DynamicScheduler.h"
	HSchedulerDynamic hs;
	#define EXTRA 1
	typedef int extraType;	
#elif SCHED == SS
	#include "../Schedulers/StaticScheduler.h"
	HSchedulerStatic hs;
    #define EXTRA 1
	typedef float extraType;
#elif SCHED == CONCORD
	#include "Concord.h"
	HSchedulerConcord hs;
#elif SCHED == PROF
	#include "ProfScheduler.h"
	HSchedulerLog hs;
#elif SCHED == LOGFIT
	#include "../Schedulers/LogFitScheduler.h"  //MODIFIED LINES
	HSchedulerLogFit hs;		//MODIFIED LINES
	#define EXTRA 1
	typedef float extraType;
#else
	#include "../Schedulers/LogScheduler.h"
	HSchedulerLog hs;
#endif 


#include "Body.h"

using namespace std;

char* SchedulerName[] = {"LS", "HDSS", "FIXS", "DYNAMIC", "SS", "CONCORD", NULL, "LOGFIT"};

#ifdef PJTRACER
	PFORTRACER *pftrace;
#endif

/*****************************************************************************
 * Global variables
 * **************************************************************************/
ofstream resultfile;

/*****************************************************************************
 * Main Function
 * **************************************************************************/

int main(int argc, char **argv) {

  cout << "START" << endl;

//// int memfd;
////    void *mapped_base, *mapped_dev_base;
////    off_t dev_base = HW_ADDR_GPIO; //GPIO hardware


////    memfd = open("/dev/mem", O_RDWR | O_SYNC);
////    if (memfd == -1) {
////    	printf("Can't open /dev/mem.\n");
////        exit(0);
////    }
////    printf("/dev/mem opened for gpio.\n");

    // Map one page of memory into user space such that the device is in that page, but it may not
      // be at the start of the page.


////      mapped_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~MAP_MASK);
////          if (mapped_base == (void *) -1) {
////          printf("Can't map the memory to user space.\n");
////          exit(0);
////      }
////       printf("GPIO mapped at address %p.\n", mapped_base);



      // get the address of the device in user space which will be an offset from the base
      // that was mapped as memory is mapped at the start of a page

////      mapped_dev_base = mapped_base + (dev_base & MAP_MASK);




////  file_desc = open("/dev/my_driver",O_RDWR);
////  if (file_desc < 0)
////  {	
////		printf("cant open device: /dev/my_driver\n");
////		exit(0);
////  }
  
  // #endif
////   char led_value = 255;

////    #ifndef __SDSVHLS__
               //   axi_gpio_write_reg(axi_gpio_0.baseaddr, 0, (unsigned char) led_value);
////    #endif


   // interrupt = (int *) sds_alloc(sizeof(int));


////    interrupt = (int*)mapped_dev_base;
    /*int* enable_channel = (int*)mapped_dev_base + fpga_mmio_ier;
    int* enable_general = (int*)mapped_dev_base + fpga_mmio_gier;*/

////    status = (int*)mapped_dev_base+fpga_mmio_gpio; //(int*)read(file_desc,receive,10); //axi_gpio_0.baseaddr;

    //status = (int*)mapped_dev_base;

////    sds_mmap((void *)HW_ADDR_GPIO_INT,4,(void *)status);

    //sds_mmap((void *)HW_ADDR_GPIO,4,(void *)status);

////    printf("Interrupt located at address %x\n",(int)status);

    //printf("Switch on leds\n");
    
    //*interrupt = (unsigned char) led_value; //leds on


   /* *enable_channel = 0xFFFFFFFF; //leds on
    *enable_general = 0xFFFFFFFF; //leds on*/

     //printf("Force interrupt\n");  
     //getchar();
     //*status = 255;
    
////     printf("Switch off leds\n");

////     led_value = 0;

////    *interrupt = (unsigned char) led_value; //leds off

	//Time variables
	tick_count startcompute, endcompute, startfile, endfile;
	float runtime = 0, runtimegrow = 0, runtimeadvance = 0, runtimecompute = 0, runtimefile = 0;
	//end variables

	if (argc < 4 || argc > 5) {
		fprintf(stderr, "NBody scheduler %s\n", SchedulerName[SCHED]);
		fprintf(stderr, "Arguments: input_file numcpus numgpus [ext1 (float)]\n");
		fprintf(stderr, "    ext1: extra parameter\n");
		fprintf(stderr, "          - ChunkGPU (Fixed or Dynamic)\n");
		fprintf(stderr, "          - GPU weight (StaticScheduler)\n");
		exit(-1);
	}
	char* data_file_name = argv[1];
	int numcpus = atoi(argv[2]);
	int numgpus = atoi(argv[3]);
	int random = 1;

	//float* my_bodies2 = (float *) sds_alloc(sizeof(float) * 1000);

	//float* my_bodies2 = (float *) sds_alloc_non_cacheable(sizeof(float) * 1000);
	//if(!my_bodies2)
	//{
	//	printf("bodies not allocated\n");
	//	exit(1);

	//}
	//else
	//{
	//	printf("sucess\n");
	//}


	float extrarg = 0;
	int extradef = 0;
	if (argc == 5) {
		extrarg = atof(argv[4]);
		extradef = 1;
	}

	sprintf(benchName, "NBody");

#ifdef EXTRA
	if (extradef)
		cerr << "NBody simulation (" <<SchedulerName[SCHED]<<"): " << numcpus << ", " << numgpus << ", " << argv[4] << endl; 
	else {
		cerr << "Extra parameter not passed" << endl;
		exit(-1);
	}

	#ifdef PJTRACER
		char traceFname[1024];
		time_t timer;
		time(&timer);
		sprintf(traceFname, "%s_paje.G%d.C%d.EX%s.%d.trace",SchedulerName[SCHED], numgpus, numcpus, argv[4], timer);
	#endif
#else
	cout << "NBody simulation (" <<SchedulerName[SCHED]<<"): " << numcpus << ", " << numgpus << endl; 

        #ifdef PJTRACER
		char traceFname[1024];
		time_t timer;
		time(&timer);
		sprintf(traceFname, "%s_paje.G%d.C%d.%d.trace",SchedulerName[SCHED], numgpus, numcpus, timer);
	#endif
#endif


#ifdef PJTRACER
	pftrace = new PFORTRACER(traceFname);
	pftrace->beginThreadTrace();
#endif

#ifdef HOSTPRI  // rise GPU host-thread priority
	DWORD dwError, dwThreadPri;
	//dwThreadPri = GetThreadPriority(GetCurrentThread());
	//printf("Current thread priority is 0x%x\n", dwThreadPri);
	if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
	{
		dwError = GetLastError();
		if(dwError)
			cerr << "Failed to set hight priority to host thread (" << dwError << ")" << endl;
	} 
	//dwThreadPri = GetThreadPriority(GetCurrentThread());
	//printf("Current thread priority is 0x%x\n", dwThreadPri);
#endif

	NbodyTask body;
	
	task_scheduler_init init(numcpus+numgpus);

	startfile = tick_count::now();
	ReadInput(argv[1]);
	endfile =tick_count::now();
	runtimefile = (endfile-startfile).seconds() * 1000;

	cout << " -----> Nbodies : " << nbodies << endl;

#ifdef EXTRA
	if (extradef)
		////hs.Initialize(numcpus, numgpus, (extraType) extrarg);
	hs.Initialize(numcpus, numgpus, 0.0); // the starting point of chunk size exploration 0.0 = minimum, that is size=1 //MODIFIED LINES
	else {
		cerr << "Extra parameter not passed" << endl;
		exit(-1);
	}
#else
	////hs.Initialize(numcpus, numgpus);
	hs.Initialize(numcpus, numgpus, 0.0); // the starting point of chunk size exploration 0.0 = minimum, that is size=1 //MODIFIED LINES
#endif		

	hs.startTimeAndEnergy();

#ifdef ONETIMESTEP
	timesteps = ONETIMESTEP;
#endif


	// time-step the system
	printf("time steps are %d\n",timesteps);
	for (step = 0; step < timesteps; step++) {
		#ifdef PJTRACER
			pftrace->newEvent();
		#endif		
		//cerr << "Time step: " << step << endl;
		//startcompute = tick_count::now();
		
		hs.heterogeneous_parallel_for(0, nbodies, &body);

		//endcompute = tick_count::now();
		//runtimecompute += (endcompute-startcompute).seconds() * 1000;
		//cerr << "Iteration: " << step << " time: " << (endcompute-startcompute).seconds() * 1000 << " ms." << endl;
		
		for (int i = 0; i < nbodies; i++) { // this loop is parallel. Worthwhile?
			Advance(&bodies[i],&bodies_vel[i]); // advance the position and velocity of each body
		}
#ifdef VERBOSE
		for (int i =0; i < 10; i ++)
			printf("b[%i]=%f  ",i*1024,bodies_vel[i*1024].velx);
		printf("\n");
#endif
	} // end of time step

	hs.endTimeAndEnergy();
	hs.saveResultsForBench();
	
#ifdef PJTRACER
	//delete pftrace;
#endif

	return EXIT_SUCCESS;
}
