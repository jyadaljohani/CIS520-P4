#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // linux api for CPU

#define ARRAY_SIZE 10000  
#define STRING_SIZE 2001
#define ALPHABET_SIZE 26

int NUM_THREADS;
int max_char_values[ARRAY_SIZE];  // Array to store max ASCII for each line
float line_avg[ARRAY_SIZE];
char lines[ARRAY_SIZE][STRING_SIZE];
FILE *fd;

void readFile() {
    int err, i;
    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
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

//finding avg within count_array
//float find_avg(char* line, int nchars) {
    // calculate average character value
//    float sum = 0.0;
//    for (int i = 0; i < nchars; i++) {
//        sum += (int)line[i];
//    }
//    return nchars > 0 ? sum / nchars : 0.0;
//}

void *count_array(void *arg) {
    // process parts of the array and calculate average
    int myID = (int)(intptr_t)arg;
    int start = myID * (ARRAY_SIZE / NUM_THREADS);
    int end = (myID + 1) * (ARRAY_SIZE / NUM_THREADS);

    printf("Thread %d starting\n", myID);

    for (int i = start; i < end; i++) {
        int max_val = 0;
        int sum = 0;
        int len = strlen(lines[i]);
        for (int j = 0; j < len; j++) {
            int char_val = (int)lines[i][j];
            sum += char_val;
            if (char_val > max_val) {
                max_val = char_val;
            }
        }
        max_char_values[i] = max_val;
        line_avg[i] = len > 0 ? (float)sum / len : 0.0;
    }

    pthread_exit(NULL);
}


void print_results() {
    // print the results for each line
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (max_char_values[i] != 0) { // This checks if the line was processed
            printf("Line %d: Max ASCII value = %d, Average ASCII value = %.2f\n", i, max_char_values[i], line_avg[i]);
        }
    }
}


int main() {
    // Setup dynamic thread creation
    NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN); // LINUX online processors
    pthread_t threads[NUM_THREADS];
    
    readFile();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, count_array, (void *)(intptr_t)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
	
	
    print_results();

    //Add performance analysis
    printf("Main: May be a Completed successfully. Exiting.\n");
	
    return 0;
}
