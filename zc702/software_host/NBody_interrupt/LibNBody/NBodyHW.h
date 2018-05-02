 #ifndef NBODYHW_H_
#define NBODYHW_H_

#include "math.h"
#define NUMBODIES 10000 
#define stsize 11
#define imass 0
#define iposx 1
#define iposy 2
#define iposz 3
#define ivelx 4
#define ively 5
#define ivelz 6
#define iaccx 7
#define iaccy 8
#define iaccz 9
#define idsq 10

#pragma SDS data zero_copy(b[0:10000*stsize])
#pragma SDS data access_pattern(b:SEQUENTIAL)

int ComputeChunkFPGA(
bool firsttime,
int nbodies,
int step,
float epssq,
float dthf,
int begin, 
int end,
float b[NUMBODIES*stsize]);

#endif 
