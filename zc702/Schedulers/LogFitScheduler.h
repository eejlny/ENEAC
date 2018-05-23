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

#define GPU 1
#define MAXPOINTS 1024

int computeUnits =1;

/*****************************************************************************
 * Global variables
 * **************************************************************************/

//~ tbb::atomic<int> gpuStatus;
//~ tbb::atomic<int> firsttime_cpu; //andres
int numCPUs;
int numGPUs;
//~ int chunkGPU;
//int* interrupt_address;
//~ __thread int chunkCPU;
//~ __thread float cpuThroughput;
//~ float gpuThroughput;
//~ float fG;
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
 * Global variables
 * **************************************************************************/

atomic<int> gpuStatus;
float meanchunkGPU=0;
unsigned long num_chunkGPU=0;
int chunkGPU;
int chunkCPU;
int minChunkGPU;
int minChunkCPU;
bool stopConditionModeOn;

bool explorationPhase;
bool stablePhase;
bool veryfirsttime;
int measurements;
int numberPoints;
float cpuThroughput;
float gpuThroughput;
float fG;
bool fGvalid = false;

int totaltries=1;
int initchunk=0;
int finalchunk=0;

// Model variables
float y[MAXPOINTS];
float x[MAXPOINTS];
float threshold;
bool calculateLog = true;
float C1[MAXPOINTS];
float C2[MAXPOINTS];
bool calcluateC1C2 = true;
float gpuThDecay = 0.98f;
float chunkGPU_initratio = 0.002f;
float gpuSlope = 0.01f;  //SLOPE

//Summarize GPU statistics
int itemsOnGPU = 0;
int totalIterationsGPU = 0;
tick_count end_tc;

#ifdef OVERHEAD_STUDY
// overhead accumulators
float overhead_sp = 0.0;
float overhead_h2d = 0.0;
float overhead_kl = 0.0;
float kernel_execution = 0.0;
float overhead_d2h = 0.0;
float overhead_td = 0.0;
cl_ulong tg1, tg2, tg3, tg4, tg5;
#endif

#ifdef NEWLOGFIT
bool newfit = true;
float baseGpuThroughputMean = 0.0;
float gpuThroughputMean = 0.0;
float thresholdThrGpuMean = 0.70;
float alfaThrGpuMean = 0.5;
#ifdef NEWLOGFIT2
float fitTime = 0.0;
tick_count lastFitTime;
tick_count startFitTime;
float overheadFit = 0.05;
#endif
#endif

float fitTime = 0.0;
tick_count lastFitTime;
tick_count startFitTime;



// FUNCTIONS FOR LOGFIT MODEL


int calculateLogarithmicModelv2(float *x, float *y, int numberPoints, int last_point) {
	//variables
	int calculatedChunk;
	float numerador, denominador;
	static float sumatorio1s = 0.0, sumatorio2s = 0.0, sumatorio3s = 0.0, sumatorio4s = 0.0;
	float sumatorio1 = 0.0, sumatorio2 = 0.0, sumatorio3 = 0.0;
	float sumatorio4 = 0.0, sumatorio5 = 0.0;
	float a;
	//points
	int cero, one, two, three;
	//end variables

	int i;
	if (calculateLog) {
		// la primera vez se calculan todos los coeficientes para todos los puntos
		// excepto para el ultimo que se hace siempre
		calculateLog = false;
		for (int j = 0; j < numberPoints - 1; j++) {
			i = ceil((float) j * (float) (last_point) / (float) numberPoints);
#ifdef DEBUGLOG
			cerr << " x[" << x[i] << "] = " << y[i];
#endif
			float logaux = logf(x[i]);
			sumatorio1s += y[i] * logaux;
			sumatorio2s += logaux;
			sumatorio3s += y[i];
			sumatorio4s += logaux*logaux;
		}
#ifdef DEBUGLOG
		cerr << endl;
#endif
	}
	// calculamos el del ultimo
	i = last_point;
#ifdef DEBUGLOG
	cerr << " x[" << x[i] << "] = " << y[i];
#endif
	float logaux = logf(x[i]);
	sumatorio1 = sumatorio1s + y[i] * logaux;
	sumatorio2 = sumatorio2s + logaux;
	sumatorio3 = sumatorio3s + y[i];
	sumatorio4 = sumatorio4s + logaux*logaux;
#ifdef DEBUGLOG
	cerr << endl;
#endif
	int np = numberPoints;
	sumatorio5 = sumatorio2*sumatorio2;
	sumatorio4 *= np;
	numerador = (np * sumatorio1) - (sumatorio3 * sumatorio2);
	denominador = sumatorio4 - sumatorio5;

	//Getting the value for a
	a = numerador / denominador;
	
	//if(a<0) return -1;
	
	//The threshold is set during first call
	if (threshold == 0.0) {
		threshold = a / x[last_point];
		//cerr << "thr " << threshold << endl;
	}
	//Just to get a multiple of computeUnit
	calculatedChunk = ceil((a / threshold) / (float) computeUnits)*(float) computeUnits;
#ifdef DEBUGLOG
	cerr << "threshold " << threshold << " a " << a << " chunk " << calculatedChunk << endl;
#endif
	//return 100000;
#ifdef LOGFITFIXEDCHUNK
	return LOGFITFIXEDCHUNK;
#else
	return (calculatedChunk < computeUnits) ? computeUnits : calculatedChunk;
#endif
}

