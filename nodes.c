#include "nodes.h"

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, int parentPipeDesc[2]) {
    FILE* fp = fopen(dataFileName, "rb");

    unsigned int entriesRemaining = searchRangeEnd - searchRangeStart + 1;
    unsigned int curEntryNum = searchRangeStart;
    char intS[MAX_STRING_INT_SIZE];
    char floatS[MAX_STRING_FLOAT_SIZE];
    struct timeval startTime, endTime;
    Entry entry;
    // char entriesFoundS[entriesRemaining * sizeof(Entry) + 1];
    char entryS[MAX_STRING_ENTRY_SIZE];
    // char patternFoundAtLeastOnce = 0;

    gettimeofday(&startTime, NULL);
    while (entriesRemaining > 0) {
        if (readEntryFromFile(fp, curEntryNum, &entry) == 1) {
            printf("Error while reading entry from file\nExiting...\n");
            exit(1);
        }

        sprintf(intS, "%d", entry.AM);
        sprintf(floatS, "%f", entry.salary);

        if (strstr(intS, searchPattern) != NULL || strstr(floatS, searchPattern) != NULL ||
            strstr(entry.name, searchPattern) != NULL || strstr(entry.surname, searchPattern) != NULL) {
            entryToString(entry, entryS);  // with end delimiter
            close(parentPipeDesc[0]);
            write(parentPipeDesc[1], entryS, /*strlen(entryS) + 1*/ MAX_STRING_ENTRY_SIZE);
            close(parentPipeDesc[1]);

            // strcat(entriesFoundS, entryS);
            // patternFoundAtLeastOnce = 1;
        }
    }
    gettimeofday(&endTime, NULL);

    // if (patternFoundAtLeastOnce == 0)  // nothing has been written to pipe
    // {
    //     close(parentPipeDesc[0]);
    //     write(parentPipeDesc[1], "no_result", 10);
    //     close(parentPipeDesc[1]);

    //     fclose(fp);
    //     return;
    // }

    close(parentPipeDesc[0]);
    write(parentPipeDesc[1], "m", 2);  // end of entries delimiter
    close(parentPipeDesc[1]);

    // add more things like the time that it took to search the entries, maybe the pid of current process and more

    char metadata[MAX_STRING_METADATA_SIZE];  // pid + del + timeElapsed + end of metadata + end of string

    sprintf(intS, "%d", getpid());
    strcpy(metadata, intS);
    strcat(metadata, "$");

    char uintS[12];
    sprintf(uintS, "%u", endTime.tv_usec - startTime.tv_usec);
    strcat(metadata, uintS);
    strcat(metadata, "&");

    close(parentPipeDesc[0]);
    write(parentPipeDesc[1], metadata, MAX_STRING_METADATA_SIZE /*+1*/);
    close(parentPipeDesc[1]);

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

char splitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, unsigned int remainingTreeDepth, int parentPipeDesc[2], char isFirstSplitterMerger) {
    int child1FileDesc[2], child2FileDesc[2];
    struct timeval startTime, endTime;
    pid_t pid1 = -1, pid2 = -1;  // initialize for splitting code afterwards

    gettimeofday(&startTime, NULL);

    if (pipe(child1FileDesc) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    if (pipe(child2FileDesc) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    if (remainingTreeDepth > 1) {
        pid1 = fork();
        if (pid1 < 0) {
            fprintf(stderr, "fork Failed");
            return 1;
        }
        if (pid1 > 0) {
            pid2 = fork();
            if (pid2 < 0) {
                fprintf(stderr, "fork Failed");
                return 1;
            }
        }
        // child process 1
        if (pid1 == 0) {
            splitterMergerNodeJob(dataFileName, /*calculate start of range*/, /*calculate end of range*/, searchPattern, remainingTreeDepth - 1, child1FileDesc, 0);
        }
        // child process 2
        if (pid2 == 0) {
            splitterMergerNodeJob(dataFileName, /*calculate start of range*/, /*calculate end of range*/, searchPattern, remainingTreeDepth - 1, child2FileDesc, 0);
        }

        // parent process
        if (pid1 > 0) {
            readAndSendResultsOfChild(child1FileDesc, parentPipeDesc);
            readAndSendResultsOfChild(child2FileDesc, parentPipeDesc);
        }
    } else if (remainingTreeDepth == 1) {
        pid1 = fork();
        if (pid1 < 0) {
            fprintf(stderr, "fork Failed");
            return 1;
        }
        if (pid1 > 0) {
            pid2 = fork();
            if (pid2 < 0) {
                fprintf(stderr, "fork Failed");
                return 1;
            }
        }

        // child process 1
        if (pid1 == 0) {
            leafNodeJob(dataFileName, /*calculate start of range*/, /*calculate end of range*/, searchPattern, child1FileDesc);
        }
        // child process 2
        if (pid2 == 0) {
            leafNodeJob(dataFileName, /*calculate start of range*/, /*calculate end of range*/, searchPattern, child2FileDesc);
        }

        // parent process
        if (pid1 > 0) {
            readAndSendResultsOfChild(child1FileDesc, parentPipeDesc);
            readAndSendResultsOfChild(child2FileDesc, parentPipeDesc);
        }
    }

    if (pid1 > 0) {
        gettimeofday(&endTime, NULL);

        char metadata[MAX_STRING_METADATA_SIZE];

        close(parentPipeDesc[0]);
        write(parentPipeDesc[1], "m", /*strlen(entryS) + 1*/ 2);
        close(parentPipeDesc[1]);

        char intS[MAX_STRING_INT_SIZE];
        sprintf(intS, "%d", getpid());
        strcpy(metadata, intS);
        strcat(metadata, "$");

        char uintS[MAX_STRING_INT_SIZE];
        sprintf(uintS, "%u", endTime.tv_usec - startTime.tv_usec);
        strcat(metadata, uintS);
        strcat(metadata, "&");

        // // get metadata and send to parent
        // // read from child
        // close(childFileDesc[1]);  // Close writing end of second pipe
        // read(childFileDesc[0], metadata, MAX_STRING_METADATA_SIZE);
        // // printf("Concatenated string %s\n", concat_str);
        // close(childFileDesc[0]);

        close(parentPipeDesc[0]);
        write(parentPipeDesc[1], metadata, /*strlen(entryS) + 1*/ MAX_STRING_METADATA_SIZE);
        close(parentPipeDesc[1]);

        if (isFirstSplitterMerger) {
            close(parentPipeDesc[0]);
            write(parentPipeDesc[1], "end", 4);
            close(parentPipeDesc[1]);
        }
    }

    return 0;
}

char rootNodeJob(int argc, char** argv, unsigned int* height, char** dataFileName, char** searchPattern, char* skewFlag) {
    pid_t pid;
    int childFileDesc[2];

    handleFlags(argc, argv, height, dataFileName, searchPattern, skewFlag);

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    } else if (pid == 0) {
        splitterMergerNodeJob(*dataFileName, /*range start*/, /*range end*/, searchPattern, height, childFileDesc, 1);
    } else {
        FILE* outFileP;
        char entryS[MAX_STRING_ENTRY_SIZE];
        char metadata[MAX_STRING_METADATA_SIZE];
        Entry resEntries[100 /* max entries number */];  /////////////////////////////////////////////// CHANGE THIS NUMBER

        outFileP = fopen("results.txt", "w");

        close(childFileDesc[1]);
        read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
        // printf("Concatenated string %s\n", concat_str);
        close(childFileDesc[0]);

        unsigned int resIndex = 0;
        while (strcmp(entryS, "end") != 0) {
            if (strcmp(entryS, "m")) {
                close(childFileDesc[1]);
                read(childFileDesc[0], metadata, MAX_STRING_METADATA_SIZE);
                // printf("Concatenated string %s\n", concat_str);
                close(childFileDesc[0]);

                // handle metadata of one process ////////////////////////////////////////////////// TO DO
            } else {
                // Entry resEntry;
                // stringToEntry(entryS, &resEntry);

                // print entry to file to a new line
                writeEntryToFile(entryS, outFileP);

                // resEntries[resIndex].AM = resEntry.AM;
                // strcpy(resEntries[resIndex].name, resEntry.name);
                // strcpy(resEntries[resIndex].surname, resEntry.surname);
                // resEntries[resIndex].salary = resEntry.salary;

                resIndex++;
            }

            close(childFileDesc[1]);
            read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
            // printf("Concatenated string %s\n", concat_str);
            close(childFileDesc[0]);
        }

        fclose(outFileP);

        sortNodeJob(resEntries, resIndex);
        for (unsigned int i = 0; i < 2 ^ height - 1; i++) /* ^ ?????????????????*/ {
            pid = wait(NULL);
            printf("Process with pid: %d exited succesfully\n", pid);
        }
    }
}

char sortNodeJob(Entry resEntries[], unsigned int resSize) {
    // pid_t pid = fork();

    // if (pid < 0) {
    //     fprintf(stderr, "fork Failed");
    //     return 1;
    // } else if (pid == 0) {
    execvp("sort", "--sort=-h", "results.txt");
    // }
}