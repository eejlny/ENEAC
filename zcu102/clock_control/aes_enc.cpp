/*Copyright (c) 2018, Jose Nunez-Yanez*/
/*University of Bristol. ENEAC project*/


#include "aes_enc.h"


#include <arm_acle.h>
#include <arm_neon.h>


void aes_wrapper_sw(uint8_t state[16],uint8_t cipher[16],uint8_t ekey[240]);
void shift_row_enc_sw(uint8_t state[16], uint8_t result[16]);
void subbytes_sw(uint8_t state[16], uint8_t result[16]);
void addroundkey_sw(uint8_t state[16],uint8_t iteration, uint8_t result[16],uint8_t ekey[240]);
void mixcolumn_sw(uint8_t state[16], uint8_t result[16]);

void aes_enc_v8(const uint8_t key[],const uint8_t subkeys[],uint32_t rounds,const uint8_t input[],uint8_t output[],uint32_t length) {
	  uint8_t test[16];
          while (length >= 16) {
		       uint8x16_t block = vld1q_u8(input);
               
               block = vld1q_u8(input);

               // AES single round encryption
               block = vaeseq_u8(block, vld1q_u8(subkeys));
               // AES mix columns
               block = vaesmcq_u8(block);
              
               // AES single round encryption
               block = vaeseq_u8(block, vld1q_u8(subkeys+16));
               // AES mix columns
               block = vaesmcq_u8(block);

               for (unsigned int i=2; i<rounds-1; ++i) {
                    // AES single round encryption
                    block = vaeseq_u8(block, vld1q_u8(subkeys+i*16));
                    // AES mix columns
                    block = vaesmcq_u8(block);
               }

               // AES single round encryption
               block = vaeseq_u8(block, vld1q_u8(subkeys+(rounds-1)*16));
               // Final Add (bitwise Xor)
               block = veorq_u8(block, vld1q_u8(subkeys+(rounds)*16));

               vst1q_u8(output, block);
             
               input += 16; output += 16;
               length -= 16;
               //printf("remaining length is %d\n", length);
          }
     };


void aes_enc_sw(uint8_t *state,uint8_t *cipher,uint8_t ekey[240],unsigned int block_size)
{

	int i,j;
	uint8_t x,y;
	uint8_t state_buf[16];
	uint8_t cipher_buf[16];

	#pragma HLS array_partition variable=state_buf complete
	#pragma HLS array_partition variable=cipher_buf complete

for(i=0;i<block_size;i+=16)
{

	 for(j=0; j<16; j++) {
	             state_buf[j] = *(state+i+j);
	 }
     aes_wrapper_sw(state_buf,cipher_buf,ekey);
    for(j=0; j<16; j++) {
    	        *(cipher+i+j) = cipher_buf[j];
		//*(cipher+i+j) = 0;

    }

  }
}


void aes_wrapper_sw(uint8_t state[16],uint8_t cipher[16],uint8_t ekey[240])
{
	uint8_t sub[16];
	uint8_t shift[16];
	uint8_t mix[16];
	uint8_t round[16];

	addroundkey_sw(state,0,sub,ekey);
	loop_main : for(int iteration = 1; iteration < nr; iteration++)
	{
		   subbytes_sw(sub,shift);
		   shift_row_enc_sw(shift,mix);
		   mixcolumn_sw(mix,round);
		   addroundkey_sw(round,iteration,sub,ekey);
    }
	subbytes_sw(sub,shift);
	shift_row_enc_sw(shift,round);
	addroundkey_sw(round,nr,cipher,ekey);

}

void shift_row_enc_sw(uint8_t state[16], uint8_t result[16])
{

   	  result[0] = state[0];
          result[1] = state[5];
          result[2] = state[10];
          result[3] = state[15];

          result[4] = state[4];
          result[5] = state[9];
          result[6] = state[14];
          result[7] = state[3];

          result[8] = state[8];
          result[10] = state[2];
          result[9] = state[13];
          result[11] = state[7];

          result[12] = state[12];
          result[15] = state[11];
          result[14] = state[6];
          result[13] = state[1];
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

void addroundkey_sw(uint8_t state[16], uint8_t iteration,uint8_t result[16],uint8_t ekey[240])
{

	  uint8_t x,y;
	  loop_rk1 :for(x=0;x<4;x++) {
		  loop_rk2 : for(y=0;y<4;y++){
			  result [y+x*4] = state [y+x*4] ^ ekey[iteration * nb * 4 + x * nb + y];
	    }
	  }
}

void mixcolumn_sw(uint8_t state[16],uint8_t result[16])
{
	//B1’ = (B1 * 2) XOR (B2 * 3) XOR (B3 * 1) XOR (B4 * 1)
	//B2’ = (B1 * 1) XOR (B2 * 2) XOR (B3 * 3) XOR (B4 * 1)
	//B3’ = (B1 * 1) XOR (B2 * 1) XOR (B3 * 2) XOR (B4 * 3)
	//B4’ = (B1 * 3) XOR (B2 * 1) XOR (B3 * 1) XOR (B4 * 2)

	uint8_t x; // control of the column


	loop_mx1 :for(x=0;x<4;x++){

      	       result[x*4] = (gf2[state[x*4]])^(gf3[state[1+x*4]])^(state[2+x*4])^(state[3+x*4]);
               result[1+x*4] = (state[x*4])^(gf2[state[1+x*4]])^(gf3[state[2+x*4]])^(state[3+x*4]);
               result[2+x*4] = (state[x*4])^(state[1+x*4])^(gf2[state[2+x*4]])^(gf3[state[3+x*4]]);
               result[3+x*4] = (gf3[state[x*4]])^(state[1+x*4])^(state[2+x*4])^(gf2[state[3+x*4]]);
	}
}
