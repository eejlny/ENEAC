//#include <stdio.h>
//#include <stdlib.h>
//#include <cstdlib>
//#include <iostream>
//#include <fstream>
//#include "hotspot.h"

//#include "tbb/task_scheduler_init.h"
//#include "tbb/tbb.h"


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/stat.h>
#include <assert.h>
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
//using namespace tbb;

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


	for (i=0; i <= grid_rows-1; i++) 
	 for (j=0; j <= grid_cols-1; j++)
	 {
		if (fgets(str, STR_SIZE, fp) == NULL)
			fatal("Error reading file\n");
		
		if (feof(fp))
			fatal("not enough lines in file");
		
		//if ((sscanf(str, "%d%f", &index, &val) != 2) || (index != ((i-1)*(grid_cols-2)+j-1)))
		if ((sscanf(str, "%f", &val) != 1))
			fatal("invalid file format");
		
		vect[i*grid_cols+j] = val;
	}

	fclose(fp);	

}


/*
   compute N time steps
*/


int compute_tran_temp(float *array_temp,float* array_power, float *array_result, int col, int row, \
		int total_iterations, int num_iterations) 
{ 

	//variables
	NbodyTask body;
	int numcpus = atoi(argv[7]);
	int numgpus = atoi(argv[8]);
	
	sprintf(benchName, "hotspot");


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
		
		// Specify kernel arguments
		int iter = MIN(num_iterations, total_iterations - t);

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


	//end variables
	argv = argvv;


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
	
    size=grid_rows*grid_cols;

    printf("Allocating memories\n");

    // --------------- pyramid parameters --------------- 
   
   // int smallBlockCol = BLOCK_SIZE-(pyramid_height)*EXPAND_RATE;
   // int smallBlockRow = BLOCK_SIZE-(pyramid_height)*EXPAND_RATE;
   // int blockCols = grid_cols/smallBlockCol+((grid_cols%smallBlockCol==0)?0:1);
   // int blockRows = grid_rows/smallBlockRow+((grid_rows%smallBlockRow==0)?0:1);

    //array_temp = (float *) alignedMalloc(size*sizeof(float));
    //array_power = (float *) alignedMalloc(size*sizeof(float));
    //array_out = (float *) alignedMalloc(size*sizeof(float));


    //array_temp = (float *) sds_alloc(size*sizeof(float));
    //array_power = (float *) sds_alloc(size*sizeof(float));
    //array_out = (float *) sds_alloc(size*sizeof(float));

	float* my_bodies2 = (float *) sds_alloc(4 * 1000);
	if(!my_bodies2)
	{
		printf("bodies not allocated\n");
		exit(1);

	}

   /*
    array_temp = (float *)sds_alloc(10000*sizeof(float));
    //array_power = (float *) alignedMalloc(size*sizeof(float));
    //array_out = (float *) alignedMalloc(size*sizeof(float));

    if( !array_temp)
    {
        printf("unable to allocate memory\n");
        exit(1);
    }

    printf("Memories allocated\n");
	
    // Read input data from disk
    readinput(array_temp, grid_rows, grid_cols, tfile);
    readinput(array_power, grid_rows, grid_cols, pfile);
	


	// Perform the computation
	ret = compute_tran_temp(array_temp, array_power,array_out, grid_cols, grid_rows, total_iterations, pyramid_height);
	
	
	// Write final output to output file
	writeoutput(array_out, grid_rows, grid_cols, ofile);
    
//	

	return 0;*/
}
