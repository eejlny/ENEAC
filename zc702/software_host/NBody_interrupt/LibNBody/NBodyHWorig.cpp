#include "NBodyHW.h"

void ComputeChunkFPGA(
int nbodies,			
int step,
float epssq,
float dthf,
int begin, 
int end,
float bb[1024*stsize]) {

	register float ax, ay, az;
	register float drx, dry, drz, drsq, nphi, scale, idr;


	for (int i = begin; i < end; i++) {

		ax = b[i].accx;
		ay = b[i].accy;
		az = b[i].accz;

		b[i].accx = 0.0;
		b[i].accy = 0.0;
		b[i].accz = 0.0;

		//IterativeForce(b);

		for(int j=0; j < nbodies; j++){
#pragma HLS PIPELINE II=1
#pragma HLS loop_tripcount min=256 max=1024
			drx = b[j].posx - b[i].posx;
			dry = b[j].posy - b[i].posy;
			drz = b[j].posz - b[i].posz;

			drsq = drx * drx + dry * dry + drz * drz + epssq;
			idr = 1 / sqrtf(drsq);
			nphi = b[j].mass * idr;
			scale = nphi * idr * idr;

			b[i].accx += drx * scale;
			b[i].accy += dry * scale;
			b[i].accz += drz * scale;
		}

		if (step > 0) {
			b[i].velx += (b[i].accx - ax) * dthf;
			b[i].vely += (b[i].accy - ay) * dthf;
			b[i].velz += (b[i].accz - az) * dthf;
		}
	}
}