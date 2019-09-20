#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/time.h>
#include <assert.h>
#include <iostream>
#include <fstream>

/* maximum power density possible (say 300W for a 10mm x 10mm chip)	*/
#define MAX_PD	(3.0e6)
/* required precision in degrees	*/
#define PRECISION	0.001
#define SPEC_HEAT_SI 1.75e6
#define K_SI 100
/* capacitance fitting factor	*/
#define FACTOR_CHIP	0.5

/* performance timing and energy monitoring macros */
#define TIME_STAMP_INIT_HW  unsigned long long clock_start_hw, clock_end_hw;  clock_start_hw = sds_clock_counter();
#define TIME_STAMP_HW { clock_end_hw = sds_clock_counter(); fprintf(stderr,"SPARSE FPGA ON: execution time : %f ms\n", 1000*(clock_end_hw-clock_start_hw)/(1200*10e6)); clock_start_hw = sds_clock_counter(); }

using namespace std;

/*****************************************************************************
 * Global Variables 
 * **************************************************************************/

/* chip parameters	*/
const static float t_chip = 0.0005;
const static float chip_height = 0.016;
const static float chip_width = 0.016;
/* ambient temperature, assuming no package at all	*/
const static float amb_temp = 80.0;

int size, ret, grid_rows, grid_cols, borderCols, borderRows;
float grid_height, grid_width, Cap, Rx, Ry, Rz, max_slope, step;
int src = 0, dst = 1;

float *array_temp; 
float *array_power; 
float *array_out;

float *array_temp_noncache;
float *array_power_noncache; 
float *array_out_noncache;

//These are user input parameters defined in hotspot-top.cpp
int numhpacc;
int numhpcacc;
int cont_read;
int ioctl_flag;
int frame_width;
long unsigned int bodies_C=0, bodies_F=0;

#ifdef HWDEBUG
     int debug_flag = 1;
#else
     int debug_flag;
#endif

/*Interrupt drivers*/
#define DRIVER_FILE_NAME_1 "/dev/intgendriver1"
int file_desc_1;
#define DRIVER_FILE_NAME_2 "/dev/intgendriver2"
int file_desc_2;
#define DRIVER_FILE_NAME_3 "/dev/intgendriver3"
int file_desc_3;
#define DRIVER_FILE_NAME_4 "/dev/intgendriver4"
int file_desc_4;

//Variables used for timing
struct timespec start1, start2, start3, start4, finish1, finish2, finish3, finish4;
double elapsed1, elapsed2, elapsed3, elapsed4;


/*****************************************************************************
 * Functions 
 * **************************************************************************/

void usage(
int argc,
char **argv) {
     std::cout << "Usage: <grid_rows/grid_cols> <temp_file> <power_file> <continuous_read> <numcpus> <numhpacc> <numhpcacc> <chunkACC> <IOCTL_flag> <output_file>" << endl;
     std::cout << "\t<grid_rows/grid_cols>  - number of rows/cols in the grid (positive integer)" << endl;
     std::cout << "\t<temp_file>  - name of the file containing the initial temperature values of each cell" << endl;
     std::cout << "\t<power_file> - name of the file containing the dissipated power values of each cell" << endl;
     std::cout << "\t<continuous_read> - enable continuous read of input files to fill buffer with data (0-No or 1-Yes)" << endl;
     std::cout << "\t<numcpus> - number of CPU cores (integer: 0-4)" << endl;
     std::cout << "\t<numhpacc> - number of HP accelerators (must be preloaded in fpga)" << endl;
     std::cout << "\t<numhpcacc> - number of HPC accelerators (must be preloaded in fpga)" << endl;
     std::cout << "\t<chunkACC> - (Fixed or Dynamic) (0 or higher integer)" << endl;
     std::cout << "\t<IOCTL flag> - enable hardware interrupts (0-No or 1-Yes)" << endl;     
     std::cout << "\t<output file> - write output to file" << endl;     
	exit(0);
};

void freemem(){
     sds_free(array_temp);
     sds_free(array_power);
     sds_free(array_out);
     if (numhpacc > 0){     
          sds_free(array_temp_noncache);
          sds_free(array_power_noncache);
          sds_free(array_out_noncache);
     }     
};

void readinput(
float *vect,
int grid_rows,
int grid_cols,
char *file
) {
  	int i,j;
	FILE *fp;
	char str[15];
	float val;

	if( (fp  = fopen(file, "r" )) == 0 )
          fprintf(stderr, "ERROR: The file <%s> was not opened\n", file);

	for (i=0; i < grid_rows; i++) 
          for (j=0; j < grid_cols; j++) {
               if(i==0 || i==(grid_rows-1) || j==0 || j==(grid_cols-1))
                    vect[i*grid_cols+j] = 0.0;
               else {
                    if (feof(fp) && cont_read) {
                         (debug_flag) && (printf("Not enough lines in file, but continuous read is on. Resetting input file.\n"));
                         rewind(fp);
                         fgets(str, sizeof(str), fp);
                    } else if (feof(fp) && !cont_read) {
                         fprintf(stderr,"ERROR: Reached end of file <%s> before filling input buffer size <%d:%d>. Consider using continuous read.\n", file, grid_rows, grid_cols);
                         fclose(fp);
                         freemem();
                         exit(1);
                    } else {
                         fgets(str, sizeof(str), fp);
                    }
                    
                    if ((sscanf(str, "%f", &val) != 1)){
                         fprintf(stderr, "ERROR: The file <%s> has invalid file format\n", file);
                         fclose(fp);
                         freemem();
                         exit(1);
                    }
                    
                    vect[i*grid_cols+j] = val;
               }	
          }
	fclose(fp);
};
 
void writeoutput(
float *vect,
int grid_rows,
int grid_cols,
char *file) {
	int i,j, index=0;
	FILE *fp;
	char str[15];

	if( (fp = fopen(file, "w" )) == 0 ) {
          fprintf(stderr,"ERROR: The file: <%s> was not opened\n",file);
          freemem();
          exit(1);
     }
          
	for (i=0; i < grid_rows; i++) 
          for (j=0; j < grid_cols; j++) {
               sprintf(str, "%d\t%g\n", index, vect[i*grid_cols+j]);
               fputs(str,fp);
               index++;
          }
     fclose(fp);	
};

void hotcpotCPU(
int begin,
int end
) {
     float *temp;
     float *power;
     float *result;
     int row;
     int col;
     float delta;        

     temp = array_temp+(grid_cols+2);
     power = array_power+(grid_cols+2);
     result = array_out+(grid_cols+2);
     col = (grid_cols+2);
     float Rx_1=1.f/Rx;
     float Ry_1=1.f/Ry;
     float Rz_1=1.f/Rz;
     float Cap_1 = step/Cap;

     for (int r = begin; r < end; ++r ) {
          for (int  c = 1; c <= grid_cols; ++c ) {
               result[r*col+c] = temp[r*col+c] + (Cap_1 * 
                                                  (power[r*col+c] + (temp[(r+1)*col+c] + temp[(r-1)*col+c] - 2.f*temp[r*col+c]) 
                                                  * Ry_1 + (temp[r*col+c+1] + temp[r*col+c-1] - 2.f*temp[r*col+c]) 
                                                  * Rx_1 + (amb_temp - temp[r*col+c]) * Rz_1));
          }
     }
};