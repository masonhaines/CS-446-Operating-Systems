//Mason Haines - Simple Shell - CS446 - 02/2024
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

//prototypes
void parseInput(char* cmdLineInput, char* argumentList[], int* numOfArg);// CLI = command line input 
void changeDirectories(char* newPath);
int executeCommand(char* argument[]);

int main(){

    char CWD[1024]; // Array of char-> C string with static 1024 for buffer 
    char cmdLineInput[1024]; // Array of char-> C string with static 1024 for buffer 
    char* argumentList[50]; // List of strings 
    int numOfArg = 0; // Counter for number of arguments


    while(1) {
        // Print current working directory 
        if (getcwd(CWD, sizeof(CWD)) != NULL) {
            printf("masonhaines:%s$ ", CWD);
        } else {
            perror("cannot find cwd");
            return 1;
        }

        // Get Command Line Input - CLI
        if (fgets(cmdLineInput, sizeof(cmdLineInput), stdin) == NULL) {
            break;
        }
        int length = strlen(cmdLineInput);
        cmdLineInput[length - 1] = '\0'; // Remove the newline character if present
        
        // Turn command line input into tokens
        parseInput(cmdLineInput, argumentList, &numOfArg);
        // If CLI is exit leave while loop and end program exec
        if (strcmp(cmdLineInput, "exit") == 0) {
            break;
        } // If CLI is cd use change directories function to traverse paths
        else if (strcmp(argumentList[0], "cd") == 0) {
            if (argumentList[1] != NULL) { // Check to see if there is a valid path argument after the first command 
                changeDirectories(argumentList[numOfArg- 1]);
            } else {
                printf("Path Not Formatted Correctly!");
            }
        } else {
            int errorStat = executeCommand(argumentList); // Set return value of command execution to error status variable 

            if (errorStat != 0 ) { // Use error status to determine if print status to terminal
                printf("error status %d\n", errorStat);
            }
        }
    } 

    return 0;
};

// Function used to change directories 
void changeDirectories(char* newPath) {
    //if chdir function returns -1 throw error
    if (chdir(newPath) == -1) {
        perror("Failed to change directories");
    }
    
}

// Function used to parse CLI into tokens and return back into argument list for commands and argument execution
void parseInput(char* cmdLineInput, char* argumentList[], int* numOfArg) {
    *numOfArg = 0; // Reset the argument counter
    char* cliToken = strtok(cmdLineInput, " ");// Parse cmd input into tokens

    // Move to the next token if available, will be arguments
    while (cliToken != NULL && *numOfArg < 49) 
    { // Copy tokens into arguments
        argumentList[*numOfArg] = cliToken;
        cliToken = strtok(NULL, " ");
        (*numOfArg)++;
    }

    int length = strlen(argumentList[*numOfArg -1]); // Get length of argument list at current MAX - 1
    argumentList[*numOfArg -1][length] = '\0'; // Remove newline character
    argumentList[*numOfArg] = NULL; // Last element is NULL for execvp
}

// Function used to execute fork and monitor status of child while parent waits. Returns error status 
int executeCommand(char* argument[]) {
    int childPid = fork(); // Create a child process
    int waitStat; // Variable to store the status of the child process

    if (childPid < 0) {// Fork Failure, print an error message indicating fork failure
        fprintf(stderr, "Fork failed: %s\n", strerror(errno));
        exit(1); // Exit the program with an error code
    } else if (childPid == 0) {
        
        if (execvp(argument[0], argument) == -1) { // Execute the command
            fprintf(stderr, "Command not found: %s\n", strerror(errno));
            _exit(1); // Failure
        }else {
            _exit(0); // Success
        }
    }else { // Parent process

        if(wait(&waitStat) == -1){
            fprintf(stderr, "Error waiting: %s\n", strerror(errno));
            return 2;
        } 
        //for testing purposes
        // else if (WIFEXITED(waitStat)) { // Check and print status of child 
        //     if (waitStat != 0) {
        //         printf("Error status: %d\n", WEXITSTATUS(waitStat));
        //     }
        // } 
        // else {
        //     psignal(WTERMSIG(waitStat), "Exit signal"); // Exit signal indicating abnormal termination - geeks for geeks 
        // }
    }

    return 0;
}