// aproxima el througput de la GPU para un "chunk" dado, tomando como base
// las medidas tomadas de th. en la fase inicial antes del fitting (puntos x,y)
// comprueba a que intervalo de x's pertenece "chunk" y calcula el th. de la
// GPU para ese "chunk" considerando lineal la varacición del th. entre las dos
// y's correspondientes a ese intervalo (ecuación 4.13 tesis Vilches)

float aproxGPUTh(int chunk) {
	int intervalo = 0;
	bool found = false;
	if (calcluateC1C2) {
		// los coeficientes C1 y C2 que definen las restas en cada intervalo
		// [x1,x2] se precalculan y almacenan en vectores. El único que no se
		// puede almacenar es el del ultimo intevalo ya que el valor de x e y
		// van variando durante la ejecucion
		calcluateC1C2 = false;
		for (int i = 0; i < measurements; i++) {
			C1[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
			C2[i] = y[i] - C1[i] * x[i];
			if (!found && chunk <= x[i]) {
				intervalo = i;
				found = true;
			}
		}
	} else {
		// siempre se calculan los coeficientes C1 y C2 para el ultimo tramo
		// ya que el ultimo punto (chunk, throughput) siempre se va actualizando
		int i = measurements - 1;
		C1[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
		C2[i] = y[i] - C1[i] * x[i];
	}
	while (!found && intervalo <= measurements) {
		if (chunk <= x[intervalo]) {
			found = true;
		} else {
			intervalo++;
		}
	}
	if (!found) { // mayor que el chunk mas grande medido en GPU
		return y[measurements];
	} else if (intervalo == 0) {
		return chunk * y[0] / x[0];
	} else {
		int i = intervalo - 1;
		return C1[i] * chunk + C2[i];
	}
}

// Implementacion del "final phase" de la tesis de Vilches. 
// Para un ultimo trozo de iteraciones "remain", va a calcular el trozo de GPU (chGPU)
// y el primero de CPU (chCPU, el resto se hará por self-guided) que minimicen el 
// tiempo de cálculo para ese "remain".
// Coge el minimo tiempo que tardarían las CPUs solas, o la GPU sola, o los dos
// dispositivos trabajando en paralelo (usando medidas x,y tomadas para el fitting e 
// igualando tiempo estimado para GPU y para CPU al repartir ramain)

void getLastChunks(int remain, int* chCPU, int *chGPU, int nCpus) {
	float tmin;
	float tcpu = (float) remain / (cpuThroughput * nCpus);
	(*chCPU) = remain / nCpus;
	(*chGPU) = 0;
	tmin = tcpu;

#ifdef DEBUGLOG
	cerr << "getLastChunks: Only CPU Throughput: " << cpuThroughput * nCpus << " T " << tcpu << endl;
#endif

	float tgpu = (float) remain / aproxGPUTh(remain); // se calculan C1 y C2
	if (tgpu < tmin) {
		(*chCPU) = 0;
		(*chGPU) = remain;
		tmin = tcpu;
	}

#ifdef DEBUGLOG
	cerr << "getLastChunks: Only GPU Throughput: " << remain / tgpu << " T " << tgpu << endl;
#endif

	float thet;
	for (int i = 0; i < measurements; i++) {
		float B = (cpuThroughput * nCpus) - C1[i] * remain + C2[i];
		float C = -C2[i] * remain;
		float sq = sqrt(B * B - 4 * C1[i] * C);
		float cG1 = (-B + sq) / (2 * C1[i]);
		float cG2 = (-B - sq) / (2 * C1[i]);
#ifdef DEBUGLOG
		cerr << "getLastChunks: Het" << i << "[" << x[i] << ":" << x[i + 1] << "][" << y[i] << ":" << y[i + 1] << "]: cG1 " << cG1 << " cG2 " << cG2 << endl;
#endif            
		if (cG1 >= x[i] && cG1 <= x[i + 1]) {
			thet = cG1 / aproxGPUTh(cG1);
#ifdef DEBUGLOG
			cerr << "getLastChunks: Het Throughput" << i << ": " << cG1 / thet << " para " << cG1 << " T " << thet << endl;
#endif            
			if (thet < tmin) {
				tmin = thet;
				(*chCPU) = (remain - cG1) / nCpus;
				(*chGPU) = cG1;
			}
		} else if (cG2 >= x[i] && cG2 <= x[i + 1]) {
			thet = cG2 / aproxGPUTh(cG2);
#ifdef DEBUGLOG
			cerr << "getLastChunks: Het Throughput" << i << ": " << cG2 / thet << " para " << cG2 << endl;
#endif            
			if (thet < tmin) {
				tmin = thet;
				(*chCPU) = (remain - cG2) / nCpus;
				(*chGPU) = cG2;
			}
		}
	}
}



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
public:
	/*Class constructor, it only needs the first and last iteration indexes.*/
	MySerialFilter(int b, int e, int ncpus, int ngpus) : filter(true) {
		begin = b;
		end = e;
		nCPUs = ncpus;
		nGPUs = ngpus;
	    stopConditionModeOn = false;
	}

	/*Mandatory operator method, TBB rules*/
void * operator()(void *) {

		Bundle *bundle = new Bundle();
		//If there are remaining iterations
		if (begin < end) {
			//Checking whether the GPU is idle or not.
			if (--gpuStatus >= 0) {
#ifdef DEBUG 
				cerr << "Serial Filter GPU: " << chunkGPU << " Begin = " << begin << ", End = " << end << " fg " << fG << endl;
#endif
				//Checking stop condition
#ifdef LOGFITFIXEDCHUNK
				stopConditionModeOn = false; // Borrar cuando se quite tamaño fijo
#else
				if (stablePhase) stopConditionModeOn = GPU_Stop_Condition();
#endif
#ifdef NEWLOGFIT
				if (newfit) stopConditionModeOn = false;
#endif
				if (!stopConditionModeOn) {
					int auxEnd = begin + chunkGPU;
					auxEnd = (auxEnd > end) ? end : auxEnd;
					bundle->begin = begin;
					bundle->end = auxEnd;
					begin = auxEnd;
					bundle->type = GPU;
					meanchunkGPU+=(float)(bundle->end-bundle->begin);
					num_chunkGPU++;
					return bundle;
				} else { //stop Condition is true
					//concord case
#ifdef DEBUG
					//system("pause");
#endif
					/*
                                                            float estGPUThroughput = simulateGPUThroughput(bundle->end - bundle->begin);
                                                            cerr << "***** Concord Case => estGPUThro: "<< estGPUThroughput << endl;
                                                            cpuThroughput = cpuThroughput*nCPUs;
                                                            int chunkGPU_last = max((end-begin)*estGPUThroughput/(estGPUThroughput+cpuThroughput), (float)(computeUnits*vectorization)/2.0f);
                                                            chunkCPU = max(ceil( ((end-begin)-chunkGPU_last)/nCPUs), 1.0);
                                                            cerr << "***** Concord Case => Begin: "<< begin << " end: " << end << " chunkG " << chunkGPU_last << " chunkC " << chunkCPU << endl;
					 */
					int lastChCPU, lastChGPU;
					getLastChunks(end - begin, &lastChCPU, &lastChGPU, nCPUs);

#ifdef DEBUG
					cerr << "***** Concord Case => Begin: " << begin << " end: " << end << " chunkG " << lastChGPU << " chunkC " << lastChCPU << endl;
					//system("pause");
#endif
					if (lastChGPU > 0) {
						int auxEnd = begin + lastChGPU;
						auxEnd = (auxEnd > end) ? end : auxEnd;
						bundle->begin = begin;
						bundle->end = auxEnd;
						begin = auxEnd;
						meanchunkGPU+=(float)(bundle->end-bundle->begin);
						num_chunkGPU++;
						bundle->type = GPU;
						return bundle;
					} else { // paramos la GPU
						// no incrementamos gpuStatus para dejar pillada la GPU
						nCPUs++;
						int auxEnd = begin + lastChCPU;
						auxEnd = (auxEnd > end) ? end : auxEnd;
						bundle->begin = begin;
						bundle->end = auxEnd;
						begin = auxEnd;
						bundle->type = CPU;
						return bundle;
					}
				}
			} else {
				//CPU WORK
				gpuStatus++;
				if (!stopConditionModeOn && nGPUs) {
					if (fGvalid) {
						chunkCPU = max( min(chunkGPU/fG , (end - begin) / (fG + nCPUs)),  (float) minChunkCPU);
					}
				} else {
					chunkCPU = max((end - begin) / (nCPUs+1), minChunkCPU);
				}
#ifdef DEBUG
				cerr << "Serial Filter CPU: " << chunkCPU << " Begin = " << begin << ", End = " << end << " fg " << fG << " chunkGPU " << chunkGPU << endl;
#endif
				//cout << "fg " << fG << " chunkCPU " << chunkCPU << endl;
				int auxEnd = begin + chunkCPU;
				auxEnd = (auxEnd > end) ? end : auxEnd;
				bundle->begin = begin;
				bundle->end = auxEnd;
				begin = auxEnd;
				bundle->type = CPU;
				return bundle;
			}
		}
		return NULL;
	} // end operator

	bool GPU_Stop_Condition() {
		bool stopGPU = false;

		//Checking whether there are enough remaining itexrations
		if ((nCPUs > 1) && fGvalid && ((chunkGPU / fG) > ((end - begin) - chunkGPU) / (nCPUs))) {
			stopGPU = true;
		}
		return stopGPU;
	}
};

/*MyParallelFilter class is the executor component of the engine, it executes the subrange onto the device selected by SerialFilter*/

/*MyParallelFilter class is the executor component of the engine, it executes the subrange onto the device selected by SerialFilter*/
//template <class B>
class MyParallelFilter : public filter {
private:
//	B *body;
	NbodyTask *body;
	int iterationSpace;

public:

	MyParallelFilter(NbodyTask *b, int i) :
		filter(false) {
		body = b;
		iterationSpace = i;

	}

	void * operator()(void * item) {

		//variables
		Bundle *bundle = (Bundle*) item;

		if (bundle->type == GPU) {
			// GPU WORK
#ifdef PJTRACER
			tracer->gpuStart();
#endif
			executeOnGPU(bundle);
#ifdef PJTRACER
			tracer->gpuStop();
#endif
			//cout << " ****** h2d: " << eprof_overhead_h2d << " ****** " << endl;
			//To release the GPU token
			gpuStatus++;
		} else {
			// CPU WORK
#ifdef PJTRACER
			tracer->cpuStart();
#endif
			executeOnCPU(bundle);
#ifdef PJTRACER
			tracer->cpuStop();
#endif
		}
		delete bundle;
		return NULL;
	}

	void executeOnGPU(Bundle *bundle) {
		static bool firstmeasurement = true;
		totGPU+= (bundle->end - bundle->begin) ;

		tick_count start_tc = tick_count::now();
#ifdef OVERHEAD_STUDY
		//Calculating partition scheduling overhead
		overhead_sp = overhead_sp + ((start_tc - end_tc).seconds()*1000);

		//Adding a marker in the command queue
		cl_event event_before_h2d;
		int error = clEnqueueMarker(command_queue, &event_before_h2d);
		if (error != CL_SUCCESS) {
			cerr << "Failed equeuing start event" << endl;
			exit(0);
		}
#endif

		body->sendObjectToGPU(bundle->begin, bundle->end);

#ifdef OVERHEAD_STUDY
		//Adding a marker in the command queue
		cl_event event_before_kernel;
		error = clEnqueueMarker(command_queue, &event_before_kernel);
		if (error != CL_SUCCESS) {
			cerr << "Failed equeuing start event" << endl;
			exit(0);
		}
		cl_event event_kernel;
		body->OperatorGPU(bundle->begin, bundle->end, &event_kernel);
#else
		body->OperatorGPU(bundle->begin, bundle->end);
#endif

#ifdef OVERHEAD_STUDY
		//Adding a marker in the command queue
		cl_event event_after_kernel;
		error = clEnqueueMarker(command_queue, &event_after_kernel);
		if (error != CL_SUCCESS) {
			cerr << "Failed equeuing start event" << endl;
			exit(0);
		}
#endif

		body->getBackObjectFromGPU(bundle->begin, bundle->end);

#ifdef OVERHEAD_STUDY
		//Adding a marker in the command queue
		cl_event event_after_d2h;
		error = clEnqueueMarker(command_queue, &event_after_d2h);
		if (error != CL_SUCCESS) {
			cerr << "Failed equeuing start event" << endl;
			exit(0);
		}
	
#endif
	//clFinish(command_queue);

#ifdef OVERHEAD_STUDY
		//Adding a marker in the command queue to computer thread dispatch
		cl_event event_after_finish;
		error = clEnqueueMarker(command_queue, &event_after_finish);
		if (error != CL_SUCCESS) {
			cerr << "Failed equeuing start event" << endl;
			exit(0);
		}
#endif

		end_tc = tick_count::now();

#ifdef OVERHEAD_STUDY
		//Calculating host to device transfer overhead
		clGetEventProfilingInfo(event_before_h2d, CL_PROFILING_COMMAND_END, sizeof (cl_ulong), &tg1, NULL);
		clGetEventProfilingInfo(event_before_kernel, CL_PROFILING_COMMAND_START, sizeof (cl_ulong), &tg2, NULL);
		if (tg2 > tg1) {
			overhead_h2d = overhead_h2d + (tg2 - tg1) / 1000000.0; // ms
		}
		//cerr << "Overhead h2d: " << overhead_h2d << ": " << tg1 << ", " << tg2 << " = " << (tg2-tg1) <<endl;

		//Calculating kernel launch overheads
		clGetEventProfilingInfo(event_before_kernel, CL_PROFILING_COMMAND_END, sizeof (cl_ulong), &tg2, NULL);
		clGetEventProfilingInfo(event_kernel, CL_PROFILING_COMMAND_START, sizeof (cl_ulong), &tg3, NULL);
		if (tg3 > tg2) {
			overhead_kl = overhead_kl + (tg3 - tg2) / 1000000.0; // ms
		}
		//Calculating kernel execution
		//clGetEventProfilingInfo(event_kernel, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &tg3, NULL);
		clGetEventProfilingInfo(event_kernel, CL_PROFILING_COMMAND_END, sizeof (cl_ulong), &tg4, NULL);
		if (tg4 > tg3) {
			kernel_execution = kernel_execution + (tg4 - tg3) / 1000000.0; // ms
		}
		//Calculating device to host transfer overhead
		//clGetEventProfilingInfo(event_kernel, CL_PROFILING_COMMAND_COMPLETE, sizeof(cl_ulong), &tg4, NULL);
		clGetEventProfilingInfo(event_after_d2h, CL_PROFILING_COMMAND_START, sizeof (cl_ulong), &tg5, NULL);
		if (tg5 > tg4) {
			overhead_d2h = overhead_d2h + (tg5 - tg4) / 1000000.0; // ms
		}
		//Calculating device overhead thread dispatch
		if (tg5 > tg1) {
			overhead_td = overhead_td + ((end_tc - start_tc).seconds()*1000) - ((tg5 - tg1) / 1000000.0); // ms
		}

		//Releasing event objects
		clReleaseEvent(event_before_h2d);
		clReleaseEvent(event_before_kernel);
		clReleaseEvent(event_kernel);
		clReleaseEvent(event_after_kernel);
		clReleaseEvent(event_after_d2h);
		clReleaseEvent(event_after_finish);

#endif

		float time = (end_tc - start_tc).seconds()*1000;

#ifdef OUTPUT_FREQ_HISTOGRAM
		float th = (bundle->end - bundle->begin) / ((end - start).seconds() * 1000);
		for (int i = bundle->begin; i < bundle->end; i++) {
			histogram << bundle->end - bundle->begin << "\t" << th << endl;
		}
#endif
		//Summarizing GPU Statistics
		totalIterationsGPU = totalIterationsGPU + (bundle->end - bundle->begin);
		itemsOnGPU = itemsOnGPU + 1;

		//It doesn't update gpuThorughput in the last scheduling interval
		if (!stopConditionModeOn || (stopConditionModeOn && (bundle->begin == 0))) {
			gpuThroughput = (bundle->end - bundle->begin) / time;
			if (cpuThroughput > 0) {
				fG = gpuThroughput / cpuThroughput;
				fGvalid = true;
			}
		}

#if defined(DEBUG_GPU) || defined(DEBUG)
		cerr << "Chunk GPU: " << bundle->end - bundle->begin << " TH: " << gpuThroughput /*((tg5-tg1)/1000000.0) */  << " it: " << bundle->begin << endl;
#endif
		if (explorationPhase) {
			//it only stores increasing chunksize points
			if (x[measurements] < (bundle->end - bundle->begin) && !stopConditionModeOn) {
				//Update the number of points
				if (firstmeasurement) {
					firstmeasurement = false;
					y[measurements] = gpuThroughput;
					x[measurements] = (bundle->end - bundle->begin);
					

				} else {
					y[measurements + 1] = gpuThroughput;
					x[measurements + 1] = (bundle->end - bundle->begin);
					measurements++;
					totaltries++;
				}
				//Checking for Stable condition
				
#ifdef DEBUG
					cerr << "******** Checking ********* np: " <<  numberPoints << " mea: " << measurements << "ref: " <<  y[measurements - 2] << " , " <<y[measurements - 1] * gpuThDecay  << " , " << y[measurements] * gpuThDecay << endl;
#endif

				if(measurements && (measurements < (numberPoints)) && (y[measurements] <= y[measurements - 1]) )
					{
					 	firstmeasurement=true;
					 	explorationPhase = true;
						stablePhase = false;
						measurements=0;
						numberPoints=4;
						chunkGPU = x[0]*0.1;
						if(chunkGPU<1) chunkGPU=1;
						calculateLog=true;
						for(int i=0; i<MAXPOINTS; i++) x[i]=0;
						
#ifdef DEBUG
					cerr << "******** Reset exploration ********* " << "Mea: x[" << measurements << "]=" << x[measurements]  << endl;
#endif
		
					}
					else
					{
#ifdef SLOPE
				if ((measurements >= (numberPoints + 2))
						&& ((y[measurements-2]-y[measurements])/(x[measurements-2]-x[measurements])<=gpuSlope)
						&& ((y[measurements-2]-y[measurements-1])/(x[measurements-2]-x[measurements-1])<=gpuSlope)
						|| x[measurements] >= iterationSpace) {
#endif

#ifndef SLOPE							
				if ((measurements >= (numberPoints + 2)) && (y[measurements] * gpuThDecay < y[measurements - 2]) && (y[measurements - 1] * gpuThDecay < y[measurements - 2])
						|| x[measurements] >= iterationSpace) {
#endif

#ifdef DEBUG
					cerr << "******** Stable phase ********* np: " <<  numberPoints << " mea: " << measurements << endl;
#endif
					finalchunk=x[measurements-2];
					// hemos llegado a stable sin llegar a medir todo el espacio para la GPU
					// es decir, se ha encontrado punto de aplanamiento del th de GPU (2 puntos atras)
					if (x[measurements] < iterationSpace) measurements -= 2;
					// ajustamos el numero de puntos por si el numero de medidas
					// se ha quedado por debajo
					if (numberPoints > measurements + 1) numberPoints = measurements + 1;
					//chunkGPU = calculateLogarithmicModel(x, y, numberPoints, measurements);
					chunkGPU = calculateLogarithmicModelv2(x, y, numberPoints, measurements);
					
					

					explorationPhase = false;
					stablePhase = true;

#ifdef NEWLOGFIT
					cerr << "Mea: x[" << measurements << "]=" << x[measurements] << " chunkGPU: " << chunkGPU << endl;
					newfit = false;
					baseGpuThroughputMean = y[measurements];
					gpuThroughputMean = baseGpuThroughputMean;
#ifdef NEWLOGFIT2
					lastFitTime = end_tc;
					fitTime = (end_tc - startFitTime).seconds()*1000;
#endif
#endif
				lastFitTime = end_tc;
				fitTime = (end_tc - startFitTime).seconds()*1000;
				
				} else {
					chunkGPU = chunkGPU * 2;
					if (chunkGPU >= iterationSpace) {
						chunkGPU = iterationSpace;

					}
				}
			}
		}
		} else if (stablePhase && !stopConditionModeOn) {
			//stablePhase
			y[measurements] = gpuThroughput;
			x[measurements] = (bundle->end - bundle->begin);
#ifdef NEWLOGFIT
			gpuThroughputMean = alfaThrGpuMean * gpuThroughput + (1 - alfaThrGpuMean) * gpuThroughputMean;
			y[measurements] = gpuThroughputMean;
#endif

				chunkGPU = calculateLogarithmicModelv2(x, y, numberPoints, measurements);
#ifdef NEWLOGFIT
#ifdef DEBUG
			cerr << "Thr.ref " << baseGpuThroughputMean << " Thr.moving.avg " << gpuThroughputMean << " ("
					<< gpuThroughputMean / baseGpuThroughputMean << ") Threshold " << thresholdThrGpuMean << endl;
#endif
			int motivo = 0;
			if (gpuThroughputMean < thresholdThrGpuMean * baseGpuThroughputMean
					//				|| gpuThroughputMean * thresholdThrGpuMean > baseGpuThroughputMean
#ifdef NEWLOGFIT2
					|| (motivo = ((end_tc - lastFitTime).seconds()*1000 * overheadFit > fitTime))
#endif
			) {
				newfit = true;
				measurements = 0;
				chunkGPU = computeUnits; // ???
				explorationPhase = true;
				stablePhase = false;
				threshold = 0.0;
				cerr << "Redo logfit ";
#ifdef NEWLOGFIT2
				startFitTime = end_tc;
				if (motivo) cerr << "(time)";
#endif
				cerr << endl;
#ifdef DEBUG
				//cerr << "Redo logfit ..." << chunkGPU << endl;
				//exit(0);
#endif
			}
#endif
		}
	}

	void executeOnCPU(Bundle *bundle) {
		tick_count start = tick_count::now();
		body->OperatorCPU(bundle->begin, bundle->end);
		tick_count end = tick_count::now();
		totCPU+= (bundle->end - bundle->begin) ;

		cpuThroughput = (bundle->end - bundle->begin) / ((end - start).seconds()*1000);

#ifdef DEBUG
		cerr << "=== Chunk CPU: " << bundle->end - bundle->begin << " TH: " << cpuThroughput /*((tg5-tg1)/1000000.0) */ << endl;
#endif
		if ((gpuThroughput > 0) && (!stopConditionModeOn || (stopConditionModeOn && (bundle->begin == 0)))) {
			fG = gpuThroughput / cpuThroughput;
			fGvalid = true;
		}
	}
};
//end class


class HSchedulerLogFit{
public:
	//void Initialize(int cpus, int gpus, int gpuchunk, int* interrupt) {
	void Initialize(int cpus, int gpus, float chunkGPU_initratio_) {
		numCPUs=cpus;
		numGPUs=gpus;
	    chunkGPU_initratio=chunkGPU_initratio_;
		veryfirsttime=true;
	    explorationPhase = true;
		stablePhase = false;
		minChunkCPU = 10;  //TODO: ver como calcular este minimo
		minChunkGPU = computeUnits;
		chunkCPU = 10;
		//chunkGPU = computeUnits * vectorization / 2;
		chunkGPU = computeUnits;
		measurements = 0;
		fG = 1.0;
		gpuThroughput = 0.0;
		cpuThroughput = 0.0;
		numberPoints = 4;
		//numberPoints = 7;
		threshold = 0.0;


		//Initializing library PJTRACER
		initializePJTRACER();
#ifdef NEWLOGFIT
#ifdef NEWLOGFIT2
		startFitTime = tick_count::now();
#endif
#endif

#ifdef HOST_PRIORITY
		sprintf(p->benchName, "%s_PRIO", p->benchName);
#endif

#ifdef OVERHEAD_STUDY
		sprintf(p->benchName, "%s_OVERHEAD", p->benchName);
#endif
	}

	/*Initializes PJTRACER library*/
	void initializePJTRACER() {
#ifdef PJTRACER
		char traceFname[1024];
		sprintf(traceFname, "%s_LOGFITMODEL_C_%d_G_%d.trace", pars->benchName, nCPUs, nGPUs);
		tracer = new PFORTRACER(traceFname);
		tracer->beginThreadTrace();
#endif

	}

	void heterogeneous_parallel_for(int begin, int end,  NbodyTask *body) {
	gpuStatus = numGPUs;

		if(veryfirsttime) 
		{ 
			veryfirsttime=false; 
			chunkGPU= (end-begin)*chunkGPU_initratio;
			
			if (chunkGPU<1) chunkGPU=1;
			initchunk= chunkGPU;
			startFitTime = tick_count::now();
		 } //ANDRES
        
		body->firsttime = true;

		if (numGPUs < 1) {
			pipeline pipe;
			MySerialFilter serial_filter(begin, end, numCPUs, numGPUs);
			MyParallelFilter parallel_filter(body, end - begin);
			pipe.add_filter(serial_filter);
			pipe.add_filter(parallel_filter);

			pipe.run(numCPUs + numGPUs);
			//ParallelFORCPUs(begin, end, body);


			pipe.clear();
		} else {
			pipeline pipe;

				MySerialFilter serial_filter(begin, end, numCPUs, numGPUs);
				MyParallelFilter parallel_filter(body, end - begin);
				pipe.add_filter(serial_filter);
				pipe.add_filter(parallel_filter);

#ifdef OVERHEAD_STUDY
				end_tc = tick_count::now();
#endif
				/*Launch the pipeline*/
#ifdef DEBUG
				cerr << "Running Pipeline: " << numCPUs << " " << numGPUs << " " << gpuStatus << endl;
#endif

#ifdef PJTRACER
				tracer->newEvent();
#endif


				pipe.run(numCPUs + numGPUs);



			pipe.clear();

		}
	}


	/*Sets initial mark of energy and time*/
	void startTimeAndEnergy(){
	  
#ifndef NOENERGY
	  sample1=energy_meter_init(100, 0  /*0=no debug*/);  // sample period 100 miliseconds
	  power_meter_idle(sample1); //get idle power
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
	  float totalE = sample1->eCPU + sample1->eFPGA + sample1->eMEM;
	  float totalCF = sample1->eCPU + sample1->eFPGA;
	  float FPGAoffload = (totGPU/(float)(totGPU+totCPU)*100);
#endif	  
	  file << numCPUs << "\t" << numGPUs << "\t"  << meanchunkGPU / num_chunkGPU << "\t" << runtime << "\t" << fitTime << "\t" << fitTime*100.0f/runtime  
#ifndef NOENERGY
	       << "\t" << sample1->eCPU << "\t" << sample1->eFPGA << "\t" << sample1->eMEM << "\t" << totalCF <<"\t"<< totalE <<"\t"<< final_em.wCPU << "\t" << final_em.wFPGA << "\t" << final_em.wMEM << "\t" << FPGAoffload 
#endif
	       << endl;
	  file.close();

#ifdef WRITERESULT
		resultfile.close();
#endif

	}

	void printHeaderToFile(ofstream &file){
	  file << "N._CPUs" << SEP << "N._FPGAs" << SEP << "MeanChunkFPGA" << SEP << "RunTime(ms)" << SEP << "fitTime" << SEP <<  "fitTime%"  
#ifndef NOENERGY 
	       SEP << "CPU Energy" << SEP << "FPGA Energy " << SEP<< "MEM Energy " << SEP<< "CPU+FPGA Energy " << SEP<< "Total Energy "  << SEP<< "offload "  << SEP<< "W CPU "  << SEP<< "W FPGA "  << SEP<< "W MEM"
#endif
	       << endl;
	}
};

