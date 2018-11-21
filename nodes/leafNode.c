#include "nodes.h"

int main(int argc, char** argv) {
    char skewFlag = argv[6][0];

    if (skewFlag == 0)
        leafNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], argv[5], skewFlag, 0, 0, 0, atoi(argv[7]));
    else
        leafNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], argv[5], skewFlag, atoi(argv[7]), atoi(argv[8]), atoi(argv[9]), atoi(argv[10]));

    return 0;
}