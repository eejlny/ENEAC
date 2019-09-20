/*
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

#define nk 8
#define nb 4
//#define nr 2
#define nr 14
//#define DEBUG 
//#define BLOCK_SIZE 16


// #ifdef DEBUG
	// int block_size = 1;   //the size of the data in multiples of 16
	// //#define int_block_size 8
// #else
	// //#define block_size 1048576   //the size of the d in multiples of 16
	// //#define int_block_size 262144
// #endif

////#define HW_ADDR_GPIO 0x41200000
////#define HW_ADDR_GPIO_INT 0x41200120
////#define fpga_mmio_gpio 0x48 /*location of status register in GPIO. wrte this to generate interrupt*/

/* aes-arm.c - ARMv8 AES extensions using C intrinsics         */
/*   Written and placed in public domain by Jeffrey Walton     */
/*   Based on code from ARM, and by Johannes Schneiders, Skip  */
/*   Hovsmith and Barry O'Rourke for the mbedTLS project.      */

/* gcc -std=c99 -march=armv8-a+crypto aes-arm.c -o aes-arm.exe */

/* Visual Studio 2017 and above supports ARMv8, but its not clear how to detect */
/* it or use it at the moment. Also see http://stackoverflow.com/q/37244202,    */
/* http://stackoverflow.com/q/41646026, and http://stackoverflow.com/q/41688101 */
#if defined(__arm__) || defined(__aarch32__) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM)
# if defined(__GNUC__)
#  include <stdint.h>
# endif

# if defined(__ARM_NEON) || defined(_MSC_VER)
     # include <arm_neon.h>
# endif

/* GCC and LLVM Clang, but not Apple Clang */
# if defined(__GNUC__) && !defined(__apple_build_version__)
#  if defined(__ARM_ACLE) || defined(__ARM_FEATURE_CRYPTO)
#   include <arm_acle.h>
#  endif
# endif
#endif  /* ARM Headers */

#include <arm_acle.h>


//AES specific defines
#define nk 8
#define nb 4
#define nr 14

#define TIME_STAMP_INIT_HW  unsigned long long clock_start_hw, clock_end_hw;  clock_start_hw = sds_clock_counter();
#define TIME_STAMP_HW { clock_end_hw = sds_clock_counter(); fprintf(stderr,"SPARSE FPGA ON: execution time : %f ms\n", 1000*(clock_end_hw-clock_start_hw)/(1200*10e6)); clock_start_hw = sds_clock_counter(); }

/*****************************************************************************
 * Global variables
 * **************************************************************************/

uint8_t *state;
uint8_t *cipher;
uint8_t *ekey;
uint8_t *state_noncache;
uint8_t *cipher_noncache;
uint8_t *ekey_noncache;
uint8_t vbyte[4];
uint8_t sbyte[4];
unsigned int numfpgas;
uint8_t key[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };  // initial key

ofstream resultfile;

int main_thread=0;
int main_core=0;
int block_size; //how many blocks of 16 bytes to process
float step;
const unsigned AOCL_ALIGNMENT = 64;
char *infile;
FILE *fp;

//These are user input parameters defined in hotspot-top.cpp
int numhpacc;
int numhpcacc;
int cont_read;
int ioctl_flag;
int frame_width;
long unsigned int bodies_C=0, bodies_F=0;

#ifdef HWDEBUG
     int debug_flag = 1;
#else
     int debug_flag;
#endif

/*Interrupt drivers*/
#define DRIVER_FILE_NAME_1 "/dev/intgendriver1"
int file_desc_1;
#define DRIVER_FILE_NAME_2 "/dev/intgendriver2"
int file_desc_2;

//Variables used for timing
struct timespec start1, start2, start3, start4, finish1, finish2, finish3, finish4;
double elapsed1, elapsed2, elapsed3, elapsed4;

/*****************************************************************************
 * Functions 
 * **************************************************************************/

