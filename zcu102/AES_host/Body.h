/*
Author: Kris Nikov - kris.nikov@bris.ac.uk
Date: 21 Aug 2019
Description: TBB Scheduler top-level header for the AES benchmark
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

#include "AES.h"
#include "kernelaes.h"

using namespace tbb;

class Body {
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
                                   kernelaes1_hp((uint8_t*)state,(uint8_t*)state_noncache,(uint8_t*)cipher,(uint8_t*)cipher_noncache,ekey_noncache, file_desc_1, ioctl_flag, debug_flag, (begin), (end)); 
                                   break;
                              case 2 : 
                                   (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.2.HP;\n"));                              
                                   kernelaes2_hp((uint8_t*)state,(uint8_t*)state_noncache,(uint8_t*)cipher,(uint8_t*)cipher_noncache,ekey_noncache, file_desc_2, ioctl_flag, debug_flag, (begin), (end)); 
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
                                   kernelaes1_hpc((uint8_t*)state,(uint8_t*)cipher,ekey, file_desc_1, ioctl_flag, debug_flag, (begin), (end)); 
                                   break;
                              case 2 : 
                                   (debug_flag) && (fprintf(stderr,"DBG SCHEDULER.2.HPC;\n"));
                                   kernelaes2_hpc((uint8_t*)state,(uint8_t*)cipher,ekey, file_desc_2, ioctl_flag, debug_flag, (begin), (end)); 
                                   break;    
                         }
                    #endif     
               }
          }
          void OperatorCPU(int begin, int end) {
               bodies_C+=end-begin;
               (debug_flag) && (fprintf(stderr,"<CPU> LINES --- begin: %d; end: %d\n",begin,end));                 
               aes_process_arm((const uint8_t*)key, (const uint8_t*)ekey, nr, (uint8_t*)&state[begin*16], (uint8_t*)&cipher[begin*16], (16*(end-begin)));
               //aes_slow(begin,end);
          }
};