/*
 * system-metrics.hpp
 *
 *  Created on: 10 ene. 2020
 *      Author: ariel
 */

#ifndef HEADER_SYSTEM_METRICS_HPP_
#define HEADER_SYSTEM_METRICS_HPP_

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/times.h"
#include "sys/vtimes.h"
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <string>
#include "memcount.h"
#include <limits>
#include <malloc.h>
#include <map>
#include <list>

using namespace std;

class systemMetrics {
private:
	static map<void *, size_t> pointersSize;
	static size_t memoryUsage;
	static list<systemMetrics *> registeredSm;

	string metricName;

	//Time variables
	clock_t lastTime;
	double durationInSeconds;//Resultado del calculate
	double durationInMiliseconds;//Resultado del calculate

	//Memory variables
	long lastMemory;
	long difMemory;
	long lastPeakMemory;
	long peakDifMemory;

	//CPU varialbes
	int numProcessors;
	clock_t lastCPU;
	clock_t lastSysCPU;
	clock_t lastUserCPU;
	double cpuPercent;

	void resetTimeCounters();
	void calculateTime();

	void resetMemoryCounters();
	void calculateMemory();
	void checkMemoryPeak();

	void setNumProcessors();
	void resetCPUCounters();
	void calculateCPU();

public:
	static int cantMallocs;
	static int cantFree;

	static void *registerMalloc(void *pointer, size_t size);
	static void registerFree(void *pointer);
	static void resetMallocCounts();
	static void registerSystemMetric(systemMetrics *sm);
	static void unregisterSystemMetric(systemMetrics *sm);

	systemMetrics(string metricName);
	~systemMetrics();
	void resetCounters();
	void calculate();
	void printMetrics();
	double getCpuPercent();
	long getDifMemoryKb();
	long getPeakDifMemoryKb();
	double getDurationInSeconds();
	double getDurationInMiliseconds();
};

#endif /* HEADER_SYSTEM_METRICS_HPP_ */
