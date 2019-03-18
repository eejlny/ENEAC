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
		start = tick_count::now();
	}

	/*Sets the end mark of energy and time*/
	void endTimeAndEnergy(){
		end = tick_count::now();

		runtime = (end-start).seconds()*1000;
	}

	/*Checks if a File already exists*/
	bool isFile(char *filename){
		//open file
    std::ifstream ifile(filename);
		return ifile;
	}
};

void cleanup() 
{
     
}

template <typename T, typename PARAMS>
T* Scheduler<T, PARAMS>::instance = NULL;

