/*
/*Copyright (c) 2018, Jose Nunez-Yanez*/
/*University of Bristol. ENEAC project*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sds_lib.h"
#include "sbox.h"

#ifndef AES_ENC_H_
#define AES_ENC_H_
#define nk 8
#define nb 4
#define nr 14

const unsigned int INI_VAL=1;
const unsigned int CU=1;

void aes_enc_sw(uint8_t *state,uint8_t *cipher,uint8_t ekey[240],unsigned int block_size);
void aes_enc_v8(const uint8_t key[],const uint8_t subkeys[],uint32_t rounds,const uint8_t input[], uint8_t output[],uint32_t length);



#endif /* AES_ENC_H_ */
