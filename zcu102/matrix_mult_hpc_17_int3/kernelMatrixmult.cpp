#include <stdio.h>
#include <stdlib.h>
#include <sds_lib.h>
#include "matrix_mult.h"

int kernelMatrixmult(
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
  mmult_top(array_temp_a, array_temp_b, array_temp_c, line_count);
}
