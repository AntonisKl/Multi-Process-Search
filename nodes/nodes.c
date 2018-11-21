#include "nodes.h"

void execSplitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, unsigned int remainingTreeDepth,
                               char* childPipeName, char isFirstSplitterMerger, char skewFlag, unsigned int recordsNum, unsigned int leafsNum,
                               unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd, int rootPid) {
    char searchRangeEndS[MAX_STRING_INT_SIZE], searchRangeStartS[MAX_STRING_INT_SIZE], remainingTreeDepthS[MAX_STRING_INT_SIZE],
        skewFlagS[2], isFirstSplitterMergerS[2], recordsNumS[MAX_STRING_INT_SIZE], leafsNumS[MAX_STRING_INT_SIZE], leafIndexRangeStartS[MAX_STRING_INT_SIZE],
        leafIndexRangeEndS[MAX_STRING_INT_SIZE], rootPidS[MAX_STRING_INT_SIZE];

    sprintf(searchRangeStartS, "%u", searchRangeStart);
    sprintf(searchRangeEndS, "%u", searchRangeEnd);
    sprintf(remainingTreeDepthS, "%u", remainingTreeDepth);
    sprintf(skewFlagS, "%c", skewFlag);
    sprintf(isFirstSplitterMergerS, "%c", isFirstSplitterMerger);
    sprintf(rootPidS, "%d", rootPid);

    if (skewFlag == 1)  // skew enabled
    {
        sprintf(recordsNumS, "%u", recordsNum);
        sprintf(leafsNumS, "%u", leafsNum);
        sprintf(leafIndexRangeStartS, "%u", leafIndexRangeStart);
        sprintf(leafIndexRangeEndS, "%u", leafIndexRangeEnd);
        char* args[] = {"executables/splitterMergerNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern, remainingTreeDepthS,
                        childPipeName, isFirstSplitterMergerS, skewFlagS, recordsNumS, leafsNumS, leafIndexRangeStartS, leafIndexRangeEndS, rootPidS, NULL};
        if (execvp(args[0], args) < 0)
            printf("Exec failed\n");
    } else {
        char* args[] = {"executables/splitterMergerNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern, remainingTreeDepthS,
                        childPipeName, isFirstSplitterMergerS, skewFlagS, rootPidS, NULL};
        if (execvp(args[0], args) < 0)
            printf("Exec failed\n");
    }

    return;
}

void execLeafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* childPipeName,
                     char skewFlag, unsigned int leafIndex, unsigned int recordsNum, unsigned int leafsNum, int rootPid) {
    char searchRangeEndS[MAX_STRING_INT_SIZE], searchRangeStartS[MAX_STRING_INT_SIZE],
        leafIndexS[MAX_STRING_INT_SIZE], recordsNumS[MAX_STRING_INT_SIZE],
        leafsNumS[MAX_STRING_INT_SIZE], skewFlagS[2], rootPidS[MAX_STRING_INT_SIZE];

    sprintf(searchRangeStartS, "%u", searchRangeStart);
    sprintf(searchRangeEndS, "%u", searchRangeEnd);
    sprintf(leafIndexS, "%u", leafIndex);
    sprintf(recordsNumS, "%u", recordsNum);
    sprintf(leafsNumS, "%u", leafsNum);
    sprintf(skewFlagS, "%c", skewFlag);
    sprintf(rootPidS, "%d", rootPid);

    if (skewFlag == 1) {
        char* args[] = {"executables/leafNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern,
                        childPipeName, skewFlagS, leafIndexS, recordsNumS, leafsNumS, rootPidS, NULL};
        execvp(args[0], args);
    } else {
        char* args[] = {"executables/leafNode", dataFileName, searchRangeStartS, searchRangeEndS, searchPattern,
                        childPipeName, skewFlagS, rootPidS, NULL};
        execvp(args[0], args);
    }

    return;
}

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* parentPipeName,
                 char skewFlag, unsigned int leafIndex, unsigned int recordsNum, unsigned int leafsNum, int rootPid) {
    
    unsigned int recordsRemaining; // remaining records for this child to read

    if (skewFlag == 1) {
        unsigned int sum = 0;
        for (unsigned int j = 1; j <= leafsNum; j++)
            sum += j;

        searchRangeStart = 0;
        for (unsigned int i = 1; i <= leafIndex - 1; i++)
            searchRangeStart += round(((float)(recordsNum * i) / sum));

        searchRangeStart++;

        searchRangeEnd = searchRangeStart + round(((float)(recordsNum * leafIndex) / sum)) - 1;
    }

    recordsRemaining = searchRangeEnd - searchRangeStart + 1;

    unsigned int curRecordNum = searchRangeStart;
    char longS[MAX_STRING_INT_SIZE], int1S[MAX_STRING_INT_SIZE];
    char floatS[MAX_STRING_FLOAT_SIZE];
    struct timeval startTime, endTime;
    Record record;
    char recordS[MAX_STRING_RECORD_SIZE];

    gettimeofday(&startTime, NULL); // start measuring time in usecs
    FILE* fp = fopen(dataFileName, "rb");

    int parentPipeDesc = open(parentPipeName, O_WRONLY);

    while (recordsRemaining > 0) {
        if (readRecordFromFile(fp, curRecordNum, &record) == 1) {
            printf("Error while reading record from file\nExiting...\n");
            close(parentPipeDesc);
            unlink(parentPipeName);
            fclose(fp);
            kill(rootPid, SIGUSR2);
            exit(1);
        }

        // prepare numbers for search
        sprintf(longS, "%ld", record.AM);
        sprintf(floatS, "%f", record.salary);
        sprintf(int1S, "%d", record.houseNumber);

        if (strstr(longS, searchPattern) != NULL || strstr(floatS, searchPattern) != NULL || strstr(int1S, searchPattern) != NULL ||
            strstr(record.name, searchPattern) != NULL || strstr(record.surname, searchPattern) != NULL || strstr(record.cityName, searchPattern) != NULL ||
            strstr(record.postCode, searchPattern) != NULL || strstr(record.streetName, searchPattern) != NULL) {
            
            strcpy(recordS, longS);
            strcat(recordS, "$");  // end of field
            strcat(recordS, record.name);
            strcat(recordS, "$");
            strcat(recordS, record.surname);
            strcat(recordS, "$");
            strcat(recordS, record.streetName);
            strcat(recordS, "$");
            strcat(recordS, int1S);
            strcat(recordS, "$");
            strcat(recordS, record.cityName);
            strcat(recordS, "$");
            strcat(recordS, record.postCode);
            strcat(recordS, "$");
            strcat(recordS, floatS);
            strcat(recordS, "&");  // end of record

            // write to parent's named pipe
            write(parentPipeDesc, recordS, /*strlen(recordS) + 1*/ MAX_STRING_RECORD_SIZE);
        }

        recordsRemaining--;
        curRecordNum++;
    }
    gettimeofday(&endTime, NULL);

    // write metadata to parent's named pipe
    char metadata[MAX_STRING_METADATA_SIZE];  // pid + del + timeElapsed + end of metadata + end of metadata delimiter

    strcpy(metadata, "ml");  // "m" indicates metadata, "l" indicates leaf

    sprintf(int1S, "%d", getpid());
    strcat(metadata, int1S);
    strcat(metadata, "$");

    sprintf(int1S, "%lu", endTime.tv_usec - startTime.tv_usec);
    strcat(metadata, int1S);
    strcat(metadata, "&");

    write(parentPipeDesc, metadata, MAX_STRING_METADATA_SIZE);
    close(parentPipeDesc);

    fclose(fp);

    kill(rootPid, SIGUSR2);
    return;
}

char splitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern,
                           unsigned int remainingTreeDepth, char* parentPipeName, char isFirstSplitterMerger, char skewFlag,
                           unsigned int recordsNum, unsigned int leafsNum, unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd, int rootPid) {
    char child1PipeName[20], child2PipeName[20], pidS[MAX_STRING_INT_SIZE];
    strcpy(child1PipeName, "tmp/child1_");
    sprintf(pidS, "%d", getpid());
    strcat(child1PipeName, pidS);

    strcpy(child2PipeName, "tmp/child2_");
    sprintf(pidS, "%d", getpid());
    strcat(child2PipeName, pidS);

    struct timeval startTime, endTime;
    pid_t pid1 = -1, pid2 = -1;  // initialize for splitting code afterwards
    unsigned int child1RangeStart, child1RangeEnd, child2RangeStart, child2RangeEnd, child1LeafIndexRangeStart, child1LeafIndexRangeEnd,
        child2LeafIndexRangeStart, child2LeafIndexRangeEnd;

    gettimeofday(&startTime, NULL);

    child1RangeStart = searchRangeStart;
    child2RangeEnd = searchRangeEnd;

    unsigned int curRecordsNum = searchRangeEnd - searchRangeStart + 1;
    if (skewFlag == 0) {
        child1RangeEnd = searchRangeStart + (curRecordsNum / 2) - 1;
        child2RangeStart = searchRangeStart + (curRecordsNum / 2);
    } else {
        if (remainingTreeDepth > 1) {
            unsigned int curLeafsNum = leafIndexRangeEnd - leafIndexRangeStart + 1;

            child1LeafIndexRangeStart = leafIndexRangeStart;
            child2LeafIndexRangeEnd = leafIndexRangeEnd;

            child1LeafIndexRangeEnd = leafIndexRangeStart + (curLeafsNum / 2) - 1;
            child2LeafIndexRangeStart = leafIndexRangeStart + (curLeafsNum / 2);
        }
    }

    // create the 2 children's named pipes
    mkfifo(child1PipeName, 0666);
    mkfifo(child2PipeName, 0666);

    int parentPipeDesc;
    if (remainingTreeDepth > 1) {
        pid1 = fork();
        if (pid1 < 0) {
            fprintf(stderr, "fork failed");
            return 1;
        }

        if (pid1 > 0) {
            printf("Forked splitter/merger with pid %d\n", pid1);
            pid2 = fork();
            if (pid2 < 0) {
                fprintf(stderr, "fork failed");
                return 1;
            }
            if (pid2 > 0)
                printf("Forked spitter/merger with pid %d\n", pid2);
        }
        if (pid1 == 0) {
            execSplitterMergerNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, remainingTreeDepth - 1, child1PipeName, 0, skewFlag,
                                      recordsNum, leafsNum, child1LeafIndexRangeStart, child1LeafIndexRangeEnd, rootPid);
        }
        if (pid2 == 0) {
            execSplitterMergerNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, remainingTreeDepth - 1, child2PipeName, 0, skewFlag,
                                      recordsNum, leafsNum, child2LeafIndexRangeStart, child2LeafIndexRangeEnd, rootPid);
        }

        // parent process
        if (pid1 > 0 && pid2 > 0) {
            parentPipeDesc = open(parentPipeName, O_WRONLY); // open parent's named pipe
            readAndSendResultsOfChild(child1PipeName, parentPipeDesc, pid1);
            readAndSendResultsOfChild(child2PipeName, parentPipeDesc, pid2);
        }
    } else if (remainingTreeDepth == 1) {
        pid1 = fork();
        if (pid1 < 0) {
            fprintf(stderr, "fork failed");
            return 1;
        }
        if (pid1 > 0) {
            printf("Forked leaf with pid %d\n", pid1);
            pid2 = fork();
            if (pid2 < 0) {
                fprintf(stderr, "fork failed");
                return 1;
            }
            if (pid2 > 0)
                printf("Forked leaf with pid %d\n", pid2);
        }

        // child process 1
        if (pid1 == 0) {
            execLeafNodeJob(dataFileName, child1RangeStart, child1RangeEnd, searchPattern, child1PipeName, skewFlag, leafIndexRangeStart, recordsNum, leafsNum, rootPid);
        }
        // child process 2
        if (pid2 == 0) {
            execLeafNodeJob(dataFileName, child2RangeStart, child2RangeEnd, searchPattern, child2PipeName, skewFlag, leafIndexRangeEnd, recordsNum, leafsNum, rootPid);
        }

        // parent process
        if (pid1 > 0 && pid2 > 0) {
            parentPipeDesc = open(parentPipeName, O_WRONLY);
            readAndSendResultsOfChild(child1PipeName, parentPipeDesc, pid1);
            readAndSendResultsOfChild(child2PipeName, parentPipeDesc, pid2);
        }
    }

    if (pid1 > 0 && pid2 > 0) {
        gettimeofday(&endTime, NULL);

        char metadata[MAX_STRING_METADATA_SIZE];

        strcpy(metadata, "ms");  // "m" indicates metadata, "s" indicates splitter/merger

        char intS[MAX_STRING_INT_SIZE];
        sprintf(intS, "%d", getpid());
        strcat(metadata, intS);
        strcat(metadata, "$");

        char uintS[MAX_STRING_INT_SIZE];
        sprintf(uintS, "%lu", endTime.tv_usec - startTime.tv_usec);
        strcat(metadata, uintS);
        strcat(metadata, "&");

        write(parentPipeDesc, metadata, MAX_STRING_METADATA_SIZE);

        if (isFirstSplitterMerger)
            write(parentPipeDesc, "end", 4);

        // close named pipe and unlink it since we don't need it anymore
        close(parentPipeDesc);
        unlink(parentPipeName);

        // wait for both children to exit
        pid1 = wait(NULL);
        if (remainingTreeDepth == 1)
            printf("Leaf ");
        else
            printf("Splitter/Merger ");

        printf("with pid %d exited succesfully\n", pid1);
        pid2 = wait(NULL);
        if (remainingTreeDepth == 1)
            printf("Leaf ");
        else
            printf("Splitter/Merger ");

        printf("with pid %d exited succesfully\n", pid2);
    }

    return 0;
}

