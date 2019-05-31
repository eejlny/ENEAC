#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sched.h>
#include <asm/unistd.h>
#include "Scheduler.h"
#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include <cmath>

#ifdef Win32
#include "PCM_Win/windriver.h"
//#else
//#include "cpucounters.h"
#endif

#ifdef PJTRACER
#include "pfortrace.h"
#endif

//#define _GNU_SOURCE already defined
#include <sys/syscall.h>
#include <sys/types.h>

//#define  OVERHEAD_ANALYSIS

//#define DEBUGLOG
//#define DEBUG
//#define DEBUG_GPU

using namespace std;
using namespace tbb;


int set_thread_affinity_CORE(pthread_t th, int cpu)
{
	cpu_set_t mask;
	__CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	__CPU_SET_S(cpu,sizeof(cpu_set_t), &mask);

	return pthread_setaffinity_np(th, sizeof(cpu_set_t), &mask);
}


/*****************************************************************************
 * Defines
 * **************************************************************************/
#define CPU 0
#define GPU 1
#define GPU_OFF -100 //Arbitrary value
#define SEP "\t"
#define MAXPOINTS 1024

size_t CHUNKGPUALIGN=1;
size_t CHUNKCPUALIGN=1;

//#define ALIGN(A,B) max((B),(int)(round((float)(A)/(float)(B))*(float)(B)))

// std::max and std::round are not constexpr in C++11
size_t ALIGN(size_t A, size_t B) {
  return std::max(B,
      static_cast<size_t>(round(A/static_cast<float>(B)) * B));
}

size_t ALIGNFLOOR(size_t A, size_t B) {
  return static_cast<size_t>(floor(A/static_cast<float>(B)) * B);
}

size_t ALIGNGPU(size_t A) {
  return ALIGN(A, CHUNKGPUALIGN);
}

size_t ALIGNFLOORGPU(size_t A) {
  return ALIGNFLOOR(A, CHUNKGPUALIGN);
}

size_t ALIGNCPU(size_t A) {
  return ALIGN(A, CHUNKCPUALIGN);
}


/*****************************************************************************
 * types
 * **************************************************************************/
typedef struct{
    int numcpus;
    int numgpus;
    int gpuChunk;
    float chunkGPU_initratio;
    char benchName[100];
    char kernelName[100];
    pid_t main_thread_id;
    size_t CHUNKGPUALIGN;
    size_t CHUNKCPUALIGN;
} Params;

/*****************************************************************************
 * Global variables
 * **************************************************************************/
// NEW VARIABLES---------------
#define NACC 16
int chunkGPU;
float gpuThroughput[NACC]; // -> maxGPUThrouput se calcula en cada GPUchunk
__thread int chunkCPU;
float fG=8;
__thread float cpuThroughput =0.0;
__thread int lastchunkGPU=0;
__thread int myAccId = 0; // core assingment
int nAcc;  // new numGPUS
// NEW VARIABLES-------------END

tbb::atomic<int> gpuStatus; // USED to stop FPGAs

float meanchunkGPU=0;          //accounting chunks
unsigned long num_chunkGPU=0;  //accounting chunks

//int chunkGPU;  // superseded
//int chunkCPU;
int minChunkGPU;
int minChunkCPU;
bool stopConditionModeOn;

bool explorationPhase;
bool stablePhase;
bool veryfirsttime;
int measurements;
int numberPoints;
//float cpuThroughput;// superseded
//float gpuThroughput;// superseded
//float fG;// superseded
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


float fitTime = 0.0;
tick_count lastFitTime;
tick_count startFitTime;





// calcula la aproximacion logaritmica a los puntos x,y medidos de chunk,thr de la
// GPU, y devuevle el chunk siguiente de GPU adecuado para la ultima medida de througput
// hace el fitting con "numberPoints" independiente de cuantos se hayan medido (last_point)
// ademas se almacenan calculos en la primera ejecucion que se reusan en todas las siguientes

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
	//Just to get a multiple of CHUNKGPUALIGN
	//calculatedChunk = round((a / threshold) / (float) CHUNKGPUALIGN)*(float) CHUNKGPUALIGN;
	calculatedChunk = ALIGNGPU((a / threshold));
