#ifndef MM_H
#define MM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

#include  "kernelMatrixmult.h"

#define N             2048  // N should be power of 2, and larger than 8
#define M             2048  // M should be power of 2, and larger than 8
#define P             2048   // P should be power of 2, and larger than 8

#define rows  	      2048


#define BLOCK 256
#define BLOCK_I 1


void writeoutput(float *, int, int, char *);
void readinput(float *, int, int, char *);

int compute_matrix_mult(float *,float*, float*);

void usage(int, char **);
void run(int, char **);

#endif
