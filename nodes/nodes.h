#ifndef NODES_H
#define NODES_H

#include "../utils/utils.h"

// calls exec with the needed arguments to execute Splitter/Merger node's job
void execSplitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, unsigned int remainingTreeDepth,
                               char* childPipeName, char isFirstSplitterMerger, char skewFlag, unsigned int recordsNum, unsigned int leafsNum,
                               unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd, int rootPid);

// calls exec with the needed arguments to execute Leaf node's job
void execLeafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* childPipeName,
                     char skewFlag, unsigned int leafIndex, unsigned int recordsNum, unsigned int leafsNum, int rootPid);

// does Leaf node's job
// searchRangeStart and searchRangeEnd are used only if skewFlag = 0
// if skewFlag = 1: leafIndex, recordsNum and leafsNum are used to calculate the search range of the current Leaf node
void leafNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern, char* parentPipeName,
                 char skewFlag, unsigned int leafIndex, unsigned int recordsNum, unsigned int leafsNum, int rootPid);

// does Splitter/Merger node's job
// searchRangeStart and searchRangeEnd are used only if skewFlag = 0
// if skewFlag = 1: leafIndexRangeStart and leafIndexRangeEnd are handled to be passed to children in order for the Leaf nodes to know which index they have in the tree
// rootPid: pid of Root node
char splitterMergerNodeJob(char* dataFileName, unsigned int searchRangeStart, unsigned int searchRangeEnd, char* searchPattern,
                           unsigned int remainingTreeDepth, char* parentPipeName, char isFirstSplitterMerger, char skewFlag,
                           unsigned int recordsNum, unsigned int leafsNum, unsigned int leafIndexRangeStart, unsigned int leafIndexRangeEnd, int rootPid);

// does Sort node's job
void sortNodeJob();

// does Root node's job
char rootNodeJob(unsigned int height, char* dataFileName, char* searchPattern, char skewFlag);

#endif