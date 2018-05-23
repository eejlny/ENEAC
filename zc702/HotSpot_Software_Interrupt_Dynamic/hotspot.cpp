#include <stdio.h>
#include <sds_lib.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <fcntl.h>
#include <sys/mman.h>

#include "hotspot.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/tbb.h"


//#define DEBUG
//#define DEEP_CPU_REPORT
//#define DEEP_GPU_REPORT
#define DYNAMIC
//#define ORACLE
//#define FIXEDCHUNK
//#define CONCORD
//#define HDSSS
//#define LOGFIT
//#define PJTRACER
//#define OVERHEAD_STUDY

int main_thread=0;
int main_core=0;


#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

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


void writeoutput(float *vect, int grid_rows, int grid_cols, char *file) {

	int i,j, index=0;
	FILE *fp;
	char str[STR_SIZE];

	if( (fp = fopen(file, "w" )) == 0 )
          printf( "The file was not opened\n" );


	for (i=0; i < grid_rows; i++) 
	 for (j=0; j < grid_cols; j++)
	 {

		 sprintf(str, "%d\t%g\n", index, vect[i*grid_cols+j]);
		 fputs(str,fp);
		 index++;
	 }
		
      fclose(fp);	
}


void readinput(float *vect, int grid_rows, int grid_cols, char *file) {

  	int i,j;
	FILE *fp;
	char str[STR_SIZE];
	float val;

	if( (fp  = fopen(file, "r" )) ==0 )
            fatal( "The file was not opened" );


	for (i=0; i < grid_rows; i++) 
	 for (j=0; j < grid_cols; j++)
	 {
 		if(i==0 || i==(grid_rows-1) || j==0 || j==(grid_cols-1))
		{
			  vect[i*grid_cols+j] = 0.0;
		}
		else
		{
			fgets(str, STR_SIZE, fp);
		
			if (feof(fp))
			{
				rewind(fp);
				fgets(str, STR_SIZE, fp);
				printf("not enough lines in file\n");
			}
		
			//if ((sscanf(str, "%d%f", &index, &val) != 2) || (index != ((i-1)*(grid_cols-2)+j-1)))	
			if ((sscanf(str, "%f", &val) != 1))
				fatal("invalid file format");
		
			vect[i*grid_cols+j] = val;
		}	
	}

	fclose(fp);	

}


/*
   compute N time steps
*/


int compute_tran_temp(float *array_temp,float* array_power, float *array_result, int col, int row, \
		int total_iterations, int num_iterations, int* interrupt) 
{ 

	//variables
	NbodyTask body;
	int numcpus = atoi(argv[7]);
	int numgpus = atoi(argv[8]);
	
	//sprintf(benchName, "hotspot");


//Declaraciones movidas a hotspot.h	
	grid_height = chip_height / row;
	grid_width = chip_width / col;

	Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
	Rx = grid_width / (2.0 * K_SI * t_chip * grid_height);
	Ry = grid_height / (2.0 * K_SI * t_chip * grid_width);
	Rz = t_chip / (K_SI * grid_height * grid_width);

	max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
	step = PRECISION / max_slope;
	int t;
	
	task_scheduler_init init(numcpus+numgpus);
	hs.Initialize(numcpus, numgpus, atoi(argv[9]));
	
	// Begin iterations
	hs.startTimeAndEnergy();

	for (t = 0; t < total_iterations; t += 1) {
		
		mystep = t;

		hs.heterogeneous_parallel_for(0, grid_rows, &body);
		
	// Swap input and output GPU matrices
		float* tmp_array = array_temp;
		array_temp = array_result;
		array_result = tmp_array;
	}
	

	//MatrixOut = FilesavingTemp[ret];

	hs.endTimeAndEnergy();
	hs.saveResultsForBench();
	return src;
}

void usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s <grid_rows/grid_cols> <pyramid_height> <sim_time> <temp_file> <power_file> <output_file> <numcpus> <numgpus> <chunkGPU>\n", argv[0]);
	fprintf(stderr, "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)\n");
	fprintf(stderr, "\t<pyramid_height> - pyramid heigh(positive integer)\n");
	fprintf(stderr, "\t<sim_time>   - number of iterations\n");
	fprintf(stderr, "\t<temp_file>  - name of the file containing the initial temperature values of each cell\n");
	fprintf(stderr, "\t<power_file> - name of the file containing the dissipated power values of each cell\n");
	fprintf(stderr, "\t<numcpus> - \n");
	fprintf(stderr, "\t<numgpus> - \n");
	fprintf(stderr, "\t<chunkGPU> - (Fixed or Dynamic)\n");
	exit(1);
}



