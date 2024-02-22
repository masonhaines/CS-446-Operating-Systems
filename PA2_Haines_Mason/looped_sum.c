#include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <ctype.h>
// #include <time.h>


int readFile(char file[], int parsedValues[]);
int sumArray(int[], int);


int main(int argc, char* argv[]) {
    
    int sumArray[1000];

    /* FOR TESTING 
    int count = readFile("ten.txt", sumArray);
    printf("Values read from file: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", sumArray[i]);
    }
    printf("\n");
    printf("total count of numbers in file: %d\n", count); 
    */

    return 0;
}


// Function reads integers from a file and stores them in an array and returns the count of values read.
int readFile(char txtFile[], int parsedValues[]) {
    FILE *in_file = fopen(txtFile, "r"); // Open the file for reading ONLY using r flag 
    int counter = 0;

    // Check if file exists
    if (in_file == NULL) {
        printf("File not found...\n");
        return -1; // indicate failure to read file to main
    }
    
    // Read values from the file using fscanf
    while (fscanf(in_file, "%d", &parsedValues[counter]) == 1) {
        counter++;
    }
    fclose(in_file); // Close the file stream

    return counter; // Return the number of values parsed
}