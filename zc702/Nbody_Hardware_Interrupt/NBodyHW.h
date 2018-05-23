 #ifndef NBODYHW_H_
#define NBODYHW_H_

#include "math.h"
#define NUMBODIES 10000
#define stsize 8
#define iposx 0
#define iposy 1
#define iposz 2
#define imass 3
#define iaccx 4
#define iaccy 5
#define iaccz 6
#define idsq 7




void ComputeChunkFPGA(
bool firsttime,
int nbodies,
int step,
float epssq,
float dthf,
int begin, 
int end,
float b[NUMBODIES*stsize],
float b_vel[NUMBODIES*3],
float b_temp[NUMBODIES*3],
int *interrupt,
int *status);

#endif 
