/*
/*Copyright (c) 2018, Jose Nunez-Yanez*/
/*University of Bristol. ENEAC project*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <hls_stream.h>
#include <sds_lib.h>
#include <ap_int.h>
#include "sbox.h"

#ifndef AES_ENC_H_
#define AES_ENC_H_
#define nk 8
#define nb 4
#define nr 14


typedef ap_uint<128> data_t;
typedef hls::stream<data_t> data_stream_t;
const unsigned int INI_VAL=1;
const unsigned int CU=4;


void aes_enc(data_t *state,data_t *cipher,uint8_t ekey[240],unsigned int block_size);

#endif /* AES_ENC_H_ */
