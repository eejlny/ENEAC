/*
 Author: Kris Nikov - kris.nikov@bris.ac.uk
 Date: 11 Jun 2019
 Description: Header for the SPMM harware interface library
*/
#ifndef KERNELSPMM_H_
#define KERNELSPMM_H_

typedef float DTYPE;

void kernelspmm1_hp(
int *rowPtr, 
int *rowPtr_noncache, 
int *columnIndex,
int *columnIndex_noncache,
DTYPE *values, 
DTYPE *values_noncache, 
DTYPE *y,
DTYPE *y_noncache_trans, 
DTYPE *x_noncache_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

void kernelspmm2_hp(
int *rowPtr, 
int *rowPtr_noncache, 
int *columnIndex,
int *columnIndex_noncache,
DTYPE *values, 
DTYPE *values_noncache, 
DTYPE *y,
DTYPE *y_noncache_trans, 
DTYPE *x_noncache_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

void kernelspmm3_hp(
int *rowPtr, 
int *rowPtr_noncache, 
int *columnIndex,
int *columnIndex_noncache,
DTYPE *values, 
DTYPE *values_noncache, 
DTYPE *y,
DTYPE *y_noncache_trans, 
DTYPE *x_noncache_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

void kernelspmm4_hp(
int *rowPtr, 
int *rowPtr_noncache, 
int *columnIndex,
int *columnIndex_noncache,
DTYPE *values, 
DTYPE *values_noncache, 
DTYPE *y,
DTYPE *y_noncache_trans, 
DTYPE *x_noncache_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

void kernelspmm1_hpc(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y,
DTYPE *y_trans, 
DTYPE *x_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

void kernelspmm2_hpc(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y,
DTYPE *y_trans, 
DTYPE *x_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

void kernelspmm3_hpc(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y,
DTYPE *y_trans, 
DTYPE *x_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

void kernelspmm4_hpc(
int *rowPtr, 
int *columnIndex,
DTYPE *values, 
DTYPE *y,
DTYPE *y_trans, 
DTYPE *x_trans, 
int row_size, 
int nnz,
int x_width,
int file_desc,
int ioctl_flag,
int debug_flag,
unsigned int begin,
unsigned int end);

#endif 
