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
#include "aes_enc.h"
#include "sbox.h"
#ifndef __SDSVHLS__
//#include "uio_axi_gpio.h"
#endif


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

int main_thread=0;
int main_core=0;
int block_size; //how many blocks of 16 bytes to process

long unsigned int bodies_C=0, bodies_F=0;



////#define MAP_SIZE 4096UL
////#define MAP_MASK (MAP_SIZE - 1)

#ifdef ORACLE
#include "Oracle.h"
#endif
#ifdef DYNAMIC
#include "../Schedulers/DynamicScheduler.h"
HSchedulerDynamic hs;
#define EXTRA 1
typedef int extraType;
#endif
#ifdef FIXEDCHUNK
#include "FixedScheduler.h"
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


#include "Body.h"

using namespace std;
using namespace tbb;

char **argv;


/*****************************************************************************
 * Global variables
 * **************************************************************************/
ofstream resultfile;



const unsigned AOCL_ALIGNMENT = 64;
void *alignedMalloc(size_t size) {
  void *result = NULL;
  posix_memalign (&result, AOCL_ALIGNMENT, size);
  return result;
}

void alignedFree(void * ptr) {
  free (ptr);
}


void keyexpansion(uint8_t key[32], uint8_t ekey[240])
{
	  uint32_t i, j, k;
	  uint8_t temp[4];

	  for(i = 0; i < nk; ++i)
	  {
	    ekey[(i * 4) + 0] = key[(i * 4) + 0];
	    ekey[(i * 4) + 1] = key[(i * 4) + 1];
	    ekey[(i * 4) + 2] = key[(i * 4) + 2];
	    ekey[(i * 4) + 3] = key[(i * 4) + 3];
	  }


	  for(; (i < (nb * (nr + 1))); ++i)
	  {
	    for(j = 0; j < 4; ++j)
	    {
	      temp[j]= ekey[(i-1) * 4 + j];
	    }
	    if (i % nk == 0)
	    {
	      {
	        k = temp[0];
	        temp[0] = temp[1];
	        temp[1] = temp[2];
	        temp[2] = temp[3];
	        temp[3] = k;
	      }


	      {
	        temp[0] = sbox[temp[0]];
	        temp[1] = sbox[temp[1]];
	        temp[2] = sbox[temp[2]];
	        temp[3] = sbox[temp[3]];
	      }

	      temp[0] =  temp[0] ^ Rcon[i/nk];
	    }
	    else if (nk > 6 && i % nk == 4)
	    {
	      // Function Subword()
	      {
	        temp[0] = sbox[temp[0]];
	        temp[1] = sbox[temp[1]];
	        temp[2] = sbox[temp[2]];
	        temp[3] = sbox[temp[3]];
	      }
	    }
	    ekey[i * 4 + 0] = ekey[(i - nk) * 4 + 0] ^ temp[0];
	    ekey[i * 4 + 1] = ekey[(i - nk) * 4 + 1] ^ temp[1];
	    ekey[i * 4 + 2] = ekey[(i - nk) * 4 + 2] ^ temp[2];
	    ekey[i * 4 + 3] = ekey[(i - nk) * 4 + 3] ^ temp[3];
	  }

}


int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}





void read_input(uint8_t *state, FILE *fp)
{
	

	
	fread(state, (16*block_size), 1, fp); // Read in the entire block
	
	

	fclose(fp);
}


void write_output(uint8_t *state, char *file)
{
	FILE *fp;

	fp = fopen(file, "wb");
	if (!fp)
	{
		printf("file could not be opened for writing\n");
		exit(1);
	}
	fwrite(state,(16*block_size), 1, fp); // Read in the entire block
	fclose(fp);
}

/*
   compute N time steps
*/


////int compute_aes(uint32_t *state, uint32_t *cipher, uint8_t ekey[240], int* interrupt)
int compute_aes(uint8_t *state, uint8_t *cipher, uint8_t ekey[240])
 
