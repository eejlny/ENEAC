//============================================================================
// Name			: Body.h
// Author		: Antonio Vilches
// Version		: 1.0
// Date			: 13 / 01 / 2015
// Copyright	: Department. Computer's Architecture (c)
// Description	: This file contains the Body class
//============================================================================

#ifndef _BODY_TASK_
#define _BODY_TASK_

#ifdef USEBARRIER
#include "barrier.h"
#endif

#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
using namespace tbb;

#include <stdio.h>
#include <sds_lib.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/fs.h>


#define TIME_STAMP_INIT_HW  unsigned long long clock_start_hw, clock_end_hw;  clock_start_hw = sds_clock_counter();
#define TIME_STAMP_HW  { clock_end_hw = sds_clock_counter(); printf("SPARSE FPGA ON: execution time : %f ms\n", 1000*(clock_end_hw-clock_start_hw)/(1200*10e6)); clock_start_hw = sds_clock_counter();  }



/*Interrupt drivers*/
#define DRIVER_FILE_NAME_1 "/dev/intgendriver1"
int file_desc_1 = open(DRIVER_FILE_NAME_1, O_RDWR);	//Open interrupt driver 1
#define DRIVER_FILE_NAME_2 "/dev/intgendriver2"
int file_desc_2 = open(DRIVER_FILE_NAME_2, O_RDWR);	//Open interrupt driver 2
#define DRIVER_FILE_NAME_3 "/dev/intgendriver3"
int file_desc_3 = open(DRIVER_FILE_NAME_3, O_RDWR);	//Open interrupt driver 3
#define DRIVER_FILE_NAME_4 "/dev/intgendriver4"
int file_desc_4 = open(DRIVER_FILE_NAME_4, O_RDWR);	//Open interrupt driver 4
int ioctl_flag = 1; //enable ioctl calls to driver

/*****************************************************************************
 * NbodyTask
 * **************************************************************************/
class Body
{
public:
	bool firsttime;
public:

	void OperatorGPU(int begin, int end, int id) {
		//cerr << "GPU: " << begin << " " << end << " id: " << id << endl; 
		bodies_F+=end-begin;
		
		//if ((end-begin) == 1024)
		//{
		cerr << "Activating FPGA " << id << " with " << begin << " begin " << end << " end " << endl;
		
		switch(id)
		{
			case 1 : kernelspmm1(h_rowDelimiters, h_cols, h_val, h_out, h_vec, numRows,nItems,x_width,file_desc_1,begin,end); break;
			case 2 : kernelspmm2(h_rowDelimiters, h_cols, h_val, h_out, h_vec, numRows,nItems,x_width,file_desc_2,begin,end); break;
			case 3 : kernelspmm3(h_rowDelimiters, h_cols, h_val, h_out, h_vec, numRows,nItems,x_width,file_desc_3,begin,end); break;
			case 4 : kernelspmm4(h_rowDelimiters, h_cols, h_val, h_out, h_vec, numRows,nItems,x_width,file_desc_4,begin,end); break;
		}
		//}
	}

	void OperatorCPU(int begin, int end) {
		//cerr << "CPU: " << begin << " " << end  << endl; 
		bodies_C+=end-begin;
		spmvCpu/*_vilches*/(h_val, h_cols, h_rowDelimiters, h_vec, h_out, begin, end);
	}

};
//end class

#endif