void sortNodeJob() {
    char* args[] = {"sort", "results.txt", NULL};
    execvp(args[0], args);
}

unsigned int rootSignalsNum = 0;

// signal handler
void handleSIGUSR2(int sig) {
    rootSignalsNum++;
}

char rootNodeJob(unsigned int height, char* dataFileName, char* searchPattern, char skewFlag) {
    pid_t pid;
    char childPipeName[] = "tmp/rootChild";
    long lSize;
    unsigned int recordsNum;
    FILE* inputFileP = fopen(dataFileName, "rb");
    signal(SIGUSR2, handleSIGUSR2);
    // printf("Waiting for 1 second to let signal() function setup\n");
    // sleep(1);

    // check number of records
    fseek(inputFileP, 0, SEEK_END);
    lSize = ftell(inputFileP);
    rewind(inputFileP);
    recordsNum = (int)lSize / sizeof(Record);

    unsigned int leafIndexRangeStart, leafIndexRangeEnd;
    unsigned int leafsNum;

    if (skewFlag == 1) {
        leafsNum = (unsigned int)pow((double)2, (double)height);
        leafIndexRangeStart = 1;
        leafIndexRangeEnd = leafsNum;
    }

    fclose(inputFileP);

    if (recordsNum == 0)
        printf("0 records in file. Exiting...\n");

    mkfifo(childPipeName, 0666);

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    } else if (pid == 0) {
        if (skewFlag == 0)
            execSplitterMergerNodeJob(dataFileName, 1, recordsNum, searchPattern, height, childPipeName, 1, skewFlag, 0, 0, 0, 0, getppid());
        else
            execSplitterMergerNodeJob(dataFileName, 1, recordsNum, searchPattern, height, childPipeName, 1, skewFlag, recordsNum, leafsNum, leafIndexRangeStart, leafIndexRangeEnd, getppid());
    } else {
        FILE* outFileP;
        char recordS[MAX_STRING_RECORD_SIZE];
        int readReturn;

        unsigned int size = (int)pow(2, height + 1) - 2;
        int pids[size];
        long unsigned int times[size], leafNodesTotalTime = 0, splitterMergerNodesTotalTime = 0;
        char isLeaf[size];
        long long unsigned int programTotalTime = 0;
        unsigned int splitterMergerNodesNum = pow(2, height + 1) - 1 - pow(2, height);
        long double averageLeafNodesTime, averageSplitterMergerNodesTime;
        unsigned int index = 0;
        char *token, *ptr;

        outFileP = fopen("results.txt", "w");
        int childFileDesc = open(childPipeName, O_RDONLY);
        readReturn = read(childFileDesc, recordS, MAX_STRING_RECORD_SIZE);

        while (strcmp(recordS, "end") != 0 && readReturn > 0) {
            if (recordS[0] == 'm') {
                // handle metadata //

                isLeaf[index] = 0;
                if (recordS[1] == 'l')
                    isLeaf[index] = 1;

                // skip the two first letters
                for (unsigned int i = 0; i < MAX_STRING_RECORD_SIZE; i++) {
                    if (recordS[i + 2] != '\0')
                        recordS[i] = recordS[i + 2];
                    else {
                        recordS[i] = '\0';
                        recordS[i + 1] = '\0';
                        break;
                    }
                }

                // save metadata
                token = strtok(recordS, "$");
                pids[index] = atoi(token);

                token = strtok(NULL, "&");
                times[index] = strtol(token, &ptr, 10);

                if (isLeaf[index])
                    leafNodesTotalTime += times[index];
                else
                    splitterMergerNodesTotalTime += times[index];

                index++;
            } else
                writeRecordToFile(recordS, outFileP);

            // strcpy(recordS, ""); // flush string
            readReturn = read(childFileDesc, recordS, MAX_STRING_RECORD_SIZE);
        }
        close(childFileDesc);
        unlink(childPipeName);
        fclose(outFileP);

        // wait for child to exit
        pid = wait(NULL);
        printf("First splitter/merger with pid %d exited succesfully\n", pid);

        for (unsigned int i = 0; i < index; i++)
            programTotalTime += times[i];

        averageLeafNodesTime = (long double)leafNodesTotalTime / leafsNum;
        averageSplitterMergerNodesTime = (long double)splitterMergerNodesTotalTime / splitterMergerNodesNum;

        printf("\n\nStatistics:\n\n");

        for (unsigned int i = 0; i < index; i++) {
            if (isLeaf[i])
                printf("Leaf ");
            else
                printf("Splitter/Merger ");

            printf("with pid %d took %lu microseconds to complete.\n", pids[i], times[i]);
        }

        printf("\nTotal program time: %llu microseconds\nTotal time of leaf nodes: %lu microseconds\nTotal time of splitter/merger nodes: %lu microseconds\nAverage time of leaf nodes: %Lf microseconds\nAverage time of splitter/merger nodes: %Lf microseconds\n",
               programTotalTime, leafNodesTotalTime, splitterMergerNodesTotalTime, averageLeafNodesTime, averageSplitterMergerNodesTime);

        printf("Total number of SIGUSR2 signals: %u\n\n\n", rootSignalsNum);

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork failed");
            return 1;
        } else if (pid == 0) {
            sortNodeJob();
        } else {
            // wait for Sort node to exit
            pid = wait(NULL);
            printf("\nSort node with pid: %d exited succesfully\n", pid);
        }

        // remove temporary named pipe files
        char command[] = "rm -rf tmp/*";
        system(command);
    }

    return 0;
}