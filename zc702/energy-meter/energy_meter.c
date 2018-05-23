/*
 * Energy meter library v1.0
 * for ODROID XU+E
 * 2014 DAC UMA andres@uma.es
 * 
 * Read header file for changes log
 */

#include "energy_meter.h"

#include "i2c-dev.h"
 
float convert_pmbus_reading2(uint16_t pmbus_reading)
{
	uint8_t x_e_u;
	int x_e,exp_v;
	int i;
	float x_p;
	int16_t x_m;

	/* TODO: Handle negative mantissa:
		It will never happen since current and power can't be negative */

	/* Extract the mantissa and exponent. See Section 7.1 in PMBus Spec. - Part 2*/
	x_m = (pmbus_reading & 0x07FF);
	x_e_u = ((pmbus_reading & 0xF800) >> 11);

	//printf ("mantisa is %d ,exponent is %d\n",x_m,x_e_u);

	/* Convert from 2's complement */
	if (x_e_u > 15) {
		x_e_u = ((x_e_u ^ 255) + 1) & 0x1F;
		x_e = -x_e_u;
	}
	else {
		x_e = x_e_u;
	}

	x_e = -x_e;

	//printf ("Exponent after adjustment %d  \n",x_e);

	exp_v = 1;
	//x_p = 1/exp2((float)x_e);
	for (i=0;i<x_e;i++)
		exp_v = exp_v * 2;

	x_p = 1.0/(float)exp_v;
	//printf ("x_p is %f  \n",x_p);
	return ((float)x_m)*x_p;
}


float readVoltage(int iic_fd, unsigned char deviceAddress, unsigned char pageAddress) {
	float voltage;
	int status;

	if (ioctl(iic_fd, I2C_SLAVE, deviceAddress) < 0) {
		printf("ERROR: Unable to set I2C slave address 0x%02X\n", deviceAddress);
		exit(1);
	}

	status = i2c_smbus_write_byte_data(iic_fd, CMD_PAGE, pageAddress);
	if (status < 0) {
		printf("ERROR: Unable to write page address to I2C slave at 0x%02X: %d\n", deviceAddress, status);
		exit(1);
	}

	/* Read in the voltage value */
	status = i2c_smbus_read_word_data(iic_fd, CMD_READ_VOUT);
	if(status < 0) {
		printf("ERROR: Unable to read VOUT on I2C slave at 0x%02X: %d\n", deviceAddress, status);
		exit(1);
	}

	voltage = status / 4096.0f;

	return voltage;
}


float readCurrent(int iic_fd, unsigned char deviceAddress, unsigned char pageAddress) {
	double current;
	int status;

	if (ioctl(iic_fd, I2C_SLAVE, deviceAddress) < 0) {
		printf("ERROR: Unable to set I2C slave address 0x%02X\n", deviceAddress);
		exit(1);
	}

	status = i2c_smbus_write_byte_data(iic_fd, CMD_PAGE, pageAddress);
	if (status < 0) {
		printf("ERROR: Unable to write page address to I2C slave at 0x%02X: %d\n", deviceAddress, status);
		exit(1);
	}

	/* Read in the voltage value */
	status = i2c_smbus_read_word_data(iic_fd, CMD_READ_IOUT);
	if(status < 0) {
		printf("ERROR: Unable to read IOUT on I2C slave at 0x%02X: %d\n", deviceAddress, status);
		exit(1);
	}

	/* We have to decode the LINEAR11 format. The format is composed of a 16-bit
	 * value with an 11-bit mantissa and a 5-bit exponent. The mantissa has 10
	 * significant bits with a sign bit allowing for values between -1024 and +1023.
	 * The exponent has four significant bits plus one sign bit allowing for exponent
	 * values between -16 to +15, or a full range of 2^-16 to 1023*2^15
	 */
	//current = linear11ToFloat((unsigned char)((status >> 8) & 0xff), (unsigned char)(status & 0xff));
	current = convert_pmbus_reading2((uint16_t)status);
	//printf("current from sensor %f\n",current);
	return current;
}



