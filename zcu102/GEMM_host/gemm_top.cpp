#include <stdio.h>
#include <sds_lib.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h> 
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb.h"

#include "Body.h"

#ifdef MULTIDYNAMIC
     #include "MultiDynamic.h"
#endif
#ifdef MULTIHAP
     #include "MultiHap.h"
#endif

using namespace std;
using namespace tbb;

//least common multiple
int lcm( int num1, int num2)
{

   int i, gcd, lcm;
   //calculation of gcd
   for(i=1; i <= num1 && i <= num2; ++i)
   {
      if(num1 % i == 0 && num2 % i == 0)
      gcd = i;
   }
   //calculation of lcm using gcd
   lcm = (num1 * num2) / gcd;
   cout << "LCM: " << lcm << endl;

   return lcm;
}

float float_rand( float min, float max )
{
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * ( max - min );      /* [min, max] */
}

/*****************************************************************************
* Main Function
* **************************************************************************/

int main(int argc, char* argv[]) {
     printf("Starting\n");

     //Variables
     Body body;
     Params p;
     
     if ( argc < 7 || argc > 8 )
		usage(argc, argv);
	if ( atoi(argv[1]) < 8 || atoi(argv[1])%2 != 0 
          || atoi(argv[2]) < 0 || atoi(argv[2]) > 4 
          || atoi(argv[3]) < 0 
          || atoi(argv[4]) < 0
          || atoi(argv[5]) < 0 
          || ( atoi(argv[6]) != 0 && atoi(argv[6]) != 1 ) ) 
               usage(argc, argv);
     else
          mat_dim = atoi(argv[1]);
          
          
     p.numcpus = atoi(argv[2]);
     numhpacc = atoi(argv[3]);
     numhpcacc = atoi(argv[4]);	
     p.numgpus	= numhpacc + numhpcacc;
     
          
     /*Initializing scheduler*/
     #ifdef MULTIDYNAMIC
       p.gpuChunk = atoi(argv[5]);
       Dynamic * hs = Dynamic::getInstance(&p);
     #endif
     #ifdef MULTIHAP // new multihap
          p.chunkGPU_initratio = (float) atof(argv[8]);
          // lcm == least common multiple
          // the scheduler will tune chunk size to lay in a 64 bytes offset block
          int alignment = lcm(2, mat_dim * sizeof(float))/(mat_dim * sizeof(float)); //find best alignment for chunnk sizes
          cout << "ALING CHUNK SIZE: " << alignment << endl;
          p.CHUNKCPUALIGN = alignment; // set to 1 if no alignment required
          p.CHUNKGPUALIGN = alignment; // set to 1 if no alignment required
          Hap * hs = Hap::getInstance(&p);
     #endif

     ioctl_flag = atoi(argv[6]);
     
     array_a = (float *)sds_alloc(mat_dim*mat_dim*sizeof(int));
     array_b = (float *)sds_alloc(mat_dim*mat_dim*sizeof(int));
     array_c = (float *)sds_alloc(mat_dim*mat_dim*sizeof(int));

     if(!array_a || !array_b || !array_c) {
        cerr << "ERROR: Unable to allocate memory\n" <<endl;
        exit(1);
     }

     //Only allocate noncache memory if HP enabled accelerators are selected
     if (numhpacc > 0){    
          array_a_noncache = (float *)sds_alloc_non_cacheable(mat_dim*mat_dim*sizeof(int));
          array_b_noncache = (float *)sds_alloc_non_cacheable(mat_dim*mat_dim*sizeof(int));
          array_c_noncache = (float *)sds_alloc_non_cacheable(mat_dim*mat_dim*sizeof(int));
          if(!array_a_noncache || !array_b_noncache || !array_c_noncache)
          {
             cerr << "ERROR: Unable to allocate noncache memory\n" <<endl;
             freemem();
             exit(1);
          }          
     }
     (debug_flag) && (printf("Memories allocated\n"));
     
     srand(time(0));
     for(int i=0;i<mat_dim;i++)
          for(int j=0;j<mat_dim;j++)
               array_a[i*mat_dim+j]=float_rand(-150000,150000);

     for(int i=0;i<mat_dim;i++)
          for(int j=0;j<mat_dim;j++)
               array_b[i*mat_dim+j]=float_rand(-150000,150000);

     for(int i=0;i<mat_dim;i++)
          for(int j=0;j<mat_dim;j++)
               array_c[i*mat_dim+j]=0;

     cout << "GEMM Simulation: "<< mat_dim << ", " << p.numcpus << ", " << p.numgpus << endl;

     //Interrupt device drivers defined in Body.h
     //Only open if ioctl_flag is enabled
     if (ioctl_flag > 0){
     /*Interrupt drivers*/
          file_desc_1 = open(DRIVER_FILE_NAME_1, O_RDWR);	//Open interrupt driver 1
          if (file_desc_1 < 0) {
               fprintf(stderr,"Can't open driver file: %s\n", DRIVER_FILE_NAME_1);
               exit(-1);
          } else {
               (debug_flag) && (fprintf(stderr,"Driver successfully opened: %s\n", DRIVER_FILE_NAME_1));
          }
          file_desc_2 = open(DRIVER_FILE_NAME_2, O_RDWR);	//Open interrupt driver 2
          if (file_desc_2 < 0) {
               fprintf(stderr,"Can't open driver file: %s\n", DRIVER_FILE_NAME_2);
               exit(-1);
          } else {
               (debug_flag) && (fprintf(stderr,"Driver successfully opened: %s\n", DRIVER_FILE_NAME_2));
          }
          file_desc_3 = open(DRIVER_FILE_NAME_3, O_RDWR);	//Open interrupt driver 3
          if (file_desc_3 < 0) {
               fprintf(stderr,"Can't open driver file: %s\n", DRIVER_FILE_NAME_3);
               exit(-1);
          } else {
               (debug_flag) && (fprintf(stderr,"Driver successfully opened: %s\n", DRIVER_FILE_NAME_3));
          }
          file_desc_4 = open(DRIVER_FILE_NAME_4, O_RDWR);	//Open interrupt driver 4
          if (file_desc_4 < 0) {
               fprintf(stderr,"Can't open driver file: %s\n", DRIVER_FILE_NAME_4);
               exit(-1);
          } else {
               (debug_flag) && (fprintf(stderr,"Driver successfully opened: %s\n", DRIVER_FILE_NAME_4));
          }
     }
     
     #ifdef MULTIHAP
          cout << "BEGIN CALIBRATE:" << endl;
     
          //Multihap needs some time to find the correct solutions so we start it furst with 10 iters and then we update
          // Copy h_vec into h_vec_trans
          //Do it for 5 iterations
          for (int step = 0; step < 5; step++){
               (debug_flag) && (fprintf(stderr,"Calibration step: %d\n", step));
               hs->heterogeneous_parallel_for(0, mat_dim, &body);
          }
          
          cout << "BEGIN FINALRUN" << endl;    
          //reset bodies
          bodies_F=0;
          bodies_C=0;
     #endif

	// Perform the computation     
     hs->startTimeAndEnergy();
     int iters = 1;
          
     for (int step = 0; step < iters; step++) {
          (debug_flag) && (fprintf(stderr,"Iteration: %d\n", step));
          hs->heterogeneous_parallel_for(0, mat_dim, &body);
     }
     hs->endTimeAndEnergy();

     if (ioctl_flag > 0) {
          //Close interrupt drivers
          (debug_flag) && (fprintf(stderr,"Closing driver: %s\n", DRIVER_FILE_NAME_1));
          close(file_desc_1);
          (debug_flag) && (fprintf(stderr,"Closing driver: %s\n", DRIVER_FILE_NAME_2));
          close(file_desc_2);
          (debug_flag) && (fprintf(stderr,"Closing driver: %s\n", DRIVER_FILE_NAME_3));
          close(file_desc_3);
          (debug_flag) && (fprintf(stderr,"Closing driver: %s\n", DRIVER_FILE_NAME_4));
          close(file_desc_4);
     }   

     #ifdef MULTIHAP
          printf("Final FPGA Chunk: %d\n", finalGPUchunk/p.numgpus);
     #endif
     printf("Total workload : %lu\n", bodies_F+bodies_C);
     printf("Total workload on CPU: %lu\n", bodies_C);
     printf("Total workload on FPGA: %lu\n", bodies_F);
     printf("Percentage of work done by the FPGA: %lu \n",bodies_F*100/(bodies_F+bodies_C));

     //open and write data to the file
     if (argv[7]) {
          cout << "Printing resulting matrix to: " << argv[7] << endl;
          writeoutput(argv[7]);
     }
     
     if (calc_golden) {
          printf("Print golden solution\n");
          c_golden=(float *)alignedMalloc(mat_dim*mat_dim*sizeof(int));
          golden_MMM(array_a, array_b, c_golden,0,mat_dim);
          free(c_golden);
     }
     
     freemem();
     
     return 0;
}