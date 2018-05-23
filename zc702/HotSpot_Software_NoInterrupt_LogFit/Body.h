
#ifndef _BODY_TASK_
#define _BODY_TASK_


#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include "hotspot.h"
#include "kernelHotspot.h"

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

	void OperatorGPU(int begin, int end ) {

        float Rx_1=1.f/Rx;
	float Ry_1=1.f/Ry;
	float Rz_1=1.f/Rz;
	float Cap_1 = step/Cap;
        int end_fpga = end-1;
		
        //printf("FPGA is doing from %d to %d\n",begin,end_fpga);
 	////int result=kernelHotspot((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1,interrupt,status,file_desc, begin,end_fpga);
	kernelHotspot((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, begin, end_fpga);

	}

	void getBackObjectFromGPU(int begin, int end) {

		
	}

	void OperatorCPU(int begin, int end) {

	    float *temp;
	    float *power;
	    float *result;
	    int row;
	    int col;
	    float delta;

	    temp = array_temp+(grid_cols+2);
	    power = array_power+(grid_cols+2);
	    result = array_out+(grid_cols+2);
	    //row = grid_rows;
            col = (grid_cols+2);
	    float Rx_1=1.f/Rx;
	    float Ry_1=1.f/Ry;
	    float Rz_1=1.f/Rz;
	    float Cap_1 = step/Cap;
//    FLOAT Cap_1 = step/Cap;


            for (int r = begin; r < end; ++r ) {
                for (int  c = 1; c <= grid_cols; ++c ) {
			result[r*col+c] =temp[r*col+c]+ 
				( Cap_1 * (power[r*col+c] + 
				(temp[(r+1)*col+c] + temp[(r-1)*col+c] - 2.f*temp[r*col+c]) * Ry_1 + 
				(temp[r*col+c+1] + temp[r*col+c-1] - 2.f*temp[r*col+c]) * Rx_1 + 
				(amb_temp - temp[r*col+c]) * Rz_1));
		    
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
   GPUTask<T>(T *b, int beg, int e ) : body(b), begin(beg), end(e){}

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
