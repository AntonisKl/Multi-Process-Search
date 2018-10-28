#include "nodes.h"

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, int pipeFileDescriptors[2]) {
    FILE* fp = fopen(dataFileName, "rb");

    unsigned int entriesRemaining = searchRangeEnd - searchRangeStart + 1;
    unsigned int curEntryNum = searchRangeStart;
    Entry entry;
    char entriesFoundS[entriesRemaining * sizeof(Entry) + 1];
    char entryS[73];

    while (entriesRemaining > 0) {
        if (readEntryFromFile(fp, curEntryNum, &entry) == 1) {
            printf("Error while reading entry from file\nExiting...\n");
            exit(1);
        }

        char patternFound = 0;
        char intS[12];
        char floatS[20];
        sprintf(intS, "%d", entry.AM);
        sprintf(floatS, "%f", entry.salary);

        if (strstr(intS, searchPattern) != NULL || strstr(floatS, searchPattern) != NULL ||
            strstr(entry.name, searchPattern) != NULL || strstr(entry.surname, searchPattern) != NULL) {
            entryToString(entry, entryS);  // with end delimiter
            strcat(entriesFoundS, entryS);
        }
    }
    // add more things like the time that it took to search the entries, maybe the pid of current process and more

    close(pipeFileDescriptors[0]);  // Close reading end of pipe
    // Write input string and close writing end of first
    // pipe.
    write(pipeFileDescriptors[1], entriesFoundS, strlen(entriesFoundS) + 1);
    close(pipeFileDescriptors[1]);

    fclose(fp);
    return;
}
