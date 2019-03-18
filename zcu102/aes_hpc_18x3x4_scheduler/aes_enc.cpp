/*Copyright (c) 2018, Jose Nunez-Yanez*/
/*University of Bristol. ENEAC project*/


#include "aes_enc.h"

extern "C"
{
void *__dso_handle = NULL;
}




void aes_wrapper(data_t *state,data_t *cipher,uint8_t ekey[240],uint32_t byte_count);
void addroundkey(data_stream_t &state,uint8_t iteration, data_stream_t &result,uint8_t ekey[240],uint32_t byte_count);
void subbytes(data_stream_t &state, data_stream_t &result,uint32_t byte_count);
void mixcolumn(data_stream_t &state, data_stream_t &result,uint32_t byte_count);
void array2stream(data_t *state, data_stream_t &result, uint32_t byte_count);
void stream2array(data_stream_t &state, data_t *result, uint32_t byte_count);





//#pragma SDS data mem_attribute(cipher:PHYSICAL_CONTIGUOUS)
//#pragma SDS data mem_attribute(state:PHYSICAL_CONTIGUOUS)
#pragma SDS data data_mover(state:AXIDMA_SIMPLE, cipher:AXIDMA_SIMPLE)
#pragma SDS data copy(state[0:(block_size/16)])
#pragma SDS data copy(cipher[0:(block_size/16)])
#pragma SDS data access_pattern(state:SEQUENTIAL)
#pragma SDS data access_pattern(cipher:SEQUENTIAL)
#pragma SDS data sys_port(state:HPC,cipher:HPC)
void aes_enc(data_t *state,data_t *cipher,uint8_t ekey[240],unsigned int block_size)

{
	int i,j;
	uint8_t iteration = 0;
	uint8_t x,y;

	data_stream_t state_stream;
	data_stream_t cipher_stream;

	uint8_t ekey_buf[240];

    #pragma HLS array_partition variable=ekey_buf complete

	for(j=0; j<240; j++) {
	   #pragma HLS PIPELINE
	         ekey_buf[j] = ekey[j];
	}
	aes_wrapper(state,cipher,ekey_buf,block_size);


}



