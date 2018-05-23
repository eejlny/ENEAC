#ifndef KERNELNBODY_H_
#define KERNELNBODY_H_

#define NUMBODIES 10000 
#define stsize 8

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
float b_temp[NUMBODIES*3]
);

#endif 
