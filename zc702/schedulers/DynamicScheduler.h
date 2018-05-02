//============================================================================
// Name        : Dynamic Scheduler.h
// Author      : Antonio Vilches & Francisco Corbera & Rafa Asenjo
// Version     : 1.0
// Copyright   : Department. Computer's Architecture (c)
// Description : 
//============================================================================
#include <cstdlib>
#include <iostream>
#include "Body.h"

using namespace std;

/*****************************************************************************
 * Defines
 * **************************************************************************/
#define CPU 0
#define _GPU 1
#define GPU_OFF -100
#define SEP "\t"

/*****************************************************************************
 * Global variables
 * **************************************************************************/

tbb::atomic<int> gpuStatus;
tbb::atomic<int> firsttime_cpu; //andres
int numCPUs;
int numGPUs;
int chunkGPU;
//int* interrupt_address;
__thread int chunkCPU;
__thread float cpuThroughput;
float gpuThroughput;
float fG;
char benchName[100];
int totGPU=0;
int totCPU=0;
int mystep=0;

#ifndef NOENERGY
#include "../energy-meter/energy_meter.h"
#include "../energy-meter/thread_funcs.cpp"

struct energy_sample *sample1;
struct em_t final_em; // to get final energies
#endif

tbb::tick_count tstart,tend;
float runtime;

int inittime=1;

extern ofstream resultfile;
#ifdef PJTRACER
	extern PFORTRACER *pftrace;
#endif

/*****************************************************************************
 * Heterogeneous Scheduler
 * **************************************************************************/

class Bundle {
public:
	int begin;
	int end;
	int type; //GPU = 0, CPU=1
	int cht;

public:
	Bundle() {};
};

class MySerialFilter: public filter {
private:
	int begin;
	int end;
public:
	MySerialFilter(int b, int e) :
		filter(true) {
		begin = b;
		end = e;
	}

	void * operator()(void *) {
		#ifdef PJTRACER
			pftrace->beginThreadTrace();
		#endif

		Bundle *bundle = new Bundle();
		/*If there are remaining iterations*/
		if (begin < end) { 
			//Checking which resources are available
#if defined (HOSTPRI_LINUX) || defined (HOSTA7)
			
			if (main_thread == syscall(__NR_gettid) && numGPUs>0) {  // I am the one to work with GPU

				if(inittime){ 
#ifdef HOSTA7
					cout << "Set affinity to core: " << main_core << endl;
					set_thread_affinity_CORE(pthread_self(),main_core); 
#endif
#ifdef HOSTPRI_LINUX
					cout << "Set prio to 15\n";
					set_thread_prio_RT(pthread_self(),2);
#endif
					inittime=0;
				}
#else
			if ( --gpuStatus >= 0 ){
#endif
				//GPU WORK
				int auxEnd = begin + chunkGPU;
				auxEnd = (auxEnd > end) ? end : auxEnd;
				bundle->begin = begin;
				bundle->end = auxEnd;
				totGPU+= auxEnd-begin;
				bundle->cht = 0;
				begin = auxEnd;
				bundle->type = _GPU;
				return bundle;
			}
			else
			{
#if defined( HOSTPRI_LINUX)
				if(--firsttime_cpu>=0)
				{
					set_thread_prio(pthread_self(),0);
				}
#endif
				//CPU WORK
#if !defined( HOSTPRI_LINUX) && !defined  (HOSTA7)
				gpuStatus++;
#endif				
				/*Calculating next chunkCPU*/
				if(numGPUs==0){
				  chunkCPU=max((end-begin)/(numCPUs), 1);				
				}
				else
				{
				  chunkCPU = chunkGPU / fG;
				  chunkCPU = min( chunkCPU, max((end-begin)/(numCPUs), 1));
				}
#ifdef DEBUG
				resultfile << "Time-Step: "<<mystep<<" chunkCPU: "<<chunkCPU<<" cpuThroughput: "<<cpuThroughput<<" gpuThroughput: "<<gpuThroughput<<" fG: "<<fG<<endl;
#endif				
				if(chunkCPU <= 0){
					chunkCPU=1;
				}
				/*Taking a iteration chunk for CPU*/
				if((end-begin)>=chunkCPU){
					bundle->begin = begin;
					bundle->end = begin + chunkCPU;
					bundle->cht = 0;
					totCPU+= chunkCPU;
					begin = begin + chunkCPU;
					bundle->type = CPU;
					return bundle;
				}else{
					bundle->begin = begin;
					bundle->end = end;
					bundle->cht = 0;
					totGPU+= end-begin;
					begin = end;
					bundle->type = CPU;
					return bundle;
				}
			}		
		}
		return NULL;
	} // end operator
};

class MyParallelFilter: public filter {
private:
	NbodyTask *body;
    tick_count old[2];
    int old_pos;
public:
	int computedOnCPU, computedOnGPU;
	int totalIterations;

