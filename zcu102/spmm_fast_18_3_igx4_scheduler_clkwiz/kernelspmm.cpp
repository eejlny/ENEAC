<<<<<<< HEAD
/*
 Author: Kris Nikov - kris.nikov@bris.ac.uk
 Date: 11 Jun 2019
 Description: Source code for the SPMM hardware interface library
*/
=======
>>>>>>> pr/3
#include <stdio.h>
#include <stdlib.h>
#include <sds_lib.h>
#include <cstring>
#include <algorithm>


//Userspace interrupt support
#include <sys/ioctl.h>

//Interrupt generator drivers
#define IOCTL_WAIT_INTERRUPT_1 _IOR(100, 0, char *)
#define IOCTL_WAIT_INTERRUPT_2 _IOR(101, 0, char *)
#define IOCTL_WAIT_INTERRUPT_3 _IOR(102, 0, char *)
#define IOCTL_WAIT_INTERRUPT_4 _IOR(103, 0, char *)

//Pass data from userspace to drivers for debugging
struct ioctl_arguments {
     unsigned int spmm_workload;
};
struct ioctl_arguments args1, args2, args3, args4;

#include "spmm.h"

#ifdef HP
     void kernelspmm1_hp(
     int *rowPtr, 
     int *rowPtr_noncache, 
     int *columnIndex,
     int *columnIndex_noncache,
     DTYPE *values, 
     DTYPE *values_noncache, 
     DTYPE *y,
     DTYPE *y_noncache_trans, 
     DTYPE *x_noncache_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end) {
          (debug_flag) && (fprintf(stderr,"DBG 4.1.0; kernelspmm1_hp;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.1.1; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (u32 i=begin; i<=end; i++) 
          {
            rowPtr_noncache[i] = rowPtr[i];
              
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.1.2;\n"));

          nnz_int = rowPtr_noncache[end] - rowPtr_noncache[begin];
          rowPtr_int = rowPtr_noncache + begin;
          nnz_done = rowPtr_int[0];    
          (debug_flag) && (fprintf(stderr,"DBG 4.1.3; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (int i=0; i<nnz_int; i++) 
          {
            columnIndex_noncache[i+nnz_done] = columnIndex[i+nnz_done];
            values_noncache[i+nnz_done] = values[i+nnz_done];   
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.1.4;\n"));
          
          columnIndex_int = columnIndex_noncache + nnz_done;
          values_int = values_noncache + nnz_done;
          y_int = y_noncache_trans + begin;     
          (debug_flag) && (fprintf(stderr,"DBG 4.1.5;\n"));
          
          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.1.6;\n"));        

          #pragma SDS resource(1)
          #pragma SDS async(1)
          spmm_fast_hp0((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_noncache_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args1.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.1.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(1)
          (debug_flag) && (fprintf(stderr,"DBG 4.1.7;\n"));
          
          //After computation copy the contents of the transposed non-cacheable memory block to the sds-alloc/malloc block (final result)
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++)
               {
                    y[i*x_width+xw] = y_noncache_trans[xw*row_size+i];
               }
          }  
          (debug_flag) && (fprintf(stderr,"DBG 4.1.8;\n"));
     }

     void kernelspmm2_hp(
     int *rowPtr, 
     int *rowPtr_noncache, 
     int *columnIndex,
     int *columnIndex_noncache,
     DTYPE *values, 
     DTYPE *values_noncache, 
     DTYPE *y,
     DTYPE *y_noncache_trans, 
     DTYPE *x_noncache_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end) {
          (debug_flag) && (fprintf(stderr,"DBG 4.2.0; kernelspmm2_hp;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.2.2; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (u32 i=begin; i<=end; i++) 
          {
            rowPtr_noncache[i] = rowPtr[i];
              
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.2.2;\n"));

          nnz_int = rowPtr_noncache[end] - rowPtr_noncache[begin];
          rowPtr_int = rowPtr_noncache + begin;
          nnz_done = rowPtr_int[0];    
          (debug_flag) && (fprintf(stderr,"DBG 4.2.3; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (int i=0; i<nnz_int; i++) 
          {
            columnIndex_noncache[i+nnz_done] = columnIndex[i+nnz_done];
            values_noncache[i+nnz_done] = values[i+nnz_done];   
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.2.4;\n"));
          
          columnIndex_int = columnIndex_noncache + nnz_done;
          values_int = values_noncache + nnz_done;
          y_int = y_noncache_trans + begin;     
          (debug_flag) && (fprintf(stderr,"DBG 4.2.5;\n"));
          
          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.2.6;\n"));        

          #pragma SDS resource(2)
          #pragma SDS async(2)
          spmm_fast_hp1((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_noncache_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args2.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.2.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(2)
          (debug_flag) && (fprintf(stderr,"DBG 4.2.7;\n"));
          
          //After computation copy the contents of the transposed non-cacheable memory block to the sds-alloc/malloc block (final result)
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++) 
               {
                    y[i*x_width+xw] = y_noncache_trans[xw*row_size+i];
               }
          }  
          (debug_flag) && (fprintf(stderr,"DBG 4.2.8;\n"));
     }

     void kernelspmm3_hp(
     int *rowPtr, 
     int *rowPtr_noncache, 
     int *columnIndex,
     int *columnIndex_noncache,
     DTYPE *values, 
     DTYPE *values_noncache, 
     DTYPE *y,
     DTYPE *y_noncache_trans, 
     DTYPE *x_noncache_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end) {
          (debug_flag) && (fprintf(stderr,"DBG 4.3.0; kernelspmm3_hp;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.3.3; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (u32 i=begin; i<=end; i++) 
          {
            rowPtr_noncache[i] = rowPtr[i];
              
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.3.2;\n"));

          nnz_int = rowPtr_noncache[end] - rowPtr_noncache[begin];
          rowPtr_int = rowPtr_noncache + begin;
          nnz_done = rowPtr_int[0];    
          (debug_flag) && (fprintf(stderr,"DBG 4.3.3; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (int i=0; i<nnz_int; i++) 
          {
            columnIndex_noncache[i+nnz_done] = columnIndex[i+nnz_done];
            values_noncache[i+nnz_done] = values[i+nnz_done];   
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.3.4;\n"));
          
          columnIndex_int = columnIndex_noncache + nnz_done;
          values_int = values_noncache + nnz_done;
          y_int = y_noncache_trans + begin;     
          (debug_flag) && (fprintf(stderr,"DBG 4.3.5;\n"));
          
          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.3.6;\n"));        

          #pragma SDS resource(3)
          #pragma SDS async(3)
          spmm_fast_hp2((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_noncache_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args3.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.3.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(3)
          (debug_flag) && (fprintf(stderr,"DBG 4.3.7;\n"));
          
          //After computation copy the contents of the transposed non-cacheable memory block to the sds-alloc/malloc block (final result)
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++) 
               {
                    y[i*x_width+xw] = y_noncache_trans[xw*row_size+i];
               }
          }  
          (debug_flag) && (fprintf(stderr,"DBG 4.3.8;\n"));
     }

     void kernelspmm4_hp(
     int *rowPtr, 
     int *rowPtr_noncache, 
     int *columnIndex,
     int *columnIndex_noncache,
     DTYPE *values, 
     DTYPE *values_noncache, 
     DTYPE *y,
     DTYPE *y_noncache_trans, 
     DTYPE *x_noncache_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end) {
          (debug_flag) && (fprintf(stderr,"DBG 4.4.0; kernelspmm4_hp;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.4.4; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (u32 i=begin; i<=end; i++) 
          {
            rowPtr_noncache[i] = rowPtr[i];
              
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.4.2;\n"));

          nnz_int = rowPtr_noncache[end] - rowPtr_noncache[begin];
          rowPtr_int = rowPtr_noncache + begin;
          nnz_done = rowPtr_int[0];    
          (debug_flag) && (fprintf(stderr,"DBG 4.4.3; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          //Use loop to copy the data we need into the noncacheable memory buffers
          for (int i=0; i<nnz_int; i++) 
          {
            columnIndex_noncache[i+nnz_done] = columnIndex[i+nnz_done];
            values_noncache[i+nnz_done] = values[i+nnz_done];   
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.4.4;\n"));
          
          columnIndex_int = columnIndex_noncache + nnz_done;
          values_int = values_noncache + nnz_done;
          y_int = y_noncache_trans + begin;     
          (debug_flag) && (fprintf(stderr,"DBG 4.4.5;\n"));
          
          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.4.6;\n"));        

          #pragma SDS resource(4)
          #pragma SDS async(4)
          spmm_fast_hp3((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_noncache_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args4.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.4.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(4)
          (debug_flag) && (fprintf(stderr,"DBG 4.4.7;\n"));
          
          //After computation copy the contents of the transposed non-cacheable memory block to the sds-alloc/malloc block (final result)
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++) 
               {
                    y[i*x_width+xw] = y_noncache_trans[xw*row_size+i];
               }
          }  
          (debug_flag) && (fprintf(stderr,"DBG 4.4.8;\n"));
     }
#endif     

#ifdef HPC
     int kernelspmm1_hpc(
     int *rowPtr, 
     int *columnIndex,
     DTYPE *values, 
     DTYPE *y,
     DTYPE *y_trans,
     DTYPE *x_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end)
     {
          (debug_flag) && (fprintf(stderr,"DBG 4.1.0; kernelspmm1_hpc;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.1.1; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));

          nnz_int = rowPtr[end] - rowPtr[begin];
          rowPtr_int = rowPtr + begin;
          nnz_done = rowPtr_int[0];
          (debug_flag) && (fprintf(stderr,"DBG 4.1.2; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          columnIndex_int = columnIndex + nnz_done;
          values_int = values + nnz_done;
          y_int = y_trans + begin;     
          (debug_flag) && (fprintf(stderr,"DBG 4.1.3;\n"));

          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.1.4;\n"));        

          #pragma SDS resource(1)
          #pragma SDS async(1)
          spmm_fast_hpc0((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args1.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.1.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(1)
          (debug_flag) && (fprintf(stderr,"DBG 4.1.5;\n"));
          
          //Copy the transposed output matrix back into the normal output
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++) 
               {
                    y[i*x_width+xw] = y_trans[xw*row_size+i];
               }
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.1.6;\n"));     
     }

     int kernelspmm2_hpc(
     int *rowPtr, 
     int *columnIndex,
     DTYPE *values, 
     DTYPE *y,
     DTYPE *y_trans,
     DTYPE *x_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end)
     {
          (debug_flag) && (fprintf(stderr,"DBG 4.2.0; kernelspmm2_hpc;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.2.1; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));

          nnz_int = rowPtr[end] - rowPtr[begin];
          rowPtr_int = rowPtr + begin;
          nnz_done = rowPtr_int[0];
          (debug_flag) && (fprintf(stderr,"DBG 4.2.2; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          columnIndex_int = columnIndex + nnz_done;
          values_int = values + nnz_done;     
          y_int = y_trans + begin;
          (debug_flag) && (fprintf(stderr,"DBG 4.2.3;\n"));
          
          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.2.4;\n"));     

          #pragma SDS resource(2)
          #pragma SDS async(2)
          spmm_fast_hpc1((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args2.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.2.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(2)
          (debug_flag) && (fprintf(stderr,"DBG 4.2.5;\n"));
          
          //Copy the transposed output matrix back into the normal output
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++) 
               {
                    y[i*x_width+xw] = y_trans[xw*row_size+i];
               }
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.2.6;\n"));      
     }

     int kernelspmm3_hpc(
     int *rowPtr, 
     int *columnIndex,
     DTYPE *values, 
     DTYPE *y,
     DTYPE *y_trans,
     DTYPE *x_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end)
     {
          (debug_flag) && (fprintf(stderr,"DBG 4.3.0; kernelspmm3_hpc;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.3.1; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));

          nnz_int = rowPtr[end] - rowPtr[begin];
          rowPtr_int = rowPtr + begin;
          nnz_done = rowPtr_int[0];
          (debug_flag) && (fprintf(stderr,"DBG 4.3.2; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          columnIndex_int = columnIndex + nnz_done;
          values_int = values + nnz_done;     
          y_int = y_trans + begin;
          (debug_flag) && (fprintf(stderr,"DBG 4.3.3;\n"));

          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.3.4;\n"));     

          #pragma SDS resource(3)
          #pragma SDS async(3)
          spmm_fast_hpc2((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args3.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.3.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(3)
          (debug_flag) && (fprintf(stderr,"DBG 4.3.5;\n"));
          
          //Copy the transposed output matrix back into the normal output
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++) 
               {
                    y[i*x_width+xw] = y_trans[xw*row_size+i];
               }
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.3.6;\n")); 
     }

     int kernelspmm4_hpc(
     int *rowPtr, 
     int *columnIndex,
     DTYPE *values, 
     DTYPE *y,
     DTYPE *y_trans,
     DTYPE *x_trans, 
     int row_size, 
     int nnz,
     int x_width,
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     unsigned int begin,
     unsigned int end)
     {
          (debug_flag) && (fprintf(stderr,"DBG 4.4.0; kernelspmm4_hpc;\n"));
          //Internal variables for the accelerator code - this minimises data movement between the CPU and the accelerator (we only move/access the data that the accelerator operates on)
          int ret_value;
          int line_count = end-begin;
          int nnz_int, nnz_done;
          int *rowPtr_int;
          int *columnIndex_int;
          DTYPE *values_int;
          DTYPE *y_int;
          (debug_flag) && (fprintf(stderr,"DBG 4.4.1; line_count -> %d; row_size -> %d; nnz -> %d; begin ->%u; end -> %u\n", line_count, row_size, nnz, begin, end));

          nnz_int = rowPtr[end] - rowPtr[begin];
          rowPtr_int = rowPtr + begin;
          nnz_done = rowPtr_int[0];
          (debug_flag) && (fprintf(stderr,"DBG 4.4.2; nnz_int -> %d; nnz_done -> %d\n", nnz_int, nnz_done));
          
          columnIndex_int = columnIndex + nnz_done;
          values_int = values + nnz_done;     
          y_int = y_trans + begin;
          (debug_flag) && (fprintf(stderr,"DBG 4.4.3;\n"));

          //Detect if buffer sizes are being exceeded
          if ((row_size > COL_SIZE_MAX) || (line_count > ROW_SIZE_MAX))
          {
               fprintf(stderr,"Error: FPGA buffer memory overflow;\n");
               exit(1);
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.4.4;\n"));     

          #pragma SDS resource(4)
          #pragma SDS async(4)
          spmm_fast_hpc3((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x_trans, (u32)row_size, (u32)line_count, (u32)nnz_int, (u32)x_width);
          //Control interrupt calls using IOCTL flag
          if (ioctl_flag) {
               args4.spmm_workload = line_count;
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass row size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG 4.4.IOCTL; ret_value -> %d\n",ret_value));
          }     
          #pragma SDS wait(4)
          (debug_flag) && (fprintf(stderr,"DBG 4.4.5;\n"));
          
          //Copy the transposed output matrix back into the normal output
          for(int xw = 0; xw < x_width; xw++)
          {
               for (u32 i = begin; i < end; i++) 
               {
                    y[i*x_width+xw] = y_trans[xw*row_size+i];
               }
          }
          (debug_flag) && (fprintf(stderr,"DBG 4.4.6;\n")); 
     }
#endif