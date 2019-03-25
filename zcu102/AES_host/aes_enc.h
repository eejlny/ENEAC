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


#ifndef AES_ENC_H_
#define AES_ENC_H_
#define nk 8
#define nb 4
//#define nr 2
#define nr 14
#define DEBUG 0
#define BLOCK_SIZE 16


#if DEBUG==1
	#define block_size 1   //the size of the data in multiples of 16
	//#define int_block_size 8
#else
	#define block_size 1048576   //the size of the d in multiples of 16
	//#define int_block_size 262144
#endif

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
#  include <arm_neon.h>
# endif
/* GCC and LLVM Clang, but not Apple Clang */
# if defined(__GNUC__) && !defined(__apple_build_version__)
#  if defined(__ARM_ACLE) || defined(__ARM_FEATURE_CRYPTO)
#   include <arm_acle.h>
#  endif
# endif
#endif  /* ARM Headers */

void aes_process_arm(const uint8_t key[], const uint8_t subkeys[], uint32_t rounds,
                     const uint8_t input[], uint8_t output[], uint32_t length)
{
	while (length >= 16)
	{
		uint8x16_t block = vld1q_u8(input);

		// AES single round encryption
		block = vaeseq_u8(block, vld1q_u8(key));
		// AES mix columns
		block = vaesmcq_u8(block);

		// AES single round encryption
		block = vaeseq_u8(block, vld1q_u8(subkeys));
		// AES mix columns
		block = vaesmcq_u8(block);

		for (unsigned int i=1; i<rounds-2; ++i)
		{
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
	}
}


void aes_enc(uint8_t *state,uint8_t *cipher,uint8_t ekey[240]);

////void aes_enc_hw(uint32_t *state,uint32_t *cipher,uint8_t ekey[240],int block_count,int *interrupt,int *status);
void aes_enc_hw(uint32_t *state,uint32_t *cipher,uint8_t ekey[240],int block_count);

void aes_enc_async(uint32_t *state_full,uint32_t *cipher_full,uint8_t ekey[240]);

float step;

////int *interrupt;
////int *status;
uint32_t *state;
uint32_t *cipher;
uint8_t vbyte[4];
uint8_t sbyte[4];
////int file_desc;
unsigned int numfpgas;

uint8_t *ekey;

uint8_t key[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };  // initial key

#endif /* AES_ENC_H_ */