	MyParallelFilter(NbodyTask *b) :
			filter(false) {
		body = b;
		computedOnCPU = 0;
		computedOnGPU = 0;
		totalIterations = 0;
		old_pos=0;
		old[old_pos]=tick_count::now();
	}
	void * operator()(void * item) {

		//variables
		Bundle *bundle = (Bundle*) item;
		tick_count t0, t1;

		if(bundle->type == _GPU){
			// GPU WORK
			/*Performing GPU work*/
			#ifdef PJTRACER
				pftrace->gpuStart();
			#endif
			tick_count start = tick_count::now();
			old_pos=!old_pos;
			old[old_pos]=tick_count::now();
#ifdef XILINX
//			if (main_core!=sched_getcpu()) cout << "CORE GPU HOST: " << sched_getcpu() << endl;
#endif
			body->sendObjectToGPU(bundle->begin, bundle->end);
			body->OperatorGPU(bundle->begin, bundle->end);
			body->getBackObjectFromGPU(bundle->begin, bundle->end);
#ifdef XILINX
//			if (main_core!=sched_getcpu()) cout << "CORE GPU HOST (after kernel): " << sched_getcpu() << endl;
#endif
			tick_count end = tick_count::now();

			float elapsedtime = (end-start).seconds() * 1000;
			#ifdef PJTRACER
			{
					char straux[1024];
					sprintf(straux, "%d %f %f", bundle->end-bundle->begin, (bundle->end-bundle->begin)/elapsedtime, (end-start).seconds()*1000);
					pftrace->gpuStop(straux);
			}
			#endif
			//#ifdef HOSTPRI_LINUX
			/*****************************************
			 * 
			 * ****************/
		    //printf("%d %f %f            %f\n", bundle->end-bundle->begin, elapsedtime, (end-start).seconds()*1000, (end-start).seconds()*1000 - elapsedtime);
		    //printf("%f; \n %f; %f; ", (start-old[!old_pos]).seconds()*1000, elapsedtime, (end-start).seconds()*1000 - elapsedtime);
		    
		//	
		//	#endif

			gpuThroughput = ((bundle->end - bundle->begin) / (elapsedtime));

			/*If CPU has already computed some chunk, then we update fG (factor GPU)*/
			if (cpuThroughput > 0){
				fG = gpuThroughput/cpuThroughput;
			}
			#ifdef WRITERESULT
			resultfile << "Step: "<< step << "\tfpgaThroughput: " << ((bundle->end - bundle->begin) / (elapsedtime)) << "\tfpgaChunk: " << (bundle->end - bundle->begin) << endl;
			#endif
			/*To release GPU token*/
#if !defined( HOSTPRI_LINUX) && !defined  (HOSTA7)
			gpuStatus++;
#endif
		}else{
			// CPU WORK
			
			/*Performing CPU work*/
			#ifdef PJTRACER
				pftrace->cpuStart();
			#endif
			tick_count start = tick_count::now();
		//	cout << "CORE CPU: " << sched_getcpu() << endl;
			body->OperatorCPU(bundle->begin, bundle->end);
		//	cout << "CORE CPU (after): " << sched_getcpu() << endl;
			tick_count end = tick_count::now();
			float elapsedtime = (end-start).seconds() * 1000;
			#ifdef PJTRACER
			{
				char straux[1024];
				sprintf(straux, "%d %f %f", bundle->end-bundle->begin, cpuThroughput, (end-start).seconds()*1000);
				pftrace->cpuStop(straux);
			}
			#endif
			cpuThroughput = (bundle->end - bundle->begin) / elapsedtime;
			
			/*If GPU has already computed some chunk, then we update fG (factor GPU)*/
			if (gpuThroughput > 0){
				fG = gpuThroughput/cpuThroughput;
			}
			#ifdef WRITERESULT
			resultfile << "Step: "<< step << "\tcpuThroughput: " << ((bundle->end - bundle->begin) / (elapsedtime)) << "\tcpuChunk: " << (bundle->end - bundle->begin) << endl;
			#endif
		}
		delete bundle;
		return NULL;
	}
};
//end class

class HSchedulerDynamic{
public:
	//void Initialize(int cpus, int gpus, int gpuchunk, int* interrupt) {
	void Initialize(int cpus, int gpus, int gpuchunk) {
		numCPUs = cpus;
		numGPUs = gpus;
		fG = 5;
		chunkGPU = gpuchunk;
		chunkCPU = chunkGPU/fG;
		cpuThroughput = 0;
		gpuThroughput = 0;
		//interrupt_address = interrupt;

#ifdef WRITERESULT
		char fname[1024];	
	    sprintf(fname, "Debug_%s_Dyn.C%d.G%d.txt",benchName, numCPUs, numGPUs);
		resultfile.open(fname, ios::out);
#endif

	}