// read sensors *********************************
#define read_sensors(cpuW, fpgaW, memW) {\
		double voltage,current;  \
		int iic_fd = open("/dev/i2c-8", O_RDWR);\
		if (iic_fd < 0) {\
			printf("ERROR: Unable to open /dev/i2c-8 for PMBus access: %d\n", iic_fd);\
			exit(1);\
		} \
		voltage = readVoltage(iic_fd, 52, 1);\
		current = readCurrent(iic_fd, 52, 1);\
		cpuW=voltage*current;\
		voltage = readVoltage(iic_fd, 52, 0);\
		current = readCurrent(iic_fd, 52, 0);\
		fpgaW=voltage*current;\
		voltage = readVoltage(iic_fd, 53, 1);\
		current = readCurrent(iic_fd, 53, 1);\
		memW=voltage*current;\
		close(iic_fd);}
		
		
		
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
	sample=(struct energy_sample *) malloc(sizeof(struct energy_sample ));

		
	sample->sample_rate=sample_rate*1000; // in microseconds to use in usleep()
	sample->eCPU=0.0;
	sample->eFPGA=0.0;
	sample->eMEM=0.0;
	sample->pCPU=0.0;
	sample->pFPGA=0.0;
	sample->wCPU=0.0;
	sample->wFPGA=0.0;
	sample->wMEM=0.0;
	sample->pMEM=0.0;
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
   
	pthread_mutex_lock(&(sample->mutex));  // stop energy sampling
	clock_gettime(CLOCK_REALTIME, &(sample->stop_time));
	res=diff(sample->start_time, sample->stop_time);
	
	sample->time=(double)res.tv_sec+ (double)res.tv_nsec/1000000000.0;
	
	
	//read_sensors(sample1->a7W, sample1->cpuW, sample1->FPGAW, sample1->memW);
    //sample->now=!sample->now;
	// get time now**********************************
	//clock_gettime(CLOCK_REALTIME, &dif );
	// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
	dif.tv_nsec=sample->stop_time.tv_nsec - sample->res[sample->now].tv_nsec;
	if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
	// claculate energy until now **************************************
	secs= dif.tv_nsec/1000000000.0; // move to seconds
	// Watt*sec=Joules
	/*sample->eCPU += (sample->cpuW - sample->pCPU) * secs; //remove idle power	
	sample->eFPGA += (sample->fpgaW - sample->pFPGA )* secs;
	sample->eMEM += (sample->memW - sample->pMEM) * secs;*/
	
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
	res=diff(sample1->start_time, sample1->stop_time);
	
	fprintf(fout,"+--------------------+\n");
	fprintf(fout,"| POWER/ENERGY MEASUREMENTS |\n");
	fprintf(fout,"+--------------------+\n");
	fprintf(fout,"P CPU= %lf mW :: FPGA= %lf mW :: Mem= %lf mW\n",1000*sample1->cpuW ,1000*sample1->fpgaW,1000*sample1->memW);
	fprintf(fout,"E CPU= %lf mJ :: FPGA= %lf mJ :: Mem= %lf mJ\n",1000*sample1->eCPU,1000*sample1->eFPGA,1000*sample1->eMEM);
	fprintf(fout,"TOTAL E= %lf mJ\n",(1000*total_em((*sample1))));
	fprintf(fout,"CLOCK_REALTIME = %lf sec\n",(double)res.tv_sec+ (double)res.tv_nsec/1000000000.0);	
	fprintf(fout,"# of samples: %d\n", sample1->samples);
	fprintf(fout,"sample every (real) = %lf sec\n",((double)res.tv_sec+ (double)res.tv_nsec/1000000000.0)/sample1->samples);	
	fprintf(fout,"sample every: %lf sec\n",(double)sample1->sample_rate/1000000);	
	
}

void power_meter_idle(struct energy_sample *sample)
{
	// read 10 times sensors 
	int i;
	double cpuW,fpgaW,memW;

	cpuW = 0.0;
	fpgaW = 0.0;
	memW = 0.0;

	for(i=0;i<10;i++)
	{
		read_sensors(sample->pCPU, sample->pFPGA, sample->pMEM);
		cpuW += sample->pCPU;
		fpgaW += sample->pFPGA;
		memW += sample->pMEM;
	}

	sample->pCPU = cpuW/10.0;
	sample->pFPGA= fpgaW/10.0;
	sample->pMEM = memW/10.0;
	printf("idle CPU= %lf W :: idle FPGA= %lf W :: idle Mem= %lf W\n",sample->pCPU,sample->pFPGA,sample->pMEM);
	
}
	

//-------------------------------------------------------------------

