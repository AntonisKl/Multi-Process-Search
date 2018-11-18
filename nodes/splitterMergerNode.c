#include "nodes.h"

int main(int argc, char** argv) {
    // int pipeFileDesc[2] = {atoi(argv[6]), atoi(argv[7])};
    char skewFlag = argv[8][0];
    printf("Creating splitter/merger node...\n");
    if (skewFlag == 0)
        splitterMergerNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), argv[6], argv[7][0], skewFlag, 0, 0, 0, 0);
    else
        splitterMergerNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), argv[6], argv[7][0], skewFlag,
                              atoi(argv[9]), atoi(argv[10]), atoi(argv[11]), atoi(argv[12]));


    return 0;
}