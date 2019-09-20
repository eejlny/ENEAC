//============================================================================
// Name			: Body.h
// Author		: Antonio Vilches
// Version		: 1.0
// Date			: 13 / 01 / 2015
// Copyright	: Department. Computer's Architecture (c)
// Description	: This file contains the Body class
//============================================================================

#ifndef _BODY_TASK_
#define _BODY_TASK_

#ifdef USEBARRIER
#include "barrier.h"
#endif

#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include <sys/time.h>
using namespace tbb;

#include <stdio.h>
#include <sds_lib.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/fs.h>
#include  "kernelspmm.h"

#define TIME_STAMP_INIT_HW  unsigned long long clock_start_hw, clock_end_hw;  clock_start_hw = sds_clock_counter();
#define TIME_STAMP_HW { clock_end_hw = sds_clock_counter(); fprintf(stderr,"SPARSE FPGA ON: execution time : %f ms\n", 1000*(clock_end_hw-clock_start_hw)/(1200*10e6)); clock_start_hw = sds_clock_counter(); }

//Im leaving debug option as a preprocessor flag, since it is not required as input in normal operation.
//This helps remove bloated code. If no debug needed just comment this out and recompile

//These are user input parameters defined in spmm-top.cpp
int numhpacc;
int numhpcacc;
int ioctl_flag;

//Variables used for timing
struct timespec start1, start2, start3, start4, finish1, finish2, finish3, finish4;
double elapsed1, elapsed2, elapsed3, elapsed4;		

/*Interrupt drivers*/
#define DRIVER_FILE_NAME_1 "/dev/intgendriver1"
int file_desc_1;
#define DRIVER_FILE_NAME_2 "/dev/intgendriver2"
int file_desc_2;
#define DRIVER_FILE_NAME_3 "/dev/intgendriver3"
int file_desc_3;
#define DRIVER_FILE_NAME_4 "/dev/intgendriver4"
int file_desc_4;

/*****************************************************************************
 * NbodyTask
 * **************************************************************************/
class Body
{
public:
	bool firsttime;
public:

