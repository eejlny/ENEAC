/*
 * Energy meter library v1.0
 * for ODROID XU+E *** NOW XU3 LINUX VERSION
 * 2014 DAC UMA andres@uma.es
 *
 * Changes:
 * 12/11/2014-	adapted to XU3 LINUX VERSION (paths to sensors are diferent)
 * 13/2/2014-	Some macros added
 * 12/2/2014-	Bug solved: memory sensor not initialized.  Makefile added, lib directory added
 * 7/2/2014 - 	Sampling period now in miliseconds - energy_meter_init() input parameter-
 * 				Energy measuremets inside structure now in Joules (not nanoJ)
 * 				Energy values in different structures now with same member name: A15,A7,GPU,MEM
 * 				Test program has been commented and revised
 * 6/2/2014 - 	First realese
 */

#ifndef ENERGYMETER_H_GUARD
#define ENERGYMETER_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <asm/unistd.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define NUM_SENSORS 18 //ultrascale

#define FPD_rails 5  //last sensor index
#define LPD_rails 10 //last sensor index
#define PLD_rails 17 //last sensor index

/* original order 
char *rails[]={ 
// PS
 "VCCPSINTFP", //0
 "VCCPSINTLP", //1
 "VCCPSAUX",   //2
 "VCCPSPLL",   //3
 "MGTRAVCC",   //4
 "MGTRAVTT",   //5
 "VCCO_PSDDR_504", //6
 "VCCOPS", //7
 "VCCOPS3",  //8
 "VCCPSDDRPLL" , //9
// PL
 "VCCINT",
 "VCCBRAM",
 "VCCAUX", 
 "VCC1V2",
 "VCC3V3", 
 "VADJ_FMC", //15
 "MGTAVCC", 
 "MGTAVTT" 
};*/


//-------------------------------------------------------------------
//- DATA STRUCTURES -------------------------------------------------
//-------------------------------------------------------------------

    /*-------------------------------------------------------------------
     * Estructure for measured energy at one point used in
     * energy_meter_read() and energy_meter_diff()
     * */

    struct em_t
    {
        double energy[NUM_SENSORS]; // energy measured in Joules
    };

    /*-------------------------------------------------------------------
     * Internal structute for manage energy sampling
     * created at energy_meter_init()
     * freed at energy_meter_destroy()
     * */
    struct energy_sample
    {
        int sample_rate; // in microseconds to use in usleep()

        // cumulative energy (and final result) in Joules
	double energy[NUM_SENSORS];

        double power[NUM_SENSORS]; // last measurement

        int destroy;  	// 1 = sampling thread must exit
        int stop;     	//  not in use

        long samples;  	// # of samples

        struct timespec start_time; // clock_gettime(CLOCK_REALTIME, ...) at starting point
        struct timespec stop_time; 	// clock_gettime(CLOCK_REALTIME, ...) at finish
        double time;       			// elapsed time in seconds (final result)

        struct timespec res[2];    // to take times between samples
        int now;

        pthread_t th_meter;    // sampling thread
        pthread_mutex_t mutex; // mutex for controling sampling thread
    };