void usage(
     int argc, 
     char **argv) {
          fprintf(stderr, "Usage: %s <input_file> <output_file> <numcpus> <numhpacc> <numhpcacc> <chunkGPU> <IOCTL>\n", argv[0]);
          fprintf(stderr, "\t<inputfile> - name of file to encrypt\n");
          fprintf(stderr, "\t<numcpus> - number of CPU cores (integer: 0-4)\n");
          fprintf(stderr, "\t<numhpacc> - number of HP accelerators (must be preloaded in fpga)\n");
          fprintf(stderr, "\t<numhpcacc> - number of HPC accelerators (must be preloaded in fpga) \n");
          fprintf(stderr, "\t<chunkFPGA> - (Fixed or Dynamic) (0 or higher integer\n");
          fprintf(stderr, "\t<IOCTL> - 0/1 - enable hardware interrupts (0-No or 1-Yes)\n");     
          fprintf(stderr, "\t<outputfile> - write encripted output to file\n");
          exit(0);
     };


void keyexpansion(
     uint8_t key[32],
     uint8_t ekey[240]) {
          uint32_t i, j, k;
          uint8_t temp[4];

          for(i = 0; i < nk; ++i) {
               ekey[(i * 4) + 0] = key[(i * 4) + 0];
               ekey[(i * 4) + 1] = key[(i * 4) + 1];
               ekey[(i * 4) + 2] = key[(i * 4) + 2];
               ekey[(i * 4) + 3] = key[(i * 4) + 3];
          }

          for(; (i < (nb * (nr + 1))); ++i) {
               for(j = 0; j < 4; ++j)
                    temp[j]= ekey[(i-1) * 4 + j];
                    
               if (i % nk == 0) {
                    k = temp[0];
                    temp[0] = temp[1];
                    temp[1] = temp[2];
                    temp[2] = temp[3];
                    temp[3] = k;
                    temp[0] = sbox[temp[0]];
                    temp[1] = sbox[temp[1]];
                    temp[2] = sbox[temp[2]];
                    temp[3] = sbox[temp[3]];
                    temp[0] =  temp[0] ^ Rcon[i/nk];
               }
               else if (nk > 6 && i % nk == 4) {
                    temp[0] = sbox[temp[0]];
                    temp[1] = sbox[temp[1]];
                    temp[2] = sbox[temp[2]];
                    temp[3] = sbox[temp[3]];
               }
               ekey[i * 4 + 0] = ekey[(i - nk) * 4 + 0] ^ temp[0];
               ekey[i * 4 + 1] = ekey[(i - nk) * 4 + 1] ^ temp[1];
               ekey[i * 4 + 2] = ekey[(i - nk) * 4 + 2] ^ temp[2];
               ekey[i * 4 + 3] = ekey[(i - nk) * 4 + 3] ^ temp[3];
          }
     };

int fsize(
     FILE *fp) {
          int prev=ftell(fp);
          fseek(fp, 0L, SEEK_END);
          int sz=ftell(fp);
          fseek(fp,prev,SEEK_SET); //go back to where we were
          return sz;
     };

void read_input(
     uint8_t *state,
     FILE *fp) {	
          fread(state, (16*block_size), 1, fp); // Read in the entire block
          fclose(fp);
     };
     
uint8_t extractbyte_sw(
     uint32_t src,
     unsigned from,
     unsigned to) {
          unsigned mask = ( (1<<(to-from+1))-1) << from;
          return (src & mask) >> from;
     };

void shift_row_enc_sw(
     uint8_t state[16],
     uint8_t result[16]) {
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
     };

void subbytes_sw(
     uint8_t state[16],
     uint8_t result[16]) {
          uint8_t x, y; //addresses the matrix

          loop_sb1 : for(x=0;x<4;x++)
               loop_sb2 : for(y=0;y<4;y++)
                    result[x*4+y] = sbox[state[x*4+y]];
     };