	void heterogeneous_parallel_for(int begin, int end,  NbodyTask *body) {
		gpuStatus = numGPUs;

		//cerr << "Step " << step << " Heretogeneous parallel for" << endl;
		firsttime_cpu=numCPUs; //andres
		body->firsttime = true;
		pipeline pipe;
		MySerialFilter serial_filter(begin, end);
		MyParallelFilter parallel_filter(body);
		pipe.add_filter(serial_filter);
		pipe.add_filter(parallel_filter);

		pipe.run(numCPUs + numGPUs);
		pipe.clear();
	}

	/*Sets initial mark of energy and time*/
	void startTimeAndEnergy(){
	  
#ifndef NOENERGY
	  sample1=energy_meter_init(50, 0  /*0=no debug*/);  // sample period 100 miliseconds
	  //power_meter_idle(sample1); //get idle power
	  energy_meter_start(sample1);  // starts sampling thread
#endif
	  tstart = tick_count::now();
	}

	/*Sets end mark of energy and time*/
	void endTimeAndEnergy(){

	  tend = tick_count::now();

#ifndef NOENERGY
	  energy_meter_read(sample1,&final_em);  // final & total
	  energy_meter_stop(sample1);  	// stops sampling
	  energy_meter_printf(sample1, stdout);  // print total results
	  energy_meter_destroy(sample1);     // clean up everything
#endif
	  runtime = (tend-tstart).seconds()*1000;
	}

	/*Checks if a File already exists*/
	//http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	bool isFile(char *filename){
	  //open file
	  std::ifstream ifile(filename);
	  return ifile.good();
	}

	/*this function print info to a Log file*/
	void saveResultsForBench(){

		cout<<"Total n. of bodies on CPU: "<< totCPU <<endl;
		cout<<"Total n. of bodies on FPGA: "<< totGPU <<endl;
		cout<<"Actual percentage of work offloaded to the FPGA:" << (totGPU/(float)(totGPU+totCPU)*100) <<"%"<<endl;
	  char fname[1024];
	  sprintf(fname, "%s_Dyn.C%d.G%d.txt",benchName, numCPUs, numGPUs);

	  /*Checking if the file already exists*/
	  bool fileExists = isFile(fname);
	  ofstream file(fname, ios::out | ios::app);
	  if(!fileExists){
	    printHeaderToFile(file);
	  }
#ifndef NOENERGY	  
	  
	 // float totalE = sample1->eCPU + sample1->eFPGA + sample1->eMEM;
	 // float totalCF = sample1->eCPU + sample1->eFPGA;
	  
struct timespec res;
		res=diff(sample1->start_time, sample1->stop_time);
	
	float CPUE=subtotal_em((*sample1),0,FPD_rails)/1000.0;
	float LPDE=subtotal_em((*sample1),FPD_rails,LPD_rails)/1000.0;
	float FPGAE=subtotal_em((*sample1),LPD_rails,PLD_rails)/1000.0;
	float TOTALE=subtotal_em((*sample1),0,PLD_rails)/1000.0;
		
	

	float CPUW = subtotal_em((*sample1),0,FPD_rails)/1000.0/((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0);
	float LPDW = subtotal_em((*sample1),FPD_rails,LPD_rails)/1000.0/((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0);
	float FPGAW = subtotal_em((*sample1),LPD_rails,PLD_rails)/1000.0/((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0);
	
float FPGAoffload = (totGPU/(float)(totGPU+totCPU)*100);
	  
#endif	  
	  file << numCPUs << "\t" << numGPUs << "\t"  << chunkGPU << "\t" <<  runtime 
#ifndef NOENERGY
		  
	       << "\t" << CPUE << "\t" << FPGAE << "\t" << LPDE << "\t" << CPUE+FPGAE <<"\t"<< TOTALE <<"\t"<< CPUW << "\t" << FPGAW <<"\t" << LPDW << "\t" << FPGAoffload 


#endif
	       << endl;
	  file.close();

#ifdef WRITERESULT
		resultfile.close();
#endif

	}

	void printHeaderToFile(ofstream &file){
	  file << "N. CPUs" << SEP << "N. FPGAs" << SEP << "ChunkFPGA" << SEP << "Time (ms)" 
#ifndef NOENERGY 
	       SEP << "CPU Energy" << SEP << "FPGA Energy " << SEP<< "LPD Energy " << SEP<< "CPU+FPGA Energy " << SEP<< "Total Energy "  << SEP<< "W CPU "  << SEP<< "W FPGA "  << SEP<< "W LPD"   << SEP<< "offload "
#endif
	       << endl;
	}
};

