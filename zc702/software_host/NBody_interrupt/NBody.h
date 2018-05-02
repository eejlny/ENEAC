
#ifndef _N_BODY_
#define _N_BODY_

/*****************************************************************************
 * Data Structure
 * **************************************************************************/
const int CELL = 0;
const int BODY = 1;
const int UNUSED = 0;
const int USED = 1;
int *interrupt;
int *status;
int local_interrupt;
int file_desc;


#define HW_ADDR_GPIO 0x41200000
#define HW_ADDR_GPIO_INT 0x41200120
#define fpga_mmio_gpio 0x48 /*location of status register in GPIO. wrte this to generate interrupt*/


#ifdef XILINX
#include "kernelNBody.h"
#include "sds_lib.h"
#endif

typedef struct {
	float mass;
	float posx;
	float posy;
	float posz;
	float accx;
	float accy;
	float accz;
	float dsq;
} OctTreeLeafNode;

typedef struct {
	float posx;
	float posy;
	float posz;
} TempNode;

typedef struct {
	float velx;
	float vely;
	float velz;
} VelNode;

/*****************************************************************************
 * Global Variables
 * **************************************************************************/
OctTreeLeafNode *bodies, *bodies2, *aux; // array of bodies
TempNode *bodies_temp;
VelNode *bodies_vel;

int nbodies; // number of bodies
int timesteps; // number of steps
int nthreads; // number of threads
float dtime; // differential time
float eps; // potential softening parameter
float tol; // should be less than 0.57 for 3D case to bound error
float dthf, epssq, itolsq;
int step;

/*****************************************************************************
 * Functions
 * **************************************************************************/
/*
inline void copy_aux_array() {
	aux = bodies;
	bodies = bodies2;
	bodies2 = aux;
}*/

inline void copy_body(OctTreeLeafNode *a, VelNode *a_vel, OctTreeLeafNode *ch, VelNode *ch_vel) {
	a->mass = ch->mass;
	a->posx = ch->posx;
	a->posy = ch->posy;
	a->posz = ch->posz;
	a_vel->velx = ch_vel->velx;
	a_vel->vely = ch_vel->vely;
	a_vel->velz = ch_vel->velz;
	a->accx = ch->accx;
	a->accy = ch->accy;
	a->accz = ch->accz;
}

inline void setVelocity(VelNode *b, const float x, const float y,
		const float z) {
	b->velx = x;
	b->vely = y;
	b->velz = z;
}

inline void ReadInput(char *filename) {
	register float vx, vy, vz;
	register FILE *f;
	register int i;
	OctTreeLeafNode *b;
	VelNode *b_vel;

	f = fopen(filename, "r+t");
	if (f == NULL) {
		fprintf(stderr, "file not found: %s\n", filename);
		exit(-1);
	}

	fscanf(f, "%d", &nbodies);
	fscanf(f, "%d", &timesteps);
	fscanf(f, "%f", &dtime);
	fscanf(f, "%f", &eps);
	fscanf(f, "%f", &tol);

	dthf = 0.5f * dtime;
	epssq = eps * eps;
	itolsq = 1.0f / (tol * tol);

	if (bodies == NULL) {
#ifdef XILINX
		printf("Allocating memory\n");		
		bodies = (OctTreeLeafNode *) sds_alloc(sizeof(OctTreeLeafNode) * nbodies);
		bodies_temp = (TempNode *) sds_alloc(sizeof(TempNode) * nbodies);
		bodies_vel = (VelNode *) sds_alloc(sizeof(VelNode) * nbodies);
	
		if (!bodies)
		{
			printf("memory allocation error\n");
			exit(1);
		}
		else
		{
			printf("allocated %d bytes\n",(sizeof(OctTreeLeafNode) * nbodies));
		}
#else
		bodies = (OctTreeLeafNode *) malloc(sizeof(OctTreeLeafNode) * nbodies);
#endif
		

	}

	for (i = 0; i < nbodies; i++) {
		b = &bodies[i];
		b_vel = &bodies_vel[i];
		
		fscanf(f, "%fE", &(b->mass));
		fscanf(f, "%fE", &(b->posx));
		fscanf(f, "%fE", &(b->posy));
		fscanf(f, "%fE", &(b->posz));
		fscanf(f, "%fE", &vx);
		fscanf(f, "%fE", &vy);
		fscanf(f, "%fE", &vz);
		setVelocity(b_vel, vx, vy, vz);
		b->accx = 0.0;
		b->accy = 0.0;
		b->accz = 0.0;
	}
	fclose(f);
}

