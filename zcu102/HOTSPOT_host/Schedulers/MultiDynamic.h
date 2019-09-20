//============================================================================
// Name			: Dynamic.h
// Author		: Antonio Vilches
// Version		: 1.0
// Date			: 02 / 01 / 2014
// Copyright	: Department. Computer's Architecture (c)
// Description	: Dynamic scheduler implementation
//============================================================================

//#define TRACER

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sched.h>
#include <asm/unistd.h>
#include "Scheduler.h"
#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include <sys/time.h>
   
// #define _GNU_SOURCE already defined
#include <sys/syscall.h>
#include <sys/types.h>

#ifdef Win32
#include "PCM_Win/windriver.h"
//#else
//#include "cpucounters.h"
#endif

#ifdef PJTRACER
#include "pfortrace.h"
#endif

#ifdef TRACER
#include "pfortrace.h"
PFORTRACER tracer("SPMV_SDSOC_Multi.json");
#endif

//#define  OVERHEAD_ANALYSIS
//#define DEBUG

using namespace std;
using namespace tbb;

/*****************************************************************************
* Defines
* **************************************************************************/
#define CPU 0
#define GPU 1
#define GPU_OFF -100 //Arbitrary value
#define SEP "\t"

/*****************************************************************************
* types
* **************************************************************************/
typedef struct{
	int numcpus;
	int numgpus;
	int gpuChunk;
	char benchName[100];
	char kernelName[100];
	pid_t main_thread_id;
} Params;

/*****************************************************************************
* Global variables
* **************************************************************************/
__thread int chunkCPU;
int chunkGPU;
__thread float fG;
#define NACC 16
float gpuThroughput[NACC];
__thread float cpuThroughput =0.0;
__thread int myAccId = 0;
int nAcc;

// To calculate scheduling partition overhead
tick_count end_tc;

#ifdef DEEP_CPU_REPORT
ofstream deep_cpu_report;
#endif

#ifdef DEEP_GPU_REPORT
ofstream deep_gpu_report;
#endif

#ifdef OVERHEAD_ANALYSIS
// overhead accumulators
double overhead_sp = 0.0;
double overhead_h2d = 0.0;
double overhead_kl = 0.0;
double kernel_execution = 0.0;
double overhead_d2h = 0.0;
double overhead_td = 0.0;
#endif

/*int set_thread_affinity_CORE(pthread_t th, int cpu)
{
	cpu_set_t mask;
	__CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	__CPU_SET_S(cpu,sizeof(cpu_set_t), &mask);

	return pthread_setaffinity_np(th, sizeof(cpu_set_t), &mask);
}*/

/*****************************************************************************
* Heterogeneous Scheduler
* **************************************************************************/
/*Bundle class: This class is used to store the information that items need while walking throught pipeline's stages.*/
class Bundle {
public:
	int begin;
	int end;
	int type; //GPU = 0, CPU=1

	Bundle() {};
};

/*My serial filter class represents the partitioner of the engine. This class selects a device and a rubrange of iterations*/
class MySerialFilter: public filter {
private:
	int begin;
	int end;
	int nCPUs;
	int nGPUs;
	pid_t _main_thread_id;
public:
	/*Class constructor, it only needs the first and last iteration indexes.*/
	MySerialFilter(int b, int e, int ncpus, int ngpus, pid_t main_thread_id) : filter(true) {
		begin = b;
		end = e;
		nCPUs = ncpus;
		nGPUs = ngpus;
		_main_thread_id=main_thread_id;
	}

