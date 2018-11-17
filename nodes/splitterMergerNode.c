#include "nodes.h"

int main(int argc, char** argv) {
    int pipeFileDesc[2] = {atoi(argv[6]), atoi(argv[7])};
    char skewFlag = argv[9][0];
    printf("Creating splitter/merger node...\n");
    if (skewFlag == 0)
        splitterMergerNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), pipeFileDesc, argv[8][0], skewFlag, 0, 0, 0, 0);
    else
        splitterMergerNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), pipeFileDesc, argv[8][0], skewFlag,
                              atoi(argv[10]), atoi(argv[11]), atoi(argv[12]), atoi(argv[13]));


    return 0;
}