#ifndef KERNELSPMM_H_
#define KERNELSPMM_H_

typedef float DTYPE;

#define x_width_value 100

void kernelspmm1(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y, 
DTYPE *x, 
int row_size, 
int nnz,
int x_width,
int file_desc,
unsigned int begin,
unsigned int end);

void kernelspmm2(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y, 
DTYPE *x, 
int row_size, 
int nnz,
int x_width,
int file_desc,
unsigned int begin,
unsigned int end);


void kernelspmm3(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y, 
DTYPE *x, 
int row_size,
int nnz,
int x_width,
int file_desc,
unsigned int begin,
unsigned int end);

void kernelspmm4(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y, 
DTYPE *x, 
int row_size, 
int nnz,
int x_width,
int file_desc,
unsigned int begin,
unsigned int end);

#endif 
