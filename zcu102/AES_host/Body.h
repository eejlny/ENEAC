#ifndef _BODY_TASK_
#define _BODY_TASK_

#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include "aes_enc.h"
#include "kernelAES.h"
////#include <sys/ioctl.h>
////#include <sys/types.h>
////#include <sys/wait.h>

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
class Body
{
public:
  bool firsttime;
public:

  void sendObjectToGPU( int begin, int end) {  }

  void OperatorGPU(int begin, int end, int id) {
    //printf("operator FPGA being %d end %d with interrupt at %x\n",begin,end,(int)status);
    ////		kernelAES((uint32_t*)state,(uint32_t*)cipher,ekey, interrupt, status, file_desc, begin, end);
    //kernelAES((uint32_t*)state,(uint32_t*)cipher,ekey, begin, end, numfpgas);
   // cerr << "Activating FPGA " << id << " with " << begin << " begin " << end << " end " << endl;
    bodies_F+=end-begin;

    switch(id)
    {
      case 1 : kernelAES1((uint8_t*)state,(uint8_t*)cipher,ekey, (begin), (end)); break;
      case 2 : kernelAES2((uint8_t*)state,(uint8_t*)cipher,ekey, (begin), (end)); break;
      case 3 : kernelAES3((uint8_t*)state,(uint8_t*)cipher,ekey, (begin), (end)); break;
      case 4 : kernelAES4((uint8_t*)state,(uint8_t*)cipher,ekey, (begin), (end)); break;
    }

   /* if(begin == 0)
    {

    	for(int i = 0; i < 10; i++) 
   	 {
		printf("cipher %d is %hhd\n", i, *(cipher+i));
    	}
     } */
  }

  void getBackObjectFromGPU(int begin, int end) {  }

  void OperatorCPU(int begin, int end) {
    {

	//printf("operator CPU being %d end %d\n",(16*begin),(16*end));

        bodies_C+=end-begin;

#if defined(__ARM_ACLE) || defined(__ARM_FEATURE_CRYPTO)
        aes_process_arm((const uint8_t*)key, (const uint8_t*)ekey, nr, (uint8_t*)&state[begin*16], (uint8_t*)&cipher[begin*16], (16*(end-begin)));


  /*   if(begin == 0)
     {

    	for(int i = 0; i < 10; i++) 
   	 {
		printf("cipher %d is %hhd\n", i, *(cipher+i));
    	}
     }*/

#else
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
        for (j = 0; j<4; j++) {
          state_grid[4*j] = extractbyte_sw(*(state + j+4*i), 0, 7);
          state_grid[4*j + 1] = extractbyte_sw(*(state + j+4*i), 8, 15);
          state_grid[4*j + 2] = extractbyte_sw(*(state + j+4*i), 16, 23);
          state_grid[4*j + 3] = extractbyte_sw(*(state + j+4*i), 24, 31);
        }

        addroundkey_sw(state_grid, 0, sub, ekey);
        for (iteration = 1; iteration < nr; iteration++)
        {
          subbytes_sw(sub, shift);
          shift_row_enc_sw(shift, mix);
          mixcolumn_sw(mix, round);
          addroundkey_sw(round, iteration, sub, ekey);
        }
        subbytes_sw(sub, shift);
        shift_row_enc_sw(shift, round);
        addroundkey_sw(round, nr, result, ekey);
        for (z = 0; z<4; z++) {
          *(cipher + z + i * 4) = (result[4*z + 3] << 24) | (result[4*z + 2] << 16) | (result[4*z + 1] << 8) | result[4*z];
        }
      }
#endif

    }
  }

  void AllocateMemoryObjects() {

  }
};
//end class

#endif
