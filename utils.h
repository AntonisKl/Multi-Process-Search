#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    int AM;
    char name[15];
    char surname[25];
    float salary;
} Entry;

// void removeSpaces(char* str);

char stringToEntry(char* s, Entry* entry);

char readEntryFromFile(FILE* fp, unsigned int entryNum, Entry* entry);

void handleFlags(int argc, char** argv, unsigned int* height, char** dataFileName, char** pattern, char* skewFlag);

#endif
