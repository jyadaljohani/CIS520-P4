#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include <stdint.h>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <math.h>

#define MAXIMUM_TASKS 32
#define STRING_SIZE 2001
#define ARRAY_SIZE 1000000

int num_tasks;
unsigned int thread_locations[MAXIMUM_TASKS];
float line_averages[ARRAY_SIZE];
float local_average[ARRAY_SIZE];

typedef struct {
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
    if (file == NULL) {
        fprintf(stderr, "Failed to open memory status file.\n");
        return;
    }
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

float find_line_average(char* line, int nchars) {
    float sum = 0;
    for (int i = 0; i < nchars; i++) {
        sum += line[i];
    }
    return nchars > 0 ? sum / nchars : 0.0;
}

void find_avg(int rank, FILE *fp) {
    char tempBuffer[STRING_SIZE];
    int startLine = rank * (ARRAY_SIZE / num_tasks);
    int endLine = (rank + 1) * (ARRAY_SIZE / num_tasks);
    fseek(fp, thread_locations[rank], SEEK_SET);
    
    for (int i = startLine; i < endLine && fscanf(fp, "%[^\n]\n", tempBuffer) != EOF; i++) {
        local_average[i] = find_line_average(tempBuffer, strlen(tempBuffer));
    }
}

void printResults() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d: %.1f\n", i, line_averages[i]);
    }
}

int main(int argc, char *argv[]) {
    struct timeval t1, t2;
    double timeElapsedTotal;
    int rank, rc;
    FILE *fp;
    MPI_Status Status;
    processMem_t myMem;

    rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS) {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        gettimeofday(&t1, NULL);
    }

    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "File not found\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        int currentLine = 0, i = 1;
        thread_locations[0] = ftell(fp);
        while (currentLine < ARRAY_SIZE && fscanf(fp, "%[^\n]\n", tempBuffer) != EOF) {
            if (currentLine == i * (ARRAY_SIZE / num_tasks)) {
                thread_locations[i++] = ftell(fp);
            }
            currentLine++;
        }
    }
    MPI_Bcast(thread_locations, num_tasks + 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    find_avg(rank, fp);
    fclose(fp);

    MPI_Reduce(local_average, line_averages, ARRAY_SIZE, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printResults();
        gettimeofday(&t2, NULL);
        timeElapsedTotal = (t2.tv_sec - t1.tv_sec) * 1000.0; // ms
        timeElapsedTotal += (t2.tv_usec - t1.tv_usec) / 1000.0; // ms
        GetProcessMemory(&myMem);
        printf("Node: %s, vMem %u KB, pMem %u KB, Elapsed Time: %fms\n", getenv("HOSTNAME"), myMem.virtualMem, myMem.physicalMem, timeElapsedTotal);
    }

    MPI_Finalize();
    return 0;
}
