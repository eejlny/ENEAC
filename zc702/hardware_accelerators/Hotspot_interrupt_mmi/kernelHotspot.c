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

  int k,ret_value;
  int line_count = end-begin+1;

  float *array_temp_fpga = input_temp + begin*FRAME_WIDTH;
  float *array_power_fpga = input_power + begin*FRAME_WIDTH;
  float *array_out_fpga = output + begin*FRAME_WIDTH+FRAME_WIDTH;
  #pragma SDS async(1)
  sobel_filter(array_temp_fpga, array_power_fpga, array_out_fpga, Cap_1, Rx_1, Ry_1, Rz_1, line_count,interrupt,status);
  ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT,0); //this should put this process to sleep until interrupt takes place
 // printf("Waking up ...\n");
  #pragma SDS wait(1)
}
