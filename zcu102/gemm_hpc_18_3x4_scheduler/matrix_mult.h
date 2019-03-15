/* File: matrix_mult.h
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
* File name : matrix_mult.h
* author    : Mohammad hosseinabady mohammad@hosseinabady.com
* date      : 1 October 2016
* blog: https://highlevel-synthesis.com/
*/

#ifndef __MATRIX_MULT_H__
#define __MATRIX_MULT_H__


#define N    2048
#define M    2048
#define P    2048


#define A_HEIGHT   N
#define A_WIDTH    M

#define B_HEIGHT   M
#define B_WIDTH    P

#define C_HEIGHT   N
#define C_WIDTH    P


#define A_HEIGHT_BLOCK  1// 4096 //(512/4)
#define B_WIDTH_BLOCK   32//(128/4)

/*
 A_HEIGHT_BLOCK  is for software part data partitioning due to the limitation in 
 the Xilinx kernel sds_alloc so A_HEIGHT_BLOCK should be A_HEIGHT divided by 
 the number of considered blocks
*/
#define C_HEIGHT_BLOCK  A_HEIGHT_BLOCK 


#define C_WIDTH_BLOCK   B_WIDTH_BLOCK //B_WIDTH_BLOCK shoudl be less than P

typedef unsigned long u32;


int mmult_accel( float A[A_HEIGHT_BLOCK*A_WIDTH], float B[B_HEIGHT][B_WIDTH_BLOCK], float C[C_HEIGHT_BLOCK*C_WIDTH_BLOCK]);
//#pragma SDS data access_pattern(A:SEQUENTIAL)

//#pragma SDS data zero_copy(B[0:B_HEIGHT*B_WIDTH], C[0:A_HEIGHT*B_WIDTH])
//#pragma SDS data copy(A[0:A_HEIGHT*A_WIDTH])
//#pragma SDS data access_pattern(A:RANDOM)
//#pragma SDS data zero_copy(A[0:A_HEIGHT*A_WIDTH],B[0:B_HEIGHT*B_WIDTH], C[0:A_HEIGHT*B_WIDTH])
//#pragma SDS data sys_port(A:AFI,B:AFI,C:AFI)
//#pragma SDS data access_pattern(A:ACP;B:ACP;C:ACP)
//#pragma SDS data zero_copy(A[0:line_count*A_WIDTH],B[0:B_HEIGHT*B_WIDTH], C[0:A_HEIGHT*B_WIDTH])
int mmult_top(float A[A_WIDTH*A_HEIGHT], float B[B_HEIGHT*B_WIDTH], float C[C_HEIGHT*C_WIDTH],int line_count);
//int mmult_top2(float A[A_WIDTH*A_HEIGHT], float B[B_HEIGHT*B_WIDTH], float C[C_HEIGHT*C_WIDTH],int line_count);
//int mmult_top3(float A[A_WIDTH*A_HEIGHT], float B[B_HEIGHT*B_WIDTH], float C[C_HEIGHT*C_WIDTH],int line_count);
//int mmult_top4(float A[A_WIDTH*A_HEIGHT], float B[B_HEIGHT*B_WIDTH], float C[C_HEIGHT*C_WIDTH],int line_count);

#endif //__MATRIX_MULT_H__



