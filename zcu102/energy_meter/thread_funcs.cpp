#include <pthread.h>
#include <sched.h>
#include <asm/unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


#define gettid()  syscall(__NR_gettid)

///   sched_getcpu()     to get core ID

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

int set_thread_affinity_CORE(pthread_t th, int cpu)
{
	cpu_set_t mask;
	__CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	__CPU_SET_S(cpu,sizeof(cpu_set_t), &mask);
	
	return pthread_setaffinity_np(th, sizeof(cpu_set_t), &mask);
}

int set_thread_affinity_A7(pthread_t th)
{
	cpu_set_t mask;
	__CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	for(int i=0; i<4; i++) __CPU_SET_S(i,sizeof(cpu_set_t), &mask);
	
	return pthread_setaffinity_np(th, sizeof(cpu_set_t), &mask);
}

int set_thread_affinity_A15(pthread_t th)
{
	cpu_set_t mask;
	__CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	for(int i=4; i<8; i++) __CPU_SET_S(i,sizeof(cpu_set_t), &mask);
	
	return pthread_setaffinity_np(th, sizeof(cpu_set_t), &mask);
}

int set_thread_prio(pthread_t th, int n)
{
  struct sched_param sp;
  sp.sched_priority=n;
  pthread_setschedparam(pthread_self(), SCHED_OTHER, &sp);  // only n=0 allowed
}


int set_thread_prio_RT(pthread_t th, int n)
{
  struct sched_param sp;
  sp.sched_priority=n;
  pthread_setschedparam(pthread_self(), SCHED_RR, &sp);  // 
}

void display_sched_attr(int policy, struct sched_param *param)
       {
           printf("    policy=%s, priority=%d\n",
                   (policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                   (policy == SCHED_RR)    ? "SCHED_RR" :
                   (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                   "???",
                   param->sched_priority);
       }

void display_thread_prio(char *msg)
       {
           int policy, s;
           struct sched_param param;

           s = pthread_getschedparam(pthread_self(), &policy, &param);
           if (s != 0)
               handle_error_en(s, "pthread_getschedparam");

           printf("%s\n", msg);
           display_sched_attr(policy, &param);
       }
       
 
class Thread_pinning {
private:  
  pthread_mutex_t mux;   // for auto pinning
  int ncores;  // size of pool of cores for auto pinnig
  int * cores; // list of cores for auto pinnig
  int count;   // index of next core to assing in auto
  
public:
  Thread_pinning(int n, int* c)
  {
	  ncores=n;
	  cores=(int *) malloc(n*sizeof(int));
	  memcpy(cores,c,n*sizeof(int));
	  pthread_mutex_init(&mux, NULL);
	  count=0;
  }
  ~Thread_pinning()
  {
	  free(cores);
	  pthread_mutex_destroy(&mux);
  }
  int next()
  {
	  int mine=-1;
	  cpu_set_t mask;
	  pthread_mutex_lock (&mux);  // protect count
	  if(count>=ncores) count=0;
	  if(count<ncores) mine=cores[count];
	  count++;
	  pthread_mutex_unlock (&mux);
	  if(mine==-1) return mine;  // no more cores availables
	  __CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	  __CPU_SET_S(mine,sizeof(cpu_set_t), &mask);
	  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask);
	 
  }
  int last()
  {
	  cpu_set_t mask;
	  __CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	  __CPU_SET_S(cores[ncores-1] ,sizeof(cpu_set_t), &mask);
	  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask);
  }
  int core(int c)
  {
	  cpu_set_t mask;
	  __CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	  __CPU_SET_S(c ,sizeof(cpu_set_t), &mask);
	  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask);
  }
  int group(int n, int *c)
  {
	  cpu_set_t mask;
	  __CPU_ZERO_S(sizeof(cpu_set_t),&mask);
	  for(int i=0; i<n; i++) __CPU_SET_S(c[i],sizeof(cpu_set_t), &mask);
	  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask);
  }
};