void aes_wrapper(data_t *state,data_t *cipher,uint8_t ekey[240],uint32_t byte_count)
{


	data_stream_t state_stream;
	#pragma HLS STREAM variable=state_stream depth=1
	data_stream_t cipher_stream;
	#pragma HLS STREAM variable=state_stream depth=1

	data_stream_t inter0;
	#pragma HLS STREAM variable=inter0 depth=1
	data_stream_t inter1;
	#pragma HLS STREAM variable=inter1 depth=1
	data_stream_t inter2;
	#pragma HLS STREAM variable=inter2 depth=1
	data_stream_t inter3;
	#pragma HLS STREAM variable=inter3 depth=1
	data_stream_t inter4;
	#pragma HLS STREAM variable=inter4 depth=1
	data_stream_t inter5;
	#pragma HLS STREAM variable=inter5 depth=1
	data_stream_t inter6;
	#pragma HLS STREAM variable=inter6 depth=1
	data_stream_t inter7;
	#pragma HLS STREAM variable=inter7 depth=1
	data_stream_t inter8;
	#pragma HLS STREAM variable=inter8 depth=1
	data_stream_t inter9;
	#pragma HLS STREAM variable=inter9 depth=1
	data_stream_t inter10;
	#pragma HLS STREAM variable=inter10 depth=1
	data_stream_t inter11;
	#pragma HLS STREAM variable=inter11 depth=1
	data_stream_t inter12;
	#pragma HLS STREAM variable=inter12 depth=1
	data_stream_t inter13;
	#pragma HLS STREAM variable=inter13 depth=1
	data_stream_t inter14;
	#pragma HLS STREAM variable=inter14 depth=1
	data_stream_t inter15;
	#pragma HLS STREAM variable=inter15 depth=1
	data_stream_t inter16;
	#pragma HLS STREAM variable=inter16 depth=1
	data_stream_t inter17;
	#pragma HLS STREAM variable=inter17 depth=1
	data_stream_t inter18;
	#pragma HLS STREAM variable=inter18 depth=1
	data_stream_t inter19;
	#pragma HLS STREAM variable=inter19 depth=1
	data_stream_t inter20;
	#pragma HLS STREAM variable=inter20 depth=1
	data_stream_t inter21;
	#pragma HLS STREAM variable=inter21 depth=1
	data_stream_t inter22;
	#pragma HLS STREAM variable=inter22 depth=1
	data_stream_t inter23;
	#pragma HLS STREAM variable=inter23 depth=1
	data_stream_t inter24;
	#pragma HLS STREAM variable=inter24 depth=1
	data_stream_t inter25;
	#pragma HLS STREAM variable=inter25 depth=1
	data_stream_t inter26;
	#pragma HLS STREAM variable=inter26 depth=1
	data_stream_t inter27;
	#pragma HLS STREAM variable=inter27 depth=1
	data_stream_t inter28;
	#pragma HLS STREAM variable=inter28 depth=1
	data_stream_t inter29;
	#pragma HLS STREAM variable=inter29 depth=1
	data_stream_t inter30;
	#pragma HLS STREAM variable=inter30 depth=1
	data_stream_t inter31;
	#pragma HLS STREAM variable=inter31 depth=1
	data_stream_t inter32;
	#pragma HLS STREAM variable=inter32 depth=1
	data_stream_t inter33;
	#pragma HLS STREAM variable=inter33 depth=1
	data_stream_t inter34;
	#pragma HLS STREAM variable=inter34 depth=1
	data_stream_t inter35;
	#pragma HLS STREAM variable=inter35 depth=1
	data_stream_t inter36;
	#pragma HLS STREAM variable=inter36 depth=1
	data_stream_t inter37;
	#pragma HLS STREAM variable=inter37 depth=1
	data_stream_t inter38;
	#pragma HLS STREAM variable=inter38 depth=1
	data_stream_t inter39;
	#pragma HLS STREAM variable=inter39 depth=1
	data_stream_t inter40;
	#pragma HLS STREAM variable=inter40 depth=1

	#pragma HLS DATAFLOW
	array2stream(state,state_stream,byte_count);

	addroundkey(state_stream,0,inter0,ekey,byte_count);

	subbytes(inter0,inter1,byte_count);
	mixcolumn(inter1,inter2,byte_count);
	addroundkey(inter2,1,inter3,ekey,byte_count);

	subbytes(inter3,inter4,byte_count);
	mixcolumn(inter4,inter5,byte_count);
	addroundkey(inter5,2,inter6,ekey,byte_count);

	subbytes(inter6,inter7,byte_count);
	mixcolumn(inter7,inter8,byte_count);
	addroundkey(inter8,3,inter9,ekey,byte_count);

	subbytes(inter9,inter10,byte_count);
	mixcolumn(inter10,inter11,byte_count);
	addroundkey(inter11,4,inter12,ekey,byte_count);

	subbytes(inter12,inter13,byte_count);
	mixcolumn(inter13,inter14,byte_count);
	addroundkey(inter14,5,inter15,ekey,byte_count);

	subbytes(inter15,inter16,byte_count);
	mixcolumn(inter16,inter17,byte_count);
	addroundkey(inter17,6,inter18,ekey,byte_count);

	subbytes(inter18,inter19,byte_count);
	mixcolumn(inter19,inter20,byte_count);
	addroundkey(inter20,7,inter21,ekey,byte_count);

	subbytes(inter21,inter22,byte_count);
	mixcolumn(inter22,inter23,byte_count);
	addroundkey(inter23,8,inter24,ekey,byte_count);

	subbytes(inter24,inter25,byte_count);
	mixcolumn(inter25,inter26,byte_count);
	addroundkey(inter26,9,inter27,ekey,byte_count);

	subbytes(inter27,inter28,byte_count);
	mixcolumn(inter28,inter29,byte_count);
	addroundkey(inter29,10,inter30,ekey,byte_count);

	subbytes(inter30,inter31,byte_count);
	mixcolumn(inter31,inter32,byte_count);
	addroundkey(inter32,11,inter33,ekey,byte_count);

	subbytes(inter33,inter34,byte_count);
	mixcolumn(inter34,inter35,byte_count);
	addroundkey(inter35,12,inter36,ekey,byte_count);

	subbytes(inter36,inter37,byte_count);
	mixcolumn(inter37,inter38,byte_count);
	addroundkey(inter38,13,inter39,ekey,byte_count);

	subbytes(inter39,inter40,byte_count);
	addroundkey(inter40,14,cipher_stream,ekey,byte_count);

	stream2array(cipher_stream,cipher,byte_count);

}


void subbytes(data_stream_t &state, data_stream_t &result,uint32_t byte_count)
{

	data_t state_temp,state_temp2;
	uint8_t result_temp[16];
	uint8_t state_array[16];
	uint8_t x, y; //addresses the matrix

	#pragma HLS array_partition variable=result_temp complete
	#pragma HLS array_partition variable=state_array complete

	for (unsigned int i = 0; i < byte_count/16; i++)
	{
    #pragma HLS loop_tripcount min=1 max=1024
	state_temp = state.read();


	for(unsigned int s=0;s<16;s++)
	{
		#pragma HLS unroll
		state_array[s] = state_temp.range(s*8+7,s*8);
	}


 #pragma HLS PIPELINE II=INI_VAL
	loop_sb1 : for(x=0;x<4;x++){
		loop_sb2 : for(y=0;y<4;y++){
			uint8_t state_byte = state_array[x*4+y];
			result_temp[x*4+y] = sbox[state_byte];
		}//end y loop
	}//end x loop
	//row_shift_enc

	state_temp2.range(7,0) = result_temp[0];
	state_temp2.range(15,8) = result_temp[1];
	state_temp2.range(23,16) = result_temp[2];
	state_temp2.range(31,24) = result_temp[3];

	state_temp2.range(39,32) = result_temp[5];
	state_temp2.range(47,40) = result_temp[6];
	state_temp2.range(55,48) = result_temp[7];
	state_temp2.range(63,56) = result_temp[4];

	state_temp2.range(71,64) = result_temp[10];
	state_temp2.range(87,80) = result_temp[8];
	state_temp2.range(79,72) = result_temp[11];
	state_temp2.range(95,88) = result_temp[9];

	state_temp2.range(103,96) = result_temp[15];
	state_temp2.range(127,120) = result_temp[14];
	state_temp2.range(119,112) = result_temp[13];
	state_temp2.range(111,104) = result_temp[12];

	result.write(state_temp2);
	}
}