void energy_meter_read(struct energy_sample *sample, struct em_t * out)
{
	double secs;
	struct timespec dif;
	int i;
	double cpuW,fpgaW,memW;
	float loop_samples;   

	// mutex 
	pthread_mutex_lock(&(sample->mutex));

	/*	out->wCPU= sample->cpuW;
		out->wFPGA= sample->fpgaW;
		out->wMEM= sample->memW; */
	
		sample->now=!sample->now;
		// get time now**********************************
		clock_gettime(CLOCK_REALTIME, sample->res+sample->now );

		cpuW = 0.0;
		fpgaW = 0.0;
		memW = 0.0;
		loop_samples = 0.0;

		for(i=0;i<10;i++)
		{
			read_sensors(sample->cpuW, sample->fpgaW, sample->memW);
                        if (sample->cpuW < 2.0 && sample->fpgaW < 2.0 && sample->memW < 2.0)
			{
				cpuW += sample->cpuW;
				fpgaW += sample->fpgaW;
				memW += sample->memW;
				loop_samples++;
			}
		}
		
		sample->cpuW = cpuW/loop_samples;
		sample->fpgaW= fpgaW/loop_samples;
		sample->memW = memW/loop_samples;

		printf("final CPU w %f samples %d\n",sample->cpuW,sample->samples);
	
		// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
		dif.tv_nsec=sample->res[sample->now].tv_nsec-sample->res[!sample->now].tv_nsec;
		if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
		
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
        
		// Watt*sec=Joules
		//fprintf(stdout,"active= %lf W : idle= %lf W\n",sample->fpgaW ,sample->pFPGA);
		/*fprintf(stdout,"fpga active= %lf W : idle= %lf W\n",sample->fpgaW ,sample->pFPGA);
		fprintf(stdout,"cpu active= %lf W : idle= %lf W\n",sample->cpuW ,sample->pCPU);
		fprintf(stdout,"mem active= %lf W : idle= %lf W\n",sample->memW ,sample->pMEM);*/
		//sample->eCPU += (sample->cpuW - sample->pCPU)  * secs; //remove idle power
		//sample->eFPGA += (sample->fpgaW - sample->pFPGA) * secs;
		//sample->eMEM += (sample->memW - sample->pMEM) * secs;
		sample->eCPU += (sample->cpuW)  * secs; //full energy
		sample->eFPGA += (sample->fpgaW) * secs;
		sample->eMEM += (sample->memW) * secs;
		sample->wCPU += (sample->cpuW); //full power
		sample->wFPGA += (sample->fpgaW);
		sample->wMEM += (sample->memW);
		//fprintf(stdout,"E fpga= %lf J : cpu= %lf J mem=%lf J and secs %lf\n",sample->eFPGA,sample->eCPU ,sample->eMEM,secs);
				
		sample->samples++;
	
	out->eCPU= sample->eCPU;
	out->eFPGA= sample->eFPGA;
	out->eMEM= sample->eMEM;
	
	out->wCPU= (sample->wCPU/sample->samples);
	out->wFPGA= (sample->wFPGA/sample->samples);
	out->wMEM= (sample->wMEM/sample->samples);
	printf("final average CPU power %f \n",out->wCPU);


	//printf("final CPU w %f\n",out->wCPU);
	
	pthread_mutex_unlock(&(sample->mutex));
	
}
//-------------------------------------------------------------------
/*void energy_meter_diff(struct energy_sample *sample, struct em_t * diff)
{
	double secs;
	struct timespec dif;
   
	double CPU,FPGA,MEM;
		
	// mutex 
	pthread_mutex_lock(&(sample->mutex));
	
		sample->now=!sample->now;
		// get time now**********************************
		clock_gettime(CLOCK_REALTIME, sample->res+sample->now );
		read_sensors(sample->cpuW, sample->fpgaW, sample->memW);
		// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
		dif.tv_nsec=sample->res[sample->now].tv_nsec-sample->res[!sample->now].tv_nsec;
		if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
        
		// Watt*sec=Joules
		sample->eCPU += sample->cpuW * secs;
		sample->eFPGA += sample->fpgaW * secs;
		sample->eMEM += sample->memW * secs;
				
		sample->samples++;
	
	
	diff->eCPU= sample->eCPU - diff->eCPU;
	diff->eFPGA= sample->eFPGA - diff->eFPGA;
	diff->eMEM= sample->eMEM - diff->eMEM;
	
	pthread_mutex_unlock(&(sample->mutex));

}*/
//-------------------------------------------------------------------
void energy_meter_read_printf(struct em_t * diff, FILE *fout)
{
	fprintf(fout,"POWER READ ----------------\n");
	fprintf(fout,"CPU= %lf J :: FPGA= %lf J :: Mem= %lf J\n",diff->eCPU,diff->eFPGA,diff->eMEM);
}
//-------------------------------------------------------------------


