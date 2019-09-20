#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

#include "kernelgemm.h"

/* performance timing and energy monitoring macros */
#define TIME_STAMP_INIT_HW  unsigned long long clock_start_hw, clock_end_hw;  clock_start_hw = sds_clock_counter();
#define TIME_STAMP_HW { clock_end_hw = sds_clock_counter(); fprintf(stderr,"SPARSE FPGA ON: execution time : %f ms\n", 1000*(clock_end_hw-clock_start_hw)/(1200*10e6)); clock_start_hw = sds_clock_counter(); }

using namespace std;

/*****************************************************************************
* Global variables
* **************************************************************************/
const int BLOCK=1024;
const int BLOCK_I=1;
const unsigned AOCL_ALIGNMENT = 64;

/*
 A_HEIGHT_BLOCK  is for software part data partitioning due to the limitation in 
 the Xilinx kernel sds_alloc so A_HEIGHT_BLOCK should be mat_dim_fpga divided by 
 the number of considered blocks
*/
#define A_HEIGHT_BLOCK  1
#define B_WIDTH_BLOCK   32
#define C_HEIGHT_BLOCK  A_HEIGHT_BLOCK 
#define C_WIDTH_BLOCK   B_WIDTH_BLOCK //B_WIDTH_BLOCK shoudl be less than P
const int STEP=1;
const int mat_buffer_dim = 1024;

int mat_dim; //mat_dim is a user input and should be power of 2, and larger than 8
float *array_a, *array_b, *array_c;
float *array_a_noncache, *array_b_noncache, *array_c_noncache;
float *c_golden;

//These are user input parameters defined in hotspot-top.cpp
int numhpacc;
int numhpcacc;
int ioctl_flag;
long unsigned int bodies_C=0, bodies_F=0;
const int calc_golden = 0;

#ifdef HWDEBUG
     int debug_flag = 1;
#else
     int debug_flag;
#endif

/*Interrupt drivers*/
#define DRIVER_FILE_NAME_1 "/dev/intgendriver1"
int file_desc_1;
#define DRIVER_FILE_NAME_2 "/dev/intgendriver2"
int file_desc_2;
#define DRIVER_FILE_NAME_3 "/dev/intgendriver3"
int file_desc_3;
#define DRIVER_FILE_NAME_4 "/dev/intgendriver4"
int file_desc_4;

//Variables used for timing
struct timespec start1, start2, start3, start4, finish1, finish2, finish3, finish4;
double elapsed1, elapsed2, elapsed3, elapsed4;

typedef unsigned long u32;

/*****************************************************************************
 * Functions 
 * **************************************************************************/

void usage(
     int argc, 
     char **argv) {
          std::cout << "Usage: <mat_dim> <numcpus> <numhpacc> <numhpcacc> <chunkACC> <IOCTL_flag> <output_file>" << endl;
          std::cout << "\t<mat_dim> - number of mat_dim/cols for the square matrices (positive integer, multiple of 2 and greater than 8)" << endl;          
          std::cout << "\t<numcpus> - number of CPU cores (integer: 0-4)" << endl;
          std::cout << "\t<numhpacc> - number of HP accelerators (must be preloaded in fpga)" << endl;
          std::cout << "\t<numhpcacc> - number of HPC accelerators (must be preloaded in fpga)" << endl;
          std::cout << "\t<chunkACC> - (Fixed or Dynamic) (0 or higher integer)" << endl;
          std::cout << "\t<IOCTL flag> - enable hardware interrupts (0-No or 1-Yes)" << endl;     
          std::cout << "\t<output file> - write output to file (optional as final input variable)" << endl; 
          exit(0);
     };

void gemmCPU(
     int begin, 
     int end){
          float *c_p, *b_p, *a_p;

          for (int i_m = begin; i_m < end; i_m += BLOCK_I) {
               for (int j_m = 0; j_m < mat_dim; j_m += BLOCK) {
                    for (int k_m = 0; k_m < mat_dim; k_m += BLOCK) {
                         c_p = &array_c[i_m*mat_dim+j_m];
                         a_p = &array_a[i_m*mat_dim+k_m];
                         for (int i_block = 0; i_block < BLOCK_I; i_block++ ) {
                              b_p = &array_b[k_m*mat_dim+j_m];
                              for (int j_block = 0; j_block < BLOCK; j_block++) {
                                   for (int k_block = 0; k_block < BLOCK; k_block++) {
                                       c_p[k_block] += a_p[j_block] * b_p[k_block];
                                   }
                                   b_p += mat_dim;
                              }
                              c_p += mat_dim;
                              a_p += mat_dim;
                         }
                    }
               }
          }
     };
     
