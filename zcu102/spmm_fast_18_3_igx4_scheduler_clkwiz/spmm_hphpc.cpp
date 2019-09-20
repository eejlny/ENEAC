<<<<<<< HEAD
/*
 Author: Mohammad Hosseinabady - mohammad@hosseinabady.com
 Date: 18 Feb 2019
 Description: Source code for the optimised SPMM accelerators
*/
=======
>>>>>>> pr/3
void* __dso_handle;

#include "spmm.h"
#include <hls_stream.h>

void spmv_kernel(
u32 *rowSize_local_rs,
u32 *rowSize_local_nrs,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x_local,
u32 row_size,
u32 nnz,
u32 new_nnz) {
     #pragma HLS DATAFLOW
	int row_size_tmp=0;
	int j = 0;

	DTYPE y_tmp = 0;
	u32 row_counter = 0;

	hls::stream<DTYPE>       values_fifo;
     #pragma HLS STREAM variable=values_fifo depth=64 dim=1
	hls::stream<u32>             col_indices_fifo;
     #pragma HLS STREAM variable=col_indices_fifo depth=64 dim=1
	hls::stream<DTYPE>       y_fifo;
     #pragma HLS STREAM variable=y_fifo depth=64 dim=1

	for (u32 i = 0; i < nnz; i+=1) {
		#pragma HLS pipeline
          values_fifo << values[i];
     }
     for (u32 i = 0; i < nnz; i+=1) {
          #pragma HLS pipeline
          col_indices_fifo << columnIndex[i];
     }

	u32 row_size_remains = 0;
	for (u32 i = 0; i < new_nnz; i+=II) {
          #pragma HLS pipeline
		if (row_size_tmp == 0) {
			row_size_tmp = rowSize_local_nrs[j];
			row_size_remains = 0;
			y_tmp = 0;
			row_counter	= rowSize_local_rs[j++];
		}
          
          DTYPE y_local = 0;
		for (u32 p = 0; p < II; p++) {
			row_size_remains++;
			if (row_size_remains > row_counter) {
				y_local +=  0;
			} else {
				DTYPE v = values_fifo.read();
				u32   ci = col_indices_fifo.read();
                    y_local +=  v*x_local[ci];
			}
		}

		y_tmp += y_local;
		row_size_tmp-=II;

		if (row_size_tmp == 0) {
			y_fifo << y_tmp;
		}
	}

	for (u32 i = 0; i < row_size; i+=1) {
          #pragma HLS pipeline
		y[i] = y_fifo.read();
	}
}

#pragma SDS data sys_port(rowPtr:HP0)
#pragma SDS data sys_port(columnIndex:HP0)
#pragma SDS data sys_port(values:HP0)
#pragma SDS data sys_port(y:HP0)
#pragma SDS data sys_port(x:HP0)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hp0(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}  

#pragma SDS data sys_port(rowPtr:HP1)
#pragma SDS data sys_port(columnIndex:HP1)
#pragma SDS data sys_port(values:HP1)
#pragma SDS data sys_port(y:HP1)
#pragma SDS data sys_port(x:HP1)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hp1(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}

#pragma SDS data sys_port(rowPtr:HP2)
#pragma SDS data sys_port(columnIndex:HP2)
#pragma SDS data sys_port(values:HP2)
#pragma SDS data sys_port(y:HP2)
#pragma SDS data sys_port(x:HP2)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hp2(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}

#pragma SDS data sys_port(rowPtr:HP3)
#pragma SDS data sys_port(columnIndex:HP3)
#pragma SDS data sys_port(values:HP3)
#pragma SDS data sys_port(y:HP3)
#pragma SDS data sys_port(x:HP3)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hp3(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}

#pragma SDS data sys_port(rowPtr:HPC0)
#pragma SDS data sys_port(columnIndex:HPC0)
#pragma SDS data sys_port(values:HPC0)
#pragma SDS data sys_port(y:HPC0)
#pragma SDS data sys_port(x:HPC0)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hpc0(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}  

#pragma SDS data sys_port(rowPtr:HPC1)
#pragma SDS data sys_port(columnIndex:HPC1)
#pragma SDS data sys_port(values:HPC1)
#pragma SDS data sys_port(y:HPC1)
#pragma SDS data sys_port(x:HPC1)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hpc1(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}

#pragma SDS data sys_port(rowPtr:HPC0)
#pragma SDS data sys_port(columnIndex:HPC0)
#pragma SDS data sys_port(values:HPC0)
#pragma SDS data sys_port(y:HPC0)
#pragma SDS data sys_port(x:HPC0)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hpc2(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}

#pragma SDS data sys_port(rowPtr:HPC1)
#pragma SDS data sys_port(columnIndex:HPC1)
#pragma SDS data sys_port(values:HPC1)
#pragma SDS data sys_port(y:HPC1)
#pragma SDS data sys_port(x:HPC1)

#pragma SDS data zero_copy(rowPtr[0:(row_size+1)])
#pragma SDS data zero_copy(columnIndex[0:(nnz)])
#pragma SDS data zero_copy(values[0:(nnz)])
#pragma SDS data zero_copy(y[0:(col_size*x_width)])
#pragma SDS data zero_copy(x[0:(col_size*x_width)])

void spmm_fast_hpc3(
u32 *rowPtr,
u32 *columnIndex,
DTYPE *values,
DTYPE *y,
DTYPE *x,
u32 col_size,
u32 row_size,
u32 nnz,
u32 x_width) {
     #pragma HLS DATAFLOW
     u32 rowSizeNew_local_rs[ROW_SIZE_MAX];
     u32 rowSizeNew_local_nrs[ROW_SIZE_MAX];
     DTYPE x_local[COL_SIZE_MAX];
     DTYPE y_local[ROW_SIZE_MAX];

     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_rs cyclic  factor=4 dim=1
     #pragma HLS ARRAY_PARTITION variable=rowSizeNew_local_nrs cyclic factor=4 dim=1
     
     u32 new_nnz = nnz;
     u32 idx_previous = rowPtr[0];
     for (u32 i=1; i<(row_size+1); i++) {
          #pragma HLS pipeline
          u32 rs;
          u32 nrs;
          int idx2 = rowPtr[i];
          rs = idx2 - idx_previous;
          idx_previous = idx2;
          if (rs  == 0) {
               nrs = II;
               new_nnz += II;
          }
          else if (rs%II == 0) {
               nrs = rs;
               new_nnz += 0;
          } else {
               nrs = rs + (II-rs%II);
               new_nnz += (II-rs%II);
          }
          rowSizeNew_local_rs[i-1] = rs;
          rowSizeNew_local_nrs[i-1] = nrs;
     }
     for (u32 x_index = 0; x_index < x_width; x_index++) {
          for (u32 i=0; i<(col_size); i++)   {
                #pragma HLS pipeline
                x_local[i] = x[x_index*col_size+i];
          }
          
          spmv_kernel(rowSizeNew_local_rs, rowSizeNew_local_nrs, columnIndex, values, y_local, x_local, row_size, nnz, new_nnz);
          
          for (u32 i=0; i<(row_size); i++){
                #pragma HLS pipeline
                y[x_index*col_size+i] = y_local[i];
          }
     }
}