// calculamos la velocidad y posicion de un cuerpo para un paso de tiempo
/*
void toFPGA(OctTreeLeafNode b, int nbodies)
{
	OctTreeLeafNode bodybuf[TAMBODIES];
}
*/

#include "NBodyHW.h"


const int UNROLL_FACTOR=20;
////const int PART_FACTOR = 5;
const int PART_FACTOR = 1;

////#pragma SDS data zero_copy(b_vel[0:NUMBODIES*3])
//#pragma SDS data access_pattern(b_vel:SEQUENTIAL)
////#pragma SDS data mem_attribute(b_vel:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(b[0:NUMBODIES*stsize])
//#pragma SDS data access_pattern(b:RANDOM)
//#pragma SDS data access_pattern(b:SEQUENTIAL)
#pragma SDS data mem_attribute(b:PHYSICAL_CONTIGUOUS)
////#pragma SDS data zero_copy(b_temp[0:NUMBODIES*3])
//#pragma SDS data access_pattern(b_temp:SEQUENTIAL)
////#pragma SDS data mem_attribute(b_temp:PHYSICAL_CONTIGUOUS)
////#pragma SDS data zero_copy(status[0:1])
//#pragma SDS data access_pattern(b_temp:RANDOM)
//#pragma SDS data mem_attribute(b: NON_CACHEABLE)
//#pragma SDS data data_mover(b_in:AXIDMA_SIMPLE)
void ComputeChunkFPGA(
bool firstime,
int nbodies,
int step,
float epssq,
float dthf,
int begin,
int end,
float b[NUMBODIES*stsize]
////float b_vel[NUMBODIES*3],
////float b_temp[NUMBODIES*3]
////int *interrupt,
////int *status
, int &dummy) {

    register float ax, ay, az, waccx, waccy, waccz, px, py, pz;
    float mass[NUMBODIES/PART_FACTOR];
    float posx[NUMBODIES/PART_FACTOR];
    float posy[NUMBODIES/PART_FACTOR];
    float posz[NUMBODIES/PART_FACTOR];


////for(int z=0; z<PART_FACTOR; z++)
////{

	#pragma HLS array_partition variable=mass cyclic factor=UNROLL_FACTOR
	#pragma HLS array_partition variable=posx cyclic factor=UNROLL_FACTOR
	#pragma HLS array_partition variable=posy cyclic factor=UNROLL_FACTOR
	#pragma HLS array_partition variable=posz cyclic factor=UNROLL_FACTOR


        for(int i=0; i<NUMBODIES/PART_FACTOR;i++){
	    #pragma HLS PIPELINE II=1
            ////mass[i]=b[i*stsize+imass+z*(NUMBODIES/PART_FACTOR)];
            ////posx[i]=b[i*stsize+iposx+z*(NUMBODIES/PART_FACTOR)];
            ////posy[i]=b[i*stsize+iposy+z*(NUMBODIES/PART_FACTOR)];
            ////posz[i]=b[i*stsize+iposz+z*(NUMBODIES/PART_FACTOR)];
			
			mass[i]=b[i*stsize+imass];
            posx[i]=b[i*stsize+iposx];
            posy[i]=b[i*stsize+iposy];
            posz[i]=b[i*stsize+iposz];
        }


    for (int i = begin; i < end; i++) {

        ax = b[i*stsize+iaccx];
        ay = b[i*stsize+iaccy];
        az = b[i*stsize+iaccz];

        px = b[i*stsize+iposx]; //posx[i];
        py = b[i*stsize+iposy]; //posy[i];
        pz = b[i*stsize+iposz]; //posz[i];

        waccx = 0.0;
        waccy = 0.0;
        waccz = 0.0;


        for(int j=0; j < NUMBODIES/PART_FACTOR; j+=UNROLL_FACTOR){
	#pragma HLS PIPELINE II=1
	#pragma HLS PIPELINE
            float drx[UNROLL_FACTOR], dry[UNROLL_FACTOR], drz[UNROLL_FACTOR], drsq[UNROLL_FACTOR], nphi[UNROLL_FACTOR], scale[UNROLL_FACTOR], idr[UNROLL_FACTOR];


            for (int k = 0; k < UNROLL_FACTOR; k++) {
                drx[k] = posx[j+k] - px;
                dry[k] = posy[j+k] - py;
                drz[k] = posz[j+k] - pz;
            }

            for (int k = 0; k < UNROLL_FACTOR; k++) {
                drsq[k] = drx[k] * drx[k] + dry[k] * dry[k] + drz[k] * drz[k] + epssq;
                idr[k] = 1 / sqrtf(drsq[k]);
                nphi[k] = mass[j+k] * idr[k];
                scale[k] = nphi[k] * idr[k] * idr[k];
            }

            float x=0, y=0, z=0;
            for (int k = 0; k < UNROLL_FACTOR; k++) {
                x += drx[k] * scale[k];
                y += dry[k] * scale[k];
                z += drz[k] * scale[k];
            }
            waccx += x;
            waccy += y;
            waccz += z;
        } //numbodies

       

 	////if (z == (PART_FACTOR-1)) { //end
        	////b[i*stsize+iaccx]=waccx+b_temp[i*3+0];
        	////b[i*stsize+iaccy]=waccy+b_temp[i*3+1];
        	////b[i*stsize+iaccz]=waccz+b_temp[i*3+2];
			b[i*stsize+iaccx]=waccx;
        	b[i*stsize+iaccy]=waccy;
        	b[i*stsize+iaccz]=waccz;
		////waccx = waccx+b_temp[i*3+0];
		////waccy = waccy+b_temp[i*3+1];
		////waccz = waccz+b_temp[i*3+2];
   	////}
	
	/*
	else if (z > 0) { //midle
		b_temp[i*3+0]+=waccx;
        	b_temp[i*3+1]+=waccy;
        	b_temp[i*3+2]+=waccz;

	} 
	else   //start
	{
		b_temp[i*3+0]=waccx;
        	b_temp[i*3+1]=waccy;
        	b_temp[i*3+2]=waccz;
	}
	*/
	
	/*
 	if ((step > 0) && (z==(PART_FACTOR-1))) {
            b_vel[i*3+0] += (waccx - ax) * dthf;
            b_vel[i*3+1] += (waccy - ay) * dthf;
            b_vel[i*3+2] += (waccz - az) * dthf;
    	}    
	*/
	
   }//beginend
////}//partition

////         *interrupt = 255; //switch on leds
	 //status[0] = 255;
	 //status[1] = 255;
////	 *status = 255; //trigger interrupt by writting to gpio
   // return 0;
dummy = 1;
}



