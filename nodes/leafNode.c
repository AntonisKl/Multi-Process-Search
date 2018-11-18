#include "nodes.h"

int main(int argc, char** argv) {
    // int pipeFileDesc[2] = {atoi(argv[5]), atoi(argv[6])};
    char skewFlag = argv[6][0];

    if (skewFlag == 0)
        leafNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], argv[5], skewFlag, 0, 0, 0);
    else
        leafNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], argv[5], skewFlag, atoi(argv[7]), atoi(argv[8]), atoi(argv[9]));

    printf("Leaf node's job executed.\n");
    // _exit(0);
    return 0;
    // return 0;
}