void *alignedMalloc(
     size_t size) {
          void *result = NULL;
          posix_memalign (&result, AOCL_ALIGNMENT, size);
          return result;
     };
     
void golden_MMM(
     float *a, 
     float *b, 
     float *c,
     int begin,
     int end) {
       int i,j,k;

       for(i=begin;i<end;i++)
          for(j=0;j<mat_dim;j++)
               for(k=0;k<mat_dim;k++)
                    c[i*mat_dim+j] += a[i*mat_dim+k]*b[k*mat_dim+j];
     };     

void writeoutput(
     char *outfile ) {
          ofstream out(outfile);
          for(int i=0;i<mat_dim;i++) {
               for(int j=0;j<mat_dim;j++) {
                    out << array_c[i*mat_dim+j] << "\n";
               }
          }
          out.close();
     };
  
void freemem(){
     sds_free(array_a);
     sds_free(array_b);
     sds_free(array_c);
     if (numhpacc > 0){     
          sds_free(array_a_noncache);
          sds_free(array_b_noncache);
          sds_free(array_c_noncache);
     }     
};

void mxv(
     float *A, 
     float* B, 
     float* C,
     int mat_dim_fpga) {
          for (int j = 0; j < C_WIDTH_BLOCK; j++)
               C[j] = 0;
               
          for(int k = 0; k < mat_dim_fpga; k+=1)
               for (int j = 0; j < B_WIDTH_BLOCK; j++)
                    C[j] += A[k]*B[k*B_WIDTH_BLOCK+j];
     };

void gemmFPGA_accel(
     float *A,
     float *B,
     float *C,
     int mat_dim_fpga) {
          for (int p = 0; p < A_HEIGHT_BLOCK; p+=STEP)
               mxv(A+p*mat_dim_fpga, B, C+p*C_WIDTH_BLOCK, mat_dim_fpga);
     };

void gemmFPGA_hpc(
     float *A,
     float *B,
     float *C,
     int mat_dim_fpga,
     int line_count) {
          float A_accel[A_HEIGHT_BLOCK * mat_buffer_dim];
          float B_accel[mat_buffer_dim * B_WIDTH_BLOCK];
          float C_accel[C_HEIGHT_BLOCK * C_WIDTH_BLOCK];

          for (int A_index = 0; A_index < line_count/A_HEIGHT_BLOCK; A_index++)
               for (int B_index = 0; B_index < mat_dim_fpga/B_WIDTH_BLOCK; B_index++) {

                    for (int i = 0; i < A_HEIGHT_BLOCK; i++)
                         for (int j = 0; j < mat_dim_fpga; j++)
                              A_accel[i*mat_dim_fpga+j] = A[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j];

                    for (int i = 0; i < mat_dim_fpga; i++)
                         for (int j = 0; j < B_WIDTH_BLOCK; j++)
                              B_accel[i*B_WIDTH_BLOCK+j] = B[i*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK];

                    gemmFPGA_accel(A_accel, B_accel, C_accel, mat_dim_fpga);

                    for (int i = 0; i < C_HEIGHT_BLOCK; i++)
                         for (int j = 0; j < C_WIDTH_BLOCK; j++)
                              C[(i+A_index*A_HEIGHT_BLOCK)*mat_dim_fpga+j+B_index*B_WIDTH_BLOCK] = C_accel[i*C_WIDTH_BLOCK+j];
               }
     };
     

void gemmFPGA_kernel(
     float *array_a_fpga,
     float *array_b_fpga,
     float *array_c_fpga,
     int mat_dim_fpga,
     int begin, 
     int end) {
          int line_count = end-begin;
          float *array_temp_a = array_a_fpga + begin*mat_dim_fpga;
          float *array_temp_b = array_b_fpga;
          float *array_temp_c = array_c_fpga + begin*mat_dim_fpga;
          gemmFPGA_hpc(array_temp_a, array_temp_b, array_temp_c, mat_dim_fpga, line_count); 
     };     