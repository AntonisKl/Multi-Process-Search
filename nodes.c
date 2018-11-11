#include "nodes.h"

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, int parentPipeDesc[2]) {
    FILE* fp = fopen(dataFileName, "rb");

    unsigned int entriesRemaining = searchRangeEnd - searchRangeStart + 1;
    unsigned int curEntryNum = searchRangeStart;
    char int1S[MAX_STRING_INT_SIZE], int2S[MAX_STRING_INT_SIZE];
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

        sprintf(int1S, "%d", entry.AM);
        sprintf(floatS, "%f", entry.salary);
        sprintf(int2S, "%d", entry.houseNumber);

        if (strstr(int1S, searchPattern) != NULL || strstr(floatS, searchPattern) != NULL || strstr(int2S, searchPattern) != NULL ||
            strstr(entry.name, searchPattern) != NULL || strstr(entry.surname, searchPattern) != NULL || strstr(entry.cityName, searchPattern) != NULL ||
            strstr(entry.postCode, searchPattern) != NULL || strstr(entry.streetName, searchPattern) != NULL) {
            entryToString(entry, entryS);  // with end delimiter
            close(parentPipeDesc[0]);
            write(parentPipeDesc[1], entryS, /*strlen(entryS) + 1*/ MAX_STRING_ENTRY_SIZE);
            close(parentPipeDesc[1]);

            // strcat(entriesFoundS, entryS);
            // patternFoundAtLeastOnce = 1;
        }

        entriesRemaining--;
        curEntryNum++;
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

    // strcpy(int1S, ""); //    <--------------------  MAYBE USEFUL

    sprintf(int1S, "%d", getpid());
    strcpy(metadata, int1S);
    strcat(metadata, "$");

    // char uintS[12];
    sprintf(int1S, "%u", endTime.tv_usec - startTime.tv_usec);
    strcat(metadata, int1S);
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

char splitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern,
                           unsigned int remainingTreeDepth, int parentPipeDesc[2], char isFirstSplitterMerger, char skewFlag) {
    int child1FileDesc[2], child2FileDesc[2];
    struct timeval startTime, endTime;
    pid_t pid1 = -1, pid2 = -1;  // initialize for splitting code afterwards
    unsigned int child1RangeStart, child1RangeEnd, child2RangeStart, child2RangeEnd;

    gettimeofday(&startTime, NULL);

    child1RangeStart = searchRangeStart;
    child2RangeEnd = searchRangeEnd;

    unsigned int entriesNum = searchRangeEnd - searchRangeStart + 1;
    if (skewFlag == 0) {
        child1RangeEnd = entriesNum / 2;
        child2RangeStart = (entriesNum / 2) + 1;
    } else {
        // code here for skew | calculate starts and ends
    }

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
            splitterMergerNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, remainingTreeDepth - 1, child1FileDesc, 0, skewFlag);
        }
        // child process 2
        if (pid2 == 0) {
            splitterMergerNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, remainingTreeDepth - 1, child2FileDesc, 0, skewFlag);
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
            leafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1FileDesc);
        }
        // child process 2
        if (pid2 == 0) {
            leafNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, child2FileDesc);
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

    long lSize;
    unsigned int entriesNum;
    FILE* inputFileP = fopen(dataFileName, "r");
    unsigned int searchRangeStart, searchRangeEnd;

    // check number of records
    fseek(inputFileP, 0, SEEK_END);
    lSize = ftell(inputFileP);
    rewind(inputFileP);
    entriesNum = (int)lSize / sizeof(Entry);

    // if (skewFlag == 0) {
    //     searchRangeStart =
    // } else{
    //     // CODE HERE
    // }

    fclose(inputFileP);

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    } else if (pid == 0) {
        splitterMergerNodeJob(*dataFileName, 1, entriesNum, searchPattern, height, childFileDesc, 1, skewFlag);
    } else {
        FILE* outFileP;
        char entryS[MAX_STRING_ENTRY_SIZE];
        char metadata[MAX_STRING_METADATA_SIZE];
        // Entry resEntries[100 /* max entries number */];  /////////////////////////////////////////////// CHANGE THIS NUMBER

        outFileP = fopen("results.txt", "w");

        close(childFileDesc[1]);
        read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
        // printf("Concatenated string %s\n", concat_str);
        close(childFileDesc[0]);

        // unsigned int resIndex = 0;
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

                // resIndex++;
            }

            close(childFileDesc[1]);
            read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
            // printf("Concatenated string %s\n", concat_str);
            close(childFileDesc[0]);
        }
        fclose(outFileP);

        // print statistics here

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork Failed");
            return 1;
        } else if (pid == 0) {
            sortNodeJob();
        } else {
            for (unsigned int i = 0; i < (int)pow((double)2, (double)((*height) - 1)); i++) /* ^ ?????????????????*/ {
                pid = wait(NULL);
                printf("Process with pid: %d exited succesfully\n", pid);
            }
        }
    }
}

void sortNodeJob() {
    // pid_t pid = fork();

    // if (pid < 0) {
    //     fprintf(stderr, "fork Failed");
    //     return 1;
    // } else if (pid == 0) {
    execvp("sort", "--sort=-h", "results.txt");
    // }
}