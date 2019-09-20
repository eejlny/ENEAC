/*
 * edge_detect.h
 *
 *  Created on: May 29, 2015
 *      Author: shaunpur
 */

/* includes */
#include <inttypes.h>

/* macros */
#define WIDOW_WIDTH      3
#define WIDOW_HEIGHT     3
#define FRAME_WIDTH      (16384+2)
#define LBUF_HEIGHT      WIDOW_HEIGHT
#define LBUF_WIDTH       FRAME_WIDTH
#define NUMCOLS          (frame_width+2)
#define BYTESHIFT        0
#define BLACK_VAL		0

/* ambient temperature, assuming no package at all	*/
const float amb_temp = 80.0;

/* define types and structures */
typedef float window_t[WIDOW_HEIGHT][WIDOW_WIDTH];
typedef float linebuffer_t[LBUF_HEIGHT][LBUF_WIDTH];

/* function prototypes */
static float sobel_operator(
window_t window,
window_t power_window,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1);

static void linebuffer_shift_up(
linebuffer_t M,
int col);

static float linebuffer_getval(
linebuffer_t M,
int RowIndex,
int ColIndex);

static void linebuffer_insert_bottom(
linebuffer_t M,
float value,
int col);

static void window_shift_right(
window_t M);

static void window_insert(
window_t M,
float value,
int row,
int col);

static float window_getval(
window_t M,
int RowIndex,
int ColIndex);

void sobel_filter1_hp(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);

void sobel_filter2_hp(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);

void sobel_filter3_hp(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);

void sobel_filter4_hp(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);

void sobel_filter1_hpc(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);

void sobel_filter2_hpc(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);

void sobel_filter3_hpc(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);

void sobel_filter4_hpc(
float *input,
float *input_power,
float *output,
float Cap_1,
float Rx_1,
float Ry_1,
float Rz_1,
int frame_width,
int vchunk_size);