//============================================================================
// Name			: main.cpp
// Author		: Antonio Vilches
// Version		: 1.0
// Date			: 13 / 01 / 2015
// Copyright	: Department. Computer's Architecture (c)
// Description	: Main file of SPMM 
//============================================================================

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
//#define MULTIHAP

#include "kernelspmm.h"

#define NDEBUG

#ifndef ITERS
#define ITERS 1
#endif

long unsigned int bodies_C=0, bodies_F=0;

// compile in broadwell1 using:  
//            make -f Makefile.andres CFLAGS=-DMALAGA
//#define MALAGA  

#ifdef MALAGA
#define sds_alloc_non_cacheable malloc
#else
#include <sds_lib.h> 
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "SPMM.h"

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
#ifdef MULTIHAP
#include "MultiHap.h"
#endif
#include "Body.h"

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


/*****************************************************************************
 * Main Function
 * **************************************************************************/
int main (int argc, char * argv[]){
	//variables
	Body body;
	Params p;
	//end variables

	if (argc != 7 && argc != 8) {
		fprintf(stderr, "arguments: inFile numcpus numhpacc numhpcacc chunk ioctl (matrixout)\n");
		exit(-1);
	}

	p.numcpus	= atoi(argv[2]);	 
     numhpacc = atoi(argv[3]);
     numhpcacc = atoi(argv[4]);
     ioctl_flag = atoi(argv[6]);
     
     if(p.numcpus > 4){
          fprintf(stderr, "System can use up to 4 CPU cores. Invalid number of cores specified: %d!\n", p.numcpus);
          exit(1);
     }
     
     if(numhpacc > 4){
          fprintf(stderr, "System can use up to 4 HP enabled accelerators. Invalid number of accelerators specified: %d!\n", numhpacc);
          exit(1);
     }
     
     if(numhpcacc > 4){
          fprintf(stderr, "System can use up to 4 HPC enabled accelerators. Invalid number of accelerators specified: %d!\n", numhpcacc);
          exit(1);
     }
     
     if(numhpacc > 0 && numhpcacc > 0){
          fprintf(stderr, "Please use either an HP or and HPC hardware configuration!\n");
          exit(1);
     }
     
     p.numgpus	= numhpacc + numhpcacc;
     	
	sprintf(p.benchName, "SPMM");
	sprintf(p.kernelName, "spmm_csr_scalar_kernel");

/*Initializing scheduler*/
#ifdef DYNAMIC
	p.gpuChunk = atoi(argv[5]);
	Dynamic * hs = Dynamic::getInstance(&p);
#endif
#ifdef MULTIDYNAMIC
	p.gpuChunk = atoi(argv[5]);
	Dynamic * hs = Dynamic::getInstance(&p);
#endif
#ifdef MULTIHAP // new multihap
     p.chunkGPU_initratio = (float) atof(argv[5]);
     // lcm == least common multiple
     // the scheduler will tune chunk size to lay in a 64 bytes offset block
     int alignment = lcm(64,x_width * sizeof(DTYPE))/(x_width * sizeof(DTYPE)); //find best alignment for chunnk sizes
     cout << "ALING CHUNK SIZE: " << alignment << endl;
     p.CHUNKCPUALIGN = alignment; // set to 1 if no alignment required
     p.CHUNKGPUALIGN = alignment; // set to 1 if no alignment required
     Hap * hs = Hap::getInstance(&p);
#endif
	
	cerr << "SPMM Simulation: "<< argv[1] << ", " << p.numcpus << ", " << p.numgpus << endl;

	cerr << "Reading Matrix Input File" << endl;
	char filename[FIELD_LENGTH];
     readMatrix(argv[1], &h_val, &h_cols, &h_rowDelimiters, &nItems, &numRows, &numColumns);
     if ((!h_val) || (!h_cols) || (!h_rowDelimiters)) {
          fprintf(stderr,"Host sds_alloc failed (input matrix)\n");
          exit(1);
     }
     else
     {
          (debug_flag) && (fprintf(stderr,"Host sds_alloc sucess (input matrix)\n"));	
     }     
         
     //Only allocate noncache memory if HP enabled accelerators are selected
     if (numhpacc > 0){    
          h_val_noncache =  (DTYPE *)sds_alloc_non_cacheable(nItems * sizeof(DTYPE));
          h_cols_noncache = (int *)sds_alloc_non_cacheable(nItems * sizeof(int));
          h_rowDelimiters_noncache = (int *)sds_alloc_non_cacheable((numRows+1) * sizeof(int));
          if ((!h_val_noncache) || (!h_cols_noncache) || (!h_rowDelimiters_noncache)) {
               fprintf(stderr,"Host sds_alloc_non_cacheable failed (input matrix)\n");
               exit(1);
          }
          else
          {
               (debug_flag) && (printf("Host sds_alloc_non_cacheable sucess (input matrix)\n"));	
               (debug_flag) && (printf("numRows->%d nItems->%d\n",numRows,nItems));
          }
     }

	h_vec = (DTYPE *)sds_alloc(numRows * x_width * sizeof(DTYPE));
	h_out = (DTYPE *)sds_alloc(numRows * x_width * sizeof(DTYPE));
	if ((!h_vec) || (!h_out)) {
			fprintf(stderr,"Host sds_alloc failed (output matrix)\n");
			exit(1);
	}
	else
	{
		(debug_flag) && (fprintf(stderr,"Host sds_alloc sucess (output matrix)\n"));	
	}
     
     //Allocate transpose matrices for use in the hardware accelerators
     h_vec_trans = (DTYPE *)sds_alloc(numRows * x_width * sizeof(DTYPE));
	h_out_trans = (DTYPE *)sds_alloc(numRows * x_width * sizeof(DTYPE));
	if ((!h_vec_trans) || (!h_out_trans)) {
			fprintf(stderr,"Host sds_alloc failed (output matrix)\n");
			exit(1);
	}
	else
	{
		(debug_flag) && (fprintf(stderr,"Host sds_alloc sucess (output matrix)\n"));	
	} 

     if (numhpacc > 0){
          h_vec_noncache_trans = (DTYPE *)sds_alloc_non_cacheable(numRows * x_width * sizeof(DTYPE));
          h_out_noncache_trans = (DTYPE *)sds_alloc_non_cacheable(numRows * x_width * sizeof(DTYPE));
          if ((!h_vec_noncache_trans) || (!h_out_noncache_trans)) {
               fprintf(stderr,"Host sds_alloc_non_cacheable failed (output matrix)\n");
               exit(1);
          }
          else
          {
               (debug_flag) && (fprintf(stderr,"Host sds_alloc_non_cacheable sucess (output matrix)\n"));	
          }          
     }
	
	cerr << "Number of Rows: " << numRows << endl; 
	numNonZeroes = nItems;
	cerr << "NNZ: " << numNonZeroes << endl;
	
	iters = ITERS;
     
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
          };
          file_desc_2 = open(DRIVER_FILE_NAME_2, O_RDWR);	//Open interrupt driver 2
          if (file_desc_2 < 0) {
               fprintf(stderr,"Can't open driver file: %s\n", DRIVER_FILE_NAME_2);
               exit(-1);
          } else {
               (debug_flag) && (fprintf(stderr,"Driver successfully opened: %s\n", DRIVER_FILE_NAME_2));
          };
          file_desc_3 = open(DRIVER_FILE_NAME_3, O_RDWR);	//Open interrupt driver 3
          if (file_desc_3 < 0) {
               fprintf(stderr,"Can't open driver file: %s\n", DRIVER_FILE_NAME_3);
               exit(-1);
          } else {
               (debug_flag) && (fprintf(stderr,"Driver successfully opened: %s\n", DRIVER_FILE_NAME_3));
          };
          file_desc_4 = open(DRIVER_FILE_NAME_4, O_RDWR);	//Open interrupt driver 4
          if (file_desc_4 < 0) {
               fprintf(stderr,"Can't open driver file: %s\n", DRIVER_FILE_NAME_4);
               exit(-1);
          } else {
               (debug_flag) && (fprintf(stderr,"Driver successfully opened: %s\n", DRIVER_FILE_NAME_4));
          };        
     }
     
     for (int row=0;row<numRows;row++)
          for (int xw=0;xw<x_width;xw++)
          {
               *(h_vec+row*x_width+xw)=(DTYPE)row;
          }
     
     #ifdef MULTIHAP
     
     cout << "BEGIN CALIBRATE:" << endl;
     
          //Multihap needs some time to find the correct solutions so we start it furst with 10 iters and then we update
          // Copy h_vec into h_vec_trans
          for(int rowix = 0; rowix < numRows; rowix++)
          {
               for(int colix = 0; colix < x_width; colix++)
               {
                    h_vec_trans[colix*(numRows)+rowix] = h_vec[rowix*(x_width)+colix];
                    //Copy to noncache mem if HP enabled accelerators are used
                    (numhpacc > 0) && (h_vec_noncache_trans[colix*(numRows)+rowix] = h_vec[rowix*(x_width)+colix]);
               }
          }
          //Do it for 10 iterations
          for (int step = 0; step < 15; step++){
               cerr << "STEP: " << step << endl;
               hs->heterogeneous_parallel_for(0, numRows, &body);
          }
          
          cout << "BEGIN FINALRUN" << endl;    
          //reset bodies
          bodies_F=0;
          bodies_C=0;
     #endif
     
     // Begin iterations
     (debug_flag) && (fprintf(stderr,"DBG 0\n"));
	hs->startTimeAndEnergy();
     (debug_flag) && (fprintf(stderr,"DBG 1\n"));

     // Copy h_vec into h_vec_trans
     for(int rowix = 0; rowix < numRows; rowix++)
     {
          for(int colix = 0; colix < x_width; colix++)
          {
               h_vec_trans[colix*(numRows)+rowix] = h_vec[rowix*(x_width)+colix];
               //Copy to noncache mem if HP enabled accelerators are used
               (numhpacc > 0) && (h_vec_noncache_trans[colix*(numRows)+rowix] = h_vec[rowix*(x_width)+colix]);
          }
     }     
     
     //run to collect data
   	//iters = 200;
	for (int step = 0; step < iters; step++){
          (debug_flag) && (fprintf(stderr,"DBG 2.%d\n",step));
		hs->heterogeneous_parallel_for(0, numRows, &body);
	}
	(debug_flag) && (fprintf(stderr,"DBG 5\n"));
	hs->endTimeAndEnergy();
	hs->saveResultsForBench();
     (debug_flag) && (fprintf(stderr,"DBG 6\n"));

     //open and write data to the file
     if (argv[7]) {
          ofstream out(argv[7]);    
          for (int i=0; i<numRows; i++) {
               for(int xw = 0; xw < x_width; xw++) {                
                    out << h_out[i*x_width+xw] << "\n"; 
               }
          }
          out.close();
     }
     (debug_flag) && (fprintf(stderr,"DBG 7\n"));
     //Free memory
     sds_free(h_vec);
     sds_free(h_out);
     sds_free(h_val);
     sds_free(h_cols);
     sds_free(h_rowDelimiters);
     sds_free(h_vec_trans);
     sds_free(h_out_trans);
     //Free memory for HP port accelerators
     if (numhpacc > 0){     
          sds_free(h_vec_noncache_trans);
          sds_free(h_out_noncache_trans);
          sds_free(h_val_noncache);
          sds_free(h_cols_noncache);
     }
     (debug_flag) && (fprintf(stderr,"DBG 8\n"));

     
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
     (debug_flag) && (fprintf(stderr,"DBG 9\n"));

     #ifdef MULTIHAP
          printf("Final GPU Chunk: %d\n", finalGPUchunk/p.numgpus);
     #endif
     printf("Total n. rows : %lu\n", bodies_F+bodies_C);
     printf("Total n. rows on CPU: %lu\n", bodies_C);
     printf("Total n. rows on FPGA: %lu\n", bodies_F);
     printf("Actual percentage of work offloaded to the FPGA: %lu \n",bodies_F*100/(bodies_F+bodies_C));
}
