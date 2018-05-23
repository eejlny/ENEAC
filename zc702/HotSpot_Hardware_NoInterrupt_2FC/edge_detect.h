/*
 * edge_detect.h
 *
 *  Created on: May 29, 2015
 *      Author: shaunpur
 */

//#ifndef EDGE_DETECT_H_
//#define EDGE_DETECT_H_

/* macros */
#define edge_detect	sobel_filter

/* function proto-types */

//#pragma SDS data sys_port(input:ACP, input_power:AFI, output:ACP)
//#pragma SDS data buffer_depth(input:524288, input_power:524288, output:522240)
//#pragma SDS data buffer_depth(input:262144, input_power:262144, output:261632)
//#pragma SDS data buffer_depth(input:525312, input_power:525312, output:523264)
////void sobel_filter(float *input, float *input_power, float *output,float Cap_1, float Rx_1, float Ry_1, float Rz_1,int vchunk_size,int *interrupt,int *status,int enable);
void sobel_filter(float *input, float *input_power, float *output,float Cap_1, float Rx_1, float Ry_1, float Rz_1,int vchunk_size);

//#pragma SDS data buffer_depth(input:262144, input_power:262144, output:262144)
//int sobel_filter(float *input,  float *output,float Cap_1, float Rx_1, float Ry_1, float Rz_1);

//#endif /* EDGE_DETECT_H_ */
