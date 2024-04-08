#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE 1000000  // Example size, need to adjust based on actual data
#define STRING_SIZE 2001
#define ALPHABET_SIZE 26

int NUM_THREADS = 2; // Temporary hard-coded value, need to be dynamically set based on system capabilities
float line_avg[ARRAY_SIZE];
char lines[ARRAY_SIZE][STRING_SIZE];
FILE *fd;

void readFile() {
    int err, i;
    fd = fopen("/path/to/data/wiki_dump.txt", "r");
    if (fd == NULL) {
        perror("Error opening file");
        return;
    }

    // reading the file in chunks to handle large files
    for (i = 0; i < ARRAY_SIZE; i++) {
        err = fscanf(fd, "%[^\n]\n", lines[i]);
        if (err == EOF) break;
    }

    fclose(fd);
}

float find_avg(char* line, int nchars) {
    // calculate average character value
    return 0.0;
}

void *count_array(void *arg) {
    // process parts of the array and calculate average
    int myID = (int)arg;
    printf("Thread %d starting\n", myID);

    // Placeholder for actual processing
    for (int i = 0; i < ARRAY_SIZE / NUM_THREADS; i++) {
        // Some processing should happen here
    }

    pthread_exit(NULL);
}

void print_results() {
    // print the results
    printf("Results printing not implemented yet\n");
}

int main() {
    // Setup dynamic thread creation
    NUM_THREADS = 2; // This should be dynamic
    pthread_t threads[NUM_THREADS];
    
    readFile();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, count_array, (void *)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    print_results();

    //Add performance analysis
    printf("Main: incomplete version of the program. Exiting.\n");

    return 0;
}
