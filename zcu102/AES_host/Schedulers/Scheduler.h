//============================================================================
// Name			: Scheduler.h
// Author		: Antonio Vilches
// Version		: 1.0
// Date			: 26 / 12 / 2014
// Copyright	: Department. Computer's Architecture (c)
// Description	: Main scheduler interface class
//============================================================================

//#define ENERGYCOUNTERS

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"

#define ENERGY

#ifdef ENERGY
#include "../../energy_meter/energy_meter.h"
#include "../../energy_meter/thread_funcs.cpp"

struct energy_sample *sample1;
struct em_t final_em; // to get final energies
#else
int set_thread_affinity_CORE(pthread_t th, int cpu)
{
	cpu_set_t mask;
	__CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	__CPU_SET_S(cpu,sizeof(cpu_set_t), &mask);

	return pthread_setaffinity_np(th, sizeof(cpu_set_t), &mask);
}
#endif


#ifdef PJTRACER
#include "pfortrace.h"
#endif



// using namespace std;
using namespace tbb;

/*****************************************************************************
 * Global Variables For OpenCL
 * **************************************************************************/

int computeUnits;

//profiler
#ifdef PJTRACER
PFORTRACER * tracer;
#endif

/*****************************************************************************
 * OpenCL fucntions
 * **************************************************************************/


/*****************************************************************************
 * Base Scheduler class
 * **************************************************************************/
/*This Scheduler Base class implementation follows a Singleton pattern*/
template <typename T, typename PARAMS >
class Scheduler{
protected:
// Class members
	//Scheduler Itself
	static T *instance;
	task_scheduler_init *init;
	int nCPUs;
	int nGPUs;


	//timing
	tick_count start, end;
	float runtime;

//End class members

	/*Scheduler Constructor, forbidden access to this constructor from outside*/
	Scheduler(PARAMS * params) {
   
		nCPUs = params->numcpus;
		nGPUs = params->numgpus;
#ifndef NDEBUG
    std::cerr << "TBB scheduler is active " << "(" << nCPUs << ", " << nGPUs << ")" << std::endl;
#endif
		init = new task_scheduler_init(nCPUs + nGPUs);


#ifndef NDEBUG
    std::cerr << "INITIALIZING HOSTPRIORITY" << std::endl;
#endif
		
		runtime = 0.0;
	}



public:
	/*Class destructor*/
	~Scheduler(){
		init->~task_scheduler_init();
		delete instance;
		instance = NULL;
	}

	/*This function creates only one instance per process, if you want a thread safe behavior protect the if clausule with a Lock*/
	static T * getInstance(PARAMS * params){
		if(! instance){
			instance = new T(params);
		}
		return instance;
	}


	/*Sets the start mark of energy and time*/
	void startTimeAndEnergy(){
		printf("starting time energy\n");
		#ifdef ENERGY
	  		sample1=energy_meter_init(50, 0  /*0=no debug*/);  // sample period 100 miliseconds
	  		//power_meter_idle(sample1); //get idle power
	  		energy_meter_start(sample1);  // starts sampling thread
		#endif
		start = tick_count::now();
	}

	/*Sets the end mark of energy and time*/
	void endTimeAndEnergy(){
		end = tick_count::now();

		#ifdef ENERGY
	  		energy_meter_read(sample1,&final_em);  // final & total
	  		energy_meter_stop(sample1);  	// stops sampling
	  		energy_meter_printf(sample1, stdout);  // print total results
	  		energy_meter_destroy(sample1);     // clean up everything
		#endif

		runtime = (end-start).seconds()*1000;
	}

	/*Checks if a File already exists*/
	bool isFile(char *filename){
		//open file
          std::ifstream ifile(filename);
		//return ifile;
	}
};

void cleanup() 
{
     
}

template <typename T, typename PARAMS>
T* Scheduler<T, PARAMS>::instance = NULL;