void *meter_function(void *arg)
{
	struct energy_sample *sample=(struct energy_sample *) arg;
	
	struct timespec dif;
	int i;
	double cpuW,fpgaW,memW;
	double secs;
	float loop_samples;
	
    sample->now=0;
    // first sample
 	pthread_mutex_lock(&(sample->mutex));
 	clock_gettime(CLOCK_REALTIME, sample->res);
 	read_sensors(sample->cpuW, sample->fpgaW, sample->memW);
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

		cpuW = 0.0;
		fpgaW = 0.0;
		memW = 0.0;
		loop_samples =0.0;

		for(i=0;i<10;i++)
		{
			read_sensors(sample->cpuW, sample->fpgaW, sample->memW);		 
			if (sample->cpuW < 2.0 && sample->fpgaW < 2.0 && sample->memW < 2.0)
			{
				cpuW += sample->cpuW;
				fpgaW += sample->fpgaW;
				memW += sample->memW;
				loop_samples++;
			}
		}
		
		sample->cpuW = cpuW/loop_samples;
		sample->fpgaW= fpgaW/loop_samples;
		sample->memW = memW/loop_samples;
	
		// get time interval    !!! only nanoseconds, sampling rate must be below 1 second
		dif.tv_nsec=sample->res[sample->now].tv_nsec-sample->res[!sample->now].tv_nsec;
		if(	dif.tv_nsec <0)	dif.tv_nsec += 1000000000;
		
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
        
	    // Watt*sec=Joules

		//fprintf(stdout,"fpga active= %lf W : idle= %lf W\n",sample->fpgaW ,sample->pFPGA);
		//fprintf(stdout,"cpu = %lf J : sample= %d secs =%lf power = %lf \n",sample->eCPU ,sample->samples,secs,(sample->cpuW - sample->pCPU));
		//fprintf(stdout,"mem active= %lf W : idle= %lf W\n",sample->memW ,sample->pMEM);*/
		//sample->eCPU += (sample->cpuW - sample->pCPU)  * secs; //remove idle power
		//sample->eFPGA += (sample->fpgaW - sample->pFPGA) * secs;
		//sample->eMEM += (sample->memW - sample->pMEM) * secs;

		sample->eCPU += (sample->cpuW)  * secs; //full energy
		sample->eFPGA += (sample->fpgaW) * secs;
		sample->eMEM += (sample->memW) * secs;
		sample->wCPU += (sample->cpuW); //full power
		sample->wFPGA += (sample->fpgaW);
		sample->wMEM += (sample->memW);

//printf("CPU w %f\n",sample->cpuW);
		
		sample->samples++;
		// DEBUG
		// fprintf(stdout,"a7= %lf W : cpu= %lf W : FPGA= %lf W \n",a7W,cpuW,FPGAW);
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
	int fcpu,ffpga, fmem;
	//double a7W=0.0, cpuW=0.0, FPGAW=0.0, memW=0.0;
	double secs;
    int c1,c2,c3,c4,cFPGA;
    char fn[256];
    sprintf(fn,"debug_energy_meter%d.csv",getpid());
    debugf=fopen(fn,"w");
    fprintf(debugf,"#;sample;time;CPU;FPGA;Mem;Mhz1;Mhz2;Mhz3;Mhz4;MhzFPGA\n");	
    // first sample
    sample->now=0;
 	pthread_mutex_lock(&(sample->mutex));
 	clock_gettime(CLOCK_REALTIME, sample->res);
 	read_sensors(sample->cpuW, sample->fpgaW, sample->memW);
	
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
		
		read_sensors(sample->cpuW, sample->fpgaW, sample->memW);
		
		// claculate energy  **************************************
		secs= dif.tv_nsec/1000000000.0; // move to seconds
        
		// Watt*sec=Joules
		sample->eCPU += sample->cpuW * secs;
		sample->eFPGA += sample->fpgaW * secs;
		sample->eMEM += sample->memW * secs;

		// read sensors ****************************
		
		sample->samples++;
		// DEBUG
		fprintf(debugf,"%d;", sample->samples);
		
		fprintf(debugf,"%ld;", (long)dif.tv_nsec);	
		
		dif=diff(sample->start_time, sample->res[sample->now]); 
	
	//sample->time=(double)res.tv_sec+ (double)res.tv_nsec/1000000000.0;

		fprintf(debugf,"%lf ;", (double)dif.tv_sec+ (double)dif.tv_nsec/1000000000.0);
	
		fprintf(debugf,"%lf;%lf;",sample->cpuW,sample->fpgaW);
		fprintf(debugf,"%lf;",sample->memW);

		
		//read_sensors(sample->a7W, sample->cpuW, sample->FPGAW, sample->memW);
		
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
