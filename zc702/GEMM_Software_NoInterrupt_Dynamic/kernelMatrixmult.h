#ifndef KERNELMM_H_
#define KERNELMM_H_

int kernelMatrixmult(
float *array_a,
float *array_b,
float *array_c,
////int *interrupt,
////int *status,
////int file_desc,
int begin,
int end);
/*
int kernelMatrixmult(
float *array_a,
float *array_b,
float *array_c,
int begin,
int end);
*/

#endif 
