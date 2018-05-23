#include <stdio.h>
#include <stdlib.h>
#include <sds_lib.h>
#include <stdio.h>
#include <sys/ioctl.h>


#define IOCTL_WAIT_INTERRUPT _IOR(100, 0, char *)    /*wait for interrupt*/

#include "NBodyHW.h"

int kernelNBody(
bool firsttime,
int nbodies,			
int step,
float epssq,
float dthf,
int *interrupt,
int *status,
int file_desc,
int begin, 
int end,
float b[NUMBODIES*stsize],
float b_vel[NUMBODIES*3],
float b_temp[NUMBODIES*3]) {

  int k;
  int ret_value;
  #pragma SDS async(1)
  ComputeChunkFPGA(firsttime,nbodies,step,epssq, dthf,begin,end,b,b_vel,b_temp,interrupt,status);
  //printf("Sleeping ...\n");
  ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT,0); //this should put this process to sleep until interrupt takes place
  //printf("Waking up ...\n");
  #pragma SDS wait(1)
/*  #pragma SDS async(1)
  ComputeChunkFPGA(firsttime,nbodies,step,epssq, dthf,begin,end,k,b);
  #pragma SDS wait(1)
*/
/*
  int begin2,end1,done1,done2;
  end1 = begin + (end-begin)/2;
  //printf("launching k1 with %d and %d\n",begin,end1);
  #pragma SDS async(1)
  ComputeChunkFPGA(firsttime,nbodies,step,epssq, dthf,begin,end1,done1,b);
  begin2 = end1;
  //printf("launching k2 with %d and %d\n",begin2,end);
  #pragma SDS async(2)
  ComputeChunkFPGA(firsttime,nbodies,step,epssq, dthf,begin2,end,done2,b);
  #pragma SDS wait(1)
  #pragma SDS wait(2)

  k=done1+done2;*/

 
 //return k;

}
