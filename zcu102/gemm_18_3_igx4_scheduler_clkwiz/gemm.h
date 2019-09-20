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

const int A_HEIGHT_BLOCK=1;
const int B_WIDTH_BLOCK=32;
const int C_HEIGHT_BLOCK=A_HEIGHT_BLOCK; 
const int C_WIDTH_BLOCK=B_WIDTH_BLOCK;
const int STEP=1;
const int mat_buffer_dim = 1024;

inline void mxv(
     float *A, 
     float B[mat_buffer_dim][B_WIDTH_BLOCK], 
     float *C,
     int mat_dim_fpga);

void gemm_accel(
     float A[A_HEIGHT_BLOCK*mat_buffer_dim],
     float B[mat_buffer_dim][B_WIDTH_BLOCK],
     float C[C_HEIGHT_BLOCK*C_WIDTH_BLOCK],
     int mat_dim_fpga);

#ifdef HP
     void gemm1_hp(
          float *A,
          float *B,
          float *C,     
          int mat_dim_fpga,
          int line_count);
     void gemm2_hp(
          float *A,
          float *B,
          float *C,                
          int mat_dim_fpga,
          int line_count);
     void gemm3_hp(
          float *A,
          float *B,
          float *C,             
          int mat_dim_fpga,
          int line_count);
     void gemm4_hp(
          float *A,
          float *B,
          float *C,           
          int mat_dim_fpga,
          int line_count);
#endif

#ifdef HPC
     void gemm1_hpc(
          float *A,
          float *B,
          float *C,            
          int mat_dim_fpga,
          int line_count);
     void gemm2_hpc(
          float *A,
          float *B,
          float *C,               
          int mat_dim_fpga,
          int line_count);
     void gemm3_hpc(
          float *A,
          float *B,
          float *C,            
          int mat_dim_fpga,
          int line_count);
     void gemm4_hpc(
          float *A,
          float *B,
          float *C,             
          int mat_dim_fpga,
          int line_count);
#endif