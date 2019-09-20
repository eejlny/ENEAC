// Author: Kris Nikov - kris.nikov@bris.ac.uk
// Date: 15 Jul 2019
// Description - Source file for the software interface library to the FPGA accelerators for the HOTSPOT platform

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sds_lib.h>
#include <cstring>

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

#include "hotspot.h"

#ifdef HP

void kernelhotspot1_hp(
float *input_temp,
float *input_temp_noncache,
float *input_power,
float *input_power_noncache,
float *output,
float *output_noncache,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.0; kernelhotspot1_hp;\n"));
     int line_count = end-begin;
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.1; line_count -> %d; begin ->%d; end -> %d\n", line_count, begin, end));     
     //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
     //Use loop to copy the data we need into the noncacheable memory buffers     
     for (int i=0; i<(line_count+2)*(frame_width+2); i++) 
     {
          input_temp_noncache[i + begin*(frame_width+2)] = input_temp[i + begin*(frame_width+2)];
          input_power_noncache[i + begin*(frame_width+2)] = input_power[i + begin*(frame_width+2)];  
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.2; HP INPUT BUFFER COPY DONE!\n"));
     
     float *array_temp_fpga = input_temp_noncache + begin*(frame_width+2);
     float *array_power_fpga = input_power_noncache + begin*(frame_width+2);
     float *array_out_fpga = output_noncache + begin*(frame_width+2) + (frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.3; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     
     #pragma SDS async(1)
     sobel_filter1_hp(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args1.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(1)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.4; COMPUTE DONE!\n"));
     
     //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
     for (int i=0; i<line_count*(frame_width+2); i++) 
     {
            output[i+begin*(frame_width+2)+(frame_width+2)] = output_noncache[i+begin*(frame_width+2)+(frame_width+2)];
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.5; HP OUTPUT BUFFER COPY DONE!\n"));   
}

void kernelhotspot2_hp(
float *input_temp,
float *input_temp_noncache,
float *input_power,
float *input_power_noncache,
float *output,
float *output_noncache,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.0; kernelhotspot2_hp;\n"));
     int line_count = end-begin;
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.1; line_count -> %d; begin ->%d; end -> %d\n", line_count, begin, end));     
     //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
     //Use loop to copy the data we need into the noncacheable memory buffers
     for (int i=0; i<(line_count+2)*(frame_width+2); i++) 
     {
          input_temp_noncache[i + begin*(frame_width+2)] = input_temp[i + begin*(frame_width+2)];
          input_power_noncache[i + begin*(frame_width+2)] = input_power[i + begin*(frame_width+2)];  
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.2; HP INPUT BUFFER COPY DONE!\n"));
     
     float *array_temp_fpga = input_temp_noncache + begin*(frame_width+2);
     float *array_power_fpga = input_power_noncache + begin*(frame_width+2);
     float *array_out_fpga = output_noncache + begin*(frame_width+2) + (frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.3; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     
     #pragma SDS async(2)
     sobel_filter2_hp(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args2.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(2)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.4; COMPUTE DONE!\n"));
     
     //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
     for (int i=0; i<line_count*(frame_width+2); i++) 
     {
            output[i+begin*(frame_width+2)+(frame_width+2)] = output_noncache[i+begin*(frame_width+2)+(frame_width+2)];
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.5; HP OUTPUT BUFFER COPY DONE!\n"));
}

void kernelhotspot3_hp(
float *input_temp,
float *input_temp_noncache,
float *input_power,
float *input_power_noncache,
float *output,
float *output_noncache,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.0; kernelhotspot3_hp;\n"));
     int line_count = end-begin;
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.1; line_count -> %d; begin ->%d; end -> %d\n", line_count, begin, end));     
     //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
     //Use loop to copy the data we need into the noncacheable memory buffers
     for (int i=0; i<(line_count+2)*(frame_width+2); i++) 
     {
          input_temp_noncache[i + begin*(frame_width+2)] = input_temp[i + begin*(frame_width+2)];
          input_power_noncache[i + begin*(frame_width+2)] = input_power[i + begin*(frame_width+2)];  
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.2; HP INPUT BUFFER COPY DONE!\n"));
     
     float *array_temp_fpga = input_temp_noncache + begin*(frame_width+2);
     float *array_power_fpga = input_power_noncache + begin*(frame_width+2);
     float *array_out_fpga = output_noncache + begin*(frame_width+2) + (frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.3; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     
     #pragma SDS async(3)
     sobel_filter3_hp(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args3.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(3)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.4; COMPUTE DONE!\n"));
     
     //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
     for (int i=0; i<line_count*(frame_width+2); i++) 
     {
            output[i+begin*(frame_width+2)+(frame_width+2)] = output_noncache[i+begin*(frame_width+2)+(frame_width+2)];
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.5; HP OUTPUT BUFFER COPY DONE!\n"));
}

void kernelhotspot4_hp(
float *input_temp,
float *input_temp_noncache,
float *input_power,
float *input_power_noncache,
float *output,
float *output_noncache,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.0; kernelhotspot4_hp;\n"));
     int line_count = end-begin;
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.1; line_count -> %d; begin ->%d; end -> %d\n", line_count, begin, end));     
     //The HP port-connected accelerators need to use special data buffers in noncacheable memory to work
     //Use loop to copy the data we need into the noncacheable memory buffers
     for (int i=0; i<(line_count+2)*(frame_width+2); i++) 
     {
          input_temp_noncache[i + begin*(frame_width+2)] = input_temp[i + begin*(frame_width+2)];
          input_power_noncache[i + begin*(frame_width+2)] = input_power[i + begin*(frame_width+2)];  
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.2; HP INPUT BUFFER COPY DONE!\n"));
     
     float *array_temp_fpga = input_temp_noncache + begin*(frame_width+2);
     float *array_power_fpga = input_power_noncache + begin*(frame_width+2);
     float *array_out_fpga = output_noncache + begin*(frame_width+2) + (frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.3; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     
     #pragma SDS async(4)
     sobel_filter4_hp(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args4.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(4)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.4; COMPUTE DONE!\n"));
     
     //After computation copy the contents of the non-cacheable memory block to the sds-alloc/malloc block (final result)
     for (int i=0; i<line_count*(frame_width+2); i++) 
     {
            output[i+begin*(frame_width+2)+(frame_width+2)] = output_noncache[i+begin*(frame_width+2)+(frame_width+2)];
     }
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.5; HP OUTPUT BUFFER COPY DONE!\n"));
}

#endif     

#ifdef HPC

void kernelhotspot1_hpc(
float *input_temp,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.0; kernelhotspot1_hpc;\n"));
     int line_count = end-begin;
     float *array_temp_fpga = input_temp + begin*(frame_width+2);
     float *array_power_fpga = input_power + begin*(frame_width+2);
     float *array_out_fpga = output + begin*(frame_width+2)+(frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.1; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     #pragma SDS async(1)
     sobel_filter1_hpc(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args1.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_1, &args1); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.1.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(1)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.1.2;\n"));
}

void kernelhotspot2_hpc(
float *input_temp,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.0; kernelhotspot2_hpc;\n"));
     int line_count = end-begin;
     float *array_temp_fpga = input_temp + begin*(frame_width+2);
     float *array_power_fpga = input_power + begin*(frame_width+2);
     float *array_out_fpga = output + begin*(frame_width+2)+(frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.1; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     #pragma SDS async(2)
     sobel_filter2_hpc(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args2.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_2, &args2); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.2.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(2)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.2.2;\n"));
}

void kernelhotspot3_hpc(
float *input_temp,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.0; kernelhotspot3_hpc;\n"));
     int line_count = end-begin;
     float *array_temp_fpga = input_temp + begin*(frame_width+2);
     float *array_power_fpga = input_power + begin*(frame_width+2);
     float *array_out_fpga = output + begin*(frame_width+2)+(frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.1; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     #pragma SDS async(3)
     sobel_filter3_hpc(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args3.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_3, &args3); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.3.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(3)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.3.2;\n"));
}

void kernelhotspot4_hpc(
float *input_temp,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int file_desc,
int ioctl_flag,
int debug_flag,
int begin,
int end) {
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.0; kernelhotspot4_hpc;\n"));
     int line_count = end-begin;
     float *array_temp_fpga = input_temp + begin*(frame_width+2);
     float *array_power_fpga = input_power + begin*(frame_width+2);
     float *array_out_fpga = output + begin*(frame_width+2)+(frame_width+2);
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.1; line_count -> %d; array_temp_fpga -> %p; array_power_fpga -> %p; array_out_fpga -> %p;\n",line_count,(void *)(array_temp_fpga),(void *)(array_power_fpga),(void *)(array_out_fpga)));
     #pragma SDS async(4)
     sobel_filter4_hpc(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, frame_width, line_count);
     //Control interrupt calls using IOCTL flag     
     if (ioctl_flag) {
          int ret_value;
          args4.spmm_workload = line_count;
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.beforesleep; file_desc -> %d\n",file_desc));
          ret_value = ioctl(file_desc, IOCTL_WAIT_INTERRUPT_4, &args4); //Sleep until interrupt, pass block size to driver for debugging
          (debug_flag) && (fprintf(stderr,"DBG FPGA.4.IOCTL.aftersleep; ret_value -> %d\n",ret_value));          
     }     
     #pragma SDS wait(4)
     (debug_flag) && (fprintf(stderr,"DBG FPGA.4.2;\n"));
}
#endif