	/*Mandatory operator method, TBB rules*/
	void * operator()(void *) {
		Bundle *bundle = new Bundle();

		/*If there are remaining iterations*/
		if (begin < end) {
			bundle->begin = begin;
			//if (nAcc && !myAccId) { // asignar al thread un acelerador si hay libre y no tiene ya uno
			//	myAccId = nAcc--; // se puede modificar nAcc porque esta etapa es serie
			//	set_thread_affinity_CORE(pthread_self(), nAcc);
			//}

			if (nAcc && !myAccId) { // asignar al thread un acelerador si hay libre y no tiene ya uno
				myAccId = nAcc--; // se puede modificar nAcc porque esta etapa es serie
				set_thread_affinity_CORE(pthread_self(), nAcc);
				printf("FPGA thread: %d\n",nAcc);
			}



			//pid_t actual_thread_id = syscall(SYS_gettid);
			//Checking which resources are available
			//if ( (_main_thread_id == actual_thread_id) && (--gpuStatus >= 0) ){
			if ( myAccId ){  // vemos si es un thread con acelerador asociado
				#ifdef TRACER
				char cad[256];
				sprintf(cad, "SACC%d", myAccId);
				tracer.nodeStart(0, cad);
				#endif
				//GPU WORK
				//GPU WORK
				bundle->end = std::min(begin + chunkGPU, end);

				//~ printf("FPGA: %d / %d\n",chunkGPU, end-begin);
				//~ printf("thr   --> FPGA %f CPU %f  fG %f\n",gpuThroughput,cpuThroughput, fG);

				//GPU CHUNK
				begin = bundle->end;
				//bundle->type = GPU;
				bundle->type = myAccId;  // el tipo ahora lleva el id del acelerador (aunque no haria falta ya que esta asociado al thread)
				#ifdef TRACER
				tracer.nodeStop(0, cad);
				#endif
				return bundle;
			}else{
				#ifdef TRACER
				tracer.nodeStart(0, "SCPU");
				#endif
				//CPU WORK
				//if (_main_thread_id == actual_thread_id){
				//	gpuStatus++;
				//}
				/*Calculating next chunkCPU*/
				float maxGpuThroughput = gpuThroughput[1];
				for (int i=2; i < nGPUs+1; i++)
				maxGpuThroughput = std::max(maxGpuThroughput, gpuThroughput[i]);
				if(maxGpuThroughput >0 && cpuThroughput>0){
					fG = maxGpuThroughput/cpuThroughput;

					chunkCPU = ((end-begin < chunkGPU)? end-begin : chunkGPU) / fG ;
					chunkCPU = std::min((float)chunkCPU, (float)(end-begin)/((float)nCPUs + fG));
				}
				if(chunkCPU==0) chunkCPU=1;  // aqui si se establece minimo para CPU
				if(nGPUs==0) //implement guided scheduling between the cores
				{
					chunkCPU=max((end-begin)/(nCPUs), 1);
				}

				//~ printf("CPU: %d / %d\n",chunkCPU, end-begin);
				//~ printf("thr   --> FPGA %f CPU %f  fG %f\n",gpuThroughput,cpuThroughput, fG);

				/*Taking a iteration chunk for CPU*/
				bundle->end = std::min(begin + chunkCPU, end);
				begin = bundle->end;
				bundle->type = CPU;
				#ifdef TRACER
				tracer.nodeStop(0, "SCPU");
				#endif
				return bundle;
			}
		}
		return NULL;
	} // end operator
};


double getTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec + tv.tv_sec*1e6;
}

/*MyParallelFilter class is the executor component of the engine, it executes the subrange onto the device selected by SerialFilter*/
template <class B>
class MyParallelFilter: public filter {
private:
	B *body;

public:
	/*Class' constructor*/
	//template <class B>
	MyParallelFilter(B *b) : filter(false) {
		body = b;
	}

	/*Operator function*/
	void * operator()(void * item) {
		//variables
		Bundle *bundle = (Bundle*) item;

		if(bundle->type != CPU){
			// GPU WORK
			#ifdef DEBUG
			cerr << "launchGPU(): begin: " << bundle->begin << " end: " << bundle->end << endl;
			#endif
			#ifdef TRACER
			char cad[256];
			sprintf(cad, "ACC%d", bundle->type);
			tracer.nodeStart(bundle->type, cad);
			#endif

			tick_count start_tc = tick_count::now();

			body->OperatorGPU(bundle->begin, bundle->end, bundle->type);

			end_tc = tick_count::now();
			
			#ifdef TRACER
			tracer.nodeStop(bundle->type, cad);
			#endif

			float time =(end_tc - start_tc).seconds()*1000;
			//cout << " TIME: " << time << endl;
			//	gpuThroughput = max((bundle->end - bundle->begin) / time , gpuThroughput) ;
			gpuThroughput[bundle->type] = (bundle->end - bundle->begin) / time ;
			//cerr << "Chunk GPU: " << bundle->end - bundle->begin << " TH: " << gpuThroughput /*((tg5-tg1)/1000000.0) */ << endl;

			//if(gpuThroughput >0 && cpuThroughput>0)	fG = gpuThroughput/cpuThroughput;
			#ifdef DEBUG
			printf("FPGA SIZE:%d  TH: %f\n", bundle->end - bundle->begin, (bundle->end - bundle->begin) / time);
			#endif
			#ifdef DEEP_GPU_REPORT

			deep_gpu_report << " Step: " << step << " chunkCPU: " << chunkCPU << " chunkGPU: " << chunkGPU << " cpuThroughput: " << cpuThroughput << " gpuThroughput: " << gpuThroughput << " fG: " << fG << " begin: "<< bundle->begin <<endl;
			#endif

			/*If CPU has already computed some chunk, then we update fG (factor GPU)
			if (cpuThroughput > 0){
			fG = gpuThroughput/cpuThroughput;
		}*/

		/*To release GPU token*/

	}else{
		// CPU WORK
		#ifdef DEBUG
		cerr << "launchCPU(): begin: " << bundle->begin << " end: " << bundle->end << endl;
		#endif
		#ifdef TRACER
		tracer.nodeStart(10, "CPU");
		#endif

 		double start_m;
		double end_m;
		double execution_time_m;

		tick_count start = tick_count::now();
		start_m = getTimestamp();
		body->OperatorCPU(bundle->begin, bundle->end);
		tick_count end = tick_count::now();
		end_m = getTimestamp();

		execution_time_m = (end_m-start_m)/(1000);

		//printf("Software execution time  %.6f ms elapsed\n", execution_time_m);


		#ifdef TRACER
		tracer.nodeStop(10, "CPU");
		#endif
		float time =(end-start).seconds()*1000;
		//cout << "CPU TIME: " << time << endl;
		cpuThroughput = (bundle->end - bundle->begin) / time;
		//if(gpuThroughput >0 && cpuThroughput>0)	fG = gpuThroughput/cpuThroughput;
		#ifdef DEEP_CPU_REPORT
		//deep_cpu_report << bundle->end-bundle->begin << "\t" << cpuThroughput << endl;

		deep_cpu_report << " Step: " << step << " chunkCPU: " << chunkCPU << " chunkGPU: " << chunkGPU << " cpuThroughput: " << cpuThroughput << " gpuThroughput: " << gpuThroughput << " fG: " << fG << " begin: "<< bundle->begin << endl;
		#endif
		//printf("CPU %d->%d:%d %f %f\n", bundle->begin, bundle->end, bundle->end - bundle->begin, time, cpuThroughput);
		/*If GPU has already computed some chunk, then we update fG (factor GPU)
		if (gpuThroughput > 0){
		fG = gpuThroughput/cpuThroughput;
	}*/
}
/*Deleting bundle to avoid memory leaking*/
delete bundle;
return NULL;
}
};
//end class

