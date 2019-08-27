/*
 * Energy meter library v1.0
 * for ODROID XU+E
 * 2014 DAC UMA andres@uma.es
 * 
 * Read header file for changes log
 */

#include "energy_meter.h"

// globals
int SENSORS_ENABLED=1;
char *rails[]={ 
// PS FPD
 "VCCPSINTFP", 
 "MGTRAVCC", 
 "MGTRAVTT", 
 "VCCO_PSDDR_504",
 "VCCPSDDRPLL" ,
// PS LPD

 "VCCPSINTLP", 
 "VCCPSAUX", 
 "VCCPSPLL",
 "VCCOPS",
 "VCCOPS3", 

// PLD
 "VCCINT",
 "VCCBRAM",
 "VCCAUX", 
 "VCC1V2",
 "VCC3V3", 
 "MGTAVCC", 
 "MGTAVTT",

//OTHER
 "VADJ_FMC"
};

char *sensors[]={
//FPD
		"/sys/bus/i2c/drivers/ina2xx/3-0040/hwmon/hwmon0/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-0044/hwmon/hwmon4/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-0045/hwmon/hwmon5/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-0046/hwmon/hwmon6/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-004b/hwmon/hwmon9/power1_input",
//LPD

		"/sys/bus/i2c/drivers/ina2xx/3-0041/hwmon/hwmon1/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-0042/hwmon/hwmon2/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-0043/hwmon/hwmon3/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-0047/hwmon/hwmon7/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/3-004a/hwmon/hwmon8/power1_input",
//PLD

		"/sys/bus/i2c/drivers/ina2xx/4-0040/hwmon/hwmon10/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/4-0041/hwmon/hwmon11/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/4-0042/hwmon/hwmon12/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/4-0043/hwmon/hwmon13/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/4-0044/hwmon/hwmon14/power1_input",		
		"/sys/bus/i2c/drivers/ina2xx/4-0046/hwmon/hwmon16/power1_input",
		"/sys/bus/i2c/drivers/ina2xx/4-0047/hwmon/hwmon17/power1_input",
//OTHER
		"/sys/bus/i2c/drivers/ina2xx/4-0045/hwmon/hwmon15/power1_input"
};

// read sensors *********************************
#define read_sensors(values) {\
	int i;\
	int fd;\
	char buf[256];\
	for(i=0; i<NUM_SENSORS; i++) {\
	fd=open(sensors[i],O_RDONLY);\
	read(fd,&buf,256);\
	sscanf(buf," %lf ", values+i);\
	*(values+i)/=1000.0;\
	close(fd); } }


int set_sampler_affinity(struct energy_sample *sample, int cpu)
{
	cpu_set_t mask;
	__CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	__CPU_SET_S(cpu,sizeof(cpu_set_t), &mask);
	
	return pthread_setaffinity_np(sample->th_meter, sizeof(cpu_set_t), &mask);
	
	
}

//-------------------------------------------------------------------

struct energy_sample * energy_meter_init(int sample_rate, int debug) // sample rate in miliseconds
{
	struct energy_sample * sample;
	int i;
	sample=(struct energy_sample *) malloc(sizeof(struct energy_sample ));
		
	sample->sample_rate=sample_rate*1000; // in microseconds to use in usleep()
	for(i=0; i<NUM_SENSORS; i++) sample->energy[i]=0.0;
	sample->destroy=0;
	sample->stop=1;
	sample->samples=0;
	
	pthread_mutex_init(&(sample->mutex),NULL);
	pthread_mutex_lock(&(sample->mutex));
	if (debug)
	pthread_create(&(sample->th_meter), NULL, meter_function_debug , (void *)sample);	//thread
	else
	pthread_create(&(sample->th_meter), NULL, meter_function , (void *)sample);	//thread 
	
	return(sample);
}

//-------------------------------------------------------------------
void energy_meter_start(struct energy_sample *sample)
{
	clock_gettime(CLOCK_REALTIME, &(sample->start_time));
	
	pthread_mutex_unlock(&(sample->mutex)); //start energy sampling
}

