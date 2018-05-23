#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <sds_lib.h>
#include <sys/ioctl.h>
//#include <sys/time.h>

#define IOCTL_WAIT_INTERRUPT _IOR(100, 0, char *)    /*wait for interrupt*/

#include "matrix_mult.h"

int kernelMatrixmult(
float *array_a,
float *array_b,
float *array_c,
int *interrupt,
int *status,
int file_desc,
int begin,
int end)
{

  int ret_value;
  int line_count = end-begin;
  float *array_temp_a = array_a + begin*A_WIDTH;
  float *array_temp_b = array_b;
  float *array_temp_c = array_c + begin*A_WIDTH;
  //printf("Calling hardware with interrupt at %x\n",status);
  #pragma SDS async(1)
  mmult_top(array_temp_a, array_temp_b, array_temp_c, line_count,interrupt,status);
  //printf("Waiting for interrupt ...\n");
  ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT,0); //this should put this process to sleep until interrupt takes place
  //printf("Waking up ...\n");
  #pragma SDS wait(1)
  //printf("Simulating sleep calling before performing wait\n");
  //printf("Press Any Key to Continue\n");
  //getchar();
  //#pragma SDS wait(1)
  //printf("Exit value interrupt %x\n", *interrupt);
  //printf("Exit value status %x\n", *status);
  //#pragma SDS wait(1)
  //now wait for completion by waiting from interrupt from acceletaror
 /* printf("Waiting for interrupt ...\n");
  int ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT,0); //this should put this process to sleep until interrupt takes place
  printf("Waking up ...\n");
  #pragma SDS wait(1)*/
}
