
//#define DEBUG
//#define DEEP_CPU_REPORT
//#define DEEP_GPU_REPORT
//#define DYNAMIC
//#define ORACLE
//#define FIXEDCHUNK
//#define CONCORD
//#define HDSSS
//#define LOGFIT
//#define PJTRACER
//#define OVERHEAD_STUDY
#define MULTIDYNAMIC

#define NDEBUG

#ifndef ITERS
#define ITERS 1
#endif

long unsigned int bodies_C=0, bodies_F=0;

// compile in broadwell1 using:
//            make -f Makefile.andres CFLAGS=-DMALAGA
//#define MALAGA

#ifdef MALAGA
#define  sds_alloc malloc
#else
#include <sds_lib.h>
#include <fcntl.h>
#include <sys/mman.h>

#define HW_ADDR_GPIO 0x00A0000000
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>

#ifdef ORACLE
#include "Oracle.h"
#endif
#ifdef DYNAMIC
#include "Dynamic.h"
#endif
#ifdef FIXEDCHUNK
#include "FixedChunkSize.h"
#endif
#ifdef CONCORD
#include "Concord.h"
#endif
#ifdef HDSSS
#include "HDSS.h"
#endif
#ifdef LOGFIT
#include "LogFit.h"
#endif
#ifdef MULTIDYNAMIC
#include "MultiDynamic.h"
#endif
#include "matrix_mult.h"

using namespace std;

/*****************************************************************************
* Global variables
* **************************************************************************/
float *array_a;
float *array_b;
float *array_c;
float *c_golden;

#include "Body.h"

const unsigned AOCL_ALIGNMENT = 64;
void *alignedMalloc(size_t size) {
  void *result = NULL;
  posix_memalign (&result, AOCL_ALIGNMENT, size);
  return result;
}

void alignedFree(void * ptr) {
  free (ptr);
}

// Returns the current system time in microseconds
long long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

void fatal(const char *s) {
  fprintf(stderr, "Error: %s\n", s);
  exit(1);
}

unsigned golden_MMM(float *a, float *b, float *c)
{
  int i,j,k;

  for(i=0;i<N;i++)
  for(j=0;j<P;j++)
  for(k=0;k<M;k++)
  c[i*P+j] += a[i*M+k]*b[k*P+j];
  return 1;
}

void usage(int argc, char **argv) {
  fprintf(stderr, "Usage: %s <numcpus> <numgpus> <chunkGPU>\n", argv[0]);
  fprintf(stderr, "\t<numcpus> - \n");
  fprintf(stderr, "\t<numgpus> - \n");
  fprintf(stderr, "\t<chunkGPU> - (Fixed or Dynamic) fixed size size assigned to GPU (CPU adapts dynamically to match effort)\n");
  exit(1);
}

/*****************************************************************************
* Main Function
* **************************************************************************/

