#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define MAX_STRING_FLOAT_SIZE 20 // including end of string
#define MAX_STRING_INT_SIZE 12 // including end of string
#define MAX_STRING_ENTRY_SIZE 75 // MAYBEE 74 ??????? WITHOUT END OF ENTRY DELIMITER // 19 + 11 (not including end of string) + 15 + 25 + 4 (delimiters) + 1 (end of string)
#define MAX_STRING_METADATA_SIZE 24 // 11 + 10 + 3

typedef struct Entry {
    int AM;
    char name[15];
    char surname[25];
    float salary;
} Entry;

// void removeSpaces(char* str);

// char stringFromFileToEntry(char* s, Entry* entry);

void stringToEntry(char* s, Entry* entry);

char entryToString(Entry entry, char s[73]);

void read_from_pipe(int file);

void write_to_pipe(int file);

void writeEntryToFile(char* entryS, FILE* fileP);

char readEntryFromFile(FILE* fp, unsigned int entryNum, Entry* entry);

void handleFlags(int argc, char** argv, unsigned int* height, char** dataFileName, char** pattern, char* skewFlag);

void readAndSendResultsOfChild(int childFileDesc[2], int parentPipeDesc[2]);

#endif
