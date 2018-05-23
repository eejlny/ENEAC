/* File: matrix_mult_accel.cpp
 *
 Copyright (c) [2016] [Mohammad Hosseinabady (mohammad@hosseinabady.com)]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
* This file has been written at University of Bristol
* for the ENPOWER project funded by EPSRC
*
* File name : matrix_mult_accel.cpp
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 1 October 2016
* blog: https://highlevel-synthesis.com/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix_mult.h"

inline void mxv(float* a, float* c, float b[B_HEIGHT][B_WIDTH_BLOCK]);
typedef unsigned long u32;

// note that  BLOCK shoudl be less than B_WIDTH_BLOCK
const int BLOCK=B_WIDTH_BLOCK;   //BLOCK should be less than B_WIDTH_BLOCK
const int STEP=1;

/*
 The amount of data saved in the FPGA is B_HEIGHT*B_WIDTH_BLOCK+A_WIDTH+B_WIDTH_BLOCK which should be less than FPGA BRAM size
*/
#pragma SDS data sys_port(A:HPC,B:HPC,C:HPC)
#pragma SDS data zero_copy(A[0:line_count*A_WIDTH],B[0:B_HEIGHT*B_WIDTH], C[0:line_count*B_WIDTH])
int mmult_top(float* A, float* B, float* C,int line_count)
{
	 float A_accel[A_WIDTH], B_accel[B_HEIGHT][B_WIDTH_BLOCK], C_accel[B_WIDTH_BLOCK];
	 #pragma HLS array_partition variable=A_accel block factor=16 dim=1
	 #pragma HLS array_partition variable=B_accel block factor=16 dim=2
	 #pragma HLS array_partition variable=C_accel complete



	 for (int B_index = 0; B_index < B_WIDTH/B_WIDTH_BLOCK; B_index++) {

		 for (int i = 0; i < B_HEIGHT; i++) {
					for (int j = 0; j < B_WIDTH_BLOCK; j++) {
						B_accel[i][j] = B[i*B_WIDTH+j+B_index*B_WIDTH_BLOCK];
					}
		 }


		 for (int A_index = 0; A_index < line_count; A_index++) {

				for (int j = 0; j < A_WIDTH; j++) {
					A_accel[j] = A[A_index*A_WIDTH+j];
				}

				 mmult_accel(A_accel, B_accel, C_accel);

				 for (int i = 0; i < C_HEIGHT_BLOCK; i++) {
					for (int j = 0; j < C_WIDTH_BLOCK; j++) {
						C[(i+A_index*A_HEIGHT_BLOCK)*C_WIDTH+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
					}
				 }

	    	 }
	     }
}

int mmult_accel( float A[A_HEIGHT_BLOCK*A_WIDTH], float B[B_HEIGHT][B_WIDTH_BLOCK], float C[C_HEIGHT_BLOCK*C_WIDTH_BLOCK])
{

	//float b[B_HEIGHT][B_WIDTH_BLOCK];
//#pragma HLS ARRAY_PARTITION variable=b complete dim=2

/*
	for (int i = 0; i < B_HEIGHT; i++) {
		for (int j = 0; j < B_WIDTH_BLOCK; j++) {
	#pragma HLS PIPELINE
			b[i][j] = *(B+i*B_WIDTH_BLOCK+j);
		}
	}*/


	for (int p = 0; p < A_HEIGHT_BLOCK; p+=STEP) {
		mxv(A+p*A_WIDTH, C+p*C_WIDTH_BLOCK, B);
	}


	return 0;
}

inline void mxv(float *A, float* C, float b[B_HEIGHT][B_WIDTH_BLOCK])  {

	//float a[A_WIDTH];
	//float c[B_WIDTH_BLOCK];
    //#pragma HLS ARRAY_PARTITION variable=c complete dim=1
    //#pragma HLS array_partition variable=a block factor=16

	//memcpy(a,(float *)(A),A_WIDTH*sizeof(float));


	//for (int i = 0; i < A_WIDTH; i++) {
	//	a[i] = A[i];
	//}

	for (int j = 0; j < C_WIDTH_BLOCK; j++) {
#pragma HLS UNROLL
		C[j] = 0;
	}

	for(int k = 0; k < B_HEIGHT; k+=1) {
		for (int j = 0; j < B_WIDTH_BLOCK; j++) {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor=BLOCK
			C[j] += A[k]*b[k][j];
		}
	}

//
//	for (int i = 0; i < B_WIDTH_BLOCK; i++) {
//		C[i] = c[i];
//	}

	//memcpy((float *)(C), c, C_WIDTH_BLOCK*sizeof(float));

	return;
}
