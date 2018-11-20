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
#include <math.h>          ////////////////////////// REQUIRES SOME STUFF IN COMPILATION COMMAND

#define MAX_STRING_FLOAT_SIZE 20 // including end of string
#define MAX_STRING_INT_SIZE 12 // including end of string
#define MAX_STRING_ENTRY_SIZE 131 // MAYBEE 130 ??????? WITHOUT END OF ENTRY DELIMITER // 19 + 11 (not including end of string) + 15 + 25 + 4 (delimiters) + 1 (end of string)
#define MAX_STRING_METADATA_SIZE 24 // 11 + 10 + 3

typedef struct Entry {
    long AM;
    char name[20];
    char surname[20];
    char streetName[20];
    int houseNumber;
    char cityName[20];
    char postCode[6];
    float salary;
} Entry;

// void removeSpaces(char* str);

// char stringFromFileToEntry(char* s, Entry* entry);

void stringToEntry(char* s, Entry* entry);

// char entryToString(Entry entry, char* s[73]);

void read_from_pipe(int file);

void write_to_pipe(int file);

void writeEntryToFile(char* entryS, FILE* fileP);

char readEntryFromFile(FILE* fp, unsigned int entryNum, Entry* entry);

void handleFlags(int argc, char** argv, unsigned int* height, char** dataFileName, char** pattern, char* skewFlag);

void readAndSendResultsOfChild(char* childPipeName, int parentPipeDesc, pid_t childPid);

#endif
