

#ifndef HEADER_MEMORY_COUNT
#define HEADER_MEMORY_COUNT

int getRamUsage();

int mem_total (void);
// Devuelve la cantidad total de memoria usada por el programa
// en el instante en el que se realiza la llamada.
// El valor viene dado en Kbytes (1024 bytes).

int mem_total (int &data, int &stack);
// Igual que la funciÃ³n anterior. Y ademÃ¡s almacena en la
// variable data la memoria de datos (memoria estÃ¡tica mÃ¡s
// dinÃ¡mica), y en stack la memoria de pila, ambas en Kbytes.

#endif