int main(int argc, char* argv[]) {

  printf("Starting\n");

  int total_iterations=1,i,j,ret;
  Body body;
  Params p;
  int check = 0;

  if (argc < 4) usage(argc, argv);

  if(argc==5) check=1;
  p.numcpus			= atoi(argv[1]);
  p.numgpus			= atoi(argv[2]);

  sprintf(p.benchName, "GEMM");
  sprintf(p.kernelName, "gemm_kernel");

  #ifndef MALAGA
  int memfd;
  void *mapped_base, *mapped_dev_base;
  off_t dev_base = HW_ADDR_GPIO;
  int *interrupt;

  memfd = open("/dev/mem", O_RDWR | O_SYNC);
  if (memfd == -1) {
    printf("Can't open /dev/mem.\n");
    exit(0);
  }
  printf("/dev/mem opened for gpio.\n");
  mapped_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~MAP_MASK);
  if (mapped_base == (void *) -1) {
    printf("Can't map the memory to user space.\n");
    exit(0);
  }
  printf("GPIO mapped at address %p.\n", mapped_base);

  mapped_dev_base = mapped_base + (dev_base & MAP_MASK);

  interrupt = (int*)mapped_dev_base;

  printf("Reset interrupt status register\n");

  char led_value = 0x0; //

  *(interrupt+4) = (unsigned char) led_value; //interrupt off*/


  /*printf("Generate interrupt\n");

  char led_value = 0x2; // core 2 id

  *interrupt = (unsigned char) led_value; //leds on

  led_value = 0x1; // core 1 id

  *interrupt = (unsigned char) led_value; //leds on*/

  #endif

  printf("Matrix size= %d * %d\r\n", N, N);

  /* array_a = (float *) alignedMalloc(N*M*sizeof(float));
  array_b = (float *) alignedMalloc(M*P*sizeof(float));
  array_c = (float *) alignedMalloc(N*P*sizeof(float));*/
  
   c_golden=(float *)alignedMalloc(N*P*sizeof(float));

  //array_a = (float *) sds_alloc_non_cacheable(N*M*sizeof(float));
  //array_b = (float *) sds_alloc_non_cacheable(M*P*sizeof(float));
  //array_c = (float *) sds_alloc_non_cacheable(N*P*sizeof(float));

  array_a = (float *) sds_alloc(N*M*sizeof(float));
  array_b = (float *) sds_alloc(M*P*sizeof(float));
  array_c = (float *) sds_alloc(N*P*sizeof(float));

  //array_temp = (float *) sds_alloc(size*sizeof(float));
  //array_power = (float *) sds_alloc(size*sizeof(float));
  //array_out = (float *) sds_alloc(size*sizeof(float));

  if( !array_a)
  {
    printf("unable to allocate memory a\n");
    exit(1);
  }

  if( !array_b)
  {
    printf("unable to allocate memory b\n");
    exit(1);
  }

  if( !array_c)
  {
    printf("unable to allocate memory c\n");
    exit(1);
  }

  printf("Memories allocated\n");

  for(i=0;i<N;i++){
    for(j=0;j<M;j++) {
      //float t = rand()/(1.0*RAND_MAX);
      array_a[i*M+j]=1;
    }
  }

  for(i=0;i<M;i++) {
    for(j=0;j<P;j++) {
      //float t = rand()/(1.0*RAND_MAX);
      array_b[i*P+j]=1;
    }
  }

  for(i=0;i<N;i++)
    for(j=0;j<P;j++) {
      //float t = rand()/(1.0*RAND_MAX);
      array_c[i*P+j]=0;
    }

  printf("Starting computation\n");

  // Perform the computation
  //	compute_matrix_mult(array_a, array_b,array_c);
  //variables

  int t;
  /*Initializing scheduler*/
  #ifdef ORACLE
  p.ratioG = atof(argv[3]);
  Oracle * hs = Oracle::getInstance(&p);
  #endif
  #ifdef DYNAMIC
  p.gpuChunk = atoi(argv[3]);
  Dynamic * hs = Dynamic::getInstance(&p);
  #endif
  #ifdef MULTIDYNAMIC
  p.gpuChunk = atoi(argv[3]);
  Dynamic * hs = Dynamic::getInstance(&p);
  #endif
  #ifdef FIXEDCHUNK
  p.cpuChunk = atoi(argv[3]);
  p.gpuChunk = atoi(argv[4]);
  FixedChunkSize * hs = FixedChunkSize::getInstance(&p);
  #endif
  #ifdef CONCORD
  Concord * hs = Concord::getInstance(&p);
  #endif
  #ifdef HDSSS
  HDSS * hs = HDSS::getInstance(&p);
  #endif
  #ifdef LOGFIT
  LogFit * hs = LogFit::getInstance(&p);
  #endif

  cerr << "GEMM Simulation: " << p.numcpus << " CPUs, " << p.numgpus << " FPGAs."<< endl;

  //printf("call init\n");

  // Begin iterations
  hs->startTimeAndEnergy();
  //iters = 200;
  for (int step = 0; step < total_iterations; step++){
    //cerr << "Iteration: " << step << endl;
    hs->heterogeneous_parallel_for(0, rows, &body);
  }
  hs->endTimeAndEnergy();
  hs->saveResultsForBench();

  printf("Total n. rows : %ld\n", bodies_F+bodies_C);
  printf("Total n. rows on CPU: %ld\n", bodies_C);
  printf("Total n. rows on FPGA: %ld\n", bodies_F);
  printf("Actual percentage of work offloaded to the FPGA:%5.2f%% \n",bodies_F*100.1/(bodies_F+bodies_C));

  /*printf("Generate interrupt\n");
  led_value = 0x1; //
  *interrupt = (unsigned char) led_value; //interrupt of*/
  //printf("The value of reg1 is %x at %x\n",*(interrupt+4),(interrupt+4));


 /* printf("Verifying\n");

    for(i = 0; i < 100; i++) {
    float fc = array_c[i];
    printf("Value at %d Fast_MMM=%f \n", i, fc);
    }
  
*/

  /*golden_MMM(array_a, array_b, c_golden);

  int status = 0;

  for(i = 0; i < 100; i++) {
    float fc = array_c[i];
    float gc = c_golden[i];
    float diff = fabs(fc-gc);
    if (diff > 0.001 || diff != diff) {
      printf("Error at %d Fast_MMM=%f  Golden MMM=%f\n", i, fc, gc);
      break;
    }
  }

  if (!status) {
    printf("Evaluation PASSED\n");
  } else {
    printf("Evaluation FAILED\n");
  }*/
  return 0;
}
