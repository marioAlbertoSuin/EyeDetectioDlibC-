#include "sys/times.h"
#include "sys/vtimes.h"
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <limits>

#include <iostream>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
/*#include <malloc.h>
#include <map>
#include <list>*/
using namespace std;
class Metrics
{
private:
	clock_t time;
	double timeAvgSeconds;
	double timeAvgMiliseconds;
	clock_t lastCPU;
	clock_t lastSysCPU;
	clock_t lastUserCPU;
	int numProcessors;
	double cpuPercent;
	int startMemory;
	int lastMemory;
	int totalMemory;

public:
	//Metrics();
	void startCalculate(){
		this->startClock();
		this->startCPUcounter();
		this->startCallMemory();
		//this->registerSystemMetric(this);
		//this->resetMemoryCounters(); 

	}

	void calculate(){
		this->calculateExecuteTime();
		this->setNumProcessors();
		this->calculateCPU();
		this->calculateMemory();
		//this->calculateMemory() ;
		  writeData(NULL,timeAvgSeconds,2);
          writeData(NULL,timeAvgMiliseconds,2);
          writeData(NULL,cpuPercent,2);
          writeData(NULL,double(totalMemory),2);
		//printf("t_execute: %f mls: %f CPU: %f\n",timeAvgSeconds,timeAvgMiliseconds,cpuPercent);

/*	if(difMemory==0){
		printf("\nMemoria Resultante  %ld B",peakDifMemory);
	}else{
		printf("\nMemoria Resultante  %ld B", difMemory);
	}*/


	}

	void startClock(){
		this->time=clock();
	}

	void calculateExecuteTime(){
		this->timeAvgSeconds=((double)clock()-(double)this->time)/(double) CLOCKS_PER_SEC;
		this->timeAvgMiliseconds=((double)clock()-(double)this->time)/(double) (CLOCKS_PER_SEC/1000);
	}

	void startCPUcounter(){
		struct tms timeSample;
		this->lastCPU = times(&timeSample);
		this->lastSysCPU = timeSample.tms_stime;
		this->lastUserCPU = timeSample.tms_utime;
	}

	void calculateCPU(){
		struct tms timeSample;
		clock_t now;
		double percent;

		now = times(&timeSample);
		if (now < lastCPU) {
			//Detección de desbordamiento.
			lastCPU = lastCPU - (numeric_limits<clock_t>::max());
		}
		if (timeSample.tms_stime < lastSysCPU) {
			//Detección de desbordamiento.
			lastSysCPU = lastSysCPU - (numeric_limits<clock_t>::max());
		}
		if (timeSample.tms_utime < lastUserCPU) {
			//Detección de desbordamiento.
			lastUserCPU = lastUserCPU - (numeric_limits<clock_t>::max());
		}

		//El porcentaje de CPU usado es el tiempo de usuario usado mas el tiempo de kernel usado dividido el tiempo total de CPU dividido la cantidad de procesadores
		percent = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime - lastUserCPU);
		percent /= (now - lastCPU);
		percent /= numProcessors;
		percent *= 100;

		lastCPU = now;
		lastSysCPU = timeSample.tms_stime;
		lastUserCPU = timeSample.tms_utime;

		this->cpuPercent = percent;
	}

	void setNumProcessors() {
		FILE* file = fopen("/proc/cpuinfo", "r");
		char line[128];
		numProcessors = 0;
		while (fgets(line, 128, file) != NULL) {
			if (strncmp(line, "processor", 9) == 0)
				numProcessors++;
		}
		fclose(file);
		this->numProcessors = numProcessors;
	}
    
    /*memory method*/


	int parseLine(char* line){
	// This assumes that a digit will be found and the line ends in " Kb".
		int i = strlen(line);
		const char* p = line;
		while (*p <'0' || *p > '9') p++;
		line[i-3] = '\0';
		i = atoi(p);
		return i;
	}

	int getValue(){ //Note: this value is in KB!
		FILE* file = fopen("/proc/self/status", "r");
		int result = -1;
		char line[128];

		while (fgets(line, 128, file) != NULL){
			if (strncmp(line, "VmRSS:", 6) == 0){
			result = parseLine(line);
			break;
			}
		}
		fclose(file);
		return result;
	}

	void startCallMemory(){
		this->startMemory=this->getValue();
	}

	void calculateMemory(){
		this->lastMemory=this->getValue();
		this->totalMemory=this->lastMemory-this->startMemory;
	}
	//~Metrics();
	
};