
#ifndef _BODY_TASK_
#define _BODY_TASK_


#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include "matrix_mult.h"
#include "kernelMatrixmult.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CASO0
using namespace tbb;

/*****************************************************************************
 * NbodyTask
 * **************************************************************************/
class NbodyTask
{
public:
	bool firsttime;
public:

	void sendObjectToGPU( int begin, int end) {
		
	}

	void OperatorGPU(int begin, int end) {
        pid_t pid;
       
	//printf("FPGA is doing from %d to %d\n",begin,end_fpga);

	//int* local_int = (int *)HW_ADDR_GPIO;
        
	
 
	//printf("operator FPGA being %d end %d with interrupt at %x\n",begin,end,(int)status);
       // pid = fork();
       // if (pid == 0) 
       // {
	//   printf("child process %d\n",getpid());
 	   int result=kernelMatrixmult((float*)array_a,(float*)array_b,(float*)array_c,interrupt,status,file_desc,begin,end);
	   //now wait for completion by waiting from interrupt from acceletaror
  	   //printf("Waiting for interrupt ...\n");
  	   //int ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT,0); //this should put this process to sleep until interrupt takes place
  	   //printf("Waking up ...\n");
  	  // #pragma SDS wait(1)
	 //  exit(0);
         //}
         //else
         //{
	 //   printf("parent process %d\n",getpid());
         //   printf("Parent process waiting\n");
 	      
	 //   waitpid(-1,NULL,0);
	// }
	//int result=kernelMatrixmult((float*)array_a,(float*)array_b,(float*)array_c,begin,end);
	//printf("local int is %x\n",*interrupt);

	}

	void getBackObjectFromGPU(int begin, int end) {

		
	}

	void OperatorCPU(int begin, int end) {

		int i_m,j_m,k_m,i_block,j_block,k_block;
		float *c_p, *b_p, *a_p;

		//printf("operator CPU being %d end %d\n",begin,end);

		for (i_m = begin; i_m < end; i_m += BLOCK_I) {
		    for (j_m = 0; j_m < P; j_m += BLOCK) {
		        for (k_m = 0; k_m < M; k_m += BLOCK) {
		        	c_p = &array_c[i_m*P+j_m];
		        	a_p = &array_a[i_m*M+k_m];
		            for (i_block = 0; i_block < BLOCK_I; i_block++ ) {
		            	b_p = &array_b[k_m*P+j_m];
		                for (j_block = 0; j_block < BLOCK; j_block++) {
		                    for (k_block = 0; k_block < BLOCK; k_block++) {
		                        c_p[k_block] += a_p[j_block] * b_p[k_block];
		                    }
		                    b_p += P;
		                }
		                c_p += P;
		                a_p += M;
		            }
		        }
		    }
		}


	}

	void AllocateMemoryObjects() {

	 
	}
};
//end class

//from here to end for oracle

/*****************************************************************************
 * Parallel For
 * **************************************************************************/

class ParallelFor {
	NbodyTask *body;
public:
	ParallelFor(NbodyTask *b){
		body = b;
	}

    void operator()( const blocked_range<int>& range ) const {

		#ifdef PJTRACER
		tracer->cpuStart();
		#endif

		body->OperatorCPU(range.begin(), range.end());

		#ifdef PJTRACER
		tracer->cpuStop();
		#endif
    }
};

// Note: Reads input[0..n] and writes output[1..n-1].
void ParallelFORCPUs(size_t start, size_t end, NbodyTask *body ) {
    ParallelFor pf(body);
	parallel_for( blocked_range<int>( start, end ), pf, auto_partitioner() );
}

/*****************************************************************************
 * Raw Tasks
 * **************************************************************************/
template <class T>
class GPUTask: public task {
public:
   T *body;
   int begin;
   int end;
   GPUTask<T>(T *b, int beg, int e) : body(b), begin(beg), end(e) {}

    // Overrides virtual function task::execute
    task* execute() {
		#ifdef USEBARRIER
		//cerr << "GPU antes de barrier1" << endl;
		getLockGPU();
		//cerr << "GPU despues de barrier1" << endl;
		#endif
		#ifdef PJTRACER
		tracer->gpuStart();
		#endif

		tick_count start = tick_count::now();
		body->sendObjectToGPU(begin, end);
		body->OperatorGPU(begin, end);
		body->getBackObjectFromGPU(begin, end);
		tick_count stop = tick_count::now();
		//gpuThroughput = (end -  begin) / ((stop-start).seconds()*1000);

		#ifdef PJTRACER
		tracer->gpuStop();
		#endif
		#ifdef USEBARRIER
		//cerr << "GPU antes de barrier2" << endl;
		freeLockGPU();
		//cerr << "GPU despues de barrier2" << endl;
		getLockCPU();
		freeLockCPU();
		#endif
        return NULL;
    }
};

template <class T>
class CPUTask: public task {
public:
   T *body;
   int begin;
   int end;
   CPUTask<T>(T *b, int beg, int e ) : body(b), begin(beg), end(e){}

    // Overrides virtual function task::execute
    task* execute() {
		#ifdef USEBARRIER
		//cerr << "CPU antes de barrier1" << endl;
		getLockCPU();
		//cerr << "CPU despues de barrier1" << endl;
		#endif

		tick_count start = tick_count::now();
		ParallelFORCPUs(begin, end, body);
		tick_count stop = tick_count::now();
		//cpuThroughput = (end -  begin) / ((stop-start).seconds()*1000);

		#ifdef USEBARRIER
		//cerr << "CPU antes de barrier2" << endl;
		freeLockCPU();
		//cerr << "CPU despues de barrier2" << endl;
		getLockGPU();
		freeLockGPU();
		#endif
        return NULL;
    }
};


template <class T>
class ROOTTask : public task{

	T *body;
	int begin;
	int end;

public:

	ROOTTask<T>(T *b, int beg, int e){
		body = b;
		begin = beg;
		end = e;
		
	}

	task* execute() {
        #ifdef USEBARRIER
		//cerr << "antes de barrier_init" << endl;
		barrier_init();
		//cerr << "despues de barrier_init" << endl;
		#endif
		//GPUTask<T>& a = *new( allocate_child() ) GPUTask<T>(body, begin, begin + chunkGPU);
		//CPUTask<T>& b = *new( allocate_child() ) CPUTask<T>(body, begin + chunkGPU, end);

		// Set ref_count to 'two children plus one for the wait".
        set_ref_count(3);
        // Start b running.
        //spawn( b );
        // Start a running and wait for all children (a and b).
        //spawn_and_wait_for_all(a);
		return NULL;
    }
};

#endif
