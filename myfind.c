#include "nodes/nodes.h"

int main(int argc, char** argv) {
    unsigned int height;
    char *dataFileName, *searchPattern, skewFlag, heightS[MAX_STRING_INT_SIZE], skewFlagS[2];

    handleFlags(argc, argv, &height, &dataFileName, &searchPattern, &skewFlag);

    printf("\nArguments handled: File name->%s, search pattern->%s, height of tree->%u, skewFlag->%d\n\n", dataFileName, searchPattern, height, skewFlag);

    sprintf(heightS, "%u", height);
    sprintf(skewFlagS, "%c", skewFlag);
    
    char* args[] = {"./executables/rootNode", heightS, dataFileName, searchPattern, skewFlagS, NULL};
    execvp(args[0], args);

    return 0;
}