//Mason Haines CS446 Operating Systems PA 3
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

// typedef struct _thread_data_t { 
    
//     const int *data; //pointer to array of data read from file (ALL) 
//     int startInd; //starting index of thread’s slice     
//     int endInd; //ending index of thread’s slice 
//     long long int *totalSum; //pointer to the total sum variable in main      
//     pthread_mutex_t *lock; //critical region lock   
    
// } thread_data_t; 

typedef struct thread_data_t { 

    int localTid; 
    const int *data; 
    int numVals; 
    pthread_mutex_t *lock; 
    long long int *totalSum; 

} thread_data_t; 


// Function prototypes
// int readFile(char file[], int parsedValues[]);
// void* sumArray(void *arg);
void* arraySum(void *arg);

int main(int argc, char* argv[]) {

    // If there are not enough parameters return to user error message and exit program
    if (argc != 2) {
        printf("Not enough parameters.\n");
        return -1;
    }
    
    
    int* numArray = (int *)malloc(2000000 * sizeof(int)); // Dynamically allocate a fixed-size array of 2,000,000 ints.
    int numOfThreads = atoi(argv[1]); // Convert number of threads argc to integer
    long long int totalSum = 0; // Initialize total sum variable 
    struct timeval startTime, endTime, totalTimeStart, totalTimeEnd;
    
    // Parse file and load into array and return size of array
    // int numOfValues = readFile(argv[2], numArray);
    // End program with return 1 if file is not found 
    // if (numOfValues == -1) return 1; 
    // else if (numOfValues < numOfThreads) {
    //     printf("Too many threads requested.\n");
    //     return -1 ;
    // }

    // Start timer for recording threaded summation 
    // gettimeofday(&totalTimeStart, NULL); // Start clock for entire thread summation time 

    pthread_mutex_t lock; // local mutex lock object for locked initialization
    pthread_mutex_t *lockptr = NULL; // pointer mutex lock object for unlocked set to NULL
    // int lockUnlock = atoi(argv[3]);
    // If user argc is 1 initialize and set local lock to lock pointer 
    // if (lockUnlock == 1 ) { 
    pthread_mutex_init(&lock, NULL);
    lockptr = &lock; 
    // }

    thread_data_t threadDataArray[numOfThreads];


    // int slice = numOfValues / numOfThreads; // The grouping of values per thread, the "slice"
    // int remainder = numOfValues % numOfThreads;

    // Initialize struct variables for threaded data array
    for (int i = 0; i < numOfThreads; i++) {
        threadDataArray[i].localTid = i;
        threadDataArray[i].data = numArray;
        threadDataArray[i].numVals = 2000000;
        threadDataArray[i].lock = lockptr;
        threadDataArray[i].totalSum = &totalSum;
    }

    // Initialize struct variables for threaded data array
    // for (int i = 0; i < numOfThreads; i++) {
    //     threadDataArray[i].data = numArray;
    //     threadDataArray[i].startInd = i * slice;
    //     if (0 < remainder) {
    //         threadDataArray[i].endInd = (i + 1) * slice;
    //     } else {
    //         threadDataArray[i].endInd = (i + 1) * slice - 1;
    //     }
    //     threadDataArray[i].lock = lockptr;
    //     threadDataArray[i].totalSum = &totalSum;
    // }


    pthread_t threadsArray[numOfThreads];

    for (int i = 0; i < numOfThreads; i++) {
        int creationStatus = pthread_create(
            &threadsArray[i], 
            NULL, 
            arraySum, 
            (void*)&threadDataArray[i]); // Typecast the input pointer 
        // Edge case if thread creation fails
        if (creationStatus) {
            printf("Failed to create thread at: %d\n", i);
            return -1; 
        }
    }

    for (int i = 0; i < numOfThreads; i++) {
        pthread_join(threadsArray[i], NULL);
    }
    
    // if (lockUnlock == 1) {
    // pthread_mutex_destroy(&lock);
    // }
    
    // End timer for recording threaded summation 
    // gettimeofday(&totalTimeEnd, NULL);

    // Calculate the time taken in milliseconds
    // long totalSeconds = totalTimeEnd.tv_sec - totalTimeStart.tv_sec;
    // long totalMicros = totalTimeEnd.tv_usec - totalTimeStart.tv_usec;
    // double totalMS = (totalSeconds * 1000000) + (double) totalMicros / 1000;

    printf("Final sum: %lld\n", totalSum);
    // printf("Total Time taken (milliseconds): %.6f\n", totalMS); // Print Total time taken to calculate sum of array
    return 0;
}

void* arraySum(void *arg) {
    thread_data_t *threadDataArray = (thread_data_t*)arg; // Create new thread data object to set equal to arg
    long long int threadSum = 0;

    while(1) {
        // Loop through for thread sum
        double maxLatency;
        for (int i = 0; i < threadDataArray->numVals ; i++) {
            struct timespec startTime, endTime; // time spec objects tot record time 
            long int duration; // create time duration variable, for nano seconds 

            clock_gettime(CLOCK_MONOTONIC, &startTime);
            threadSum += threadDataArray->data[i];
            clock_gettime(CLOCK_MONOTONIC, &endTime);
            
            // Calcualte time in nnanoseconds
            duration = (endTime.tv_sec - startTime.tv_sec) * 1000000000
                                        +
                       (endTime.tv_nsec - startTime.tv_nsec);

            if (duration > maxLatency) maxLatency = duration;
            
        }
        
        pthread_mutex_lock(threadDataArray->lock);
        *threadDataArray->totalSum += threadSum; // Dereference pointer to threaded data array total Sum
        pthread_mutex_unlock(threadDataArray->lock);
    }
    
    



    return NULL;
};

// // Function to find the total sum of all numbers read from file and stored in an array
// void* sumArray(void *arg) {
//     thread_data_t *threadDataArray = (thread_data_t*)arg; // Create new thread data object to set equal to arg
//     long long int threadSum = 0;

    // // Loop through for thread sum
    // for (int i = threadDataArray->startInd; i <= threadDataArray->endInd ; i++) {
    //     threadSum += threadDataArray->data[i];
    // }

    // if (threadDataArray->lock == NULL) {
    //     *threadDataArray->totalSum += threadSum;
    // } 
    // else {
    //     pthread_mutex_lock(threadDataArray->lock);
    //     *threadDataArray->totalSum += threadSum; // Dereference pointer to threaded data array total Sum
    //     pthread_mutex_unlock(threadDataArray->lock);
    // }

//     return NULL;
// }

// // Function reads integers from a file and stores them in an array and returns the count of values read.
// int readFile(char txtFile[], int parsedValues[]) {
//     FILE *in_file = fopen(txtFile, "r"); // Open the file for reading ONLY using r flag 
//     int numOfValues = 0; // Number of values counted in the file 

//     // Check if file exists
//     if (in_file == NULL) {
//         printf("File not found...\n");
//         return -1; // indicate failure to read file to main
//     }
//     // Read values from the file using fscanf
//     while (fscanf(in_file, "%d", &parsedValues[numOfValues]) == 1) {
//         numOfValues++;
//     }
//     fclose(in_file); // Close the file stream

//     return numOfValues; // Return the number of values parsed
// }

