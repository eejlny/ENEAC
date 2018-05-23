#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sds_lib.h>
//#include <sys/time.h>
#include "aes_enc.h"

void kernelAES(
uint32_t *state,
uint32_t *cipher,
uint8_t ekey[240],
int begin,
int end)
{
int block_count = end-begin;
uint32_t *state_temp = state + begin*4;
uint32_t *cipher_temp = cipher + begin*4;
#pragma SDS async(1)
aes_enc_hw(state_temp, cipher_temp, ekey, block_count);
#pragma SDS wait(1)
}
