#ifndef NODES_H
#define NODES_H

#include "utils.h"

void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, int parentPipeDesc[2]);
char splitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern,
                           unsigned int remainingTreeDepth, int parentPipeDesc[2], char isFirstSplitterMerger, char skewFlag);

void sortNodeJob();

char rootNodeJob(int argc, char** argv, unsigned int* height, char** dataFileName, char** searchPattern, char* skewFlag);
#endif