//-------------------------------------------------------------------
void energy_meter_stop(struct energy_sample *sample)
{
	struct timespec res;
	double secs;
	struct timespec dif;
	int i;
   
	pthread_mutex_lock(&(sample->mutex));  // stop energy sampling
	clock_gettime(CLOCK_REALTIME, &(sample->stop_time));
	res=diff(sample->start_time, sample->stop_time);
	
	sample->time=(double)res.tv_sec+ (double)res.tv_nsec/1000000000.0;
	
	
	//read_sensors(sample1->a7W, sample1->a15W, sample1->gpuW, sample1->memW);
    //sample->now=!sample->now;
	// get time now**********************************
	//clock_gettime(CLOCK_REALTIME, &dif );
	// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
	dif.tv_nsec=sample->stop_time.tv_nsec - sample->res[sample->now].tv_nsec;
	if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
	// claculate energy until now **************************************
	secs= dif.tv_nsec/1000000000.0; // move to seconds
	for (i=0; i<NUM_SENSORS; i++) 
	sample->energy[i] += sample->power[i] * secs;
	
}
//-------------------------------------------------------------------

void energy_meter_destroy(struct energy_sample *sample) // always after stop
{
	sample->destroy=1;
	pthread_mutex_unlock(&(sample->mutex));  
	pthread_join(sample->th_meter,NULL);
	pthread_mutex_destroy(&(sample->mutex));
	free(sample);
}
//-------------------------------------------------------------------

