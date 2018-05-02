	typedef struct{																	
		float mass;																		
		float posx;																		
		float posy;																		
		float posz;																		
		float velx;																		
		float vely;																			
		float velz;																		
		float accx;																		
		float accy;																		
		float accz;																		
		float dsq;																																			
	} OctTreeLeafNode;																	

__kernel void IterativeForce(
			__global OctTreeLeafNode *bodies,						
			int step,																	
			float epssq,																
			float dthf,																
			int begin, 
			int end, 
			int nbodies) {

		float drx, dry, drz, drsq, nphi, scale, idr;
		float ax, ay, az;
	
		int idx = get_global_id(0) + begin;
		if (idx >= end) return;

		ax = bodies[idx].accx;													
		ay = bodies[idx].accy;													
		az = bodies[idx].accz;													
																							
		bodies[idx].accx = 0.0;													
		bodies[idx].accy = 0.0;													
		bodies[idx].accz = 0.0;													

		for(int i = 0; i < nbodies; i++){

			drx = bodies[i].posx - bodies[idx].posx;
			dry = bodies[i].posy - bodies[idx].posy;
			drz = bodies[i].posz -bodies[idx].posz;

			drsq = drx * drx + dry * dry + drz * drz + epssq;
			idr = 1 / sqrt(drsq);
			nphi = bodies[i].mass * idr;
			scale = nphi * idr * idr;

			bodies[idx].accx += drx * scale;
			bodies[idx].accy += dry * scale;
			bodies[idx].accz += drz * scale;
		}

		if (step > 0) {	
			bodies[idx].velx += (bodies[idx].accx - ax) * dthf;					
			bodies[idx].vely += (bodies[idx].accy - ay) * dthf;					
			bodies[idx].velz += (bodies[idx].accz - az) * dthf;					
		}																		
}

