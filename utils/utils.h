#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <math.h>
#include <signal.h>

#define MAX_STRING_FLOAT_SIZE 20 // including end of string
#define MAX_STRING_INT_SIZE 12 // including end of string
#define MAX_STRING_RECORD_SIZE 131 // 19 + 11 (not including end of string) + 15 + 25 + 4 (delimiters) + 1 (end of string)
#define MAX_STRING_METADATA_SIZE 24 // 11 + 10 + 3

typedef struct Record {
    long AM;
    char name[20];
    char surname[20];
    char streetName[20];
    int houseNumber;
    char cityName[20];
    char postCode[6];
    float salary;
} Record;

// converts a string to Record
void stringToRecord(char* s, Record* entry);

// writes a record to ASCII file
void writeRecordToFile(char* entryS, FILE* fileP);

// reads a record that is in the entryNum-th place in a binary file
char readRecordFromFile(FILE* fp, unsigned int entryNum, Record* entry);

// handles the flags that are passed as arguments in the myfind executable
void handleFlags(int argc, char** argv, unsigned int* height, char** dataFileName, char** pattern, char* skewFlag);

// this function is used only by Splitters/Mergers for them to read data from the childPipeName named pipe and write them to their parent's pipe
// which has parentPipeDesc as a descriptor
void readAndSendResultsOfChild(char* childPipeName, int parentPipeDesc, pid_t childPid);

#endif
