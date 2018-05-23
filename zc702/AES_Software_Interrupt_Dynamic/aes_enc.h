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

#define HW_ADDR_GPIO 0x41200000
#define HW_ADDR_GPIO_INT 0x41200120
#define fpga_mmio_gpio 0x48 /*location of status register in GPIO. wrte this to generate interrupt*/

void aes_enc(uint8_t *state,uint8_t *cipher,uint8_t ekey[240]);

void aes_enc_hw(uint32_t *state,uint32_t *cipher,uint8_t ekey[240],int block_count,int *interrupt,int *status);

void aes_enc_async(uint32_t *state_full,uint32_t *cipher_full,uint8_t ekey[240]);

float step;

int *interrupt;
int *status;
uint32_t *state;
uint32_t *cipher;
uint8_t vbyte[4];
uint8_t sbyte[4];
int file_desc;

uint8_t *ekey;

uint8_t key[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };  // initial key

#endif /* AES_ENC_H_ */
