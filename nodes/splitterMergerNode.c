#include "nodes.h"

int main(int argc, char** argv) {
    char skewFlag = argv[8][0];

    if (skewFlag == 0)
        splitterMergerNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), argv[6], argv[7][0], skewFlag, 0, 0, 0, 0, atoi(argv[9]));
    else
        splitterMergerNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), argv[6], argv[7][0], skewFlag,
                              atoi(argv[9]), atoi(argv[10]), atoi(argv[11]), atoi(argv[12]), atoi(argv[13]));

    return 0;
}