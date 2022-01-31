/*
 * system-metrics.cpp
 *
 *  Created on: 10 ene. 2020
 *      Author:
 */

#include "system_metrics.h"

size_t systemMetrics::memoryUsage = 0;
map<void *, size_t> systemMetrics::pointersSize;
list<systemMetrics *> systemMetrics::registeredSm;
int systemMetrics::cantMallocs = 0;
int systemMetrics::cantFree = 0;

void *systemMetrics::registerMalloc(void *pointer, size_t size) {
	systemMetrics::pointersSize.insert(make_pair(pointer, size));
	systemMetrics::memoryUsage += size;
	systemMetrics::cantMallocs++;
	list <systemMetrics *> :: iterator it;
	for(it = systemMetrics::registeredSm.begin(); it != systemMetrics::registeredSm.end(); ++it){
		(*it)->checkMemoryPeak();
	}
	return pointer;
}

void systemMetrics::registerFree(void *pointer) {
	systemMetrics::memoryUsage -= systemMetrics::pointersSize[pointer];
	systemMetrics::pointersSize.erase(pointer);
	systemMetrics::cantFree++;
}

void systemMetrics::resetMallocCounts(){
	systemMetrics::cantMallocs = 0;
	systemMetrics::cantFree = 0;
}

void systemMetrics::registerSystemMetric(systemMetrics *sm){
	systemMetrics::registeredSm.push_back(sm);
}

void systemMetrics::unregisterSystemMetric(systemMetrics *sm){
	systemMetrics::registeredSm.remove(sm);
}

systemMetrics::systemMetrics(string metricName) {
	this->metricName = metricName;
	this->setNumProcessors();
	this->resetCounters();
	systemMetrics::registerSystemMetric(this);
}

systemMetrics::~systemMetrics() {
	systemMetrics::unregisterSystemMetric(this);
}

/**
 * Metodo que pone los "contadores" en cero para medir el uso de CPU, memoria y tiempo a partir del llamado a este metodo
 */
void systemMetrics::resetCounters() {
	this->resetCPUCounters();
	this->resetMemoryCounters();
	this->resetTimeCounters();
}

/**
 * Metodo que calcula las metricas entre el llamado a reset y el llamado a este evento y las deja seteadas en los atributos
 */
void systemMetrics::calculate() {
	printf("calculando uso\n");
	this->calculateCPU();
	this->calculateMemory();
	this->calculateTime();
}

/*
 * Metodo que setea el contador de ticks en el valor actual para luego poder calcular la diferencia
 */
void systemMetrics::resetTimeCounters() {
	this->lastTime = clock();
}

/**
 * Metodo que saca la diferencia de ticks entre el ultimo reset y ahora y luego calcula los segundos
 */
void systemMetrics::calculateTime() {
	this->durationInSeconds = ((double) clock() - (double) lastTime) / (double) CLOCKS_PER_SEC;
	this->durationInMiliseconds = ((double) clock() - (double) lastTime) / (double) (CLOCKS_PER_SEC / 1000);
}

/**
 * Metodo que setea el contador con la cantidad de memoria utilizada para luego poder calcular la diferencia
 */
void systemMetrics::resetMemoryCounters() {
	this->lastMemory = systemMetrics::memoryUsage;
	this->lastPeakMemory = 0;
}

/**
 * Metodo que calcula la diferencia entre el uso de memoria desde el ultimo llamado al reset y "ahora"
 */
void systemMetrics::calculateMemory() {
	this->difMemory = systemMetrics::memoryUsage - this->lastMemory;
	this->peakDifMemory = this->lastPeakMemory;
}

/**
 * Metodo que es llamado como listener cada vez que hay un malloc y debe calcular el pico de memoria utilizado
 */
void systemMetrics::checkMemoryPeak() {
	long difMemory = systemMetrics::memoryUsage - this->lastMemory;
	//printf("\n\ncheck peak --- new dif: %ld", difMemory);
	if(difMemory > this->lastPeakMemory){
		this->lastPeakMemory = difMemory;
	}
}

/**
 * Metodo que obtiene la cantidad de procesadores que tiene la PC en la que esta corriendo el programa y lo deja seteado en el atributo numProcessors
 */
void systemMetrics::setNumProcessors() {
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

/**
 * Metodo que setea los contadores de tick del CPU para luego poder calcular la diferencia
 */
void systemMetrics::resetCPUCounters() {
	struct tms timeSample;
	lastCPU = times(&timeSample);
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;
}

/**
 * Metodo que calcula, segun la diferencia de ticks de CPU, la cantidad de ticks usados por esta aplicacion y el numero de CPUS, el porcentaje de uso del CPU
 */
void systemMetrics::calculateCPU() {
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

/**
 * Metodo que imprime las metricas obtenidas
 */
void systemMetrics::printMetrics() {
	printf("\nTime taken to %s is seg: %f", metricName.c_str(), durationInSeconds);
	printf("\nTime taken to %s is ms: %f", metricName.c_str(), durationInMiliseconds);

	if(difMemory==0){
		printf("\nMemoria Resultante %s: %ld B", metricName.c_str(), peakDifMemory);
	}else{
		printf("\nMemoria Resultante %s: %ld B", metricName.c_str(), difMemory);
	}
	//printf("\nMemoria Pico %s: %ld B", metricName.c_str(), peakDifMemory);
	printf("\nCPU %s: %f %%", metricName.c_str(), cpuPercent);
    printf("\nmallocsGlobal = %d freesGlobal = %d. Dif = %d\n", systemMetrics::cantMallocs, systemMetrics::cantFree,
			systemMetrics::cantMallocs - systemMetrics::cantFree);
}

/**
 * Metodo que devuelve el porcentaje de CPU usado entre el reset y el calculate
 */
double systemMetrics::getCpuPercent() {
	return this->cpuPercent;
}

/**
 * Metodo que devuelve la memoria usada entre el reset y el calculate
 */
long systemMetrics::getDifMemoryKb() {
	return difMemory / 1024;
}

long systemMetrics::getPeakDifMemoryKb() {
	return peakDifMemory / 1024;
}

/**
 * Metodo que devuelve el tiempo que paso entre el reset y el calculate
 */
double systemMetrics::getDurationInSeconds() {
	return durationInSeconds;
}

/**
 * Metodo que devuelve el tiempo que paso entre el reset y el calculate
 */
double systemMetrics::getDurationInMiliseconds() {
	return durationInMiliseconds;
}
