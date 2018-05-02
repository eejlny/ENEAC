/*
 * edge_detect.c
 *
 *  Created on: May 29, 2015
 *      Author: shaunpur
 */

#include "lab_design.h"
#include "edge_detect.h"

/* macros */
#define ABSDIFF(x,y)    ((x>y)? x - y : y - x)
#define ABS(x)          ((x>0)? x : -x)
#define LBUF_HEIGHT		WIDOW_HEIGHT
//#define NUMROWS			FRAME_HEIGHT
#define NUMCOLS			FRAME_WIDTH
#define BYTESHIFT		0
#define BLACK_VAL		0


/* ambient temperature, assuming no package at all	*/
const float amb_temp = 80.0;


/* define types and structures */
typedef float window_t[WIDOW_HEIGHT][WIDOW_WIDTH];
typedef float linebuffer_t[LBUF_HEIGHT][FRAME_WIDTH];

/* function prototypes */
static float sobel_operator(window_t window,  window_t power_window,float Cap_1, float Rx_1, float Ry_1, float Rz_1);
static void linebuffer_shift_up(linebuffer_t M, int col);
static float linebuffer_getval(linebuffer_t M, int RowIndex, int ColIndex);
static void linebuffer_insert_bottom(linebuffer_t M, float value, int col);
static void window_shift_right(window_t M);
static void window_insert(window_t M, float value, int row, int col);
static float window_getval(window_t M, int RowIndex, int ColIndex);

//Sobel Computation using a 3x3 neighborhood
float sobel_operator(window_t window, window_t power_window,float Cap_1, float Rx_1, float Ry_1, float Rz_1) {
	float x_weight = 0.0;

//	int i;
//	int j;

//	const float x_op[WIDOW_HEIGHT][WIDOW_WIDTH] =
//	        { { -1.31, 0.56, 1.34 },
//	        { -2.45, Cap_1, 2.22 },
//			{ -1.22, 0.45, 1.33 } };



	//Compute approximation of the gradients in the X-Y direction
	//for (i = 0; i < WIDOW_HEIGHT; i++) {
	//	for (j = 0; j < WIDOW_WIDTH; j++) {

			// X direction gradient
	//		x_weight = x_weight + (window_getval(window, i, j) * x_op[i][j]);


	x_weight = window_getval(window, 1, 1) +
								Cap_1 *
								(window_getval(power_window, 1, 1) + (window_getval(window, 2, 1) + window_getval(window, 0, 1) - 2.f*window_getval(window, 1, 1)) * Ry_1 +
								(window_getval(window, 1, 0) + window_getval(window, 1, 2) - 2.f*window_getval(window, 1, 1)) * Rx_1 +
								(amb_temp - window_getval(window, 1, 1)) * Rz_1);


	//	}
//	}




	return x_weight;
}

//Main function for Sobel Filtering
//This function includes a line buffer for a streaming implementation
//#pragma SDS data mem_attribute(input: NON_CACHEABLE)
//#pragma SDS data mem_attribute(input_power: NON_CACHEABLE)
//#pragma SDS data mem_attribute(input:PHYSICAL_CONTIGUOUS)
//#pragma SDS data mem_attribute(input_power:PHYSICAL_CONTIGUOUS)
//#pragma SDS data mem_attribute(output:PHYSICAL_CONTIGUOUS)
//#pragma SDS data access_pattern(input:SEQUENTIAL)
//#pragma SDS data access_pattern(input_power:SEQUENTIAL)
//#pragma SDS data access_pattern(output:SEQUENTIAL)

