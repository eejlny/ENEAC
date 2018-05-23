#include <stdio.h>
#include <stdlib.h>
#include <sds_lib.h>
#include <sys/ioctl.h>
#include "edge_detect.h"
#include "lab_design.h"

#define IOCTL_WAIT_INTERRUPT _IOR(100, 0, char *)    /*wait for interrupt*/

int kernelHotspot(
float *input_temp,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int *interrupt,
int *status,
int file_desc,
int begin,
int end)

{

  int k,ret_value,enable;
   int line_count1 = (end-begin+1)/2;

  float *array_temp_fpga = input_temp + begin*FRAME_WIDTH;
  float *array_power_fpga = input_power + begin*FRAME_WIDTH;
  float *array_out_fpga = output + begin*FRAME_WIDTH+FRAME_WIDTH;
  enable = 0;
  #pragma SDS async(1)
  sobel_filter(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, line_count1,interrupt,status,enable);
  int line_count2 = (end-begin+1) - line_count1;
  array_temp_fpga = input_temp + begin*FRAME_WIDTH - FRAME_WIDTH + line_count1*FRAME_WIDTH;
  array_power_fpga = input_power + begin*FRAME_WIDTH - FRAME_WIDTH + line_count1*FRAME_WIDTH;
  array_out_fpga = output + begin*FRAME_WIDTH + line_count1*FRAME_WIDTH;
  enable=1;
  #pragma SDS async(2) 
  sobel_filter(array_temp_fpga,array_power_fpga, array_out_fpga,Cap_1, Rx_1, Ry_1, Rz_1,line_count2,interrupt,status,enable);
  ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT,0); //this should put this process to sleep until interrupt takes place
  #pragma SDS wait(1)
  #pragma SDS wait(2)
 
}
