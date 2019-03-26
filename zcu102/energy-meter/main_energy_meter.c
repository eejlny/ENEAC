/*
 * Energy meter library v1.0
 * for ODROID XU+E
 * 2014 DAC UMA andres@uma.es
 * 
 * Read header file for changes log
 * 
 * This program runs a few threads and measures energy
 * Compile with:
 * 		gcc main_energy_meter.c energy_meter.c -pthread -o meter
 * Run with:
 * 		./meter 50 4 0 0 0
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#include "energy_meter.h"

//---------------------------------------------------------------
// Dummy thread function for generating energy consumption
void * funcion (void * arg)
{
	int i,j;
	float x,y;
	for (i=0; i<20000; i++)
	{
		for (j=0; j<10000; j++)
		{
			x=(y*(float)i+ x *(float)j)/(float)(i+j);
		}
	}
	pthread_exit(NULL);
}

//---------------------------------------------------------------
main(int argc, char **argv)
{
	int i,j;
	float x,y;
	struct timespec res;
	struct em_t em1,em2,em3;
	
	pthread_t th[4];
	int thid[4]={0,1,2,3};
	
	struct energy_sample *sample1;
	
	int sr,debug,thn,pre,post;
	printf("Params:  <sample period (msec)>  <# threads 1-4>  <debug (0/1)>  <sleep time at start>  <sleep time at end>\n");
	
	if(argc!=6) { printf("Error: incorrect number of parameters\n"); exit(-1);}
	
	sscanf(argv[2], " %d ",&thn); // number of threads
	sscanf(argv[1]," %d ",&sr);   // sampling period ms
	sscanf(argv[3]," %d ",&debug); // debug =1
	sscanf(argv[4]," %d ",&pre);  // sleep time before threads
	sscanf(argv[5]," %d ",&post); // sleep time after threads
	
	
	sample1=energy_meter_init(sr, debug);  // sample rate in miliseconds
	
	energy_meter_start(sample1);  // starts sampling thread
	
	printf("sleep previo %d secs\n",pre);
	
	energy_meter_read(sample1, &em1);   // take a reading
	sleep(pre);
	energy_meter_diff(sample1, &em1);   // make diff with previous
	energy_meter_read_printf(&em1, stderr);  // print energy while in sleep
	
	//------------------	
	energy_meter_read(sample1, &em1);	 // take start reading
	for(i=0;i<thn; i++)
	{ 
		pthread_create(&th[i],NULL,funcion,(void*)&thid[i]);
		printf("Thread #%d running...\n",thid[i]);
		sleep(5);
	}
	
	for(i=0;i<thn; i++)	
	{
		pthread_join(th[i],NULL);
		printf("thread #%d ends\n",thid[i]);		
	}
	
	energy_meter_diff(sample1, &em1);   // make diff with previous reading
	energy_meter_read_printf(&em1, stderr); // print energy while running threads
	//--------------------
	
	printf("sleep final %d secs\n",post);
	energy_meter_read(sample1, &em1);		
	sleep(post);
	energy_meter_diff(sample1, &em1);
	energy_meter_read_printf(&em1, stderr);  // print energy while in sleep 
		
	energy_meter_stop(sample1);  	// stops sampling
	
	energy_meter_printf(sample1, stderr);  // print total results
	
	energy_meter_destroy(sample1);     // clean up everything
	
}