void array2stream(data_t *state, data_stream_t &result, uint32_t byte_count)
{

	for (unsigned int i = 0; i < byte_count/16; i++)
	{
    #pragma HLS loop_tripcount min=1 max=1024
	#pragma HLS PIPELINE II=INI_VAL
		data_t state_buf = *(state+i);
		result.write(state_buf);
	}
}

void stream2array(data_stream_t &state, data_t *result, uint32_t byte_count)
{
	for (unsigned int i = 0; i < byte_count/16; i++)
	{
    #pragma HLS loop_tripcount min=1 max=1024
	#pragma HLS PIPELINE II=INI_VAL
		data_t result_buf = state.read();
		*(result+i) = result_buf;
	}
}


void addroundkey(data_stream_t &state, uint8_t iteration,data_stream_t &result,uint8_t ekey[240],uint32_t byte_count)
{

uint8_t result_array[16];
uint8_t state_array[16];

#pragma HLS array_partition variable=state_array complete
#pragma HLS array_partition variable=result_array complete


data_t result_temp;

data_t state_temp;

for (unsigned int i = 0; i < byte_count/16; i++)
{
#pragma HLS loop_tripcount min=1 max=1024
state_temp = state.read();

for(unsigned int s=0;s<16;s++)
{
	#pragma HLS unroll
	state_array[s] = state_temp(s*8+7,s*8);
}



	  unsigned int x,y;


	  #pragma HLS PIPELINE II=INI_VAL
	  loop_rk1 :for(x=0;x<4;x++) {
	 		loop_rk2 : for(y=0;y<4;y++){
			  result_array[y+x*4] = state_array[y+x*4]^ ekey[iteration * nb * 4 + x * nb + y];

	    }
   }


 for(unsigned int s=0;s<16;s++)
 {
	#pragma HLS unroll
	 result_temp.range(s*8+7,s*8) = result_array[s];
 }


result.write(result_temp);
}

}

void mixcolumn(data_stream_t &state,data_stream_t &result,uint32_t byte_count)
{
	//B1’ = (B1 * 2) XOR (B2 * 3) XOR (B3 * 1) XOR (B4 * 1)
	//B2’ = (B1 * 1) XOR (B2 * 2) XOR (B3 * 3) XOR (B4 * 1)
	//B3’ = (B1 * 1) XOR (B2 * 1) XOR (B3 * 2) XOR (B4 * 3)
	//B4’ = (B1 * 3) XOR (B2 * 1) XOR (B3 * 1) XOR (B4 * 2)

	uint8_t x; // control of the column
	uint8_t result_array[16];
	uint8_t state_array[16];


   #pragma HLS array_partition variable=state_array complete
   #pragma HLS array_partition variable=result_array complete

	data_t state_temp,result_temp;

	for (unsigned int i = 0; i < byte_count/16; i++)
	{
    #pragma HLS loop_tripcount min=1 max=1024
	state_temp = state.read();


	for(unsigned int s=0;s<16;s++)
	{
		#pragma HLS unroll
		state_array[s] = state_temp.range(s*8+7,s*8);
	}

#pragma HLS PIPELINE II=INI_VAL
	loop_mx1 :for(x=0;x<4;x++){
		result_array[x] = (gf2[state_array[x]])^(gf3[state_array[4+x]])^(state_array[8+x])^(state_array[12+x]);
		result_array[4+x] = (state_array[x])^(gf2[state_array[4+x]])^(gf3[state_array[8+x]])^(state_array[12+x]);
		result_array[8+x] = (state_array[x])^(state_array[4+x])^(gf2[state_array[8+x]])^(gf3[state_array[12+x]]);
		result_array[12+x] = (gf3[state_array[x]])^(state_array[4+x])^(state_array[8+x])^(gf2[state_array[12+x]]);


	}


	for(unsigned int s=0;s<16;s++)
	{
		#pragma HLS unroll
		result_temp.range(s*8+7,s*8) = result_array[s];
	}

	result.write(result_temp);

	}
}


