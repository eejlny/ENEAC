#ifndef HOTSPOT_H
#define HOTSPOT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

                                                                   

#define STR_SIZE 256
# define EXPAND_RATE 2// add one iteration will extend the pyramid base by 2 per each borderline

/* maximum power density possible (say 300W for a 10mm x 10mm chip)	*/
#define MAX_PD	(3.0e6)
/* required precision in degrees	*/
#define PRECISION	0.001
#define SPEC_HEAT_SI 1.75e6
#define K_SI 100
/* capacitance fitting factor	*/
#define FACTOR_CHIP	0.5

////#define HW_ADDR_GPIO 0x41200000
////#define HW_ADDR_GPIO_INT 0x41200120
////#define fpga_mmio_gpio 0x48 /*location of status register in GPIO. wrte this to generate interrupt*/


#define MIN(a, b) ((a)<=(b) ? (a) : (b))




/* chip parameters	*/
const static float t_chip = 0.0005;
const static float chip_height = 0.016;
const static float chip_width = 0.016;
/* ambient temperature, assuming no package at all	*/
const static float amb_temp = 80.0;


int size;
int ret;
	float grid_height; 
	float grid_width; 

	float Cap; 
	float Rx; 
	float Ry; 
	float Rz; 

	float max_slope; 
	float step;
int grid_rows,grid_cols;
int src = 0, dst = 1;
int borderCols, borderRows;


float *array_temp; 
float *array_power; 
float *array_out;

////int *interrupt;
////int *status;
////int local_interrupt;
////int file_desc;

void writeoutput(float *, int, int, char *);
void readinput(float *, int, int, char *);

int compute_tran_temp(float *,float*, float*, int, int, int, int);

void usage(int, char **);
void run(int, char **);



#endif
