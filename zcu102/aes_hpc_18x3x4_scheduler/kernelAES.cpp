#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sds_lib.h>
//#include <sys/time.h>
#include "aes_enc.h"

void kernelAES1(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)
{
int block_size = 16*(end-begin); //block_size in bytes
uint8_t *state_temp = state + begin*16;
uint8_t *cipher_temp = cipher + begin*16;
//#pragma SDS resource(1)
aes_enc1((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);

}

void kernelAES2(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)
{
int block_size = 16*(end-begin);  //block_size in bytes
uint8_t *state_temp = state + begin*16;
uint8_t *cipher_temp = cipher + begin*16;
//#pragma SDS resource(2)
aes_enc2((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);

}


void kernelAES3(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)
{
int block_size = 16*(end-begin);
uint8_t *state_temp = state + begin*16;
uint8_t *cipher_temp = cipher + begin*16;
//#pragma SDS resource(3)
aes_enc3((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);

}


void kernelAES4(
uint8_t *state,
uint8_t *cipher,
uint8_t ekey[240],
int begin,
int end)
{
int block_size = 16*(end-begin);
uint8_t *state_temp = state + begin*16;
uint8_t *cipher_temp = cipher + begin*16;
//#pragma SDS resource(4)
aes_enc4((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);

}
