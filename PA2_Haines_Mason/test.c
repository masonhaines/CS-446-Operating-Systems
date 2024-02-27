#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct _thread_data_t {
    const int *data; //pointer to array of data read from file (ALL)
    int startInd; //starting index of thread’s slice
    int endInd; //ending index of thread’s slice
    long long int *totalSum; //pointer to the total sum variable in main
    pthread_mutex_t *lock; //critical region lock
} thread_data_t;

// Function prototypes
int readFile(char file[], int parsedValues[]);
void* sumArray(void *arg);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Not enough parameters.\n");
        return -1;
    }
    
    int numArray[1000000];
    int numOfThreads = atoi(argv[1]);
    long long int totalSum = 0;
    struct timeval startTime, endTime, totalTimeStart, totalTimeEnd;
    
    // Parse file and load into array and return size of array
    int numOfValues = readFile(argv[2], numArray);
    // End program with return 1 if file is not found 
    if (numOfValues == -1) return 1; 
    else if (numOfValues < numOfThreads) {
        printf("Too many threads requested.\n");
        return -1;
    }

    gettimeofday(&totalTimeStart, NULL); // Start clock for entire thread summation time 

    pthread_mutex_t lock;
    if (atoi(argv[3]) != 0) {
        pthread_mutex_init(&lock, NULL);
    } 

    thread_data_t threadDataArray[numOfThreads];
    int slice = numOfValues / numOfThreads; // The grouping of values per thread, the "slice"
    int remainder = numOfValues % numOfThreads;

    // Initialize struct variables for threaded data array
    for (int i = 0; i < numOfThreads; i++) {
        threadDataArray[i].data = numArray;
        threadDataArray[i].startInd = i * slice;
        if (i < remainder) {
            threadDataArray[i].endInd = (i + 1) * slice;
        } else {
            threadDataArray[i].endInd = (i + 1) * slice - 1;
        }
        threadDataArray[i].lock = &lock;
        threadDataArray[i].totalSum = &totalSum;

        printf("Slice size for thread %d: %d\n", i, threadDataArray[i].endInd - threadDataArray[i].startInd + 1);
    }

    pthread_t threadsArray[numOfThreads];

    gettimeofday(&startTime, NULL); // Start clock for pthread summation time

    for (int i = 0; i < numOfThreads; i++) {
        int creationStatus = pthread_create(&threadsArray[i], 
        NULL, 
        sumArray, 
        (void*)&threadDataArray[i]);
        // Edge case if thread creation fails
        if (creationStatus) {
            fprintf(stderr, "Failed to create thread at: %d\n", i);
            return -1;
        }
    }

    for (int i = 0; i < numOfThreads; i++) {
        pthread_join(threadsArray[i], NULL);
    }

    gettimeofday(&endTime, NULL); // End clock for summation time 
    gettimeofday(&totalTimeEnd, NULL);

    // Calculate the time taken in milliseconds
    long seconds = endTime.tv_sec - startTime.tv_sec;
    long micros = endTime.tv_usec - startTime.tv_usec;
    double milliseconds = (seconds * 1000000) + (double) micros / 1000;

    long totalSeconds = totalTimeEnd.tv_sec - totalTimeStart.tv_sec;
    long totalMicros = totalTimeEnd.tv_usec - totalTimeStart.tv_usec;
    double totalMS = (totalSeconds * 1000000) + (double) totalMicros / 1000;

    printf("Final sum: %lld\n", totalSum);
    printf("Time taken (ms): %.6f\n", milliseconds); // Print time taken to calculate pthread array summation 
    printf("Total Time taken (ms): %.6f\n", totalMS); // Print Total time taken to calculate sum of array
    return 0;
}

// Function reads integers from a file and stores them in an array and returns the count of values read.
int readFile(char txtFile[], int parsedValues[]) {
    FILE *in_file = fopen(txtFile, "r"); // Open the file for reading ONLY using r flag 
    int numOfValues = 0; // Number of values counted in the file 

    // Check if file exists
    if (in_file == NULL) {
        printf("File not found...\n");
        return -1; // indicate failure to read file to main
    }
    // Read values from the file using fscanf
    while (fscanf(in_file, "%d", &parsedValues[numOfValues]) == 1) {
        numOfValues++;
    }
    fclose(in_file); // Close the file stream

    return numOfValues; // Return the number of values parsed
}

// Function to find the total sum of all numbers read from file and stored in an array
void* sumArray(void *arg) {
    thread_data_t *threadDataArray = arg;
    long long int threadSum = 0;

    for (int i = threadDataArray->startInd; i <= threadDataArray->endInd ; i++) {
        threadSum += threadDataArray->data[i];
        printf("thread sum %d: %lld\n", i, threadSum);
    }
    
    printf("i made it inside the sum Array function\n");

    if (threadDataArray->lock == NULL) {
        *threadDataArray->totalSum += threadSum;
    } else {
        pthread_mutex_lock(threadDataArray->lock);
        *threadDataArray->totalSum += threadSum; // Dereference pointer to threaded data array total Sum
        pthread_mutex_unlock(threadDataArray->lock);
    }

    return NULL;
}
