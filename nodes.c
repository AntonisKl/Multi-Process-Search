#include "nodes.h"

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, int pipeFileDescriptors[2]) {
    FILE* fp = fopen(dataFileName, "rb");

    unsigned int entriesRemaining = searchRangeEnd - searchRangeStart + 1;
    unsigned int curEntryNum = searchRangeStart;
    char intS[MAX_STRING_INT_SIZE];
    char floatS[MAX_STRING_FLOAT_SIZE];
    struct timeval startTime, endTime;
    

    Entry entry;
    // char entriesFoundS[entriesRemaining * sizeof(Entry) + 1];
    char entryS[MAX_STRING_ENTRY_SIZE];

    gettimeofday(&startTime, NULL);
    while (entriesRemaining > 0) {
        if (readEntryFromFile(fp, curEntryNum, &entry) == 1) {
            printf("Error while reading entry from file\nExiting...\n");
            exit(1);
        }

        char patternFound = 0;

        sprintf(intS, "%d", entry.AM);
        sprintf(floatS, "%f", entry.salary);

        if (strstr(intS, searchPattern) != NULL || strstr(floatS, searchPattern) != NULL ||
            strstr(entry.name, searchPattern) != NULL || strstr(entry.surname, searchPattern) != NULL) {
            entryToString(entry, entryS);  // with end delimiter
            close(pipeFileDescriptors[0]);
            write(pipeFileDescriptors[1], entryS, strlen(entryS) + 1);
            close(pipeFileDescriptors[1]);

            // strcat(entriesFoundS, entryS);
        }
    }
    gettimeofday(&endTime, NULL);

    close(pipeFileDescriptors[0]);
    write(pipeFileDescriptors[1], "m&", 3);  // end of entries delimiter
    close(pipeFileDescriptors[1]);

    // add more things like the time that it took to search the entries, maybe the pid of current process and more

    char metadata[11 + 10 + 3]; // pid + del + timeElapsed + end of metadata + end of string
    
    sprintf(intS, "%d", getpid());
    strcpy(metadata, intS);
    strcat(metadata, "$");

    char uintS[11];
    sprintf(uintS, "%u", endTime.tv_usec - startTime.tv_usec);
    strcat(metadata, uintS);
    strcat(metadata, "&");
    
    close(pipeFileDescriptors[0]);
    write(pipeFileDescriptors[1], metadata, sizeof(metadata) + 1);
    close(pipeFileDescriptors[1]);

    // when parent gets metadata, he stops reading

    // close(pipeFileDescriptors[0]);
    // write(pipeFileDescriptors[1], '~', 1);  // end delimiter
    // close(pipeFileDescriptors[1]);

    // close(pipeFileDescriptors[0]);
    // write(pipeFileDescriptors[1], entriesFoundS, strlen(entriesFoundS) + 1);
    // close(pipeFileDescriptors[1]);

    fclose(fp);
    return;
}
