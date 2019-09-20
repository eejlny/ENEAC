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
#include "sbox.h"

#ifdef MULTIDYNAMIC
     #include "MultiDynamic.h"
#endif
#ifdef MULTIHAP
     #include "MultiHap.h"
#endif

#include "Body.h"

using namespace std;
using namespace tbb;

int main(int argc, char* argv[]) {
     printf("Starting\n");
     
     //variables
	Body body;
	Params p;
     
     if (argc < 7 || argc > 8)
		usage(argc, argv);
	if(  atoi(argv[2]) < 0 || atoi(argv[2]) > 4 
          || atoi(argv[3]) < 0 
          || atoi(argv[4]) < 0
          || atoi(argv[5]) < 0 
          || ( atoi(argv[6]) != 0 && atoi(argv[6]) != 1 ) )
		usage(argc, argv);
     else
     
     infile = argv[1];
     p.numcpus = atoi(argv[2]);
     numhpacc = atoi(argv[3]);
     numhpcacc = atoi(argv[4]);	
     p.numgpus	= numhpacc + numhpcacc;
     
	#ifdef MULTIDYNAMIC
  		p.gpuChunk = atoi(argv[5]);
  		Dynamic * hs = Dynamic::getInstance(&p);
  	#endif

     ioctl_flag = atoi(argv[7]);

	fp = fopen(infile, "rb");
	if (!fp) {
          cerr << "ERROR: Unable to open input file;\n" <<endl;
		exit(1);
	}

	block_size = fsize(fp)/16; //number of 16 byte blocks in file
	(debug_flag) && (printf("The number of blocks to encrypt is %d;\n",block_size));
	(debug_flag) && (printf("The chunk size for FPGA as a number of blocks is %d;\n",p.gpuChunk));

	ekey = (uint8_t *)sds_alloc(240 * sizeof(uint8_t));
	state = (uint8_t*)sds_alloc(16*(block_size+1) * sizeof(uint8_t));
	cipher = (uint8_t*)sds_alloc(16*(block_size+1) * sizeof(uint8_t));
     
     if(!ekey || !state || !cipher) {
        cerr << "ERROR: Unable to allocate memory\n" <<endl;
        exit(1);
     }
     
     //Only allocate noncache memory if HP enabled accelerators are selected
     if (numhpacc > 0){    
          ekey_noncache = (uint8_t *)sds_alloc_non_cacheable(240 * sizeof(uint8_t));
          state_noncache = (uint8_t *)sds_alloc_non_cacheable(16*(block_size+1) * sizeof(uint8_t));
          cipher_noncache = (uint8_t *)sds_alloc_non_cacheable(16*(block_size+1) * sizeof(uint8_t));
          if(!ekey_noncache || !state_noncache || !cipher_noncache)
          {
             cerr << "ERROR: Unable to allocate noncache memory\n" <<endl;
             freemem();
             exit(1);
          }          
     }
     
     (debug_flag) && (printf("Memories allocated;\n"));

	keyexpansion(key, ekey);     
     if (numhpacc > 0)
          keyexpansion(key, ekey_noncache);     
     (debug_flag) && (printf("Key expanded;\n"));
          
	read_input(state, fp);
     (debug_flag) && (printf("Input read;\n"));

     
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
     }
     
     cout << "AES Simulation: "<< block_size << ", " << p.numcpus << ", " << p.numgpus << endl;

     // Perform the computation     
     int iters = 1;
     hs->startTimeAndEnergy();
	for (int step = 0; step < iters; step++){
          (debug_flag) && (fprintf(stderr,"Iteration: %d\n", step));
		hs->heterogeneous_parallel_for(0, block_size, &body);
	}
	hs->endTimeAndEnergy();
     	
     printf("Total workload : %lu\n", bodies_F+bodies_C);
     printf("Total workload on CPU: %lu\n", bodies_C);
     printf("Total workload on FPGA: %lu\n", bodies_F);
     printf("Percentage of work done by the FPGA: %lu \n",bodies_F*100/(bodies_F+bodies_C));

     fclose(fp);
     if (ioctl_flag > 0){
          //Close interrupt drivers
          (debug_flag) && (fprintf(stderr,"Closing driver: %s\n", DRIVER_FILE_NAME_1));
          close(file_desc_1);
          (debug_flag) && (fprintf(stderr,"Closing driver: %s\n", DRIVER_FILE_NAME_2));
          close(file_desc_2);
     }     

     // Write final output to output file
	if (argv[7]) {
          write_output(cipher, argv[7]);
     }
     
     freemem();

	return 0;
}
