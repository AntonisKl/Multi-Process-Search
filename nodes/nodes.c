#include "nodes.h"

void execSplitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, unsigned int remainingTreeDepth,
                               int childFileDesc[2], char isFirstSplitterMerger, char skewFlag, unsigned int entriesNum, unsigned int leafsNum,
                               unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd) {
    char searchRangeEndS[MAX_STRING_INT_SIZE], searchRangeStartS[MAX_STRING_INT_SIZE], remainingTreeDepthS[MAX_STRING_INT_SIZE],
        childFileDesc0S[MAX_STRING_INT_SIZE], childFileDesc1S[MAX_STRING_INT_SIZE], skewFlagS[2], isFirstSplitterMergerS[2],
        entriesNumS[MAX_STRING_INT_SIZE], leafsNumS[MAX_STRING_INT_SIZE], leafIndexRangeStartS[MAX_STRING_INT_SIZE],
        leafIndexRangeEndS[MAX_STRING_INT_SIZE];

    sprintf(searchRangeStartS, "%u", searchRangeStart);
    sprintf(searchRangeEndS, "%u", searchRangeEnd);
    sprintf(remainingTreeDepthS, "%u", remainingTreeDepth);
    sprintf(childFileDesc0S, "%d", childFileDesc[0]);
    sprintf(childFileDesc1S, "%d", childFileDesc[1]);
    sprintf(skewFlagS, "%c", skewFlag);
    sprintf(isFirstSplitterMergerS, "%c", isFirstSplitterMerger);

    // printf("calling exec for splitter node skewFlag=%c...\n", skewFlag);
    if (skewFlag == 1)  // with skew
    {
        sprintf(entriesNumS, "%u", entriesNum);
        sprintf(leafsNumS, "%u", leafsNum);
        sprintf(leafIndexRangeStartS, "%u", leafIndexRangeStart);
        sprintf(leafIndexRangeEndS, "%u", leafIndexRangeEnd);
        char* args[] = {"executables/splitterMergerNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern, remainingTreeDepthS,
                        childFileDesc0S, childFileDesc1S, isFirstSplitterMergerS, skewFlagS, entriesNumS, leafsNumS, leafIndexRangeStartS, leafIndexRangeEndS, NULL};
        if (execvp(args[0], args) < 0)
            printf("Exec failed\n");
    } else {
        char* args[] = {"executables/splitterMergerNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern, remainingTreeDepthS,
                        childFileDesc0S, childFileDesc1S, isFirstSplitterMergerS, skewFlagS, NULL};
        if (execvp(args[0], args) < 0)
            printf("Exec failed\n");
    }

    return;
}

void execLeafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, int childFileDesc[2],
                     char skewFlag, unsigned int leafIndex, unsigned int entriesNum, unsigned int leafsNum) {
    char searchRangeEndS[MAX_STRING_INT_SIZE], searchRangeStartS[MAX_STRING_INT_SIZE],
        childFileDesc0S[MAX_STRING_INT_SIZE], childFileDesc1S[MAX_STRING_INT_SIZE], leafIndexS[MAX_STRING_INT_SIZE], entriesNumS[MAX_STRING_INT_SIZE],
        leafsNumS[MAX_STRING_INT_SIZE], skewFlagS[2];

    sprintf(searchRangeStartS, "%u", searchRangeStart);
    sprintf(searchRangeEndS, "%u", searchRangeEnd);
    sprintf(childFileDesc0S, "%d", childFileDesc[0]);
    sprintf(childFileDesc1S, "%d", childFileDesc[1]);
    sprintf(leafIndexS, "%u", leafIndex);
    sprintf(entriesNumS, "%u", entriesNum);
    sprintf(leafsNumS, "%u", leafsNum);
    sprintf(skewFlagS, "%c", skewFlag);

    if (skewFlag == 1) {
        char* args[] = {"executables/leafNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern,
                        childFileDesc0S, childFileDesc1S, skewFlagS, leafIndexS, entriesNumS, leafsNumS, NULL};
        execvp(args[0], args);
    } else {
        char* args[] = {"executables/leafNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern,
                        childFileDesc0S, childFileDesc1S, skewFlagS, NULL};
        execvp(args[0], args);
    }

    return;
}

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, int parentPipeDesc[2],
                 char skewFlag, unsigned int leafIndex, unsigned int entriesNum, unsigned int leafsNum) {
    unsigned int entriesRemaining;

    printf("Starting leaf node's job...\n");
    if (skewFlag == 1) {
        unsigned int sum = 0;
        for (unsigned int j = 1; j <= leafsNum; j++)
            sum += j;

        searchRangeStart = 0;
        for (unsigned int i = 1; i <= leafIndex - 1; i++)
            searchRangeStart += ((entriesNum * i) / sum);

        searchRangeStart++;

        searchRangeEnd = searchRangeStart + ((leafsNum * leafIndex) / sum);
    }

    entriesRemaining = searchRangeEnd - searchRangeStart + 1;

    unsigned int curEntryNum = searchRangeStart;
    char longS[MAX_STRING_INT_SIZE], int1S[MAX_STRING_INT_SIZE];
    char floatS[MAX_STRING_FLOAT_SIZE];
    struct timeval startTime, endTime;
    Entry entry;
    // char entriesFoundS[entriesRemaining * sizeof(Entry) + 1];
    char entryS[MAX_STRING_ENTRY_SIZE];
    // char patternFoundAtLeastOnce = 0;

    gettimeofday(&startTime, NULL);
    FILE* fp = fopen(dataFileName, "rb");

    while (entriesRemaining > 0) {
        if (readEntryFromFile(fp, curEntryNum, &entry) == 1) {
            printf("Error while reading entry from file\nExiting...\n");
            fclose(fp);
            // close(parentPipeDesc[0]);
            // close(parentPipeDesc[1]);
            exit(1);
        }

        sprintf(longS, "%ld", entry.AM);
        sprintf(floatS, "%f", entry.salary);
        sprintf(int1S, "%d", entry.houseNumber);

        if (strstr(longS, searchPattern) != NULL || strstr(floatS, searchPattern) != NULL || strstr(int1S, searchPattern) != NULL ||
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
    sprintf(int1S, "%lu", endTime.tv_usec - startTime.tv_usec);
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
                           unsigned int remainingTreeDepth, int parentPipeDesc[2], char isFirstSplitterMerger, char skewFlag,
                           unsigned int entriesNum, unsigned int leafsNum, unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd) {
    int child1FileDesc[2], child2FileDesc[2];
    struct timeval startTime, endTime;
    pid_t pid1 = -1, pid2 = -1;  // initialize for splitting code afterwards
    unsigned int child1RangeStart, child1RangeEnd, child2RangeStart, child2RangeEnd, child1LeafIndexRangeStart, child1LeafIndexRangeEnd,
        child2LeafIndexRangeStart, child2LeafIndexRangeEnd;

    printf("I am a splitter/merger with arguments: dataFileName->%s, searchRangeStart->%u, searchRangeEnd->%u, searchPattern->%s, remainingTreeDepth->%u, parentPipeDesc[0]->%d, parentPipeDesc[1]->%d, isFirstSplitterMerger->%c, skewFlag->%c, entriesNum->%u, leafsNum->%u, leafIndexRangeStart->%u, leafIndexRangeEnd->%u\n",
           dataFileName, searchRangeStart, searchRangeEnd, searchPattern, remainingTreeDepth, parentPipeDesc[0], parentPipeDesc[1], isFirstSplitterMerger, skewFlag, entriesNum, leafsNum, leafIndexRangeStart, leafIndexRangeEnd);

    gettimeofday(&startTime, NULL);

    child1RangeStart = searchRangeStart;
    child2RangeEnd = searchRangeEnd;

    unsigned int curEntriesNum = searchRangeEnd - searchRangeStart + 1;
    if (skewFlag == 0) {
        child1RangeEnd = searchRangeStart + (curEntriesNum / 2) - 1;
        child2RangeStart = searchRangeStart + (curEntriesNum / 2);
    } else {
        // code here for skew | calculate starts and ends
        // printf("skew = 1 not yet implemented\n");
        if (remainingTreeDepth > 1) {
            unsigned int curLeafsNum = leafIndexRangeEnd - leafIndexRangeStart + 1;

            child1LeafIndexRangeStart = leafIndexRangeStart;
            child2LeafIndexRangeEnd = leafIndexRangeEnd;

            child1LeafIndexRangeEnd = leafIndexRangeStart + (curLeafsNum / 2) - 1;
            child2LeafIndexRangeStart = leafIndexRangeStart + (curLeafsNum / 2);
        }

        //exit(1);
    }

    if (pipe(child1FileDesc) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    if (pipe(child2FileDesc) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    // close(child1FileDesc[0]);
    // close(child1FileDesc[1]);
    // close(child2FileDesc[0]);
    // close(child2FileDesc[1]);

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
            close(parentPipeDesc[0]);
            close(parentPipeDesc[1]);
            close(child2FileDesc[0]);
            close(child2FileDesc[1]);
            close(child1FileDesc[0]);
            close(child1FileDesc[1]);
            execSplitterMergerNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, remainingTreeDepth - 1, child1FileDesc, 0, skewFlag,
                                      entriesNum, leafsNum, child1LeafIndexRangeStart, child1LeafIndexRangeEnd);
            // splitterMergerNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, remainingTreeDepth - 1, child1FileDesc, 0, skewFlag);
        }
        // child process 2
        if (pid2 == 0) {
            close(parentPipeDesc[0]);
            close(parentPipeDesc[1]);
            close(child1FileDesc[0]);
            close(child1FileDesc[1]);
            close(child2FileDesc[0]);
            close(child2FileDesc[1]);
            execSplitterMergerNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, remainingTreeDepth - 1, child2FileDesc, 0, skewFlag,
                                      entriesNum, leafsNum, child2LeafIndexRangeStart, child2LeafIndexRangeEnd);
            // splitterMergerNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, remainingTreeDepth - 1, child2FileDesc, 0, skewFlag);
        }

        // parent process
        if (pid1 > 0 && pid2 > 0) {
            printf("WAITING FOR CHILDREN 22222222222222222222222\n");
            readAndSendResultsOfChild(child1FileDesc, parentPipeDesc, pid1);
            readAndSendResultsOfChild(child2FileDesc, parentPipeDesc, pid2);
        }
    } else if (remainingTreeDepth == 1) {
        pid1 = fork();
        if (pid1 < 0) {
            fprintf(stderr, "fork Failed");
            return 1;
        }
        if (pid1 > 0) {
            printf("Forked process with pid1: %d\n", pid1);
            pid2 = fork();
            if (pid2 < 0) {
                fprintf(stderr, "fork Failed");
                return 1;
            }
            if (pid2 > 0)
                printf("Forked process with pid2: %d\n", pid2);
        }

        // child process 1
        if (pid1 == 0) {
            close(parentPipeDesc[0]);
            close(parentPipeDesc[1]);
            close(child2FileDesc[0]);
            close(child2FileDesc[1]);
            close(child2FileDesc[0]);
            close(child2FileDesc[1]);
            execLeafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1FileDesc, skewFlag, leafIndexRangeStart, entriesNum, leafsNum);
            // leafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1FileDesc, skewFlag, leafIndexRangeStart, entriesNum, leafsNum);
        }
        // child process 2
        if (pid2 == 0) {
            close(parentPipeDesc[0]);
            close(parentPipeDesc[1]);
            close(child1FileDesc[0]);
            close(child1FileDesc[1]);
            close(child2FileDesc[0]);
            close(child2FileDesc[1]);
            execLeafNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, child2FileDesc, skewFlag, leafIndexRangeEnd, entriesNum, leafsNum);
            // leafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1FileDesc, skewFlag, leafIndexRangeStart, entriesNum, leafsNum);
        }

        // parent process
        if (pid1 > 0 && pid2 > 0) {
            printf("WAITING FOR CHILDREN\n");

            // printf("HELLO\n");
            readAndSendResultsOfChild(child1FileDesc, parentPipeDesc, pid1);
            readAndSendResultsOfChild(child2FileDesc, parentPipeDesc, pid2);
        }
    }

    if (pid1 > 0 && pid2 > 0) {
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
        sprintf(uintS, "%lu", endTime.tv_usec - startTime.tv_usec);
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

        printf("Splitter/Merger node's job executed.\n");
    }

    return 0;
}

