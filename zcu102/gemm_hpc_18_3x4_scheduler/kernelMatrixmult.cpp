#include <stdio.h>
#include <stdlib.h>
//#include <sds_lib.h>
#include "matrix_mult.h"

int kernelMatrixmult1(
float *array_a,
float *array_b,
float *array_c,
int begin,
int end)
{

  int k;
  int line_count = end-begin;
  float *array_temp_a = array_a + begin*A_WIDTH;
  float *array_temp_b = array_b;
  float *array_temp_c = array_c + begin*A_WIDTH;
  //mmult_top1(array_temp_a, array_temp_b, array_temp_c, line_count);
   #pragma SDS resource(1)
   mmult_top(array_temp_a, array_temp_b, array_temp_c, line_count);
}

int kernelMatrixmult2(
float *array_a,
float *array_b,
float *array_c,
int begin,
int end)
{

  int k;
  int line_count = end-begin;
  float *array_temp_a = array_a + begin*A_WIDTH;
  float *array_temp_b = array_b;
  float *array_temp_c = array_c + begin*A_WIDTH;
  //mmult_top2(array_temp_a, array_temp_b, array_temp_c, line_count);
  #pragma SDS resource(2)
  mmult_top(array_temp_a, array_temp_b, array_temp_c, line_count);
}

int kernelMatrixmult3(
float *array_a,
float *array_b,
float *array_c,
int begin,
int end)
{

  int k;
  int line_count = end-begin;
  float *array_temp_a = array_a + begin*A_WIDTH;
  float *array_temp_b = array_b;
  float *array_temp_c = array_c + begin*A_WIDTH;
  //mmult_top3(array_temp_a, array_temp_b, array_temp_c, line_count);
  #pragma SDS resource(3)
  mmult_top(array_temp_a, array_temp_b, array_temp_c, line_count);
}

int kernelMatrixmult4(
float *array_a,
float *array_b,
float *array_c,
int begin,
int end)
{

  int k;
  int line_count = end-begin;
  float *array_temp_a = array_a + begin*A_WIDTH;
  float *array_temp_b = array_b;
  float *array_temp_c = array_c + begin*A_WIDTH;
  #pragma SDS resource(4)
  mmult_top(array_temp_a, array_temp_b, array_temp_c, line_count);
}