#include "nodes.h"

void execSplitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, unsigned int remainingTreeDepth,
                               char* childPipeName, char isFirstSplitterMerger, char skewFlag, unsigned int entriesNum, unsigned int leafsNum,
                               unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd) {
    char searchRangeEndS[MAX_STRING_INT_SIZE], searchRangeStartS[MAX_STRING_INT_SIZE], remainingTreeDepthS[MAX_STRING_INT_SIZE],
        skewFlagS[2], isFirstSplitterMergerS[2], entriesNumS[MAX_STRING_INT_SIZE], leafsNumS[MAX_STRING_INT_SIZE], leafIndexRangeStartS[MAX_STRING_INT_SIZE],
        leafIndexRangeEndS[MAX_STRING_INT_SIZE];

    sprintf(searchRangeStartS, "%u", searchRangeStart);
    sprintf(searchRangeEndS, "%u", searchRangeEnd);
    sprintf(remainingTreeDepthS, "%u", remainingTreeDepth);
    // sprintf(childFileDesc0S, "%d", childFileDesc[0]);
    // sprintf(childFileDesc1S, "%d", childFileDesc[1]);
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
                        childPipeName, isFirstSplitterMergerS, skewFlagS, entriesNumS, leafsNumS, leafIndexRangeStartS, leafIndexRangeEndS, NULL};
        if (execvp(args[0], args) < 0)
            printf("Exec failed\n");
    } else {
        char* args[] = {"executables/splitterMergerNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern, remainingTreeDepthS,
                        childPipeName, isFirstSplitterMergerS, skewFlagS, NULL};
        if (execvp(args[0], args) < 0)
            printf("Exec failed\n");
    }

    return;
}

void execLeafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* childPipeName,
                     char skewFlag, unsigned int leafIndex, unsigned int entriesNum, unsigned int leafsNum) {
    char searchRangeEndS[MAX_STRING_INT_SIZE], searchRangeStartS[MAX_STRING_INT_SIZE],
        leafIndexS[MAX_STRING_INT_SIZE], entriesNumS[MAX_STRING_INT_SIZE],
        leafsNumS[MAX_STRING_INT_SIZE], skewFlagS[2];

    sprintf(searchRangeStartS, "%u", searchRangeStart);
    sprintf(searchRangeEndS, "%u", searchRangeEnd);
    // sprintf(childFileDesc0S, "%d", childFileDesc[0]);
    // sprintf(childFileDesc1S, "%d", childFileDesc[1]);
    sprintf(leafIndexS, "%u", leafIndex);
    sprintf(entriesNumS, "%u", entriesNum);
    sprintf(leafsNumS, "%u", leafsNum);
    sprintf(skewFlagS, "%c", skewFlag);

    if (skewFlag == 1) {
        char* args[] = {"executables/leafNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern,
                        childPipeName, skewFlagS, leafIndexS, entriesNumS, leafsNumS, NULL};
        execvp(args[0], args);
    } else {
        char* args[] = {"executables/leafNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern,
                        childPipeName, skewFlagS, NULL};
        execvp(args[0], args);
    }

    return;
}

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* parentPipeName,
                 char skewFlag, unsigned int leafIndex, unsigned int entriesNum, unsigned int leafsNum) {
    unsigned int entriesRemaining;

    printf("Starting leaf node's job...\n");
    if (skewFlag == 1) {
        unsigned int sum = 0;
        for (unsigned int j = 1; j <= leafsNum; j++)
            sum += j;

        searchRangeStart = 0;
        for (unsigned int i = 1; i <= leafIndex - 1; i++)
            searchRangeStart += round(((float)(entriesNum * i) / sum));

        searchRangeStart++;

        searchRangeEnd = searchRangeStart + round(((float)(entriesNum * leafIndex) / sum)) - 1;
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

    int parentPipeDesc = open(parentPipeName, O_WRONLY);
    // printf("Leaf with pid %d is printing to pipe\n", getpid());
    printf("Leaf with pid %d has rangeStart = %u, rangeEnd = %u\n", getpid(), searchRangeStart, searchRangeEnd);

    while (entriesRemaining > 0) {
        if (readEntryFromFile(fp, curEntryNum, &entry) == 1) {
            printf("Error while reading entry from file\nExiting...\n");
            close(parentPipeDesc);
            unlink(parentPipeName);
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
            // entryToString(entry, &entryS);  // with end delimiter

            // sprintf(int1S, "%ld", entry.AM);
            // sprintf(floatS, "%f", entry.salary);
            // sprintf(int2S, "%d", entry.houseNumber);

            // printf("int string: %s\n", int1S);
            // printf("float string: %s\n", floatS);
            // printf("int2 string: %s\n", int2S);

            // printf("IN ENTRY TO STRING\n");
            // *s = "hello";
            strcpy(entryS, longS);
            strcat(entryS, "$");  // end of field
            strcat(entryS, entry.name);
            strcat(entryS, "$");
            strcat(entryS, entry.surname);
            strcat(entryS, "$");
            strcat(entryS, entry.streetName);
            strcat(entryS, "$");
            strcat(entryS, int1S);
            strcat(entryS, "$");
            strcat(entryS, entry.cityName);
            strcat(entryS, "$");
            strcat(entryS, entry.postCode);
            strcat(entryS, "$");
            strcat(entryS, floatS);
            strcat(entryS, "&");  // end of entry

            // close(parentPipeDesc[0]);
            printf("Leaf with pid %d will print entry to pipe: %s\n", getpid(), entryS);
            write(parentPipeDesc, entryS, /*strlen(entryS) + 1*/ MAX_STRING_ENTRY_SIZE);
            // close(parentPipeDesc[1]);

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

    // close(parentPipeDesc[0]);
    // write(parentPipeDesc, "m", 2);  // end of entries delimiter
    // close(parentPipeDesc[1]);

    // add more things like the time that it took to search the entries, maybe the pid of current process and more

    char metadata[MAX_STRING_METADATA_SIZE];  // pid + del + timeElapsed + end of metadata + end of string

    // strcpy(int1S, ""); //    <--------------------  MAYBE USEFUL

    strcpy(metadata, "ml");  // "l" indicates leaf

    sprintf(int1S, "%d", getpid());
    strcat(metadata, int1S);
    strcat(metadata, "$");

    // char uintS[12];
    sprintf(int1S, "%lu", endTime.tv_usec - startTime.tv_usec);
    strcat(metadata, int1S);
    strcat(metadata, "&");

    // close(parentPipeDesc[0]);
    printf("Writing metadata: %s\n", metadata);
    write(parentPipeDesc, metadata, MAX_STRING_METADATA_SIZE /*+1*/);
    // close(parentPipeDesc[1]);

    // when parent gets metadata, he stops reading

    // close(pipeFileDescriptors[0]);
    // write(pipeFileDescriptors[1], '~', 1);  // end delimiter
    // close(pipeFileDescriptors[1]);

    // close(pipeFileDescriptors[0]);
    // write(pipeFileDescriptors[1], entriesFoundS, strlen(entriesFoundS) + 1);
    // close(pipeFileDescriptors[1]);
    close(parentPipeDesc);

    fclose(fp);
    return;
}

char splitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern,
                           unsigned int remainingTreeDepth, char* parentPipeName, char isFirstSplitterMerger, char skewFlag,
                           unsigned int entriesNum, unsigned int leafsNum, unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd) {
    char child1PipeName[20], child2PipeName[20], pidS[MAX_STRING_INT_SIZE];
    strcpy(child1PipeName, "tmp/child1_");
    sprintf(pidS, "%d", getpid());
    strcat(child1PipeName, pidS);

    strcpy(child2PipeName, "tmp/child2_");
    sprintf(pidS, "%d", getpid());
    strcat(child2PipeName, pidS);

    // char *leafIndexRangeStartS[MAX_STRING_INT_SIZE], *leafIndexRangeEndS[MAX_STRING_INT_SIZE];

    // sprintf(leafIndexRangeStartS, "%u", leafIndexRangeStart);
    // sprintf(leafIndexRangeEndS, "%u", leafIndexRangeEnd);

    struct timeval startTime, endTime;
    pid_t pid1 = -1, pid2 = -1;  // initialize for splitting code afterwards
    unsigned int child1RangeStart, child1RangeEnd, child2RangeStart, child2RangeEnd, child1LeafIndexRangeStart, child1LeafIndexRangeEnd,
        child2LeafIndexRangeStart, child2LeafIndexRangeEnd;

    // printf("I am a splitter/merger with arguments: dataFileName->%s, searchRangeStart->%u, searchRangeEnd->%u, searchPattern->%s, remainingTreeDepth->%u, parentPipeDesc[0]->%d, parentPipeDesc[1]->%d, isFirstSplitterMerger->%c, skewFlag->%c, entriesNum->%u, leafsNum->%u, leafIndexRangeStart->%u, leafIndexRangeEnd->%u\n",
    //        dataFileName, searchRangeStart, searchRangeEnd, searchPattern, remainingTreeDepth, parentPipeDesc[0], parentPipeDesc[1], isFirstSplitterMerger, skewFlag, entriesNum, leafsNum, leafIndexRangeStart, leafIndexRangeEnd);

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

    mkfifo(child1PipeName, 0666);
    mkfifo(child2PipeName, 0666);

    // close(child1FileDesc[0]);
    // close(child1FileDesc[1]);
    // close(child2FileDesc[0]);
    // close(child2FileDesc[1]);
    int parentPipeDesc;
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
            // close(parentPipeDesc[0]);
            // close(parentPipeDesc[1]);
            // close(child2FileDesc[0]);
            // close(child2FileDesc[1]);
            // close(child1FileDesc[0]);
            // close(child1FileDesc[1]);
            execSplitterMergerNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, remainingTreeDepth - 1, child1PipeName, 0, skewFlag,
                                      entriesNum, leafsNum, child1LeafIndexRangeStart, child1LeafIndexRangeEnd);
            // splitterMergerNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, remainingTreeDepth - 1, child1FileDesc, 0, skewFlag);
        }
        // child process 2
        if (pid2 == 0) {
            // close(parentPipeDesc[0]);
            // close(parentPipeDesc[1]);
            // close(child1FileDesc[0]);
            // close(child1FileDesc[1]);
            // close(child2FileDesc[0]);
            // close(child2FileDesc[1]);
            execSplitterMergerNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, remainingTreeDepth - 1, child2PipeName, 0, skewFlag,
                                      entriesNum, leafsNum, child2LeafIndexRangeStart, child2LeafIndexRangeEnd);
            // splitterMergerNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, remainingTreeDepth - 1, child2FileDesc, 0, skewFlag);
        }

        // parent process
        if (pid1 > 0 && pid2 > 0) {
            parentPipeDesc = open(parentPipeName, O_WRONLY);
            readAndSendResultsOfChild(child1PipeName, parentPipeDesc, pid1);
            readAndSendResultsOfChild(child2PipeName, parentPipeDesc, pid2);
            // close(parentPipeDesc);
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
            // close(parentPipeDesc[0]);
            // close(parentPipeDesc[1]);
            // close(child2FileDesc[0]);
            // close(child2FileDesc[1]);
            // close(child2FileDesc[0]);
            // close(child2FileDesc[1]);
            execLeafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1PipeName, skewFlag, leafIndexRangeStart, entriesNum, leafsNum);
            // leafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1FileDesc, skewFlag, leafIndexRangeStart, entriesNum, leafsNum);
        }
        // child process 2
        if (pid2 == 0) {
            // close(parentPipeDesc[0]);
            // close(parentPipeDesc[1]);
            // close(child1FileDesc[0]);
            // close(child1FileDesc[1]);
            // close(child2FileDesc[0]);
            // close(child2FileDesc[1]);
            execLeafNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, child2PipeName, skewFlag, leafIndexRangeEnd, entriesNum, leafsNum);
            // leafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1FileDesc, skewFlag, leafIndexRangeStart, entriesNum, leafsNum);
        }

        // parent process
        if (pid1 > 0 && pid2 > 0) {
            printf("WAITING FOR CHILDREN\n");

            printf("Parent pipe's name: %s\n", parentPipeName);
            parentPipeDesc = open(parentPipeName, O_WRONLY);
            readAndSendResultsOfChild(child1PipeName, parentPipeDesc, pid1);
            readAndSendResultsOfChild(child2PipeName, parentPipeDesc, pid2);
            // close(parentPipeDesc);
        }
    }

    if (pid1 > 0 && pid2 > 0) {
        gettimeofday(&endTime, NULL);

        char metadata[MAX_STRING_METADATA_SIZE];

        // int parentPipeDesc = open(parentPipeName, O_WRONLY);
        // close(parentPipeDesc[0]);
        // write(parentPipeDesc, "m", /*strlen(entryS) + 1*/ 2);
        // close(parentPipeDesc[1]);

        strcpy(metadata, "ms");  // "s" indicates splitter/merger

        char intS[MAX_STRING_INT_SIZE];
        sprintf(intS, "%d", getpid());
        strcat(metadata, intS);
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

        // close(parentPipeDesc[0]);
        write(parentPipeDesc, metadata, /*strlen(entryS) + 1*/ MAX_STRING_METADATA_SIZE);
        // close(parentPipeDesc[1]);

        if (isFirstSplitterMerger) {
            // close(parentPipeDesc[0]);
            write(parentPipeDesc, "end", 4);
            // close(parentPipeDesc[1]);
        }

        printf("Splitter is in close\n");
        close(parentPipeDesc);
        unlink(parentPipeName);
        printf("Splitter closed\n");

        wait(NULL);
        wait(NULL);
        printf("Both children of splitter exited\n");

        if (isFirstSplitterMerger == 1) {
            printf("FIRST SPLITTER IS FINISHED!!!!\n");
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
    char childPipeName[] = "tmp/rootChild";

    long lSize;
    unsigned int entriesNum;
    FILE* inputFileP = fopen(dataFileName, "rb");
    // unsigned int searchRangeStart, searchRangeEnd;

    // check number of records
    fseek(inputFileP, 0, SEEK_END);
    lSize = ftell(inputFileP);
    rewind(inputFileP);
    entriesNum = (int)lSize / sizeof(Entry);

    // printf("Entries in file: %u\n", entriesNum);

    // if (pipe(childFileDesc) == -1) {
    //     fprintf(stderr, "Pipe Failed");
    //     return 1;
    // }

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

    mkfifo(childPipeName, 0666);
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
        // close(childFileDesc[0]);
        // close(childFileDesc[1]);
        // fclose(inputFileP);
        if (skewFlag == 0)
            execSplitterMergerNodeJob(dataFileName, 1, entriesNum, searchPattern, height, childPipeName, 1, skewFlag, 0, 0, 0, 0);
        else
            execSplitterMergerNodeJob(dataFileName, 1, entriesNum, searchPattern, height, childPipeName, 1, skewFlag, entriesNum, leafsNum, leafIndexRangeStart, leafIndexRangeEnd);

        // splitterMergerNodeJob(dataFileName, 1, entriesNum, searchPattern, height, childFileDesc, 1, skewFlag);
    } else {
        FILE* outFileP;
        char entryS[MAX_STRING_ENTRY_SIZE];
        // char metadata[MAX_STRING_METADATA_SIZE];
        int readReturn;

        unsigned int size = (int)pow(2, height + 1) - 2;
        int pids[size];
        long unsigned int times[size], leafNodesTotalTime = 0, splitterMergerNodesTotalTime = 0;
        char isLeaf[size];
        long unsigned int programTotalTime = 0;
        unsigned int splitterMergerNodesNum = pow(2, height + 1) - 1 - pow(2, height);
        long double averageLeafNodesTime, averageSplitterMergerNodesTime;

        // for (unsigned int i = 0; i < size; i++)
        //     pids[i] = -1;

        // Entry resEntries[100 /* max entries number */];  /////////////////////////////////////////////// CHANGE THIS NUMBER

        printf("Doing root node's job...\n");

        // pid = wait(NULL);
        // printf("Root: Process with pid: %d exited succesfully\n", pid);

        outFileP = fopen("results.txt", "w");
        int childFileDesc = open(childPipeName, O_RDONLY);
        printf("Root opened pipe\n");

        // close(childFileDesc[1]);
        // printf("Root: Will read 1st entry\n");

        readReturn = read(childFileDesc, entryS, MAX_STRING_ENTRY_SIZE);
        // printf("Root: Read entry: %s\n", entryS);
        // close(childFileDesc[0]);

        // while (strcmp(entryS, "") == 0)
        //     ;

        unsigned int index = 0;
        char *token, *ptr;
        // unsigned int resIndex = 0;
        while (strcmp(entryS, "end") != 0 && readReturn > 0) {
            if (entryS[0] == 'm') {
                // char isLeaf = 0, isSplitterMerger = 0;
                isLeaf[index] = 0;
                if (entryS[1] == 'l')
                    isLeaf[index] = 1;
                // else if (entryS[1] == 's')
                //     isLeaf[index] = 0;  // so it is a splitter/merger

                for (unsigned int i = 0; i < MAX_STRING_ENTRY_SIZE; i++) {
                    if (entryS[i + 2] != '\0')
                        entryS[i] = entryS[i + 2];
                    else {
                        entryS[i] = '\0';
                        entryS[i + 1] = '\0';
                        break;
                    }
                }

                // manage metadata
                token = strtok(entryS, "$");
                pids[index] = atoi(token);

                token = strtok(NULL, "&");
                times[index] = strtol(token, &ptr, 10);

                if (isLeaf[index])
                    leafNodesTotalTime += times[index];
                else
                    splitterMergerNodesTotalTime += times[index];

                index++;

                // close(childFileDesc[0]);

                // handle metadata of one process ////////////////////////////////////////////////// TO DO
            } else {
                // Entry resEntry;
                // stringToEntry(entryS, &resEntry);

                // print entry to file to a new line
                // printf("trap1\n");
                writeEntryToFile(entryS, outFileP);  ////////////////////// CAUSES SEGMENTATION
                // printf("trap2\n");
                // printf("Root: Read entry: %s\n", entryS);

                // resEntries[resIndex].AM = resEntry.AM;
                // strcpy(resEntries[resIndex].name, resEntry.name);
                // strcpy(resEntries[resIndex].surname, resEntry.surname);
                // resEntries[resIndex].salary = resEntry.salary;

                // resIndex++;
            }

            strcpy(entryS, "");
            // close(childFileDesc[1]);
            readReturn = read(childFileDesc, entryS, MAX_STRING_ENTRY_SIZE);
            // printf("Root: Read entry: %s\n", entryS);

            // printf("Concatenated string %s\n", concat_str);
            // close(childFileDesc[0]);
        }
        close(childFileDesc);
        unlink(childPipeName);
        fclose(outFileP);
        // printf("HELLO END OF ROOT\n");
        // print statistics here

        for (unsigned int i = 0; i < index; i++)
            programTotalTime += times[index];

        averageLeafNodesTime = (long double)leafNodesTotalTime / leafsNum;
        averageSplitterMergerNodesTime = (long double)splitterMergerNodesTotalTime / splitterMergerNodesNum;

        printf("\n\nStatistics:\n");

        for (unsigned int i = 0; i < index; i++) {
            if (isLeaf[i])
                printf("Leaf ");
            else
                printf("Splitter/Merger ");

            printf("with pid %d took %lu microseconds to complete.\n", pids[i], times[i]);
        }

        printf("\nTotal program time: %lu\nTotal time of leaf nodes: %lu microseconds\nTotal time of splitter/merger nodes: %lu microseconds\nAverage time of leaf nodes: %Lf microseconds\nAverage time of splitter/merger nodes: %Lf microseconds\n",
               programTotalTime, leafNodesTotalTime, splitterMergerNodesTotalTime, averageLeafNodesTime, averageSplitterMergerNodesTime);

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
            printf("Root: Process with pid: %d exited succesfully\n", pid);
            // }
        }

        printf("Root node's job executed.\n");
        char command[] = "rm -rf tmp/*";
        system(command);
    }
    return 0;
}