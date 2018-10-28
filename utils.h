#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct Entry {
    int AM;
    char name[15];
    char surname[25];
    float salary;
} Entry;

// void removeSpaces(char* str);

char stringFromFileToEntry(char* s, Entry* entry);

void stringToEntry(char* s, Entry* entry);

char entryToString(Entry entry, char s[73]);

void read_from_pipe(int file);

void write_to_pipe(int file);

char readEntryFromFile(FILE* fp, unsigned int entryNum, Entry* entry);

void handleFlags(int argc, char** argv, unsigned int* height, char** dataFileName, char** pattern, char* skewFlag);

#endif
