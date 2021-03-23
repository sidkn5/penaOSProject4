#include <stdlib.h>

#define MAX 18

typedef struct{
	//just this for now
	float lastExecTime;
	int cpuTimeMs;
	int systemTimeMs;
}processControlBlock;

typedef struct{
	float runTime;
	float waitTimeUsed;
	float cpuTimeUsed;

	processControlBlock pcb[MAX];
}holder;
