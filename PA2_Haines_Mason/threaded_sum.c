//Mason Haines CS446 Operating Systems 
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
    
    static int numArray[100000000];
    int numOfThreads = atoi(argv[1]);
    long long int totalSum = 0;
    struct timeval startTime, endTime, totalTimeStart, totalTimeEnd;
    
    // Parse file and load into array and return size of array
    printf("before read \n"); // testing 

    int numOfValues = readFile(argv[2], numArray);
    // End program with return 1 if file is not found 
    if (numOfValues == -1) return 1; 
    else if (numOfValues < numOfThreads) {
        printf("Too many threads requested.\n");
        return -1 ;
    }
    printf("before gettime of day  \n"); // testing 
    gettimeofday(&totalTimeStart, NULL); // Start clock for entire thread summation time 

    pthread_mutex_t lock;
    pthread_mutex_t *lockptr = NULL;
    int lockUnlock = atoi(argv[3]);
    if (lockUnlock == 1 ) {
        if(pthread_mutex_init(&lock, NULL) != 0) {
            printf("Failed to Initialize mutex lock.");
            return -1;
        }
        lockptr = &lock;
    }
    
    printf("after init of lcok  \n"); // testing -----------------------

    thread_data_t threadDataArray[numOfThreads];
    int slice = numOfValues / numOfThreads; // The grouping of values per thread, the "slice"
    int remainder = numOfValues % numOfThreads;

    // Initialize struct variables for threaded data array
    for (int i = 0; i < numOfThreads; i++) {
        threadDataArray[i].data = numArray;
        threadDataArray[i].startInd = i * slice;
        if (0 < remainder) {
            threadDataArray[i].endInd = (i + 1) * slice;
        } else {
            threadDataArray[i].endInd = (i + 1) * slice - 1;
        }
        threadDataArray[i].lock = lockptr;
        threadDataArray[i].totalSum = &totalSum;

        // printf("Slice size for thread %d: %d\n", i, threadDataArray[i].endInd - threadDataArray[i].startInd + 1); // testing 
    }
    printf("aftre struct vars init \n"); // testing 

    pthread_t threadsArray[numOfThreads];

    for (int i = 0; i < numOfThreads; i++) {
         printf("pthread create loop \n"); // testing 
        int creationStatus = pthread_create(&threadsArray[i], 
        NULL, 
        sumArray, 
        (void*)&threadDataArray[i]); // Typecast the input pointer 
         printf("post create \n"); // testing 
        // Edge case if thread creation fails
        if (creationStatus) {
            printf("Failed to create thread at: %d\n", i);
            return -1; 
        }
        
    }

    for (int i = 0; i < numOfThreads; i++) {
        pthread_join(threadsArray[i], NULL);
    }
    
    gettimeofday(&totalTimeEnd, NULL);

    // Calculate the time taken in milliseconds
    long totalSeconds = totalTimeEnd.tv_sec - totalTimeStart.tv_sec;
    long totalMicros = totalTimeEnd.tv_usec - totalTimeStart.tv_usec;
    double totalMS = (totalSeconds * 1000000) + (double) totalMicros / 1000;

    printf("Final sum: %lld\n", totalSum);
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
        printf("very begin \n"); // testing 

    thread_data_t *threadDataArray = (thread_data_t*)arg; // Create new thread data object to set equal to arg
    long long int threadSum = 0;
        printf("after vars \n"); // testing 

    // Loop through for thread sum
    for (int i = threadDataArray->startInd; i <= threadDataArray->endInd ; i++) {
        threadSum += threadDataArray->data[i];
        // printf("thread sum %d: %lld\n", i, threadSum); // testing 
    }
    
    // printf("i made it inside the sum Array function\n"); // testing 

    if (threadDataArray->lock == NULL) {
        *threadDataArray->totalSum += threadSum;
        printf("i made it inside the sum Array function and was NOT locked for crit point \n"); // testing 
    } 
    else {
        printf("i made it inside the sum Array function and--WAS--locked for crit point \n"); // testing 

        pthread_mutex_lock(threadDataArray->lock);
        *threadDataArray->totalSum += threadSum; // Dereference pointer to threaded data array total Sum
        pthread_mutex_unlock(threadDataArray->lock);
        printf("i made it inside the sum Array function and--WAS--locked for crit point \n"); // testing 

    }

    return NULL;
}