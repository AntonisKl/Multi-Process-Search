#include "nodes.h"

int main(int argc, char** argv) {
    int pipeFileDesc[2] = {atoi(argv[5]), atoi(argv[6])};
    char skewFlag = argv[7][0];

    if (skewFlag == 0)
        leafNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], pipeFileDesc, skewFlag, 0, 0, 0);
    else
        leafNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], pipeFileDesc, skewFlag, atoi(argv[8]), atoi(argv[9]), atoi(argv[10]));

    printf("Leaf node's job executed.\n");
    _exit(0);

    // return 0;
}