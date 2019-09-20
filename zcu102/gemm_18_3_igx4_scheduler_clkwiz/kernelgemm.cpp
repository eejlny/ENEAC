// Author: Kris Nikov - kris.nikov@bris.ac.uk
// Date: 15 Jul 2019
// Description - Source file for the software interface library to the FPGA accelerators for the GEMM platform

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

#include "gemm.h"

#ifdef HP

     void kernelgemm1_hp(
          float *array_a,
          float *array_a_noncache,
          float *array_b,
          float *array_b_noncache,
          float *array_c,
          float *array_c_noncache,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INIT; kernelgemm1_hp;\n"));      
               int line_count = end-begin;
               
               //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
               //Use loop to copy the data we need into the noncacheable memory buffers     
               for (int i=begin; i<end; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_a_noncache[i*mat_dim_fpga + j] = array_a[i*mat_dim_fpga + j];
               for (int i=0; i<mat_dim_fpga; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_b_noncache[i*mat_dim_fpga + j] = array_b[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.HPBUFFERFILL;\n"));                               
                                        
               float *array_temp_a = array_a_noncache + begin*mat_dim_fpga;
               float *array_temp_c = array_c_noncache + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INPUTS; line_count -> %d; array_temp_a -> %p; array_b_noncache -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b_noncache),(void *)(array_temp_c),mat_dim_fpga));

               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.BEGINCOMPUTE;\n"));
               #pragma SDS resource(1)
               #pragma SDS async(1)
               gemm1_hp(array_temp_a, array_b_noncache, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args1.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(1)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.ENDCOMPUTE;\n"));
               
               //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
               for (int i=begin; i<end; i++)
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_c[i*mat_dim_fpga + j] = array_c_noncache[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.HPBUFFERCOPYBACK;\n")); 
     }
     
    void kernelgemm2_hp(
          float *array_a,
          float *array_a_noncache,
          float *array_b,
          float *array_b_noncache,
          float *array_c,
          float *array_c_noncache,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INIT; kernelgemm2_hp;\n"));      
               int line_count = end-begin;
               
               //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
               //Use loop to copy the data we need into the noncacheable memory buffers     
               for (int i=begin; i<end; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_a_noncache[i*mat_dim_fpga + j] = array_a[i*mat_dim_fpga + j];
               for (int i=0; i<mat_dim_fpga; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_b_noncache[i*mat_dim_fpga + j] = array_b[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.HPBUFFERFILL;\n"));                               
                                        
               float *array_temp_a = array_a_noncache + begin*mat_dim_fpga;
               float *array_temp_c = array_c_noncache + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INPUTS; line_count -> %d; array_temp_a -> %p; array_b_noncache -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b_noncache),(void *)(array_temp_c),mat_dim_fpga));

               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.BEGINCOMPUTE;\n"));
               #pragma SDS resource(2)
               #pragma SDS async(2)
               gemm2_hp(array_temp_a, array_b_noncache, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args2.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(2)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.ENDCOMPUTE;\n"));
               
               //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
               for (int i=begin; i<end; i++)
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_c[i*mat_dim_fpga + j] = array_c_noncache[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.HPBUFFERCOPYBACK;\n")); 
     }

    void kernelgemm3_hp(
          float *array_a,
          float *array_a_noncache,
          float *array_b,
          float *array_b_noncache,
          float *array_c,
          float *array_c_noncache,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INIT; kernelgemm3_hp;\n"));      
               int line_count = end-begin;
               
               //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
               //Use loop to copy the data we need into the noncacheable memory buffers     
               for (int i=begin; i<end; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_a_noncache[i*mat_dim_fpga + j] = array_a[i*mat_dim_fpga + j];
               for (int i=0; i<mat_dim_fpga; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_b_noncache[i*mat_dim_fpga + j] = array_b[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.HPBUFFERFILL;\n"));                               
                                        
               float *array_temp_a = array_a_noncache + begin*mat_dim_fpga;
               float *array_temp_c = array_c_noncache + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INPUTS; line_count -> %d; array_temp_a -> %p; array_b_noncache -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b_noncache),(void *)(array_temp_c),mat_dim_fpga));

               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.BEGINCOMPUTE;\n"));
               #pragma SDS resource(3)
               #pragma SDS async(3)
               gemm3_hp(array_temp_a, array_b_noncache, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args3.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(3)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.ENDCOMPUTE;\n"));
               
               //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
               for (int i=begin; i<end; i++)
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_c[i*mat_dim_fpga + j] = array_c_noncache[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.HPBUFFERCOPYBACK;\n")); 
     }

    void kernelgemm4_hp(
          float *array_a,
          float *array_a_noncache,
          float *array_b,
          float *array_b_noncache,
          float *array_c,
          float *array_c_noncache,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INIT; kernelgemm4_hp;\n"));      
               int line_count = end-begin;
               
               //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
               //Use loop to copy the data we need into the noncacheable memory buffers     
               for (int i=begin; i<end; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_a_noncache[i*mat_dim_fpga + j] = array_a[i*mat_dim_fpga + j];
               for (int i=0; i<mat_dim_fpga; i++) 
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_b_noncache[i*mat_dim_fpga + j] = array_b[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.HPBUFFERFILL;\n"));                               
                                        
               float *array_temp_a = array_a_noncache + begin*mat_dim_fpga;
               float *array_temp_c = array_c_noncache + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INPUTS; line_count -> %d; array_temp_a -> %p; array_b_noncache -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b_noncache),(void *)(array_temp_c),mat_dim_fpga));

               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.BEGINCOMPUTE;\n"));
               #pragma SDS resource(4)
               #pragma SDS async(4)
               gemm4_hp(array_temp_a, array_b_noncache, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args4.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(4)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.ENDCOMPUTE;\n"));
               
               //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
               for (int i=begin; i<end; i++)
                    for (int j=0; j < mat_dim_fpga; j++)
                         array_c[i*mat_dim_fpga + j] = array_c_noncache[i*mat_dim_fpga + j];
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.HPBUFFERCOPYBACK;\n")); 
     }     

#endif     

#ifdef HPC

     void kernelgemm1_hpc(
          float *array_a,
          float *array_b,
          float *array_c,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INIT; kernelgemm1_hpc;\n"));
               int line_count = end-begin;
               float *array_temp_a = array_a + begin*mat_dim_fpga;
               float *array_temp_c = array_c + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.INPUTS; line_count -> %d; array_temp_a -> %p; array_b -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b),(void *)(array_temp_c),mat_dim_fpga));

               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.BEGINCOMPUTE;\n"));
               #pragma SDS resource(1)
               #pragma SDS async(1)
               gemm1_hpc(array_temp_a, array_b, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args1.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(1)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.1.ENDCOMPUTE;\n"));
     }


     void kernelgemm2_hpc(
          float *array_a,
          float *array_b,
          float *array_c,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INIT; kernelgemm2_hpc;\n"));
               int line_count = end-begin;
               float *array_temp_a = array_a + begin*mat_dim_fpga;
               float *array_temp_c = array_c + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.INPUTS; line_count -> %d; array_temp_a -> %p; array_b -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b),(void *)(array_temp_c),mat_dim_fpga));
             
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.BEGINCOMPUTE;\n"));
               #pragma SDS resource(2)
               #pragma SDS async(2)
               gemm2_hpc(array_temp_a, array_b, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args2.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(2)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.2.ENDCOMPUTE;\n"));
     }


     void kernelgemm3_hpc(
          float *array_a,
          float *array_b,
          float *array_c,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INIT; kernelgemm3_hpc;\n"));
               int line_count = end-begin;
               float *array_temp_a = array_a + begin*mat_dim_fpga;
               float *array_temp_c = array_c + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.INPUTS; line_count -> %d; array_temp_a -> %p; array_b -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b),(void *)(array_temp_c),mat_dim_fpga));
               
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.BEGINCOMPUTE;\n"));
               #pragma SDS resource(3)
               #pragma SDS async(3)
               gemm3_hpc(array_temp_a, array_b, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args3.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(3)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.3.ENDCOMPUTE;\n"));
     }


     void kernelgemm4_hpc(
          float *array_a,
          float *array_b,
          float *array_c,
          int mat_dim_fpga,
          int file_desc,
          int ioctl_flag,
          int debug_flag,
          int begin,
          int end) {
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INIT; kernelgemm4_hpc;\n"));
               int line_count = end-begin;
               float *array_temp_a = array_a + begin*mat_dim_fpga;
               float *array_temp_c = array_c + begin*mat_dim_fpga;
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.INPUTS; line_count -> %d; array_temp_a -> %p; array_b -> %p; array_temp_c -> %p; mat_dim_fpga -> %d\n",line_count,(void *)(array_temp_a),(void *)(array_b),(void *)(array_temp_c),mat_dim_fpga));
           
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.BEGINCOMPUTE;\n"));
               #pragma SDS resource(4)
               #pragma SDS async(4)
               gemm4_hpc(array_temp_a, array_b, array_temp_c, mat_dim_fpga, line_count);
               //Control interrupt calls using IOCTL flag     
               if (ioctl_flag) {
                    int ret_value;
                    args4.spmm_workload = line_count;
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
                    ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass block size to driver for debugging
                    (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
               }     
               #pragma SDS wait(4)
               (debug_flag) && (fprintf(stderr,"DBG FPGA.4.ENDCOMPUTE;\n"));
     }

#endif
