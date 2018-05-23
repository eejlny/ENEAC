
#ifndef _BODY_TASK_
#define _BODY_TASK_


#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include "aes_enc.h"
#include "kernelAES.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CASO0
using namespace tbb;



uint8_t extractbyte_sw(uint32_t src, unsigned from, unsigned to)
{
  unsigned mask = ( (1<<(to-from+1))-1) << from;
  return (src & mask) >> from;
}


void shift_row_enc_sw(uint8_t state[16], uint8_t result[16])
{


	result[0] = state[0];
	result[1] = state[1];
	result[2] = state[2];
	result[3] = state[3];

	result[4] = state[5];
	result[5] = state[6];
	result[6] = state[7];
	result[7] = state[4];

	result[8] = state[10];
	result[10] = state[8];
	result[9] = state[11];
	result[11] = state[9];

	result[12] = state[15];
	result[15] = state[14];
	result[14] = state[13];
	result[13] = state[12];
}


void subbytes_sw(uint8_t state[16], uint8_t result[16])
{

	uint8_t x, y; //addresses the matrix

	loop_sb1 : for(x=0;x<4;x++){
		loop_sb2 : for(y=0;y<4;y++){
			result[x*4+y] = sbox[state[x*4+y]];
		}//end y loop
	}//end x loop
}




void mixcolumn_sw(uint8_t state[16],uint8_t result[16])
{

	uint8_t x; // control of the column

	loop_mx1 :for(x=0;x<4;x++){
		result[x] = (gf2[state[x]])^(gf3[state[4+x]])^(state[8+x])^(state[12+x]);
		result[4+x] = (state[x])^(gf2[state[4+x]])^(gf3[state[8+x]])^(state[12+x]);
		result[8+x] = (state[x])^(state[4+x])^(gf2[state[8+x]])^(gf3[state[12+x]]);
		result[12+x] = (gf3[state[x]])^(state[4+x])^(state[8+x])^(gf2[state[12+x]]);
	}
}

void addroundkey_sw(uint8_t state[16], uint8_t iteration,uint8_t result[16],uint8_t ekey[240])
{

  uint8_t x,y;
  loop_rk1 :for(x=0;x<4;x++) {
		  loop_rk2 : for(y=0;y<4;y++){
				  result [y+x*4] = state [y+x*4] ^ ekey[iteration * nb * 4 + x * nb + y];
	    }
	  }
}




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

       
		//printf("operator FPGA being %d end %d with interrupt at %x\n",begin,end,(int)status);
     

		kernelAES((uint32_t*)state,(uint32_t*)cipher,ekey, interrupt, status, file_desc, begin, end);
	

	}

	void getBackObjectFromGPU(int begin, int end) {

		
	}

	void OperatorCPU(int begin, int end) {
		{

			int i;
			uint8_t iteration = 0;
			uint8_t x;
			int z, j;
			uint8_t sub[16];
			uint8_t shift[16];
			uint8_t mix[16];
			uint8_t round[16];
			uint8_t state_grid[16];
			uint8_t result[16];

			//printf("operator CPU being %d end %d with interrupt at %x\n",begin,end);

			for (i = begin; i<end; i ++)
			{
				//printf("hello %d %d\n",i,j);
				for (j = 0; j<4; j++) {

					state_grid[4*j] = extractbyte_sw(*(state + j+4*i), 0, 7);
					state_grid[4*j + 1] = extractbyte_sw(*(state + j+4*i), 8, 15);
					state_grid[4*j + 2] = extractbyte_sw(*(state + j+4*i), 16, 23);
					state_grid[4*j + 3] = extractbyte_sw(*(state + j+4*i), 24, 31);
				}
		     /*  for(int x=0;x<4;x++){
				printf(" %x", state[x]);
			}
			printf("\n");
			for(int x=0;x<16;x++){
				printf(" %x", state_grid[x]);
			}
			printf("\n");*/
				//printf("hello2\n");
				addroundkey_sw(state_grid, 0, sub, ekey);
				//printf("hello3\n");
			    loop_main: for (iteration = 1; iteration < nr; iteration++)
			    {

						   subbytes_sw(sub, shift);
						   shift_row_enc_sw(shift, mix);
						   mixcolumn_sw(mix, round);
						   addroundkey_sw(round, iteration, sub, ekey);
			   }
	//printf("hello4\n");
			   subbytes_sw(sub, shift);
			   shift_row_enc_sw(shift, round);
			   addroundkey_sw(round, nr, result, ekey);

			   for (z = 0; z<4; z++) {
						   *(cipher + z + i * 4) = (result[4*z + 3] << 24) | (result[4*z + 2] << 16) | (result[4*z + 1] << 8) | result[4*z];
					   }
			//for(int x=0;x<16;x++){
			//	printf(" %x", result[x]);
			//}
			//printf("\n");
			//printf("hel5\n");
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