void energy_meter_printf(struct energy_sample *sample1, FILE * fout)
{
	struct timespec res;
	int i;
	res=diff(sample1->start_time, sample1->stop_time);
	
	fprintf(fout,"+--------------------+\n");
	fprintf(fout,"| POWER MEASUREMENTS |\n");
	fprintf(fout,"+--------------------+\n");
	
	fprintf(fout,"** Full Power Domain rails ********** \n");
	for (i=0; i<FPD_rails; i++) {
	fprintf(fout,"%14s = %8.3lf J ",rails[i],sample1->energy[i]/1000.0);
	if((i+1)%3==0) fprintf(fout,"\n");
	}
	fprintf(fout,"\n\n");
	fprintf(fout,"** Low Power Domain rails ***********\n");
	for (i=FPD_rails; i<LPD_rails; i++) {
	fprintf(fout,"%14s = %8.3lf J ",rails[i],sample1->energy[i]/1000.0);
	if((i+1)%3==0) fprintf(fout,"\n");
	}
	fprintf(fout,"\n\n");
	fprintf(fout,"** Programmable Logic Domain rails ***********\n");
	for (i=LPD_rails; i<PLD_rails; i++) {
	fprintf(fout,"%14s = %8.3lf J ",rails[i],sample1->energy[i]/1000.0);
	if((i+1)%3==0) fprintf(fout,"\n");
	}
	fprintf(fout,"\n\n");
	
	fprintf(fout,"Full Power Domain ENERGY        = %lf J\n",subtotal_em((*sample1),0,FPD_rails)/1000.0);
	fprintf(fout,"Low Power Domain ENERGY         = %lf J\n",subtotal_em((*sample1),FPD_rails,LPD_rails)/1000.0);
	fprintf(fout,"Programmable Logic Domain ENERGY = %lf J\n",subtotal_em((*sample1),LPD_rails,PLD_rails)/1000.0);
	fprintf(fout,"TOTAL ENERGY                    = %lf J\n",subtotal_em((*sample1),0,PLD_rails)/1000.0);
	fprintf(fout,"\n");	
	

	fprintf(fout,"Full Power Domain POWER         = %lf W\n",subtotal_em((*sample1),0,FPD_rails)/1000.0/((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0));
	fprintf(fout,"Low Power Domain POWER          = %lf W\n",subtotal_em((*sample1),FPD_rails,LPD_rails)/1000.0/((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0));
	fprintf(fout,"Programmable Logic Domain POWER = %lf W\n",subtotal_em((*sample1),LPD_rails,PLD_rails)/1000.0/((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0));
	fprintf(fout,"TOTAL POWER                     = %lf W\n",subtotal_em((*sample1),0,PLD_rails)/1000.0/((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0));
	fprintf(fout,"\n");
	fprintf(fout,"CLOCK_REALTIME = %lf sec\n",(double)res.tv_sec+ (double)res.tv_nsec/1000000000.0);	
	fprintf(fout,"# of samples: %ld\n", sample1->samples);
	fprintf(fout,"sample every (real) = %lf sec\n",((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0)/sample1->samples);	
	fprintf(fout,"sample every: %lf sec\n",(double)sample1->sample_rate/1000000);	
	
}
//-------------------------------------------------------------------

void energy_meter_read(struct energy_sample *sample, struct em_t * out)
{
	double secs;
	struct timespec dif;
	int i;
   
	// mutex 
	pthread_mutex_lock(&(sample->mutex));
	
		sample->now=!sample->now;
		// get time now**********************************
		clock_gettime(CLOCK_REALTIME, sample->res+sample->now );
		read_sensors(sample->power);
		// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
		dif.tv_nsec=sample->res[sample->now].tv_nsec-sample->res[!sample->now].tv_nsec;
		if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
		
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
		for (i=0; i<NUM_SENSORS; i++) 
		{
		sample->energy[i] += sample->power[i] * secs;
	        out->energy[i]=  sample->energy[i]/1000.0;
		}
        
				
		sample->samples++;
	
	pthread_mutex_unlock(&(sample->mutex));
	
	
	//
	
}
//-------------------------------------------------------------------
void energy_meter_diff(struct energy_sample *sample, struct em_t * diff)
{
	double secs;
	struct timespec dif;
	int i;
   
	// mutex 
	pthread_mutex_lock(&(sample->mutex));
	
		sample->now=!sample->now;
		// get time now**********************************
		clock_gettime(CLOCK_REALTIME, sample->res+sample->now );
		read_sensors(sample->power);
		// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
		dif.tv_nsec=sample->res[sample->now].tv_nsec-sample->res[!sample->now].tv_nsec;
		if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
		
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
        
		for (i=0; i<NUM_SENSORS; i++) 
		{
		sample->energy[i] += sample->power[i] * secs;
	        diff->energy[i]=  sample->energy[i]/1000.0 - diff->energy[i];
		}
				
		sample->samples++;
	
	pthread_mutex_unlock(&(sample->mutex));

}
//-------------------------------------------------------------------
void energy_meter_read_printf(struct em_t * sample1, FILE *fout)
{
	int i;
	fprintf(fout,"POWER READ --------------------------------------------\n");
	  fprintf(fout,"** PS rails *************************\n");
	          for (i=0; i<LPD_rails; i++) {
			          fprintf(fout,"%14s = %8.3lf J ",rails[i],sample1->energy[i]);
				          if((i+1)%3==0) fprintf(fout,"\n");
					          }
		          fprintf(fout,"\n");
			          fprintf(fout,"** PL rails *************************\n");
				          for (i=LPD_rails; i<PLD_rails; i++) {
						          fprintf(fout,"%14s = %8.3lf J ",rails[i],sample1->energy[i]);
							          if((i+1)%3==0) fprintf(fout,"\n");
								          }
					          fprintf(fout,"\n");
						          fprintf(fout,"PS ENERGY= %lf J\n",subtotal_em((*sample1),0,LPD_rails));
							          fprintf(fout,"PL ENERGY= %lf J\n",subtotal_em((*sample1),LPD_rails,PLD_rails));
								          fprintf(fout,"TOTAL ENERGY= %lf J\n",total_em((*sample1)));
	fprintf(fout,"\n");
}
//-------------------------------------------------------------------


void *meter_function(void *arg)
{
	struct energy_sample *sample=(struct energy_sample *) arg;
	
	char buf[256];
	//int fa7, fa15,fgpu,fmem;
	struct timespec dif;
	double secs;
	int i;
	
    sample->now=0;
    // first sample
 	pthread_mutex_lock(&(sample->mutex));
 	clock_gettime(CLOCK_REALTIME, sample->res);
 	read_sensors(sample->power);
        pthread_mutex_unlock(&(sample->mutex));
	
	usleep(sample->sample_rate);

	while(1)  // sampling on course
	{
		pthread_mutex_lock(&(sample->mutex));
		if(sample->destroy)
		{
			pthread_mutex_unlock(&(sample->mutex));
			pthread_exit(NULL);
		}
		sample->now=!sample->now;
		// get time now**********************************
		clock_gettime(CLOCK_REALTIME, sample->res+sample->now );
		read_sensors(sample->power);
		// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
		dif.tv_nsec=sample->res[sample->now].tv_nsec-sample->res[!sample->now].tv_nsec;
		if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
		
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
        
//		sample->A7  += sample->a7W * secs ; // Watt*sec=Joules
		for (i=0; i<NUM_SENSORS; i++) 
		sample->energy[i] += sample->power[i] * secs;
		
	
		sample->samples++;
		// DEBUG
		// fprintf(stdout,"a7= %lf W : a15= %lf W : gpu= %lf W \n",a7W,a15W,gpuW);
		// fprintf(stdout,"CLOCK_REALTIME = %lld sec, %ld nsec\n",(long long) dif.tv_sec, (long)dif.tv_nsec);	
	
		
		
		pthread_mutex_unlock(&(sample->mutex));
		
		usleep(sample->sample_rate);
	}
	
}


//-------------------------------------------------------------------

void *meter_function_debug(void *arg)
{
	struct energy_sample *sample=(struct energy_sample *) arg;
	struct timespec dif;
	char buf[256];
	FILE *debugf;
	int fa7, fa15,fgpu, fmem;
	//double a7W=0.0, a15W=0.0, gpuW=0.0, memW=0.0;
	double secs;
    int c1,c2,c3,c4,cGPU;
    int i;
    char fn[256];
    sprintf(fn,"debug_energy_meter%d.csv",getpid());
    debugf=fopen(fn,"w");
    fprintf(debugf,"#;sample;time;sensors\n");	
    // first sample
    sample->now=0;
 	pthread_mutex_lock(&(sample->mutex));
 	clock_gettime(CLOCK_REALTIME, sample->res);
 	read_sensors(sample->power);
	
    pthread_mutex_unlock(&(sample->mutex));
	
	usleep(sample->sample_rate);

	while(1)  // sampling on course
	{
		pthread_mutex_lock(&(sample->mutex));
		if(sample->destroy)
		{
			pthread_mutex_unlock(&(sample->mutex));
			fclose(debugf);
			pthread_exit(NULL);
		}
		sample->now=!sample->now;
		// get time now**********************************
		clock_gettime(CLOCK_REALTIME, sample->res+sample->now );
		// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
		dif.tv_nsec=sample->res[sample->now].tv_nsec-sample->res[!sample->now].tv_nsec;
		if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
		
		read_sensors(sample->power);
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
        
		for (i=0; i<NUM_SENSORS; i++) 
		sample->energy[i] += sample->power[i] * secs;

		// read sensors ********************************* 
		
		sample->samples++;
		// DEBUG
		fprintf(debugf,"%ld;", sample->samples);
		
		fprintf(debugf,"%ld;", (long)dif.tv_nsec);	
		
		dif=diff(sample->start_time, sample->res[sample->now]); 
	
	//sample->time=(double)res.tv_sec+ (double)res.tv_nsec/1000000000.0;

		fprintf(debugf,"%lf ;", (double)dif.tv_sec+ (double)dif.tv_nsec/1000000000.0);
                for(i=0; i<NUM_SENSORS; i++)	
		fprintf(debugf,"%lf;",sample->power[i]);
		fprintf(debugf,"\n");
		//if(sample->samples<20) printf("%lf;",memW);
		
		//read_sensors(sample->a7W, sample->a15W, sample->gpuW, sample->memW);
		
		pthread_mutex_unlock(&(sample->mutex));
		
		usleep(sample->sample_rate);
	}
	
}


//-------------------------------------------------------------------

struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

//-------------------------------------------------------------------
