/*Copyright (c) 2015, Adam Taylor
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sds_lib.h>
#include "sbox.h"
#include "aes_enc.h"



void shift_row_enc(uint8_t state[16], uint8_t result[16]);
void subbytes(uint8_t state[16], uint8_t result[16]);
void addroundkey(uint8_t state[16],uint8_t iteration, uint8_t result[16],uint8_t ekey[240]);
void mixcolumn(uint8_t state[16], uint8_t result[16]);



uint8_t extractbyte(uint32_t src, unsigned from, unsigned to)
{
  unsigned mask = ( (1<<(to-from+1))-1) << from;
  return (src & mask) >> from;
}

#pragma SDS data copy(state[0:block_count*4])
#pragma SDS data copy(cipher[0:block_count*4])
#pragma SDS data access_pattern(state:SEQUENTIAL)
#pragma SDS data access_pattern(cipher:SEQUENTIAL)
#pragma SDS data data_mover(state:AXIDMA_SIMPLE)
#pragma SDS data data_mover(cipher:AXIDMA_SIMPLE)
#pragma SDS data mem_attribute(state: CACHEABLE)
#pragma SDS data mem_attribute(cipher: CACHEABLE)
#pragma SDS data mem_attribute(ekey: CACHEABLE)
////#pragma SDS data zero_copy(status[0:1])
//#pragma SDS data sys_port(state:ACP)
//#pragma SDS data sys_port(cipher:ACP)
////void aes_enc_hw(uint32_t *state,uint32_t *cipher,uint8_t ekey[240],int block_count,int *interrupt,int *status, int enable)
void aes_enc_hw(uint32_t *state,uint32_t *cipher,uint8_t ekey[240],int block_count)
{

	int i;
	uint8_t iteration = 0;
	uint8_t x,y;
	int z,j;
	uint8_t sub[16];
	uint8_t shift[16];
	uint8_t mix[16];
	uint8_t round[16];
	uint8_t state_grid[16];
	uint8_t result[16];

for(i=0;i<block_count;i++)
{

	//#pragma HLS PIPELINE //(IL 323 clock cycles)
			for(j=0; j<4; j++) {
			       #pragma HLS PIPELINE
		        		 state_grid[4*j] = extractbyte(*(state+j+i*4), 0, 7);
			             state_grid[4*j+1] = extractbyte(*(state+j+i*4), 8, 15);
			             state_grid[4*j+2] = extractbyte(*(state+j+i*4), 16, 23);
			             state_grid[4*j+3] = extractbyte(*(state+j+i*4), 24, 31);
		                 //state_grid[j] = *(state+j+i);
			}

	addroundkey(state_grid,0,sub,ekey);
	loop_main : for(iteration = 1; iteration < nr; iteration++)
	 {

	   subbytes(sub,shift);
	   shift_row_enc(shift,mix);
	   mixcolumn(mix,round);
	   addroundkey(round,iteration,sub,ekey);
	  }
	  subbytes(sub,shift);
	  shift_row_enc(shift,round);
	  addroundkey(round,nr,result,ekey);

	  for(z=0; z<4; z++) {
		    	 #pragma HLS PIPELINE
		    	   //*(cipher+z+i) = result[z];
		    	   *(cipher+z+i*4) = (result[4*z+3] << 24) | (result[4*z+2] << 16 ) | (result[4*z+1] << 8) | result[4*z];
	  }
  }
	
////         *interrupt = 255; //switch on leds
////	 if (enable == 1) 
////         	*status = 255; //trigger interrupt by writting to gpio
}
 

void shift_row_enc(uint8_t state[16], uint8_t result[16])
{
//#pragma HLS INLINE
#pragma HLS PIPELINE
#pragma HLS array_partition variable=state complete
#pragma HLS array_partition variable=result complete

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


void subbytes(uint8_t state[16], uint8_t result[16])
{
//#pragma HLS INLINE
#pragma HLS array_partition variable=state complete
#pragma HLS array_partition variable=result complete
#pragma HLS array_partition variable=sbox complete
	uint8_t x, y; //addresses the matrix
#pragma HLS PIPELINE
	loop_sb1 : for(x=0;x<4;x++){
		loop_sb2 : for(y=0;y<4;y++){
			result[x*4+y] = sbox[state[x*4+y]];
		}//end y loop
	}//end x loop
}




void mixcolumn(uint8_t state[16],uint8_t result[16])
{
	//B1� = (B1 * 2) XOR (B2 * 3) XOR (B3 * 1) XOR (B4 * 1)
	//B2� = (B1 * 1) XOR (B2 * 2) XOR (B3 * 3) XOR (B4 * 1)
	//B3� = (B1 * 1) XOR (B2 * 1) XOR (B3 * 2) XOR (B4 * 3)
	//B4� = (B1 * 3) XOR (B2 * 1) XOR (B3 * 1) XOR (B4 * 2)

	uint8_t x; // control of the column
//#pragma HLS INLINE
#pragma HLS array_partition variable=gf3 complete
#pragma HLS array_partition variable=gf2 complete
#pragma HLS array_partition variable=state complete
#pragma HLS array_partition variable=result complete

//#pragma HLS PIPELINE
	loop_mx1 :for(x=0;x<4;x++){
		#pragma HLS PIPELINE
		result[x] = (gf2[state[x]])^(gf3[state[4+x]])^(state[8+x])^(state[12+x]);
		result[4+x] = (state[x])^(gf2[state[4+x]])^(gf3[state[8+x]])^(state[12+x]);
		result[8+x] = (state[x])^(state[4+x])^(gf2[state[8+x]])^(gf3[state[12+x]]);
		result[12+x] = (gf3[state[x]])^(state[4+x])^(state[8+x])^(gf2[state[12+x]]);
	}
}

void addroundkey(uint8_t state[16], uint8_t iteration,uint8_t result[16],uint8_t ekey[240])
{
//#pragma HLS INLINE
#pragma HLS array_partition variable=ekey complete
#pragma HLS array_partition variable=state complete
#pragma HLS array_partition variable=result complete

	  uint8_t x,y;

/*
	  loop_fast:for(x=0;x<16;x++) {
  #pragma HLS PIPELINE
//#pragma HLS UNROLL
			  //if (start == 1)
				  result [x] = state [x] ^ ekey[iteration * nb * 4 + x * nb + x];
			  //else
			  //	  result [y+x*4] = state [y+x*4];
	    }
	  }*/
//#pragma HLS PIPELINE
  loop_rk1 :for(x=0;x<4;x++) {
   //#pragma HLS PIPELINE
	#pragma HLS UNROLL
		  loop_rk2 : for(y=0;y<4;y++){
			//#pragma HLS PIPELINE
			#pragma HLS UNROLL
				  result [y+x*4] = state [y+x*4] ^ ekey[iteration * nb * 4 + x * nb + y];
	    }
	  }
}

