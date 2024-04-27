#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include "sys/types.h"
#include "sys/sysinfo.h"

#define ARRAY_SIZE 500000
#define STRING_SIZE 2001

int NUM_THREADS = 4;
float line_avg[ARRAY_SIZE];
char lines[ARRAY_SIZE][STRING_SIZE];
FILE *fd;

typedef struct{
    uint32_t virtualMem;
    uint32_t physicalMem;
} processMem_t;

int parseLine(char *line) {
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i - 3] = '\0';
    return atoi(line);
}

void GetProcessMemory(processMem_t* processMem) {
    FILE *file = fopen("/proc/self/status", "r");
    char line[128];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            processMem->virtualMem = parseLine(line);
        }
        if (strncmp(line, "VmRSS:", 6) == 0) {
            processMem->physicalMem = parseLine(line);
        }
    }
    fclose(file);
}

void readFile() {
    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if (fd == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (fscanf(fd, "%[^\n]\n", lines[i]) == EOF) break;
    }
    fclose(fd);
}

float find_avg(char* line, int nchars) {
    float sum = 0;
    for (int i = 0; i < nchars; i++) {
        sum += line[i];
    }
    return nchars > 0 ? sum / nchars : 0.0;
}

void process_lines() {
    #pragma omp parallel for
    for (int i = 0; i < ARRAY_SIZE; i++) {
        line_avg[i] = find_avg(lines[i], strlen(lines[i]));
    }
}

void print_results() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d: %.1f\n", i, line_avg[i]);
    }
}

int main() {
    NUM_THREADS = atoi(getenv("SLURM_NTASKS") ? getenv("SLURM_NTASKS") : "4");
    omp_set_num_threads(NUM_THREADS);
    struct timeval t1, t2;
    double timeElapsedTotal;
    processMem_t memory;
    
    gettimeofday(&t1, NULL);
    readFile();
    process_lines();
    GetProcessMemory(&memory);
    print_results();
    gettimeofday(&t2, NULL);
    
    timeElapsedTotal = (t2.tv_sec - t1.tv_sec) * 1000.0;  // Convert to milliseconds
    timeElapsedTotal += (t2.tv_usec - t1.tv_usec) / 1000.0;
    
    printf("Tasks: %d\nTotal Time: %.2fms\n", NUM_THREADS, timeElapsedTotal);
    printf("size = %d, Node: %s, vMem %u KB, pMem %u KB\n", NUM_THREADS, getenv("HOSTNAME"), memory.virtualMem, memory.physicalMem);
    printf("Main: program completed. Exiting.\n");

    return 0;
}
