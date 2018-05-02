// calculamos la velocidad y posicion de un cuerpo para un paso de tiempo
/*
void toFPGA(OctTreeLeafNode b, int nbodies)
{
	OctTreeLeafNode bodybuf[TAMBODIES];
}
*/

#include "NBodyHW.h"

int ComputeChunkFPGA(
bool firstime,
int nbodies,			
int step,
float epssq,
float dthf,
int begin, 
int end,
float b[NUMBODIES*stsize]) {

	register float ax, ay, az, waccx, waccy, waccz, px, py, pz;
	register float drx, dry, drz, drsq, nphi, scale, idr;
	float mass[NUMBODIES];
	float posx[NUMBODIES];
	float posy[NUMBODIES];
	float posz[NUMBODIES];

#pragma HLS array_partition variable=mass cyclic factor=16
#pragma HLS array_partition variable=posx cyclic factor=16
#pragma HLS array_partition variable=posy cyclic factor=16
#pragma HLS array_partition variable=posz cyclic factor=16

	if (firstime)
	{
		for(int i=0; i<NUMBODIES;i++){
#pragma HLS PIPELINE II=1
			mass[i]=b[i*stsize+imass];
			posx[i]=b[i*stsize+iposx];
			posy[i]=b[i*stsize+iposy];
			posz[i]=b[i*stsize+iposz];
		}
	}

	for (int i = begin; i < end; i++) {

		ax = b[i*stsize+iaccx];
		ay = b[i*stsize+iaccy];
		az = b[i*stsize+iaccz];

		px = posx[i];
		py = posy[i];
		pz = posz[i];

		waccx = 0.0;
		waccy = 0.0;
		waccz = 0.0;

		//IterativeForce(b);

		for(int j=0; j < NUMBODIES; j++){
#pragma HLS PIPELINE II=1
//#pragma HLS unroll factor=16
//#pragma HLS loop_tripcount min=256 max=10000

			drx = posx[j] - px;
			dry = posy[j] - py;
			drz = posz[j] - pz;

			drsq = drx * drx + dry * dry + drz * drz + epssq;
			idr = 1 / sqrtf(drsq);
			nphi = mass[j] * idr;
			scale = nphi * idr * idr;

			waccx += drx * scale;
			waccy += dry * scale;
			waccz += drz * scale;
		}

		if (step > 0) {
			b[i*stsize+ivelx] += (waccx - ax) * dthf;
			b[i*stsize+ively] += (waccy - ay) * dthf;
			b[i*stsize+ivelz] += (waccz - az) * dthf;
		}

		b[i*stsize+iaccx]=waccx;
		b[i*stsize+iaccy]=waccy;
		b[i*stsize+iaccz]=waccz;
	}

	return 0;
}
