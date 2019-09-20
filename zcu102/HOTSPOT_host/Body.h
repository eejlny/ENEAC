/*
Author: Kris Nikov - kris.nikov@bris.ac.uk
Date: 15 Jul 2019
Description: TBB Scheduler top-level header for the HOTSPOT benchmark
*/
#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include <sys/time.h>
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

#include "HOTSPOT.h"
#include "kernelhotspot.h"

using namespace tbb;

/*****************************************************************************
 * Hotspot task
 * **************************************************************************/
class Body
{
public:
	bool firsttime;
public:
	void OperatorGPU(int begin, int end, int id ) {
          float Rx_1=1.f/Rx;
          float Ry_1=1.f/Ry;
          float Rz_1=1.f/Rz;
          float Cap_1 = step/Cap;
          bodies_F+=end-begin;
          if (numhpacc > 0) {
               #ifdef HP
                    //Use accelerators with HP ports configuration
                    (debug_flag) && (fprintf(stderr,"<HPACC> LINES --- begin: %d; end: %d\n",begin, end));
                    switch(id) {
                         case 1 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.1\n"));
                              kernelhotspot1_hp((float*)array_temp,(float*)array_temp_noncache,(float*)array_power,(float*)array_power_noncache,(float*)array_out,(float*)array_out_noncache,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_1, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 2 :
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.2\n"));
                              kernelhotspot2_hp((float*)array_temp,(float*)array_temp_noncache,(float*)array_power,(float*)array_power_noncache,(float*)array_out,(float*)array_out_noncache,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_2, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 3 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.3\n"));                              
                              kernelhotspot3_hp((float*)array_temp,(float*)array_temp_noncache,(float*)array_power,(float*)array_power_noncache,(float*)array_out,(float*)array_out_noncache,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_3, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 4 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.4\n"));                              
                              kernelhotspot4_hp((float*)array_temp,(float*)array_temp_noncache,(float*)array_power,(float*)array_power_noncache,(float*)array_out,(float*)array_out_noncache,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_4, ioctl_flag, debug_flag, begin, end);
                              break;
                    }
               #endif
          } else {
               #ifdef HPC
                    //Use accelerators with HPC ports configuration               
                    (debug_flag) && (fprintf(stderr,"<HPCACC> LINES --- begin: %d; end: %d\n",begin,end));
                    switch(id - numhpacc) {
                         case 1 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.1\n"));
                              kernelhotspot1_hpc((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_1, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 2 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.2\n"));                         
                              kernelhotspot2_hpc((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_2, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 3 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.3\n"));                                                       
                              kernelhotspot3_hpc((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_3, ioctl_flag, debug_flag, begin, end);
                              break;
                         case 4 : 
                              (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.4\n"));                                                       
                              kernelhotspot4_hpc((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, frame_width, file_desc_4, ioctl_flag, debug_flag, begin, end);
                              break;
                    }
               #endif
          }
     }

	void OperatorCPU(int begin, int end) {
          bodies_C+=end-begin;
          (debug_flag) && (fprintf(stderr,"<CPU> LINES --- begin: %d; end: %d\n",begin,end));
          hotcpotCPU(begin,end);
	}
};