inline void Printfloat(float d) {
	register int i;
	char str[16];

	sprintf(str, "%.4lE", (double)d);

	i = 0;
	while ((i < 16) && (str[i] != 0)) {
		if ((str[i] == 'E') && (str[i + 1] == '-') && (str[i + 2] == '0')
				&& (str[i + 3] == '0')) {
			printf("E00");
			i += 3;
		} else if (str[i] != '+') {
			printf("%c", str[i]);
		}
		i++;
	}
}

void ComputeCenterAndDiameter(const int n, float & diameter, float & centerx,
		float & centery, float & centerz) {
	register float minx, miny, minz;
	register float maxx, maxy, maxz;
	register float posx, posy, posz;
	register int i;
	OctTreeLeafNode *b;
	minx = (float)1.0E90;
	miny = (float)1.0E90;
	minz = (float)1.0E90;
	maxx = (float)-1.0E90;
	maxy = (float)-1.0E90;
	maxz = (float)-1.0E90;

	for (i = 0; i < n; i++) {
		b = &bodies[i];
		posx = b->posx;
		posy = b->posy;
		posz = b->posz;

		if (minx > posx)
			minx = posx;
		if (miny > posy)
			miny = posy;
		if (minz > posz)
			minz = posz;

		if (maxx < posx)
			maxx = posx;
		if (maxy < posy)
			maxy = posy;
		if (maxz < posz)
			maxz = posz;
	}

	diameter = maxx - minx;
	if (diameter < (maxy - miny))
		diameter = (maxy - miny);
	if (diameter < (maxz - minz))
		diameter = (maxz - minz);

	centerx = (maxx + minx) * 0.5f;
	centery = (maxy + miny) * 0.5f;
	centerz = (maxz + minz) * 0.5f;
}


// actualizamos la velocidad y posicion de un cuerpo de un cuerpo en un paso
void Advance(OctTreeLeafNode *b, VelNode *b_vel) {
	register float dvelx, dvely, dvelz;
	register float velhx, velhy, velhz;

	dvelx = b->accx * dthf;
	dvely = b->accy * dthf;
	dvelz = b->accz * dthf;

	velhx = b_vel->velx + dvelx;
	velhy = b_vel->vely + dvely;
	velhz = b_vel->velz + dvelz;

	b->posx += velhx * dtime;
	b->posy += velhy * dtime;
	b->posz += velhz * dtime;

	b_vel->velx = velhx + dvelx;
	b_vel->vely = velhy + dvely;
	b_vel->velz = velhz + dvelz;
}

void IterativeForce(OctTreeLeafNode *body) {
	register float drx, dry, drz, drsq, nphi, scale, idr;

	for(int i=0; i < nbodies; i++){

		drx = bodies[i].posx - body->posx;
		dry = bodies[i].posy - body->posy;
		drz = bodies[i].posz - body->posz;

		drsq = drx * drx + dry * dry + drz * drz + epssq;
		idr = 1 / sqrt(drsq);
		nphi = bodies[i].mass * idr;
		scale = nphi * idr * idr;

		body->accx += drx * scale;
		body->accy += dry * scale;
		body->accz += drz * scale;
	}
}

// calculamos la velocidad y posicion de un cuerpo para un paso de tiempo
void ComputeForce(OctTreeLeafNode *b, VelNode *b_vel) {
	register float ax, ay, az;

	ax = b->accx;
	ay = b->accy;
	az = b->accz;

	b->accx = 0.0;
	b->accy = 0.0;
	b->accz = 0.0;

	IterativeForce(b);

	if (step > 0) {
		b_vel->velx += (b->accx - ax) * dthf;
		b_vel->vely += (b->accy - ay) * dthf;
		b_vel->velz += (b->accz - az) * dthf;
	}
}



#endif
