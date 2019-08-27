/*
 Author: Kris Nikov - kris.nikov@bris.ac.uk
 Date: 11 Jun 2019
 Description: Header for the SPMM accelerator functions
*/
#ifndef __SPMM_H__
#define __SPMM_H__

/*
 * for clock frequency 100 MHZ   II=4
 * for clock frequency 200 MHZ   II=8
 * for clock frequency 300 MHZ   II=10
 */
const static int II = 8;

//Need to specify harware accelerator local buffers - these are set for mixtank_new.mtx
const static int ROW_SIZE_MAX = (30000);
const static int COL_SIZE_MAX = ROW_SIZE_MAX;

//Custom variable types used by the functions
typedef float DTYPE;
typedef unsigned int u32;

//Choose either the HP or the HPC accelerators
//#define HP
#define HPC

void spmm_fast_hp0(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);

void spmm_fast_hp1(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);

void spmm_fast_hp2(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);

void spmm_fast_hp3(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);

void spmm_fast_hpc0(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);

void spmm_fast_hpc1(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);

void spmm_fast_hpc2(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);

void spmm_fast_hpc3(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width);
            
#endif // __MATRIXMUL_H__ not defined