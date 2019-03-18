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
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <fcntl.h>

#ifndef MALAGA
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sds_lib.h>
#include <sys/mman.h>
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
#endif

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

		cerr << "Activating FPGA " << id << " with " << begin << " begin " << end << " end " << endl;

		switch(id)
		{
			case 1 : kernelMatrixmult1((float*)array_a,(float*)array_b,(float*)array_c,begin,end); break;
			case 2 : kernelMatrixmult2((float*)array_a,(float*)array_b,(float*)array_c,begin,end); break;
			case 3 : kernelMatrixmult3((float*)array_a,(float*)array_b,(float*)array_c,begin,end); break;
			case 4 : kernelMatrixmult4((float*)array_a,(float*)array_b,(float*)array_c,begin,end); break;
		}
		
		//cerr << "input is array_a 2 is " << array_a[2] << "result for array_c 2 is " << array_c[2] << endl;	

	}

	void OperatorCPU(int begin, int end) {
		//cerr << "CPU: " << begin << " " << end  << endl;
		bodies_C+=end-begin;

		int i_m,j_m,k_m,i_block,j_block,k_block;
		float *c_p, *b_p, *a_p;

		//printf("operator CPU being %d end %d\n",begin,end);

		for (i_m = begin; i_m < end; i_m += BLOCK_I) {
		    for (j_m = 0; j_m < P; j_m += BLOCK) {
		        for (k_m = 0; k_m < M; k_m += BLOCK) {
		        	c_p = &array_c[i_m*P+j_m];
		        	a_p = &array_a[i_m*M+k_m];
		            for (i_block = 0; i_block < BLOCK_I; i_block++ ) {
		            	b_p = &array_b[k_m*P+j_m];
		                for (j_block = 0; j_block < BLOCK; j_block++) {
		                    for (k_block = 0; k_block < BLOCK; k_block++) {
		                        c_p[k_block] += a_p[j_block] * b_p[k_block];
		                    }
		                    b_p += P;
		                }
		                c_p += P;
		                a_p += M;
		            }
		        }
		    }
		}
	}

};
//end class

#endif
