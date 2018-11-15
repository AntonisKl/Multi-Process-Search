#include "nodes.h"

int main(int argc, char** argv) {

    int pipeFileDesc[2] = {atoi(argv[6]), atoi(argv[7])};
    splitterMergerNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), pipeFileDesc, argv[8][0], argv[9][0]);
    printf("Splitter/Merger node job executed.\n");

    return 0;
}