{ 

	//variables
	Body body;
	Params p;
	p.numcpus = atoi(argv[3]);
	p.numgpus = atoi(argv[4]);

	int t;

	//printf("call init\n");

 	#ifdef MULTIDYNAMIC
  	p.gpuChunk = atoi(argv[5]);
  	Dynamic * hs = Dynamic::getInstance(&p);
  	#endif
	
	//task_scheduler_init init(numcpus+numgpus);
	//hs.Initialize(numcpus, numgpus, atoi(argv[3]));//,interrupt);
	// Begin iterations
	hs->startTimeAndEnergy();
        int total_iterations = 1;

	//printf("being interactions\n");

	for (t = 0; t < total_iterations; t += 1) {
		
	//	mystep = t;
	//	printf("call parallel for\n");
		hs->heterogeneous_parallel_for(0, block_size, &body);
		
	}
	

	

	hs->endTimeAndEnergy();
	hs->saveResultsForBench();
	return 0;
}

void usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s <input_file> <output_file> <numcpus> <numgpus> <chunkGPU>\n", argv[0]);
	fprintf(stderr, "\t<inputfile> - \n");
	fprintf(stderr, "\t<outputfile> - \n");
	fprintf(stderr, "\t<numcpus> - \n");
	fprintf(stderr, "\t<numgpus> - \n");
	fprintf(stderr, "\t<chunkGPU> - (Fixed or Dynamic) fixed size size assigned to GPU (CPU adapts dynamically to match effort)\n");
	exit(1);
}




