#include <stdio.h>
#include <stdlib.h>
#include <sds_lib.h>

/*
//interrupt support
#include <sys/ioctl.h>
//Interrupt generator driver identification
#define IOCTL_WAIT_INTERRUPT_1 _IOR(100, 0, char *)
#define IOCTL_WAIT_INTERRUPT_2 _IOR(101, 0, char *)
#define IOCTL_WAIT_INTERRUPT_3 _IOR(102, 0, char *)
#define IOCTL_WAIT_INTERRUPT_4 _IOR(103, 0, char *)
*/

#include "spmm.h"

int kernelspmm1(
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
unsigned int end)
{
     //int ret_value;
	int line_count = end-begin;
	int nnz_int, nnz_done;
	int *rowPtr_int;
	int *columnIndex_int;
	int error_condition;

	DTYPE *values_int;
	DTYPE *y_int;
	
        rowPtr_int = rowPtr + begin;
	nnz_done = *(rowPtr + begin);
	columnIndex_int = columnIndex +nnz_done;
	values_int = values +nnz_done;
	nnz_int = *(rowPtr + end) - *(rowPtr + begin);
	y_int = y + begin;

	error_condition = 0;
	//#pragma SDS resource(1)
     #pragma SDS async(1)
	spmm1((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int,(u32)x_width,error_condition);
	//spmm((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int,error_condition);
     //control interrupt calls using flag
     //ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT_1,0); //sleep until interrupt
     #pragma SDS wait(1)
	
	if (error_condition)
	{
		printf("Error: FPGA buffer memory overflow\n");
		printf("nnz %d row_size %d line_count %d\n", nnz_int,row_size,line_count);
		//exit(1);
	}
}

int kernelspmm2(
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
unsigned int end)
{
     //int ret_value;
	int line_count = end-begin;
	int nnz_int, nnz_done;
	int *rowPtr_int;
	int *columnIndex_int;
	int error_condition;

	DTYPE *values_int;
	DTYPE *y_int;
	
        rowPtr_int = rowPtr + begin;
	nnz_done = *(rowPtr + begin);
	columnIndex_int = columnIndex +nnz_done;
	values_int = values +nnz_done;
	nnz_int = *(rowPtr + end) - *(rowPtr + begin);
	y_int = y + begin;

	error_condition = 0;
	//#pragma SDS resource(2)
     #pragma SDS async(2)
	spmm2((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int,(u32)x_width,error_condition);
	//spmm((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int,error_condition);
     //control interrupt calls using flag
     //ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT_2,0); //sleep until interrupt
	#pragma SDS wait(2)
	if (error_condition)
	{
		printf("Error: FPGA buffer memory overflow\n");
	        printf("nnz %d row_size %d line_count %d\n", nnz_int,row_size,line_count);
		//exit(1);
	}
}


int kernelspmm3(
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
unsigned int end)
{
     //int ret_value;
	int line_count = end-begin;
	int nnz_int, nnz_done;
	int *rowPtr_int;
	int *columnIndex_int;
	int error_condition;

	DTYPE *values_int;
	DTYPE *y_int;
	
        rowPtr_int = rowPtr + begin;
	nnz_done = *(rowPtr + begin);
	columnIndex_int = columnIndex +nnz_done;
	values_int = values +nnz_done;
	nnz_int = *(rowPtr + end) - *(rowPtr + begin);
	y_int = y + begin;

	error_condition = 0;
	//#pragma SDS resource(3)
     #pragma SDS async(3)
	spmm3((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int, (u32)x_width,error_condition);
	//spmm((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int,error_condition);

     //control interrupt calls using flag
     //ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT_3,0); //sleep until interrupt
	#pragma SDS wait(3)
	if (error_condition)
	{
		printf("Error: FPGA buffer memory overflow\n");
	        printf("nnz %d row_size %d line_count %d\n", nnz_int,row_size,line_count);
		//exit(1);
	}
}

int kernelspmm4(
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
unsigned int end)
{
     //int ret_value;
	int line_count = end-begin;
	int nnz_int, nnz_done;
	int *rowPtr_int;
	int *columnIndex_int;
	int error_condition;

	DTYPE *values_int;
	DTYPE *y_int;
	
        rowPtr_int = rowPtr + begin;
	nnz_done = *(rowPtr + begin);
	columnIndex_int = columnIndex +nnz_done;
	values_int = values +nnz_done;
	nnz_int = *(rowPtr + end) - *(rowPtr + begin);
	y_int = y + begin;

	error_condition = 0;
	//#pragma SDS resource(4)
     #pragma SDS async(4)
	spmm4((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int,(u32)x_width,error_condition);
	//spmm((u32 *)rowPtr_int, (u32 *)columnIndex_int, values_int, y_int, x, (u32)row_size,(u32)line_count, (u32)nnz_int,error_condition);

     //control interrupt calls using flag
     //ret_value = ioctl(file_desc,IOCTL_WAIT_INTERRUPT_4,0); //sleep until interrupt
	#pragma SDS wait(4)
	if (error_condition)
	{
		printf("Error: FPGA buffer memory overflow\n");
	        printf("nnz %d row_size %d line_count %d\n", nnz_int,row_size,line_count);
		//exit(1);
	}
}



