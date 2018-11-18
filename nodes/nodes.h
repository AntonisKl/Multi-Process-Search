#ifndef NODES_H
#define NODES_H

#include "../utils/utils.h"

void execSplitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, unsigned int remainingTreeDepth,
                               char* childPipeName, char isFirstSplitterMerger, char skewFlag, unsigned int entriesNum, unsigned int leafsNum,
                               unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd);

void execLeafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* childPipeName,
                     char skewFlag, unsigned int leafIndex, unsigned int entriesNum, unsigned int leafsNum);

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* parentPipeName,
                 char skewFlag, unsigned int leafIndex, unsigned int entriesNum, unsigned int leafsNum);

char splitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern,
                           unsigned int remainingTreeDepth, char* parentPipeName, char isFirstSplitterMerger, char skewFlag,
                           unsigned int entriesNum, unsigned int leafsNum, unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd);

void sortNodeJob();

char rootNodeJob(unsigned int height, char* dataFileName, char* searchPattern, char skewFlag);

#endif