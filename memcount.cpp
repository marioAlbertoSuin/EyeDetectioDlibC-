/*
 * memcount.cpp
 *
 *  Created on: 20 jun. 2019
 *      Author: desarrollo
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define SIZE          8192  /* buffer size for reading /proc/<pid>/status */

int mem_total ()
{
  char *p, *q;
  int data, stack;
  int n, v, fd;
  pid_t pid= getpid();

  p = (char*)malloc(1);
  sprintf (p, "/proc/%d/status", pid);
  fd = open (p, O_RDONLY);
  if (fd < 0)
    return -1;
  do
    n = read (fd, p, SIZE);
  while ((n < 0) && (errno == EINTR));
  if (n < 0)
    return -2;
  do
    v = close (fd);
  while ((v < 0) && (errno == EINTR));
  if (v < 0)
    return -3;
  data = stack = 0;
  q = strstr (p, "VmData:");
  if (q != NULL)
    {
      sscanf (q, "%*s %d", &data);
      q = strstr (q, "VmStk:");
      if (q != NULL)
    sscanf (q, "%*s %d\n", &stack);
    }
  free(p);
  return (data + stack);
}


int mem_total (int &data, int &stack)
{
  char *p, *q;
  int n, v, fd;
  pid_t pid= getpid();

  p = (char*)malloc(1);
  sprintf (p, "/proc/%d/status", pid);
  fd = open (p, O_RDONLY);
  if (fd < 0)
    return -1;
  do
    n = read (fd, p, SIZE);
  while ((n < 0) && (errno == EINTR));
  if (n < 0)
    return -2;
  do
    v = close (fd);
  while ((v < 0) && (errno == EINTR));
  if (v < 0)
    return -3;
  data = stack = 0;
  q = strstr (p, "VmData:");
  if (q != NULL)
    {
      sscanf (q, "%*s %d", &data);
      q = strstr (q, "VmStk:");
      if (q != NULL)
    sscanf (q, "%*s %d\n", &stack);
    }
  free(p);
  return (data + stack);
}



int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}


int getRamUsage(){ //Note: this value is in KB!
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
