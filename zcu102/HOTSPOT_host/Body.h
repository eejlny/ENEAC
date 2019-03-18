
#ifndef _BODY_TASK_
#define _BODY_TASK_


#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include "hotspot.h"
#include "kernelHotspot.h"

#define CASO0
using namespace tbb;

/*****************************************************************************
 * NbodyTask
 * **************************************************************************/
class Body
{
public:
	bool firsttime;
public:

	void sendObjectToGPU( int begin, int end) {
		
	}

	void OperatorGPU(int begin, int end, int id ) {

        float Rx_1=1.f/Rx;
	float Ry_1=1.f/Ry;
	float Rz_1=1.f/Rz;
	float Cap_1 = step/Cap;
        int end_fpga = end-1;
		
        //printf("FPGA is doing from %d to %d\n",begin,end_fpga);
 	////int result=kernelHotspot((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1,interrupt,status,file_desc, begin,end_fpga);
	
	switch(id)
		{
			case 1 : kernelHotspot1((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, begin, end_fpga); break;
			case 2 : kernelHotspot2((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, begin, end_fpga); break;
			case 3 : kernelHotspot3((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, begin, end_fpga); break;
			case 4 : kernelHotspot4((float*)array_temp,(float*)array_power,(float*)array_out,Cap_1,Rx_1,Ry_1,Rz_1, begin, end_fpga); break;
		}


	}

	void getBackObjectFromGPU(int begin, int end) {

		
	}

	void OperatorCPU(int begin, int end) {

	    float *temp;
	    float *power;
	    float *result;
	    int row;
	    int col;
	    float delta;

	    temp = array_temp+(grid_cols+2);
	    power = array_power+(grid_cols+2);
	    result = array_out+(grid_cols+2);
	    //row = grid_rows;
            col = (grid_cols+2);
	    float Rx_1=1.f/Rx;
	    float Ry_1=1.f/Ry;
	    float Rz_1=1.f/Rz;
	    float Cap_1 = step/Cap;
//    FLOAT Cap_1 = step/Cap;


            for (int r = begin; r < end; ++r ) {
                for (int  c = 1; c <= grid_cols; ++c ) {
			result[r*col+c] =temp[r*col+c]+ 
				( Cap_1 * (power[r*col+c] + 
				(temp[(r+1)*col+c] + temp[(r-1)*col+c] - 2.f*temp[r*col+c]) * Ry_1 + 
				(temp[r*col+c+1] + temp[r*col+c-1] - 2.f*temp[r*col+c]) * Rx_1 + 
				(amb_temp - temp[r*col+c]) * Rz_1));
		    
                }
            }



	}

	void AllocateMemoryObjects() {

	 
	}
};
//end class

#endif