/*Oracle Class: This scheduler version let us to split the workload in two subranges, one for GPU and one for CPUs*/
class Dynamic : public Scheduler<Dynamic, Params> {
	Params * pars;
public:
	/*This constructor just call his parent's contructor*/
	Dynamic(Params *params) : Scheduler<Dynamic, Params>(params),
	pars(params)
	{
		//		Params * p = (Params*) params;
		//		pars = p;

		chunkCPU = 0;
		fG = 0.0;
		chunkGPU = pars->gpuChunk;
		//gpuThroughput=0.0;
		cpuThroughput=0.0;
		for (int i=0; i<= pars->numgpus; i++)
		gpuThroughput[i] = 0;
		nAcc = pars->numgpus;
		myAccId = 0;

		//Initializing library PJTRACER
		initializePJTRACER();
	}

	/*Initializes PJTRACER library*/
	void initializePJTRACER(){
		#ifdef PJTRACER
		char traceFname[1024];
		sprintf(traceFname, "DYNAMIC_C_%d_G_%d.trace", nCPUs, nGPUs);
		tracer = new PFORTRACER(traceFname);
		tracer->beginThreadTrace();
		#endif
	}

	/*The main function to be implemented*/
	template<class T>
	void heterogeneous_parallel_for(int begin, int end, T* body){
		#ifdef DEBUG
		cerr << "Heterogeneous Parallel For Dynamic " << nCPUs << " , " << nGPUs << ", " << chunkGPU << endl;
		#endif
		/*Preparing pipeline*/
		pipeline pipe;
		MySerialFilter serial_filter(begin, end, nCPUs, nGPUs, pars->main_thread_id);
		MyParallelFilter<T> parallel_filter(body);
		pipe.add_filter(serial_filter);
		pipe.add_filter(parallel_filter);
		//chunkCPU = chunkGPU * 0.2;
		if(fG==0.0) fG = 8; // 25 para Nbody?
		chunkCPU = min((int)(chunkGPU / fG), (int)((end-begin)/100));
		if(!chunkCPU) chunkCPU=1;
		body->firsttime = true;

		#ifdef DEEP_CPU_REPORT
		char nombre[1024];
		sprintf(nombre, "%s_Dynamic_deep_CPU_report_step_%d_GPU_%d.txt", pars->benchName, step, chunkGPU);


		deep_cpu_report.open(nombre, ios::out | ios::app);
		#endif
		#ifdef DEEP_GPU_REPORT
		{
			char nombre[1024];
			sprintf(nombre, "%s_Dynamic_deep_GPU_report_step_%d_GPU_%d.txt", pars->benchName, step, chunkGPU);
			deep_gpu_report.open(nombre, ios::out | ios::app);
		}
		#endif

		/*Seeting a mark to recognize a timestep*/
		#ifdef PJTRACER
		tracer->newEvent();
		#endif

		#ifdef OVERHEAD_ANALYSIS
		end_tc = tick_count::now();
		#endif
		/*Run the pipeline*/
		pipe.run(nCPUs + nGPUs);
		pipe.clear();
		#ifdef DEEP_CPU_REPORT
		deep_cpu_report.close();
		#endif
		#ifdef DEEP_GPU_REPORT
		deep_gpu_report.close();
		#endif
	}

