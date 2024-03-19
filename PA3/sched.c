typedef struct _thread_data_t { 
 int localTid; 
 const int *data; 
 int numVals; 
 pthread_mutex_t *lock; 
 long long int *totalSum; 
} thread_data_t; 