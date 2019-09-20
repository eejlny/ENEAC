#include <stdio.h>
#include <sds_lib.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
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

int main(int argc, char* argv[]) {
     printf("Starting\n");
     
     //variables	
     Body body;
	Params p;
     char *tfile, *pfile;
    	
     if (argc < 9)
		usage(argc, argv);
	if(  atoi(argv[1])<=0 ||
          ( atoi(argv[4]) != 0 && atoi(argv[4]) != 1 ) ||
          atoi(argv[5]) < 0 || atoi(argv[5]) > 4 
          || atoi(argv[6]) < 0 
          || atoi(argv[7]) < 0
          || atoi(argv[8]) < 0 
          || ( atoi(argv[9]) != 0 && atoi(argv[9]) != 1 ) )
		usage(argc, argv);
     else
          grid_rows = atoi(argv[1]);
          grid_cols = grid_rows;
     tfile = argv[2];
     pfile = argv[3];
	cont_read = atoi(argv[4]);
     
     p.numcpus = atoi(argv[5]);
     numhpacc = atoi(argv[6]);
     numhpcacc = atoi(argv[7]);	
     p.numgpus	= numhpacc + numhpcacc;
     
	#ifdef MULTIDYNAMIC
  		p.gpuChunk = atoi(argv[8]);
  		Dynamic * hs = Dynamic::getInstance(&p);
  	#endif
     #ifdef MULTIHAP // new multihap
          p.chunkGPU_initratio = (float) atof(argv[8]);
          // lcm == least common multiple
          // the scheduler will tune chunk size to lay in a 64 bytes offset block
          int alignment = lcm(2, grid_cols * sizeof(float))/(grid_cols * sizeof(float)); //find best alignment for chunnk sizes
          cout << "ALING CHUNK SIZE: " << alignment << endl;
          p.CHUNKCPUALIGN = alignment; // set to 1 if no alignment required
          p.CHUNKGPUALIGN = alignment; // set to 1 if no alignment required
          Hap * hs = Hap::getInstance(&p);
     #endif
     
     
     ioctl_flag = atoi(argv[9]);
    
     size=(grid_rows+2)*(grid_cols+2);
     array_temp = (float *)sds_alloc(size*sizeof(float));
     array_power = (float *)sds_alloc(size*sizeof(float));
     array_out = (float *)sds_alloc(size*sizeof(float));
     if(!array_temp || !array_power || !array_out)
     {
        cerr << "ERROR: Unable to allocate memory\n" <<endl;
        exit(1);
     }

     //Only allocate noncache memory if HP enabled accelerators are selected
     if (numhpacc > 0){    
          array_temp_noncache =  (float *)sds_alloc_non_cacheable(size*sizeof(float));
          array_power_noncache = (float *)sds_alloc_non_cacheable(size*sizeof(float));
          array_out_noncache = (float *)sds_alloc_non_cacheable(size*sizeof(float));
          if(!array_temp_noncache)
          {
             cerr << "ERROR: Unable to allocate noncache memory\n" <<endl;
             freemem();
             exit(1);
          }          
     }
     (debug_flag) && (printf("Memories allocated\n"));
    	
     // Read input data from disk
     readinput(array_temp, (grid_rows+2), (grid_cols+2), tfile);
     readinput(array_power, (grid_rows+2), (grid_cols+2), pfile); 
        
     //Interrupt device drivers defined in HOTSPOT.h
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

     grid_height = chip_height / grid_rows;
	grid_width = chip_width / grid_cols;
	Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	Rx = grid_width / (2.0 * K_SI * t_chip * grid_height);
	Ry = grid_height / (2.0 * K_SI * t_chip * grid_width);
	Rz = t_chip / (K_SI * grid_height * grid_width);
	max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	step = PRECISION / max_slope;
     frame_width = grid_rows;
     
     cout << "HOTSPOT Simulation: "<< grid_rows << ", " << p.numcpus << ", " << p.numgpus << endl;
     
     #ifdef MULTIHAP
     
          cout << "BEGIN CALIBRATE:" << endl;
     
          //Multihap needs some time to find the correct solutions so we start it furst with 10 iters and then we update
          // Copy h_vec into h_vec_trans
          //Do it for 5 iterations
          for (int step = 0; step < 5; step++){
               (debug_flag) && (fprintf(stderr,"Calibration step: %d\n", step));
               hs->heterogeneous_parallel_for(0, grid_rows, &body);
          }
          
          cout << "BEGIN FINALRUN" << endl;    
          //reset bodies
          bodies_F=0;
          bodies_C=0;
     #endif

	
	// Perform the computation     
     int iters = 1;
     hs->startTimeAndEnergy();
	for (int step = 0; step < iters; step++){
          (debug_flag) && (fprintf(stderr,"Iteration: %d\n", step));
          hs->heterogeneous_parallel_for(0, grid_rows, &body);
	}
	hs->endTimeAndEnergy();
     
     if (ioctl_flag > 0){
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
     
     // Write final output to output file
	if (argv[10]) {
          writeoutput(array_out, (grid_rows+2), (grid_cols+2), argv[9]);
     }
     
     freemem();

	return 0;
}