int main(int argc, char** argvv) {


	
	

   static char receive[10];

    printf("Starting\n");
	//end variables
	argv = argvv;

   // #ifndef __SDSVHLS__
  /*      printf("Checking interrupt LEDs ON\n");
	axi_gpio axi_gpio_0;
	int status_gpio = axi_gpio_init(&axi_gpio_0, "gpio");
	if (status_gpio < 0) {
		perror("axi_gpio_init");
		return -1;}*/

////***************comment out the following interrupt-related code******************* 
////    int memfd;
////    void *mapped_base, *mapped_dev_base;
////    off_t dev_base = HW_ADDR_GPIO; //GPIO hardware


////    memfd = open("/dev/mem", O_RDWR | O_SYNC);
////    if (memfd == -1) {
////    	printf("Can't open /dev/mem.\n");
////        exit(0);
////    }
////    printf("/dev/mem opened for gpio.\n");

    // Map one page of memory into user space such that the device is in that page, but it may not
      // be at the start of the page.


////      mapped_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~MAP_MASK);
////          if (mapped_base == (void *) -1) {
////          printf("Can't map the memory to user space.\n");
////          exit(0);
////      }
////       printf("GPIO mapped at address %p.\n", mapped_base);



      // get the address of the device in user space which will be an offset from the base
      // that was mapped as memory is mapped at the start of a page

////      mapped_dev_base = mapped_base + (dev_base & MAP_MASK);




////  file_desc = open("/dev/my_driver",O_RDWR);
////  if (file_desc < 0)
////  {	
////		printf("cant open device: /dev/my_driver\n");
////		exit(0);
////  }
  
  // #endif
////   char led_value = 255;

////    #ifndef __SDSVHLS__
               //   axi_gpio_write_reg(axi_gpio_0.baseaddr, 0, (unsigned char) led_value);
////    #endif


   // interrupt = (int *) sds_alloc(sizeof(int));


////    interrupt = (int*)mapped_dev_base;
    /*int* enable_channel = (int*)mapped_dev_base + fpga_mmio_ier;
    int* enable_general = (int*)mapped_dev_base + fpga_mmio_gier;*/

////    status = (int*)mapped_dev_base+fpga_mmio_gpio; //(int*)read(file_desc,receive,10); //axi_gpio_0.baseaddr;

    //status = (int*)mapped_dev_base;

////    sds_mmap((void *)HW_ADDR_GPIO_INT,4,(void *)status);

    //sds_mmap((void *)HW_ADDR_GPIO,4,(void *)status);

////    printf("Interrupt located at address %x\n",(int)status);

////    printf("Switch on leds\n");
    
////    *interrupt = (unsigned char) led_value; //leds on


   /* *enable_channel = 0xFFFFFFFF; //leds on
    *enable_general = 0xFFFFFFFF; //leds on*/

    // printf("press key to continue\n");  
    // getchar();
     //*status = 255;
    
////     printf("Switch off leds\n");

////     led_value = 0;

////    *interrupt = (unsigned char) led_value; //leds off
//*********************************
    
     int total_iterations,i,j,ret;

	
	if (argc < 3)
		usage(argc, argv);





    #ifdef HOSTA7
	main_thread=syscall(__NR_gettid);
	main_core=sched_getcpu();
	cout << "Main thread id: " << main_thread << endl;
	cout << "          core: " << main_core << endl;
    #endif

	char ifile[40], ofile[40];

	strcpy(ifile, argv[1]);
	strcpy(ofile, argv[2]);	


	//ekey = (uint8_t *)sds_alloc(240 * sizeof(uint8_t));
	//state = (uint32_t*)sds_alloc(16*(block_size+1) * sizeof(uint8_t));
	//cipher = (uint32_t*)sds_alloc(16*(block_size+1) * sizeof(uint8_t));

	FILE *fp;

	fp = fopen(ifile, "rb");
	if (!fp)
	{
		printf("file could not be opened for reading\n");
		exit(1);
	}

	block_size = fsize(fp)/16; //number of 16 byte blocks in file

	//block_size = 1048576;
	//block_size = 262144;


	printf("The number of blocks to encrypt is %d\n",block_size);
	printf("The chunk size for FPGA as a number of blocks is %d\n",atoi(argv[5]));


	//block_size =  1048576;
	
	ekey = (uint8_t *)malloc(240 * sizeof(uint8_t));
	state = (uint8_t*)malloc(16*block_size* sizeof(uint8_t));
	cipher = (uint8_t*)malloc(16*block_size * sizeof(uint8_t));

   if(!state) {printf("Fail to allocate memory\n");exit(1);}
  if(!cipher) {printf("Fail to allocate memory\n");exit(1);}

    printf("Memories allocated with %d bytes\n",(block_size*16));


	keyexpansion(key, ekey);

	read_input(state, fp);


        printf("Starting computation\n");


       // interrupt = &local_interrupt;


	// Perform the computation
////	compute_aes(state, cipher,ekey,interrupt);
	compute_aes(state, cipher,ekey);

	write_output(cipher, ofile);


	/*for(int x=0;x<4;x++){
			printf(" %x", cipher[x]);
	}
	printf("\n");*/
/*
   	printf("Verifying\n");

	golden_MMM(array_a, array_b, c_golden);

        int status_op = 0;	

	for(i = 0; i < N*P; i++) {
		float fc = array_c[i];
		float gc = c_golden[i];
		float diff = fabs(fc-gc);
		if (diff > 0.001 || diff != diff) {
			printf("Error at %d Fast_MMM=%f  Golden MMM=%f\n", i, fc, gc);
			status_op = -1;
			break;
		}
	}

	if (!status_op) {
		printf("Evaluation PASSED\n");
	} else {
		printf("Evaluation FAILED\n");
	}
		
 */
    

	printf("releasing memory\n");
	sds_free(ekey);
	sds_free(state);
	sds_free(cipher);

	
   	printf("Total n. rows : %d\n", bodies_F+bodies_C);
   	printf("Total n. rows on CPU: %d\n", bodies_C);
   	printf("Total n. rows on FPGA: %d\n", bodies_F);
   	printf("Actual percentage of work offloaded to the FPGA:%d%% \n",bodies_F*100/(bodies_F+bodies_C));

////  	sds_munmap((void *)status);
////	munmap(mapped_base, MAP_SIZE);
////	close(memfd);
////	close(file_desc);


	return 0;
}