void sortNodeJob() {
    // pid_t pid = fork();

    // if (pid < 0) {
    //     fprintf(stderr, "fork Failed");
    //     return 1;
    // } else if (pid == 0) {
    char* args[] = {"sort", "--sort=human-numeric", "results.txt", NULL};
    execvp("sort", args);
    // }
}

char rootNodeJob(unsigned int height, char* dataFileName, char* searchPattern, char skewFlag) {
    pid_t pid;
    int childFileDesc[2];

    long lSize;
    unsigned int entriesNum;
    FILE* inputFileP = fopen(dataFileName, "rb");
    // unsigned int searchRangeStart, searchRangeEnd;

    // check number of records
    fseek(inputFileP, 0, SEEK_END);
    lSize = ftell(inputFileP);
    rewind(inputFileP);
    entriesNum = (int)lSize / sizeof(Entry);

    printf("Entries in file: %u\n", entriesNum);

    if (pipe(childFileDesc) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    // if (skewFlag == 0) {
    //     searchRangeStart =
    // } else{
    //     // CODE HERE
    // }
    unsigned int leafIndexRangeStart, leafIndexRangeEnd;
    unsigned int leafsNum;

    if (skewFlag == 1) {
        leafsNum = (unsigned int)pow((double)2, (double)height);
        leafIndexRangeStart = 1;
        leafIndexRangeEnd = leafsNum;
    }

    fclose(inputFileP);

    if (entriesNum == 0) {
        printf("0 entries in file. Exiting...\n");
        // exit(1);
    }
    printf("Forking in root node...\n");
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    } else if (pid == 0) {
        // char searchRangeEndS[MAX_STRING_INT_SIZE], heightS[MAX_STRING_INT_SIZE],
        //     childFileDesc0S[MAX_STRING_INT_SIZE], childFileDesc1S[MAX_STRING_INT_SIZE], skewFlagS[2];

        // sprintf(searchRangeEndS, "%u", searchRangeEnd);
        // sprintf(heightS, "%u", height - 1);
        // sprintf(childFileDesc0S, "%d", childFileDesc[0]);
        // sprintf(childFileDesc1S, "%d", childFileDesc[1]);
        // sprintf(skewFlagS, "%c", skewFlag);

        // char* args[] = {"./../executables/splitterMergerNode", dataFileName, "1", searchRangeEndS, heightS,
        //                 childFileDesc0S, childFileDesc1S, "1", skewFlagS, NULL};
        // execvp(args[0], args);

        // if (skewFlag == 0)
        //     printf("skewFlag=%d\n", skewFlag);
        close(childFileDesc[0]);
        close(childFileDesc[1]);
        // fclose(inputFileP);
        if (skewFlag == 0)
            execSplitterMergerNodeJob(dataFileName, 1, entriesNum, searchPattern, height, childFileDesc, 1, skewFlag, 0, 0, 0, 0);
        else
            execSplitterMergerNodeJob(dataFileName, 1, entriesNum, searchPattern, height, childFileDesc, 1, skewFlag, entriesNum, leafsNum, leafIndexRangeStart, leafIndexRangeEnd);

        // splitterMergerNodeJob(dataFileName, 1, entriesNum, searchPattern, height, childFileDesc, 1, skewFlag);
    } else {
        FILE* outFileP;
        char entryS[MAX_STRING_ENTRY_SIZE];
        char metadata[MAX_STRING_METADATA_SIZE];
        int eofFlag;
        // Entry resEntries[100 /* max entries number */];  /////////////////////////////////////////////// CHANGE THIS NUMBER

        printf("Doing root node's job...\n");
        outFileP = fopen("results.txt", "w");

        close(childFileDesc[1]);
        eofFlag = read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
        printf("Read entry: %s\n", entryS);
        close(childFileDesc[0]);

        // unsigned int resIndex = 0;
        while (strcmp(entryS, "end") != 0 && eofFlag != EOF) {
            if (strcmp(entryS, "m")) {
                close(childFileDesc[1]);
                eofFlag = read(childFileDesc[0], metadata, MAX_STRING_METADATA_SIZE);
                printf("Read entry: %s\n", entryS);
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
            eofFlag = read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
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
            printf("Sort node's job executed.\n");
        } else {
            // for (unsigned int i = 0; i < (int)pow((double)2, (double)((height) + 1)) - 1; i++) {
            pid = wait(NULL);
            printf("Process with pid: %d exited succesfully\n", pid);
            // }
        }
        printf("Root node's job executed.\n");
    }
    return 0;
}