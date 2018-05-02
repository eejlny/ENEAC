#include "NBodyHW.h"

int kernelNBody(
bool firsttime,
int nbodies,			
int step,
float epssq,
float dthf,
int begin, 
int end,
float b[NUMBODIES*stsize]) {

  int k;
  k=ComputeChunkFPGA(firsttime,nbodies,step,epssq, dthf,begin,end,b);
  return k;

}