int main(int argc, char** argvv) {



    printf("Starting\n");
	//end variables
	argv = argvv;



    int memfd;
    void *mapped_base, *mapped_dev_base;
    off_t dev_base = HW_ADDR_GPIO; //GPIO hardware


    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (memfd == -1) {
    	printf("Can't open /dev/mem.\n");
        exit(0);
    }
    printf("/dev/mem opened for gpio.\n");

    // Map one page of memory into user space such that the device is in that page, but it may not
      // be at the start of the page.


      mapped_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~MAP_MASK);
          if (mapped_base == (void *) -1) {
          printf("Can't map the memory to user space.\n");
          exit(0);
      }
       printf("GPIO mapped at address %p.\n", mapped_base);



      // get the address of the device in user space which will be an offset from the base
      // that was mapped as memory is mapped at the start of a page

      mapped_dev_base = mapped_base + (dev_base & MAP_MASK);




  file_desc = open("/dev/my_driver",O_RDWR);
  if (file_desc < 0)
  {	
		printf("cant open device: /dev/my_driver\n");
		exit(0);
  }
  
  // #endif
   char led_value = 255;

    #ifndef __SDSVHLS__
               //   axi_gpio_write_reg(axi_gpio_0.baseaddr, 0, (unsigned char) led_value);
    #endif


   // interrupt = (int *) sds_alloc(sizeof(int));


    interrupt = (int*)mapped_dev_base;
    /*int* enable_channel = (int*)mapped_dev_base + fpga_mmio_ier;
    int* enable_general = (int*)mapped_dev_base + fpga_mmio_gier;*/

    status = (int*)mapped_dev_base+fpga_mmio_gpio; //(int*)read(file_desc,receive,10); //axi_gpio_0.baseaddr;

    //status = (int*)mapped_dev_base;

    sds_mmap((void *)HW_ADDR_GPIO_INT,4,(void *)status);

    //sds_mmap((void *)HW_ADDR_GPIO,4,(void *)status);

    printf("Interrupt located at address %x\n",(int)status);

    printf("Switch on leds\n");
    
    *interrupt = (unsigned char) led_value; //leds on


   /* *enable_channel = 0xFFFFFFFF; //leds on
    *enable_general = 0xFFFFFFFF; //leds on*/

     //printf("Force interrupt\n");  
     //getchar();
     //*status = 255;
    
     printf("Switch off leds\n");

     led_value = 0;

    *interrupt = (unsigned char) led_value; //leds off


    char *tfile, *pfile, *ofile;
    
    int total_iterations;
    int pyramid_height = 1; // number of iterations
	
	if (argc < 10)
		usage(argc, argv);
	if((grid_rows = atoi(argv[1]))<=0|| (grid_cols = atoi(argv[1]))<=0|| (pyramid_height = atoi(argv[2]))<=0|| (total_iterations = atoi(argv[3]))<=0)
		usage(argc, argv);
	

    tfile=argv[4];
    pfile=argv[5];
    ofile=argv[6];
	
    size=(grid_rows+2)*(grid_cols+2);


    // --------------- pyramid parameters --------------- 
   
   // int smallBlockCol = BLOCK_SIZE-(pyramid_height)*EXPAND_RATE;
   // int smallBlockRow = BLOCK_SIZE-(pyramid_height)*EXPAND_RATE;
   // int blockCols = grid_cols/smallBlockCol+((grid_cols%smallBlockCol==0)?0:1);
   // int blockRows = grid_rows/smallBlockRow+((grid_rows%smallBlockRow==0)?0:1);

    #ifdef HOSTA7
	main_thread=syscall(__NR_gettid);
	main_core=sched_getcpu();
	cout << "Main thread id: " << main_thread << endl;
	cout << "          core: " << main_core << endl;
    #endif

    ////array_temp = (float *) alignedMalloc(size*sizeof(float));
    ////array_power = (float *) alignedMalloc(size*sizeof(float));
    ////array_out = (float *) alignedMalloc(size*sizeof(float));


    array_temp = (float *) sds_alloc(size*sizeof(float));
    array_power = (float *) sds_alloc(size*sizeof(float));
    array_out = (float *) sds_alloc(size*sizeof(float));

	
   
    
    if( !array_temp)
    {
        printf("unable to allocate memory\n");
        exit(1);
    }

    printf("Memories allocated\n");
	
    // Read input data from disk
    readinput(array_temp, (grid_rows+2), (grid_cols+2), tfile);
    readinput(array_out, (grid_rows+2), (grid_cols+2), tfile);
    readinput(array_power, (grid_rows+2), (grid_cols+2), pfile);
	


	// Perform the computation
	ret = compute_tran_temp(array_temp, array_power,array_out, grid_cols, grid_rows, total_iterations, pyramid_height,interrupt);
	
	
	// Write final output to output file
	//writeoutput(array_out, (grid_rows+2), (grid_cols+2), ofile);
    
//	

	return 0;
}
