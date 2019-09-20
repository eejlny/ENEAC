//============================================================================
// Name			: Body.h
// Author		: Antonio Vilches
// Version		: 1.0
// Date			: 13 / 01 / 2015
// Copyright	: Department. Computer's Architecture (c)
// Description	: This file contains the Body class
//============================================================================


#ifdef USEBARRIER
#include "barrier.h"
#endif

#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
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

#include "kernelgemm.h"
#include "GEMM.h"

using namespace tbb;


/*****************************************************************************
 * GEMM Task
 * **************************************************************************/
class Body
{
public:
	bool firsttime;
public:

	void OperatorGPU(int begin, int end, int id) {
		bodies_F+=end-begin;
          if (numhpacc > 0) {
               #ifdef HP
                    //Use accelerators with HP ports configuration
                    (debug_flag) && (fprintf(stderr,"<HPACC> LINES --- begin: %d; end: %d\n",begin, end));
                    switch(id) {
                         case 1 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.1.HP;\n"));
                              kernelgemm1_hp((float *)array_a, (float *)array_a_noncache, (float *)array_b, (float *)array_b_noncache, (float *)array_c, (float *)array_c_noncache, mat_dim, file_desc_1, ioctl_flag, debug_flag, begin, end); 
                              break;
                         case 2 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.2.HP;\n"));
                              kernelgemm2_hp((float *)array_a, (float *)array_a_noncache, (float *)array_b, (float *)array_b_noncache, (float *)array_c, (float *)array_c_noncache, mat_dim, file_desc_2, ioctl_flag, debug_flag, begin, end); 
                              break;
                         case 3 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.3.HP;\n"));
                              kernelgemm3_hp((float *)array_a, (float *)array_a_noncache, (float *)array_b, (float *)array_b_noncache, (float *)array_c, (float *)array_c_noncache, mat_dim, file_desc_3, ioctl_flag, debug_flag, begin, end); 
                              break;
                         case 4 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.4.HP;\n"));
                              kernelgemm4_hp((float *)array_a, (float *)array_a_noncache, (float *)array_b, (float *)array_b_noncache, (float *)array_c, (float *)array_c_noncache, mat_dim, file_desc_4, ioctl_flag, debug_flag, begin, end); 
                              break;
                    }
               #endif
          } else {
               #ifdef HPC
                    //Use accelerators with HPC ports configuration               
                    (debug_flag) && (fprintf(stderr,"<HPCACC> LINES --- begin: %d; end: %d\n",begin,end));
                    switch(id - numhpacc) {
                         case 1 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.1.HPC;\n"));
                              kernelgemm1_hpc((float *)array_a, (float *)array_b, (float *)array_c, mat_dim, file_desc_1, ioctl_flag, debug_flag, begin, end); 
                              break;
                         case 2 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.2.HPC;\n"));
                              kernelgemm2_hpc((float *)array_a, (float *)array_b, (float *)array_c, mat_dim, file_desc_2, ioctl_flag, debug_flag, begin, end); 
                              break;
                         case 3 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.3.HPC;\n"));
                              kernelgemm3_hpc((float *)array_a, (float *)array_b, (float *)array_c, mat_dim, file_desc_3, ioctl_flag, debug_flag, begin, end); 
                              break;
                         case 4 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.4.HPC;\n"));
                              kernelgemm4_hpc((float *)array_a, (float *)array_b, (float *)array_c, mat_dim, file_desc_4, ioctl_flag, debug_flag, begin, end); 
                              break;
                    }
               #endif
          }
	}

	void OperatorCPU(int begin, int end) {
		bodies_C+=end-begin;
          (debug_flag) && (fprintf(stderr,"<CPU> LINES --- begin: %d; end: %d\n",begin,end));
          gemmCPU(begin,end);
          //gemmFPGA_kernel((float *)array_a, (float *)array_b, (float *)array_c, mat_dim, begin, end);
          //golden_MMM((float *)array_a, (float *)array_b, (float *)array_c, begin, end);
          
	}

};
