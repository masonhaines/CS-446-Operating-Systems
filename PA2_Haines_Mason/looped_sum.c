//Mason Haines CS446 Operating Systems 
#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <sys/time.h>

// Function prototypes
int readFile(char file[], int parsedValues[]);
int sumArray(int parsedValues[], int numOfValues);

int main(int argc, char* argv[]) {
    
    int numArray[1000];
    struct timeval startTime, endTime;
    
    // Parse file and load into array and return size of array
    int numOfvalues = readFile("ten.txt", numArray);
    if (numOfvalues == -1) return 1; // End program with return 1 if file is not found 

    gettimeofday(&startTime, NULL); // Start clock for summation time 

    int SUMofFile = sumArray(numArray, numOfvalues); // Find sum of array from values stored in file 
    printf("Total Sum of Array: %d\n", SUMofFile);

    gettimeofday(&endTime, NULL); // End clock for summation time 

    // Calculate the time taken in milliseconds
    long seconds = endTime.tv_sec - startTime.tv_sec;
    long micros = endTime.tv_usec - startTime.tv_usec;
    double milliseconds = (seconds * 1000) + (double) micros / 1000;

    printf("Time taken (ms): %.6f\n", milliseconds); // Print Total time taken to calculate sum of array

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
int sumArray(int parsedValues[], int numOfValues) {
    int totalSum = 0; // variable to store summation of all numbers stored in a file 

    for (int i = 0; i < numOfValues; i++) {
        totalSum += parsedValues[i];
    }

    return totalSum;
}