	/*this function print info to a Log file*/
	void saveResultsForBench(){

		char * execution_name = (char *)malloc(sizeof(char)*100);
		sprintf(execution_name, "_Dynamic_%d_%d.txt", nCPUs, nGPUs);
		strcat(pars->benchName, execution_name);

		/*Checking if the file already exists*/
		bool fileExists = isFile(pars->benchName);
		ofstream file(pars->benchName, ios::out | ios::app);
		if(!fileExists){
			printHeaderToFile(file);
		}

		cout << "TIEMPO TOTAL: " << runtime << endl;
		#ifdef OVERHEAD_STUDY
		cout
		<< "Kernel Execution: " << kernel_execution << endl
		<< "O. Kernel Launch: " << overhead_kl << endl
		<< "O. Th. Dispatch:  " << overhead_td << endl
		<< "O. sched. Part:   " << overhead_sp << endl
		<< "O. Host2Device:   " << overhead_h2d << endl
		<< "O. Device2Host:   " << overhead_d2h << endl;
		#endif

		file << nCPUs << "\t" << nGPUs << "\t"  << chunkGPU << "\t" <<  runtime << "\t"
		#ifdef PCM

		<< getPP0ConsumedJoules(sstate1, sstate2) << "\t" << getPP1ConsumedJoules(sstate1, sstate2) << "\t"
		<< getConsumedJoules(sstate1, sstate2) - getPP0ConsumedJoules(sstate1, sstate2) - getPP1ConsumedJoules(sstate1, sstate2) << "\t" <<  getConsumedJoules(sstate1, sstate2) << "\t"
		<< getL2CacheHits(sktstate1[0], sktstate2[0]) << "\t" << getL2CacheMisses(sktstate1[0], sktstate2[0]) << "\t" << getL2CacheHitRatio(sktstate1[0], sktstate2[0]) <<"\t"
		<< getL3CacheHits(sktstate1[0], sktstate2[0]) << "\t" << getL3CacheMisses(sktstate1[0], sktstate2[0]) << "\t" << getL3CacheHitRatio(sktstate1[0], sktstate2[0]) <<"\t"
		<< getCyclesLostDueL3CacheMisses(sstate1, sstate2)

		#endif
		#ifdef PMLIB
		<< energiaCPU << "\t"  << energiaFPGA << "\t"<< energiaFPGA +energiaCPU<< "\t"

		#endif
		<< endl;
		file.close();
		#ifndef NDEBUG
		cerr << nCPUs << "\t" << nGPUs << "\t"  << runtime << "\t"
		//		<< getPP0ConsumedJoules(sstate1, sstate2) << "\t" << getPP1ConsumedJoules(sstate1, sstate2) << "\t"
		//		<< getConsumedJoules(sstate1, sstate2) - getPP0ConsumedJoules(sstate1, sstate2) - getPP1ConsumedJoules(sstate1, sstate2) << "\t" <<  getConsumedJoules(sstate1, sstate2) << "\t"
		//		<< getL2CacheHits(sktstate1[0], sktstate2[0]) << "\t" << getL2CacheMisses(sktstate1[0], sktstate2[0]) << "\t" << getL2CacheHitRatio(sktstate1[0], sktstate2[0]) <<"\t"
		//		<< getL3CacheHits(sktstate1[0], sktstate2[0]) << "\t" << getL3CacheMisses(sktstate1[0], sktstate2[0]) << "\t" << getL3CacheHitRatio(sktstate1[0], sktstate2[0]) <<"\t"
		//		<< getCyclesLostDueL3CacheMisses(sstate1, sstate2)
		<< endl;
		#endif
	}

	void printHeaderToFile(ofstream &file){
		file << "N. CPUs" << SEP << "N. GPUs" << SEP << "Chunk" << SEP << "Time (ms)" << SEP
		#ifdef PCM
		<< "CPU Energy(J)" << SEP << "GPU Enegy(J)" << SEP << "Uncore Energy(J)" << SEP << "Total Energy (J)" << SEP
		<< "L2 Cache Hits" << SEP << "L2 Cache Misses" << SEP << "L2 Cache Hit Ratio" << SEP
		<< "L3 Cache Hits" << SEP << "L3 Cache Misses" << SEP << "L3 Cache Hit Ratio" << SEP << "Cycles lost Due to L3 Misses"

		#endif
		#ifdef PMLIB
		<< "CPU Energy (BBB)" << SEP << "FPGA Energy (BBB) " << SEP

		#endif
		<< endl;
	}
};