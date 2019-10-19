#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sds_lib.h>
#include <cstring>
//#include <sys/time.h>

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

#include "aes.h"

#ifdef HP

     void kernelaes1_hp(
     uint8_t *state,
     uint8_t *state_noncache,
     uint8_t *cipher,
     uint8_t *cipher_noncache,
     uint8_t ekey_noncache[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INIT; kernelaes1_hp;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers     
          for (int i=0; i<block_size; i++)
              *(state_noncache + begin*16 + i) = *(state + begin*16 + i);
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.HPBUFFERFILL;\n"));
               
          uint8_t *state_temp = state_noncache + begin*16;
          uint8_t *cipher_temp = cipher_noncache + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey_noncache address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey_noncache));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.BEGINCOMPUTE;\n"));
          #pragma SDS resource(1)
          #pragma SDS async(1)
          aes1_hp((data_t *)state_temp, (data_t *)cipher_temp, ekey_noncache, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;
               args1.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }
          #pragma SDS wait(1)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.ENDCOMPUTE;\n"));

          //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
          for (int i=0; i<block_size; i++)
              *(cipher + begin*16 + i) = *(cipher_noncache + begin*16 + i);
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.HPBUFFERCOPYBACK;\n")); 
     }

     void kernelaes2_hp(
     uint8_t *state,
     uint8_t *state_noncache,
     uint8_t *cipher,
     uint8_t *cipher_noncache,
     uint8_t ekey_noncache[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INIT; kernelaes2_hp;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers     
          for (int i=0; i<block_size; i++)
               *(state_noncache + begin*16 + i) = *(state + begin*16 + i);  
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.HPBUFFERFILL;\n"));
               
          uint8_t *state_temp = state_noncache + begin*16;
          uint8_t *cipher_temp = cipher_noncache + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey_noncache address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey_noncache));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.BEGINCOMPUTE;\n"));
          #pragma SDS resource(2)
          #pragma SDS async(2)
          aes2_hp((data_t *)state_temp, (data_t *)cipher_temp, ekey_noncache, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;
               args2.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }
          #pragma SDS wait(2)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.ENDCOMPUTE;\n"));

          //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
          for (int i=0; i<block_size; i++)
               *(cipher + begin*16 + i) = *(cipher_noncache + begin*16 + i);
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.HPBUFFERCOPYBACK;\n")); 
     }
     
     void kernelaes3_hp(
     uint8_t *state,
     uint8_t *state_noncache,
     uint8_t *cipher,
     uint8_t *cipher_noncache,
     uint8_t ekey_noncache[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INIT; kernelaes3_hp;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers     
          for (int i=0; i<block_size; i++)
               *(state_noncache + begin*16 + i) = *(state + begin*16 + i);  
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.HPBUFFERFILL;\n"));
               
          uint8_t *state_temp = state_noncache + begin*16;
          uint8_t *cipher_temp = cipher_noncache + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey_noncache address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey_noncache));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.BEGINCOMPUTE;\n"));
          #pragma SDS resource(3)
          #pragma SDS async(3)
          aes3_hp((data_t *)state_temp, (data_t *)cipher_temp, ekey_noncache, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;
               args3.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }
          #pragma SDS wait(3)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.ENDCOMPUTE;\n"));

          //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
          for (int i=0; i<block_size; i++)
               *(cipher + begin*16 + i) = *(cipher_noncache + begin*16 + i);
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.HPBUFFERCOPYBACK;\n")); 
     }
     
     void kernelaes4_hp(
     uint8_t *state,
     uint8_t *state_noncache,
     uint8_t *cipher,
     uint8_t *cipher_noncache,
     uint8_t ekey_noncache[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INIT; kernelaes4_hp;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          
          //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
          //Use loop to copy the data we need into the noncacheable memory buffers     
          for (int i=0; i<block_size; i++)
               *(state_noncache + begin*16 + i) = *(state + begin*16 + i);  
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.HPBUFFERFILL;\n"));
               
          uint8_t *state_temp = state_noncache + begin*16;
          uint8_t *cipher_temp = cipher_noncache + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey_noncache address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey_noncache));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.BEGINCOMPUTE;\n"));
          #pragma SDS resource(4)
          #pragma SDS async(4)
          aes4_hp((data_t *)state_temp, (data_t *)cipher_temp, ekey_noncache, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;
               args4.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }
          #pragma SDS wait(4)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.ENDCOMPUTE;\n"));

          //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
          for (int i=0; i<block_size; i++)
               *(cipher + begin*16 + i) = *(cipher_noncache + begin*16 + i);
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.HPBUFFERCOPYBACK;\n")); 
     }

#endif

#ifdef HPC

     void kernelaes1_hpc(
     uint8_t *state,
     uint8_t *cipher,
     uint8_t ekey[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INIT; kernelaes1_hpc;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          uint8_t *state_temp = state + begin*16;
          uint8_t *cipher_temp = cipher + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.BEGINCOMPUTE;\n"));
          #pragma SDS resource(1)
          #pragma SDS async(1)
          aes1_hpc((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;
               args1.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }     
          #pragma SDS wait(1)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.ENDCOMPUTE;\n"));
     }

     void kernelaes2_hpc(
     uint8_t *state,
     uint8_t *cipher,
     uint8_t ekey[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INIT; kernelaes2_hpc;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          uint8_t *state_temp = state + begin*16;
          uint8_t *cipher_temp = cipher + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.BEGINCOMPUTE;\n"));
          #pragma SDS resource(2)
          #pragma SDS async(2)
          aes2_hpc((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;     
               args2.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.beforesleep; file_desc -> %d\n",file_desc));               
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }     
          #pragma SDS wait(2)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.ENDCOMPUTE;\n"));
     }
     
     void kernelaes3_hpc(
     uint8_t *state,
     uint8_t *cipher,
     uint8_t ekey[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INIT; kernelaes1_hpc;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          uint8_t *state_temp = state + begin*16;
          uint8_t *cipher_temp = cipher + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.BEGINCOMPUTE;\n"));
          #pragma SDS resource(3)
          #pragma SDS async(3)
          aes3_hpc((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;     
               args3.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.beforesleep; file_desc -> %d\n",file_desc));               
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }     
          #pragma SDS wait(3)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.ENDCOMPUTE;\n"));
     }
     
     void kernelaes4_hpc(
     uint8_t *state,
     uint8_t *cipher,
     uint8_t ekey[240],
     int file_desc,
     int ioctl_flag,
     int debug_flag,
     int begin,
     int end) {
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INIT; kernelaes1_hpc;\n"));
          int block_size = 16*(end-begin); //block_size in bytes
          uint8_t *state_temp = state + begin*16;
          uint8_t *cipher_temp = cipher + begin*16;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INPUTS; block_size -> %d; state_temp address -> %p; cipher_temp address -> %p; ekey address -> %p;\n",block_size,(void *)(state_temp),(void *)(cipher_temp),&ekey));
          
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.BEGINCOMPUTE;\n"));
          #pragma SDS resource(4)
          #pragma SDS async(4)
          aes4_hpc((data_t *)state_temp, (data_t *)cipher_temp, ekey, block_size);
          //Control interrupt calls using IOCTL flag     
          if (ioctl_flag) {
               int ret_value;     
               args4.spmm_workload = block_size;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.beforesleep; file_desc -> %d\n",file_desc));               
               ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass block size to driver for debugging
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
          }     
          #pragma SDS wait(4)
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.ENDCOMPUTE;\n"));
     }

#endif