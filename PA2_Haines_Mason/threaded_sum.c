//Mason Haines CS446 Operating Systems 
#include <stdio.h>
#include <pthread.h>

typedef struct _thread_data_t { 
    
    const int *data; //pointer to array of data read from file (ALL) 
    int startInd; //starting index of thread’s slice     
    int endInd; //ending index of thread’s slice 
    long long int *totalSum; //pointer to the total sum variable in main      
    pthread_mutex_t *lock; //critical region lock   
    
} thread_data_t; 


int main() {
    printf("hello world!");
    return 0;

}