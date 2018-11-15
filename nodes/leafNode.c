#include "nodes.h"

int main(int argc, char** argv) {

    int pipeFileDesc[2] = {atoi(argv[5]), atoi(argv[6])};
    leafNodeJob(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], pipeFileDesc);
    printf("Leaf node job executed.\n");

    return 0;
}