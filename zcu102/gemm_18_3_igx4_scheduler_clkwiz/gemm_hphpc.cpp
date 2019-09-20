/* File: gemm_hphpc.cpp
Author: Kris Nikov - kris.nikov@bris.ac.uk
Date: 18 Jul 2019
Description: GEMM accelerator code for the ultrascale+ platform
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gemm.h"

inline void mxv(
     float *A, 
     float B[mat_buffer_dim][B_WIDTH_BLOCK], 
     float *C,
     int mat_dim_fpga) {
          for (int j = 0; j < C_WIDTH_BLOCK; j++)
               #pragma HLS UNROLL
               C[j] = 0;
               
          for(int k = 0; k < mat_dim_fpga; k+=1)
               #pragma HLS PIPELINE
               for (int j = 0; j < B_WIDTH_BLOCK; j++)
                    #pragma HLS UNROLL factor=B_WIDTH_BLOCK
                    C[j] += A[k]*B[k][j];
     }

void gemm_accel(
     float A[A_HEIGHT_BLOCK*mat_buffer_dim],
     float B[mat_buffer_dim][B_WIDTH_BLOCK],
     float C[C_HEIGHT_BLOCK*C_WIDTH_BLOCK],
     int mat_dim_fpga) {
          for (int p = 0; p < A_HEIGHT_BLOCK; p+=STEP)
               #pragma HLS UNROLL factor=A_HEIGHT_BLOCK
               mxv(A+p*mat_dim_fpga, B, C+p*C_WIDTH_BLOCK, mat_dim_fpga);
     }

#ifdef HP
     #pragma SDS data sys_port(A:HP0)
     #pragma SDS data sys_port(B:HP0)
     #pragma SDS data sys_port(C:HP0)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm1_hp(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete
               
               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }

     #pragma SDS data sys_port(A:HP1)
     #pragma SDS data sys_port(B:HP1)
     #pragma SDS data sys_port(C:HP1)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm2_hp(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete
               
               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }

     #pragma SDS data sys_port(A:HP2)
     #pragma SDS data sys_port(B:HP2)
     #pragma SDS data sys_port(C:HP2)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm3_hp(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete
               
               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }

     #pragma SDS data sys_port(A:HP3)
     #pragma SDS data sys_port(B:HP3)
     #pragma SDS data sys_port(C:HP3)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm4_hp(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete

               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }
#endif

#ifdef HPC
     #pragma SDS data sys_port(A:HPC0)
     #pragma SDS data sys_port(B:HPC0)
     #pragma SDS data sys_port(C:HPC0)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm1_hpc(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete

               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }

     #pragma SDS data sys_port(A:HPC1)
     #pragma SDS data sys_port(B:HPC1)
     #pragma SDS data sys_port(C:HPC1)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm2_hpc(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete

               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }

     #pragma SDS data sys_port(A:HPC0)
     #pragma SDS data sys_port(B:HPC0)
     #pragma SDS data sys_port(C:HPC0)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm3_hpc(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete

               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }

     #pragma SDS data sys_port(A:HPC1)
     #pragma SDS data sys_port(B:HPC1)
     #pragma SDS data sys_port(C:HPC1)

     #pragma SDS data zero_copy(A[0:(line_count*mat_dim_fpga)])
     #pragma SDS data zero_copy(B[0:(mat_dim_fpga*mat_dim_fpga)])
     #pragma SDS data zero_copy(C[0:(line_count*mat_dim_fpga)])
     void gemm4_hpc(
          float* A,
          float* B,
          float* C,    
          int mat_dim_fpga,
          int line_count) {
               float A_accel[A_HEIGHT_BLOCK*mat_buffer_dim], B_accel[mat_buffer_dim][B_WIDTH_BLOCK], C_accel[C_HEIGHT_BLOCK*C_WIDTH_BLOCK];
               #pragma HLS array_partition variable=A_accel block factor=B_WIDTH_BLOCK dim=1
               #pragma HLS array_partition variable=B_accel block factor=B_WIDTH_BLOCK dim=2
               #pragma HLS array_partition variable=C_accel complete

               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {
                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i][j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];
                              
                    for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++) {
                         for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                              for (int j = 0; j < mat_dim_fpga; j++)
                                   A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];
                              
                         gemm_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                         for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                              #pragma HLS UNROLL factor=C_HEIGHT_BLOCK
                              for (int j = 0; j < C_WIDTH_BLOCK; j++)
                                   #pragma HLS UNROLL factor=C_WIDTH_BLOCK
                                   C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
                    }
               }
          }
#endif