//-------------------------------------------------------------------
//- PUBLIC API ------------------------------------------------------
//-------------------------------------------------------------------

    /*------------------------------------------------------------------
      struct energy_sample * energy_meter_init(int sample_rate, int debug);

      It creates a new energy_sample structrure and internal sampling thread ready to go
      sample_rate is the sampling period in miliseconds must be < 1 second
      debug = 1 will output a file "debug_energy_meter####.txt" with samplig raw data
      Example code:
    	struct energy_sample * mysample;
    	...
    	mysample = energy_meter_init(50, 0);  // for 50 ms sampling period and no debugging
    */
    struct energy_sample * energy_meter_init(int sample_rate, int debug);
    
    int set_sampler_affinity(struct energy_sample *sample, int cpu);
  
    /*-------------------------------------------------------------------
      void energy_meter_start(struct energy_sample *sample);

      It starts energy sampling thread, it also get time at starting point (stored in estructure)
      Example code:
    	energy_meter_start(mysample);
    */
    void energy_meter_start(struct energy_sample *sample);

    /*-------------------------------------------------------------------
      void energy_meter_stop(struct energy_sample *sample);

      It stops energy sampling thread, it also get time at end point (stored in estructure)
     Example code:
    	energy_meter_stop(mysample);
    	printf("A15 total energy measured= %lf Joules\n", sample->A15 );  // energy is in Joules

    */
    void energy_meter_stop(struct energy_sample *sample);

    /*-------------------------------------------------------------------
      void energy_meter_printf(struct energy_sample *sample1, FILE * fout);
      It prints energy totals and more on the file
      fout can be also stdout or stderr
     Example code:
    	energy_meter_printf(mysample, stdout);
    	...// you get somthing like:
    +--------------------+
    | POWER MEASUREMENTS |
    +--------------------+
    A7= 0.451691 J :: A15= 206.333552 J :: GPU= 0.280862 J :: Mem= 4.438422 J
    CLOCK_REALTIME = 49.533113 sec
    # of samples: 2429
    sample every (real) = 0.020392 sec
    sample every: 0.020000 sec
    */
    void energy_meter_printf(struct energy_sample *sample1, FILE * fout);

    /*-------------------------------------------------------------------
      It destroy structures, finish sampling thread and free memory
      To use always after stop
     Example code:
    	energy_meter_destroy(mysample);

     */
    void energy_meter_destroy(struct energy_sample *sample);

    /*-------------------------------------------------------------------
      void energy_meter_read(struct energy_sample *sample, struct em_t * read);
      read the current accumulated energy (Makes a new sample => readings are updated up to this moment)
     Example code:
        struct em_t read;
        ...
    	energy_meter_read(mysample, &read);
    	printf("A15 partial energy measured= %lf Joules\n", read->A15);  // energy is in Joules

    */
    void energy_meter_read(struct energy_sample *sample, struct em_t * read);

    /*-------------------------------------------------------------------
      void energy_meter_diff(struct energy_sample *sample, struct em_t * start_diff);
      read the current accumulated energy between to points (A) and (B) (see example)
      * (Makes a new sample => readings are updated up to this moment)
     Example code:
        struct em_t read;
        ...
    	energy_meter_read(mysample, &read);  //(A)
    	...
    	...
    	...
    	energy_meter_diff(mysample, &read);  //(B)
    	printf("A15 partial energy between two measurements= %lf Joules\n", read->A15);  // energy is in Joules

    */
    void energy_meter_diff(struct energy_sample *sample, struct em_t * start_diff);

    /*-------------------------------------------------------------------
      void energy_meter_read_printf(struct em_t * read_or_diff, FILE * fout);
      It prints energy in the read structure
      fout can be also stdout or stderr
     Example code:
    	energy_meter_read_printf(&read, stdout);
    	...// you get somthing like:
    POWER READ ----------------
     A7= 0.000000 J :: A15= 0.000000 J :: GPU= 0.000000 J :: Mem= 0.000000 J
    */
    void energy_meter_read_printf(struct em_t * read_or_diff, FILE *fout);


    /***********************************************
     * PUBLIC MACROS
     * *********************************************/
// inicialize to 0.0 energy structure
#define init_em(EM) { int i; for(i=0; i<NUM_SENSORS; i++) EM.energy[i]=0.0; }
	 
// accumulate energy NEW in structure EM
#define sum_em(EM, NEW) { int i; for(i=0; i<NUM_SENSORS; i++) EM.energy[i]+=NEW.energy[i]; }
// calculates sum of all energies
#define total_em(EM) ({ double sum=0.0; int i; for(i=0; i<NUM_SENSORS; i++) sum+=EM.energy[i]; sum; })
// calculates sum of all energies
#define subtotal_em(EM,a,b) ({ double sum=0.0; int i; for(i=a; i<b; i++) sum+=EM.energy[i]; sum; })


//-------------------------------------------------------------------
//-----INTERNAL USE -------------------------------------------------
    struct timespec diff(struct timespec start, struct timespec end); // internal use
    void *meter_function(void *arg); // internal use thread function
    void *meter_function_debug(void *arg); // internal use thread function
//-------------------------------------------------------------------

#if defined(__cplusplus)
}
#endif


#endif
