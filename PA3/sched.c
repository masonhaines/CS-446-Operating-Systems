//Mason Haines CS446 Operating Systems PA 3
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/mman.h>

#define ANSI_COLOR_GRAY    "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"

#define ANSI_COLOR_RESET   "\x1b[0m"

#define TERM_CLEAR() printf("\033[H\033[J")
#define TERM_GOTOXY(x,y) printf("\033[%d;%dH", (y), (x))

typedef struct thread_data_t { 

    int localTid; 
    const int *data; 
    int numVals; 
    pthread_mutex_t *lock; 
    long long int *totalSum; 

} thread_data_t; 

void* arraySum(void *arg); // array for busy work 
void print_progress(pid_t localTid, size_t value);

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

    pthread_mutex_t lock; // local mutex lock object for locked initialization
    pthread_mutex_t *lockptr = NULL; // pointer mutex lock object for unlocked set to NULL
    
    pthread_mutex_init(&lock, NULL); // Initialize lock
    lockptr = &lock; // Set lock pointer to memory address of lock 

    thread_data_t threadDataArray[numOfThreads];

    // Initialize struct variables for threaded array
    for (int i = 0; i < numOfThreads; i++) {
        threadDataArray[i].localTid = i;
        threadDataArray[i].data = numArray;
        threadDataArray[i].numVals = 2000000;
        threadDataArray[i].lock = lockptr;
        threadDataArray[i].totalSum = &totalSum;
    }

    pthread_t threadsArray[numOfThreads]; // create array to hold thread identifiers

    for (int i = 0; i < numOfThreads; i++) {
        // Create thread
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
    // Wait for each thread to finish execution
    for (int i = 0; i < numOfThreads; i++) {
        pthread_join(threadsArray[i], NULL);
    }
    
    return 0;
}

void* arraySum(void *arg) {
    thread_data_t *threadDataArray = (thread_data_t*)arg; // Create new thread data object to set equal to arg
    long long int threadSum = 0;

    while(1) {
        // Loop through for thread sum
        double maxLatency; // Variable for newest and highest latency 

        for (int i = 0; i < threadDataArray->numVals ; i++) {
            struct timespec startTime, endTime; // time spec objects to record timed latency 
            long int duration; // create time duration variable, for nano seconds, is variable to hold latency 

            clock_gettime(CLOCK_MONOTONIC, &startTime);
            threadSum += threadDataArray->data[i];
            clock_gettime(CLOCK_MONOTONIC, &endTime);
            
            // Calcualte time in nanoseconds
            duration = (endTime.tv_sec - startTime.tv_sec) * 1000000000
                                        +
                       (endTime.tv_nsec - startTime.tv_nsec);

            if (duration > maxLatency) maxLatency = duration; // Update maximum observed latency 
        }
        
        pthread_mutex_lock(threadDataArray->lock);
        *threadDataArray->totalSum += threadSum; // Dereference pointer to threaded data array total Sum
        pthread_mutex_unlock(threadDataArray->lock);

        print_progress(threadDataArray->localTid, maxLatency);
    }
    
    return NULL;
};

void print_progress(pid_t localTid, size_t value) {
        pid_t tid = syscall(__NR_gettid);

        TERM_GOTOXY(0,localTid+1);

	char prefix[256];
        size_t bound = 100;
        sprintf(prefix, "%d: %ld (ns) \t[", tid, value);
	const char suffix[] = "]";
	const size_t prefix_length = strlen(prefix);
	const size_t suffix_length = sizeof(suffix) - 1;
	char *buffer = calloc(bound + prefix_length + suffix_length + 1, 1);
	size_t i = 0;

	strcpy(buffer, prefix);
	for (; i < bound; ++i)
	{
	    buffer[prefix_length + i] = i < value/10000 ? '#' : ' ';
	}
	strcpy(&buffer[prefix_length + i], suffix);
        
        if (!(localTid % 7)) 
            printf(ANSI_COLOR_WHITE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 6)) 
            printf(ANSI_COLOR_BLUE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 5)) 
            printf(ANSI_COLOR_RED "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 4)) 
            printf(ANSI_COLOR_GREEN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 3)) 
            printf(ANSI_COLOR_CYAN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 2)) 
            printf(ANSI_COLOR_YELLOW "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 1)) 
            printf(ANSI_COLOR_MAGENTA "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else
            printf("\b%c[2K\r%s\n", 27, buffer);

	fflush(stdout);
	free(buffer);
}