#ifdef DEBUGLOG
	cerr << "threshold " << threshold << " a " << a << " chunk " << calculatedChunk << endl;
#endif
	//return 100000;
#ifdef LOGFITFIXEDCHUNK
	return LOGFITFIXEDCHUNK;
#else
	return (calculatedChunk < CHUNKGPUALIGN) ? CHUNKGPUALIGN : calculatedChunk;
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
		//(*chCPU) = 0;
		//(*chGPU) = remain;
                int align = ALIGNFLOORGPU(remain);
                if (align > 0) {  // solo si se le puede dar alineado
                    (*chGPU) = align;
                    (*chCPU) = (remain - align) / nCpus;
                    tmin = tgpu;
                }
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
				//(*chCPU) = (remain - cG1) / nCpus;
				//(*chGPU) = cG1;
                                int align = ALIGNFLOORGPU(cG1);
				(*chGPU) = align;
				(*chCPU) = (remain - align) / nCpus;
			}
		} else if (cG2 >= x[i] && cG2 <= x[i + 1]) {
			thet = cG2 / aproxGPUTh(cG2);
#ifdef DEBUGLOG
			cerr << "getLastChunks: Het Throughput" << i << ": " << cG2 / thet << " para " << cG2 << endl;
#endif
			if (thet < tmin) {
				tmin = thet;
				//(*chCPU) = (remain - cG2) / nCpus;
				//(*chGPU) = cG2;
                                int align = ALIGNFLOORGPU(cG2);
				(*chGPU) = align;
				(*chCPU) = (remain - align) / nCpus;
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
	int iterationSpace;     // moved from parallel filter
    bool firstmeasurement;  // moved from parallel filter
	pid_t _main_thread_id;
public:
	/*Class constructor, it only needs the first and last iteration indexes.*/
	MySerialFilter(int b, int e, int ncpus, int ngpus, pid_t main_thread_id) : filter(true) {
		begin = b;
		end = e;
		iterationSpace=end-begin;
		firstmeasurement=true;
		nCPUs = ncpus;
		nGPUs = ngpus;
	    stopConditionModeOn = false;
		_main_thread_id = main_thread_id;
	}

	/*Mandatory operator method, TBB rules*/
void * operator()(void *) {

		Bundle *bundle = new Bundle();
		//If there are remaining iterations
		if (begin < end) {

		//	pid_t actual_thread_id = syscall(SYS_gettid);  // superseded
			//Checking whether the GPU is idle or not.
			
			if (nAcc && !myAccId) { // asignar al thread un acelerador si hay libre y no tiene ya uno
				myAccId = nAcc--; // se puede modificar nAcc porque esta etapa es serie
				set_thread_affinity_CORE(pthread_self(), nAcc);
				cerr << "** FPGA thread: " << nAcc << endl;
			}
			
			if ( myAccId && gpuStatus){  // vemos si es un thread con acelerador asociado
				
				bundle->type = myAccId;
			/************************************************************************
			 * MODEL CALCULATIONS. moved from parallel filter
			 ***/
			if (explorationPhase && gpuThroughput[bundle->type]!=-1) {
			//it only stores increasing chunksize points
			if (x[measurements] < lastchunkGPU && !stopConditionModeOn) {
				//Update the number of points
				if (firstmeasurement) {
					firstmeasurement = false;
					y[measurements] = gpuThroughput[bundle->type];
					x[measurements] = (lastchunkGPU);


				} else {
					y[measurements + 1] = gpuThroughput[bundle->type];
					x[measurements + 1] = (lastchunkGPU);
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
                                                chunkGPU = ALIGNGPU(chunkGPU);
						calculateLog=true;
						for(int i=0; i<MAXPOINTS; i++) x[i]=0;
						for(int i=0; i<nGPUs; i++) gpuThroughput[i]=-1; // ADDED to reset th of all FPGA cores

#ifdef DEBUG
					cerr << "******** Reset exploration ********* " << "Mea: x[" << measurements << "]=" << x[measurements]  << endl;
#endif

					}
					else
					{
						int filledOK=1; // make sure there is no gaps
						for(int k=0; k<measurements; k++) if (x[k]==0) { filledOK=0; break;}

				if (filledOK && (measurements >= (numberPoints + 2)) && (y[measurements] * gpuThDecay < y[measurements - 2]) && (y[measurements - 1] * gpuThDecay < y[measurements - 2])
						|| x[measurements] >= iterationSpace) {

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
                                        meanchunkGPU=chunkGPU;
                                        num_chunkGPU=1;

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
		} else if (stablePhase && !stopConditionModeOn && gpuThroughput[bundle->type]!=-1) {
			//stablePhase
			y[measurements] = gpuThroughput[bundle->type];
			x[measurements] = (lastchunkGPU);

				chunkGPU = calculateLogarithmicModelv2(x, y, numberPoints, measurements);
		}
		
		/****
		 * END MODEL CALCULATIONS. Moved from parallel filter
		 *************************************************************************/	
				
		
		//	if ((_main_thread_id == actual_thread_id) && (--gpuStatus >= 0)) { // superseded
#ifdef DEBUG
				cerr << "Serial Filter GPU: " << chunkGPU << " Begin = " << begin << ", End = " << end << " fg " << fG << endl;
#endif
				//Checking stop condition

				if (stablePhase) stopConditionModeOn = GPU_Stop_Condition();

				if (!stopConditionModeOn) {
					int auxEnd = begin + chunkGPU;
                                        if (auxEnd <= end) {
                                            //auxEnd = (auxEnd > end) ? end : auxEnd;
                                            bundle->begin = begin;
                                            bundle->end = auxEnd;
                                            begin = auxEnd;
                                           // bundle->type = GPU;
                                      		bundle->type = myAccId;  // el tipo ahora lleva el id del acelerador (aunque no haria falta ya que esta asociado al thread)
		
                                            meanchunkGPU+=(float)(bundle->end-bundle->begin);
                                            num_chunkGPU++;
                                            return bundle;
                                        } else {  // no hay suficientes iteraciones, tomar un trozo alineado y parar GPU
                                            int align = ALIGNFLOORGPU(end-begin);
                                            if (align > 0) {  // se le puede dar trozo alineado
                                                int auxEnd = begin + align;
                                                bundle->begin = begin;
                                                bundle->end = auxEnd;
                                                begin = auxEnd;
                                             //   bundle->type = GPU;
                                           		bundle->type = myAccId;  // el tipo ahora lleva el id del acelerador (aunque no haria falta ya que esta asociado al thread)
		
                                                meanchunkGPU+=(float)(bundle->end-bundle->begin);
                                                num_chunkGPU++;
                                                gpuStatus=0;  // para no liberar la GPU al finalizar y no alimentarla más
                                                
                                                return bundle;
                                            } else { // no se le puede dar alineado, todo a CPU
						// no incrementamos gpuStatus para dejar pillada la GPU
						gpuStatus=0;
						nCPUs++;
						//auxEnd = (auxEnd > end) ? end : auxEnd;
						auxEnd = begin + (end-begin)/nCPUs;
						bundle->begin = begin;
						bundle->end = auxEnd;
						begin = auxEnd;
						bundle->type = CPU;
						return bundle;
                                            }
                                        }
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
						//bundle->type = GPU;
						bundle->type = myAccId;  // el tipo ahora lleva el id del acelerador (aunque no haria falta ya que esta asociado al thread)
		
						return bundle;
					} else { // paramos la GPU
						// no incrementamos gpuStatus para dejar pillada la GPU
						gpuStatus=0; //stop GPU
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
				//if(_main_thread_id == actual_thread_id ){
				//	gpuStatus++;
				//}
				
				/*Calculating next chunkCPU*/
				float maxGpuThroughput = gpuThroughput[1];
				for (int i=2; i < nGPUs+1; i++)
				maxGpuThroughput = std::max(maxGpuThroughput, gpuThroughput[i]);
				if(maxGpuThroughput >0 && cpuThroughput>0) { fG = maxGpuThroughput/cpuThroughput; fGvalid=true;}

				
				if (!stopConditionModeOn && nGPUs) {
					
					
					
					if (fGvalid) {
						chunkCPU = max( min(chunkGPU/fG , (end - begin) / (fG + nCPUs)),  (float) minChunkCPU);
                                                chunkCPU = ALIGNCPU(chunkCPU);
					}
				} else {
					
					
					chunkCPU = max((end - begin) / (nCPUs+1), minChunkCPU);
                                        chunkCPU = ALIGNCPU(chunkCPU);
				}
				if(chunkCPU==0) chunkCPU=minChunkCPU;  // aqui si se establece minimo para CPU
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
template <class B>
class MyParallelFilter : public filter {
private:
	B *body;
//	int iterationSpace;

public:

	MyParallelFilter(B *b, int i) :
		filter(false) {
		body = b;
		//iterationSpace = i;

	}

	void * operator()(void * item) {

		//variables
		Bundle *bundle = (Bundle*) item;

		if (bundle->type != CPU) {
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
			//gpuStatus++;
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
		
		tick_count start_tc = tick_count::now();


		body->OperatorGPU(bundle->begin, bundle->end, bundle->type);


		end_tc = tick_count::now();

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
			
			gpuThroughput[bundle->type] = (bundle->end - bundle->begin) / time ;
			lastchunkGPU=bundle->end - bundle->begin;
			//gpuThroughput = (bundle->end - bundle->begin) / time;
			
			//corregir
			//if (cpuThroughput > 0) {
			//	fG = gpuThroughput / cpuThroughput;
			//	fGvalid = true;
		    //	}
		}

#if defined(DEBUG_GPU) || defined(DEBUG)
		cerr << "Chunk GPU: " << bundle->end - bundle->begin << " TH: " << gpuThroughput[bundle->type]  /*((tg5-tg1)/1000000.0) */  << " it: " << bundle->begin << endl;
#endif
		/****
		 * remove model calculation from here. moved to serial filter
		 ***/
	}

	void executeOnCPU(Bundle *bundle) {
		tick_count start = tick_count::now();
		body->OperatorCPU(bundle->begin, bundle->end);
		tick_count end = tick_count::now();

		cpuThroughput = (bundle->end - bundle->begin) / ((end - start).seconds()*1000);

#if defined(DEBUG_GPU) || defined(DEBUG)
		cerr << "=== Chunk CPU: " << bundle->end - bundle->begin << " TH: " << cpuThroughput /*((tg5-tg1)/1000000.0) */ << endl;
#endif
   // lo calcula en el serial filter ??
  /*      float maxGpuThroughput = gpuThroughput[1];
				for (int i=2; i < nGPUs+1; i++)
				maxGpuThroughput = std::max(maxGpuThroughput, gpuThroughput[i]);
				
		if ((maxGpuThroughput > 0) && (!stopConditionModeOn || (stopConditionModeOn && (bundle->begin == 0)))) {
			fG = maxGpuThroughput / cpuThroughput;
			fGvalid = true;
		}*/
	}
};
//end class


class Hap: public Scheduler<Hap, Params> {
	Params *pars;


public:

	/*This constructor just call his parent's contructor*/
	Hap(Params *params) : Scheduler<Hap, Params>(params), pars(params) {
		//Params * p = (Params*) params;
		//pars = p;
		chunkGPU_initratio=pars->chunkGPU_initratio;
		veryfirsttime=true;
        explorationPhase = true;
		stablePhase = false;
        CHUNKCPUALIGN=pars->CHUNKCPUALIGN;
        CHUNKGPUALIGN=pars->CHUNKGPUALIGN;
		//minChunkCPU = 10;  //TODO: ver como calcular este minimo
		//minChunkGPU = computeUnits;
		minChunkGPU = CHUNKGPUALIGN;
		//chunkCPU = 10;
		//chunkCPU = CHUNKCPUALIGN;
		//chunkGPU = computeUnits * vectorization / 2;
		//chunkGPU = computeUnits;
		chunkGPU = CHUNKGPUALIGN;
		measurements = 0;
		fG = 1.0;
		//NEW
		cpuThroughput=0.0;
		for (int i=0; i<= pars->numgpus; i++) gpuThroughput[i] = -1;
		nAcc = pars->numgpus;
		//myAccId = 0;  // defaut vaule already = 0
		//NEW
	    //	gpuThroughput = 0.0;
	    //	cpuThroughput = 0.0;
		numberPoints = 4;
		//numberPoints = 7;
		threshold = 0.0;


		//Initializing library PJTRACER
		initializePJTRACER();

#ifdef HOST_PRIORITY
		sprintf(p->benchName, "%s_PRIO", p->benchName);
#endif

#ifdef OVERHEAD_STUDY
	//	sprintf(p->benchName, "%s_OVERHEAD", p->benchName);
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

public:

	/*The main function to be implemented*/
	template<class T>
	void heterogeneous_parallel_for(int begin, int end, T *body) {
		gpuStatus = 1; // activamos las FPGAs

		if(veryfirsttime)
		{
			veryfirsttime=false;
			chunkGPU= (end-begin)*chunkGPU_initratio;

			if (chunkGPU<1) chunkGPU=1;
                        chunkGPU = ALIGNGPU(chunkGPU);
			initchunk= chunkGPU;
			startFitTime = tick_count::now();
			minChunkCPU = min((int)(CHUNKCPUALIGN / fG), (int)((end-begin)/100));
	    	minChunkCPU = ALIGNCPU(minChunkCPU);
	
		 } //ANDRES

		body->firsttime = true;

		if (nGPUs < 1) {
			pipeline pipe;
			MySerialFilter serial_filter(begin, end, nCPUs, nGPUs, pars->main_thread_id);
			MyParallelFilter<T> parallel_filter(body, end - begin);
			pipe.add_filter(serial_filter);
			pipe.add_filter(parallel_filter);

			pipe.run(nCPUs + nGPUs);
			//ParallelFORCPUs(begin, end, body);


			pipe.clear();
		} else {
			pipeline pipe;

				MySerialFilter serial_filter(begin, end, nCPUs, nGPUs, pars->main_thread_id);
				MyParallelFilter<T> parallel_filter(body, end - begin);
				pipe.add_filter(serial_filter);
				pipe.add_filter(parallel_filter);

#ifdef OVERHEAD_STUDY
				end_tc = tick_count::now();
#endif
				/*Launch the pipeline*/
#ifdef DEBUG
				cerr << "Running Pipeline: " << nCPUs << " " << nGPUs << " " << gpuStatus << endl;
#endif

#ifdef PJTRACER
				tracer->newEvent();
#endif


				pipe.run(nCPUs + nGPUs);



			pipe.clear();

		}
	}


	/*this function print info to a Log file*/
	void saveResultsForBench() {

		char * execution_name = (char *) malloc(sizeof (char)*256);

		sprintf(execution_name, "_LOGFITMODEL_%d_%d.txt", nCPUs, nGPUs);
		strcat(pars->benchName, execution_name);

		/*Checking if the file already exists*/
		bool fileExists = isFile(pars->benchName);
		ofstream file(pars->benchName, ios::out | ios::app);
		if (!fileExists) {
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
		meanchunkGPU/=num_chunkGPU;
		file << nCPUs << "\t" << nGPUs << "\t"  <<  pars->chunkGPU_initratio << "\t"  << meanchunkGPU << "\t" <<  runtime << "\t" << fitTime << "\t" << fitTime*100.0f/runtime << "\t"
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
     << initchunk << "\t" << finalchunk << "\t" << totaltries << "\t" << itemsOnGPU << endl;
		file.close();
#ifndef NDEBUG
		cerr << nCPUs << "\t" << nGPUs << "\t"  << runtime << "\t"
#ifdef PCM
		<< getPP0ConsumedJoules(sstate1, sstate2) << "\t" << getPP1ConsumedJoules(sstate1, sstate2) << "\t"
		<< getConsumedJoules(sstate1, sstate2) - getPP0ConsumedJoules(sstate1, sstate2) - getPP1ConsumedJoules(sstate1, sstate2) << "\t" <<  getConsumedJoules(sstate1, sstate2) << "\t"
		<< getL2CacheHits(sktstate1[0], sktstate2[0]) << "\t" << getL2CacheMisses(sktstate1[0], sktstate2[0]) << "\t" << getL2CacheHitRatio(sktstate1[0], sktstate2[0]) <<"\t"
		<< getL3CacheHits(sktstate1[0], sktstate2[0]) << "\t" << getL3CacheMisses(sktstate1[0], sktstate2[0]) << "\t" << getL3CacheHitRatio(sktstate1[0], sktstate2[0]) <<"\t"
		<< getCyclesLostDueL3CacheMisses(sstate1, sstate2)
#endif // PCM
<< endl;
#endif // NDEBUG
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