//#pragma SDS data mem_attribute(input_power: NON_CACHEABLE)
#pragma SDS data copy(input[0:(vchunk_size+2)*FRAME_WIDTH])
#pragma SDS data copy(input_power[0:(vchunk_size+2)*FRAME_WIDTH])
#pragma SDS data copy(output[0:(vchunk_size)*FRAME_WIDTH])
//#pragma SDS data mem_attribute(input:PHYSICAL_CONTIGUOUS)
//#pragma SDS data mem_attribute(input_power:PHYSICAL_CONTIGUOUS)
//#pragma SDS data mem_attribute(output:PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern(input:SEQUENTIAL)
#pragma SDS data access_pattern(input_power:SEQUENTIAL)
#pragma SDS data access_pattern(output:SEQUENTIAL)
#pragma SDS data data_mover(input:AXIDMA_SG)
#pragma SDS data data_mover(input_power:AXIDMA_SG)
#pragma SDS data data_mover(output:AXIDMA_SG)
#pragma SDS data sys_port(input:ACP, input_power:ACP, output:ACP)
#pragma SDS data zero_copy(status[0:1])
//#pragma SDS data mem_attribute(input: NON_CACHEABLE)
//#pragma SDS data mem_attribute(input_power: NON_CACHEABLE)
//#pragma SDS data mem_attribute(output: NON_CACHEABLE)
void sobel_filter(float *input, float *input_power, float *output,float Cap_1, float Rx_1, float Ry_1, float Rz_1, int vchunk_size,int *interrupt,int *status) {


//int sobel_filter(float input[1048576], float input_power[1048576], float output[1048576],float Cap_1, float Rx_1, float Ry_1, float Rz_1) {


//#pragma AP INTERFACE ap_fifo port=input depth=525312
//#pragma AP INTERFACE ap_fifo port=input_power depth=525312
//#pragma AP INTERFACE ap_fifo port=output depth=523264

//#pragma AP INTERFACE ap_fifo port=input depth=524288
//#pragma AP INTERFACE ap_fifo port=input_power depth=524288
//#pragma AP INTERFACE ap_fifo port=output depth=522240

//#pragma AP INTERFACE ap_fifo port=input depth=262144
//#pragma AP INTERFACE ap_fifo port=input_power depth=262144
//#pragma AP INTERFACE ap_fifo port=output depth=261632

//#pragma AP INTERFACE ap_fifo port=input depth=262144
//#pragma AP INTERFACE ap_fifo port=input_power depth=262144
//#pragma AP INTERFACE ap_fifo port=output depth=262144

	int row;
	int col;
	int index = 0;

	linebuffer_t buff_A;
	linebuffer_t buff_A_power;
	window_t buff_C;
	window_t buff_C_power;


	for (row = 0; row < vchunk_size+2; row++) {
		for (col = 0; col < NUMCOLS + 1; col++) {

#pragma AP PIPELINE II = 1

			// Temp values are used to reduce the number of memory reads
			float input_data = 0.0;
			float temp = 0.0;
			float tempx = 0.0;
			float input_data_power = 0.0;
			float temp_power = 0.0;
			float tempx_power = 0.0;

			//Line Buffer fill
			if (col < NUMCOLS) {
				linebuffer_shift_up(buff_A, col);
				temp = linebuffer_getval(buff_A, 0, col);
				linebuffer_shift_up(buff_A_power, col);
				temp_power = linebuffer_getval(buff_A_power, 0, col);
			}

			//There is an offset to accomodate the active pixel region
			//There are only NUMCOLS and NUMROWS valid pixels in the image
			if ((col < NUMCOLS) & (row < vchunk_size+2)) {
				float y;
				float y_power;
				index = row * NUMCOLS + col;
				input_data = input[index]; //[row*NUMCOLS+col];
				input_data_power = input_power[index]; //[row*NUMCOLS+col];
				//input_data_power = input[index]; //[row*NUMCOLS+col];
				y = input_data;
				y_power = input_data_power;
				tempx = y;
				tempx_power = y_power;
				linebuffer_insert_bottom(buff_A, tempx, col);
				linebuffer_insert_bottom(buff_A_power, tempx_power, col);
			}

			//Shift the processing window to make room for the new column
			window_shift_right(buff_C);
			window_shift_right(buff_C_power);

			//The Sobel processing window only needs to store luminance values
			//rgb2y function computes the luminance from the color pixel
			if (col < NUMCOLS) {
				window_insert(buff_C, linebuffer_getval(buff_A, 2, col), 0, 2);
				window_insert(buff_C, temp, 1, 2);
				window_insert(buff_C, tempx, 2, 2);
				window_insert(buff_C_power, linebuffer_getval(buff_A_power, 2, col), 0, 2);
				window_insert(buff_C_power, temp_power, 1, 2);
				window_insert(buff_C_power, tempx_power, 2, 2);
			}

			float edge;
			//The sobel operator only works on the inner part of the image
			//This design assumes there are no edges on the boundary of the image
			//Sobel operation on the inner portion of the image
			edge = sobel_operator(buff_C,buff_C_power, Cap_1,  Rx_1, Ry_1, Rz_1);


			//The output image is offset from the input to account for the line buffer
			if (row > 1 && col > 0) {
				index = (row - 2) * NUMCOLS + (col - 1);
				if ((col == 1) || col == (NUMCOLS))
					output[index] = 0;
				else
				    output[index] = edge;
			}
		}
	}
	//return 0;
 	*interrupt = 255; //switch on leds
	*status = 255; //trigger interrupt by writting to gpio
}

/* Line buffer shift up
 * Assumes new data pixel will be entered at the bottom of the line buffer
 * The bottom is row = 0
 */
static void linebuffer_shift_up(linebuffer_t M, int col) {
#pragma AP inline
	int i;
	for (i = LBUF_HEIGHT - 1; i > 0; i--) {
#pragma AP unroll
		M[i][col] = M[i - 1][col];
	}
}

/* Line buffer getval
 * Returns the data value in the line buffer at position RowIndex, ColIndex
 */
static float linebuffer_getval(linebuffer_t M, int RowIndex, int ColIndex) {
#pragma AP inline

	float return_value;
	return_value = M[RowIndex][ColIndex];
	return return_value;
}

/* Line buffer insert bottom
 * Inserts a new value in the bottom row of the line buffer at column = col
 * The bottom is row = 0
 */
static void linebuffer_insert_bottom(linebuffer_t M, float value, int col) {
#pragma AP inline

	M[0][col] = value;
}

/* Window shift right
 * Moves all the contents of the window horizontally
 * Assumes new values will be placed in column = WIDOW_WIDTH-1
 */
static void window_shift_right(window_t M) {
#pragma AP inline
	int i, j;
	for (i = 0; i < WIDOW_HEIGHT; i++) {
#pragma AP unroll
		for (j = 0; j < WIDOW_WIDTH - 1; j++) {
#pragma AP unroll
			M[i][j] = M[i][j + 1];
		}
	}
}

/* Window insert
 * Inserts a new value at any location of the window
 */
static void window_insert(window_t M, float value, int row, int col) {
#pragma AP inline
	M[row][col] = value;
}

/* Window getval
 * Returns the value of any window location
 */
static float window_getval(window_t M, int RowIndex, int ColIndex) {
#pragma AP inline
	float return_value;
	return_value = M[RowIndex][ColIndex];
	return return_value;
}