void mixcolumn_sw(
     uint8_t state[16],
     uint8_t result[16]) {
          uint8_t x; // control of the column
          loop_mx1 :for(x=0;x<4;x++){
               result[x] = (gf2[state[x]])^(gf3[state[4+x]])^(state[8+x])^(state[12+x]);
               result[4+x] = (state[x])^(gf2[state[4+x]])^(gf3[state[8+x]])^(state[12+x]);
               result[8+x] = (state[x])^(state[4+x])^(gf2[state[8+x]])^(gf3[state[12+x]]);
               result[12+x] = (gf3[state[x]])^(state[4+x])^(state[8+x])^(gf2[state[12+x]]);
          }
     };

void addroundkey_sw(
     uint8_t state[16],
     uint8_t iteration,
     uint8_t result[16],
     uint8_t ekey[240]) {
          uint8_t x,y;
          loop_rk1 :for(x=0;x<4;x++)
               loop_rk2 : for(y=0;y<4;y++)
                    result [y+x*4] = state [y+x*4] ^ ekey[iteration * nb * 4 + x * nb + y];
     };     
     
void aes_slow(
     int begin,
     int end
     ) {
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

          for (i = begin; i<end; i ++) {
               for (j = 0; j<4; j++) {
                    state_grid[4*j] = extractbyte_sw(*(state + j+4*i), 0, 7);
                    state_grid[4*j + 1] = extractbyte_sw(*(state + j+4*i), 8, 15);
                    state_grid[4*j + 2] = extractbyte_sw(*(state + j+4*i), 16, 23);
                    state_grid[4*j + 3] = extractbyte_sw(*(state + j+4*i), 24, 31);
               }

               addroundkey_sw(state_grid, 0, sub, ekey);
               
               for (iteration = 1; iteration < nr; iteration++) {
                    subbytes_sw(sub, shift);
                    shift_row_enc_sw(shift, mix);
                    mixcolumn_sw(mix, round);
                    addroundkey_sw(round, iteration, sub, ekey);
               }
               
               subbytes_sw(sub, shift);
               shift_row_enc_sw(shift, round);
               addroundkey_sw(round, nr, result, ekey);
               for (z = 0; z<4; z++)
                    *(cipher + z + i * 4) = (result[4*z + 3] << 24) | (result[4*z + 2] << 16) | (result[4*z + 1] << 8) | result[4*z];
          }
     };

void aes_process_arm(
     const uint8_t key[],
     const uint8_t subkeys[],
     uint32_t rounds,
     const uint8_t input[], 
     uint8_t output[],
     uint32_t length) {
          while (length >= 16) {
               uint8x16_t block = vld1q_u8(input);

               // AES single round encryption
               block = vaeseq_u8(block, vld1q_u8(key));
               // AES mix columns
               block = vaesmcq_u8(block);

               // AES single round encryption
               block = vaeseq_u8(block, vld1q_u8(subkeys));
               // AES mix columns
               block = vaesmcq_u8(block);

               for (unsigned int i=1; i<rounds-2; ++i) {
                    // AES single round encryption
                    block = vaeseq_u8(block, vld1q_u8(subkeys+i*16));
                    // AES mix columns
                    block = vaesmcq_u8(block);
               }

               // AES single round encryption
               block = vaeseq_u8(block, vld1q_u8(subkeys+(rounds-2)*16));
               // Final Add (bitwise Xor)
               block = veorq_u8(block, vld1q_u8(subkeys+(rounds-1)*16));

               vst1q_u8(output, block);

               input += 16; output += 16;
               length -= 16;
               //printf("remaining length is %d\n", length);
          }
     };

void freemem(){
     sds_free(ekey);
     sds_free(state);
     sds_free(cipher);
     if (numhpacc > 0) {     
          sds_free(ekey_noncache);
          sds_free(state_noncache);
          sds_free(cipher_noncache);
     }     
};

void write_output(uint8_t *state, char *file)
{
	fp = fopen(file, "wb");
	if (!fp) {
		printf("file could not be opened for writing\n");
          freemem();
		exit(1);
	}
	fwrite(state,(16*block_size), 1, fp); // Read in the entire block
	fclose(fp);
};

