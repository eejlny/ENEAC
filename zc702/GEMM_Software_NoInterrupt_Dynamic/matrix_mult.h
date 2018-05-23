#ifndef MM_H
#define MM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>



#define N             1024   // N should be power of 2, and larger than 8
#define M             1024  // M should be power of 2, and larger than 8
#define P             1024   // P should be power of 2, and larger than 8

#define rows  	      1024


#define BLOCK 64
#define BLOCK_I 1
////#define HW_ADDR_GPIO 0x41200000
////#define HW_ADDR_GPIO_INT 0x41200120
////#define fpga_mmio_gpio 0x48 /*location of status register in GPIO. wrte this to generate interrupt*/
//#define fpga_mmio_gier 0x47
//#define fpga_mmio_ier 0x4A

float step;

float *array_a; 
float *array_b; 
float *array_c;
float *c_golden;

////int *interrupt;
////int *status;
////int local_interrupt;
////int file_desc;

void writeoutput(float *, int, int, char *);
void readinput(float *, int, int, char *);

int compute_matrix_mult(float *,float*, float*);

void usage(int, char **);
void run(int, char **);

//#define IOCTL_WAIT_INTERRUPT _IOR(100, 0, char *)    /*wait for interrupt*/



#endif
