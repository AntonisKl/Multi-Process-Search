#include "nodes.h"

int main(int argc, char** argv) {
    // char* args[] = {"./executables/rootNode", NULL};
    rootNodeJob(atoi(argv[1]), argv[2], argv[3], argv[4][0]);

    printf("Root node job executed.\n");
    return 0;
}