	void OperatorGPU(int begin, int end, int id) {
          bodies_F+=end-begin;
          if (numhpacc > 0){
               #ifdef HP
                    (debug_flag) && (fprintf(stderr,"<HPACC> ROWS --- begin: %d; end: %d\n",begin,end));
                    switch(id)
                    {
                         //if dual mem, use accelerators with HP ports configuration
                         case 1 :
                              (debug_flag) && (fprintf(stderr,"DBG 3.1\n"));
                              kernelspmm1_hp(h_rowDelimiters, h_rowDelimiters_noncache, h_cols, h_cols_noncache, h_val, h_val_noncache, h_out, h_out_noncache_trans, h_vec_noncache_trans, numRows, nItems, x_width, file_desc_1, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 2 : 
                              (debug_flag) && (fprintf(stderr,"DBG 3.2\n"));
                              kernelspmm2_hp(h_rowDelimiters, h_rowDelimiters_noncache, h_cols, h_cols_noncache, h_val, h_val_noncache, h_out, h_out_noncache_trans, h_vec_noncache_trans, numRows, nItems, x_width, file_desc_2, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 3 : 
                              (debug_flag) && (fprintf(stderr,"DBG 3.3\n"));              
                              kernelspmm3_hp(h_rowDelimiters, h_rowDelimiters_noncache, h_cols, h_cols_noncache, h_val, h_val_noncache, h_out, h_out_noncache_trans, h_vec_noncache_trans, numRows, nItems, x_width, file_desc_3, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 4 : 
                              (debug_flag) && (fprintf(stderr,"DBG 3.4\n"));             
                              kernelspmm4_hp(h_rowDelimiters, h_rowDelimiters_noncache, h_cols, h_cols_noncache, h_val, h_val_noncache, h_out, h_out_noncache_trans, h_vec_noncache_trans, numRows, nItems, x_width, file_desc_4, ioctl_flag, debug_flag, begin, end);
                              break;
                    }
               #endif
          } else {
               #ifdef HPC
                    (debug_flag) && (fprintf(stderr,"<HPCACC> ROWS --- begin: %d; end: %d\n",begin,end));
                    switch(id - numhpacc){
                         case 1 : 
                              (debug_flag) && (fprintf(stderr,"DBG 3.1\n"));
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &start1));
                              kernelspmm1_hpc(h_rowDelimiters, h_cols, h_val, h_out, h_out_trans, h_vec_trans, numRows, nItems, x_width, file_desc_1, ioctl_flag, debug_flag, begin, end);
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &finish1));
                              (debug_flag) && (elapsed1 = (finish1.tv_sec - start1.tv_sec));
                              (debug_flag) && (elapsed1 += ((finish1.tv_nsec - start1.tv_nsec) / 1000000000.0));
                              (debug_flag) && (fprintf(stderr,"DBG 3.1.TIMING; start -> %lf s; elapsed -> %lf\n",start1.tv_sec+start1.tv_nsec/1000000000.0,elapsed1));
                              break;
                         case 2 :
                              (debug_flag) && (fprintf(stderr,"DBG 3.2\n"));
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &start2));                              
                              kernelspmm2_hpc(h_rowDelimiters, h_cols, h_val, h_out, h_out_trans, h_vec_trans, numRows, nItems, x_width, file_desc_2, ioctl_flag, debug_flag, begin, end);
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &finish2));
                              (debug_flag) && (elapsed2 = (finish2.tv_sec - start2.tv_sec));
                              (debug_flag) && (elapsed2 += ((finish2.tv_nsec - start2.tv_nsec) / 1000000000.0));
                              (debug_flag) && (fprintf(stderr,"DBG 3.2.TIMING; start -> %lf s; elapsed -> %lf\n",start2.tv_sec+start2.tv_nsec/1000000000.0,elapsed2));                              
                              break;
                         case 3 :
                              (debug_flag) && (fprintf(stderr,"DBG 3.3\n"));
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &start3));                                
                              kernelspmm3_hpc(h_rowDelimiters, h_cols, h_val, h_out, h_out_trans, h_vec_trans, numRows, nItems, x_width, file_desc_3, ioctl_flag, debug_flag, begin, end);
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &finish3));
                              (debug_flag) && (elapsed3 = (finish3.tv_sec - start3.tv_sec));
                              (debug_flag) && (elapsed3 += ((finish3.tv_nsec - start3.tv_nsec) / 1000000000.0));
                              (debug_flag) && (fprintf(stderr,"DBG 3.3.TIMING; start -> %lf s; elapsed -> %lf\n",start3.tv_sec+start3.tv_nsec/1000000000.0,elapsed3));                              
                              break;
                         case 4 : 
                              (debug_flag) && (fprintf(stderr,"DBG 3.4\n"));
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &start4));                                
                              kernelspmm4_hpc(h_rowDelimiters, h_cols, h_val, h_out, h_out_trans, h_vec_trans, numRows, nItems, x_width, file_desc_4, ioctl_flag, debug_flag, begin, end);
                              (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &finish4));
                              (debug_flag) && (elapsed4 = (finish4.tv_sec - start4.tv_sec));
                              (debug_flag) && (elapsed4 += ((finish4.tv_nsec - start4.tv_nsec) / 1000000000.0));
                              (debug_flag) && (fprintf(stderr,"DBG 3.4.TIMING; start -> %lf s; elapsed -> %lf\n",start4.tv_sec+start4.tv_nsec/1000000000.0,elapsed4));                              
                              break;
                    }
               #endif
          }
	}

	void OperatorCPU(int begin, int end) {
          (debug_flag) && (fprintf(stderr,"<CPU> ROWS --- begin: %d; end: %d\n",begin,end));
		bodies_C+=end-begin;
          struct timespec start, finish;
          double elapsed;
          (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &start));
          spmvCpu/*_vilches*/(h_val, h_cols, h_rowDelimiters, h_vec, h_out, numRows, begin, end);
          (debug_flag) && (clock_gettime(CLOCK_MONOTONIC, &finish));
          (debug_flag) && (elapsed = (finish.tv_sec - start.tv_sec));
          (debug_flag) && (elapsed += ((finish.tv_nsec - start.tv_nsec) / 1000000000.0));
          (debug_flag) && (fprintf(stderr,"DBG CPU.TIMING; start -> %lf s; elapsed -> %lf\n",start.tv_sec+start.tv_nsec/1000000000.0,elapsed));
          //spmvCpu_fpgacode(h_val, h_cols, h_rowDelimiters, h_vec, h_out, numRows, begin, end);
          // //if dual mem, then make the two memory blocks coherent if needed
          // #ifdef NONCACHE
               // for(int xw = 0; xw < x_width; xw++) {     
                    // for (int i=begin; i<end; i++) {   
                        // *(h_out_noncache+i+xw*x_width) = *(h_out+i+xw*x_width); 
                    // }
               // }
          // #endif
	}

};
//end class

#endif
