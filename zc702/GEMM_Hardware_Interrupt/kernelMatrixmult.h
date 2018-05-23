#ifndef KERNELMATRIXMULT_H_
#define KERNELMATRIXMULT_H_

int kernelMatrixmult(
float *array_a,
float *array_b,
float *array_c,
int *interrupt,
int *status,
int file_desc,
int begin,
int end);

#endif 
