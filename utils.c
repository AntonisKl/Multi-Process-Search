#include "utils.h"

// // Function to remove all spaces from a given string
// void removeSpaces(char* str) {
//     // To keep track of non-space character count
//     if (str == NULL)
//         return;

//     int count = 0;
//     int stringSize = strlen(str);

//     // Traverse the given string. If current character
//     // is not space, then place it at index 'count++'
//     for (int i = 0; i < stringSize; i++) {
//         if (str[i] != ' ')
//             str[count++] = str[i];  // here count is
//                                     // incremented
//     }
//     str[count] = '\0';
//     return;
// }

void handleFlags(int argc, char** argv, unsigned int* height, char** dataFileName, char** pattern, char* skewFlag) {
    if (argc == 7 || argc == 8) {
        if (strcmp(argv[1], "-h") == 0) {
            int heightArg = atoi(argv[2]);
            if (heightArg < 1 || heightArg > 5) {
                printf("Invalid height parameter\nExiting...\n");
                exit(1);
            }

            (*height) = (unsigned int)heightArg;
        } else {
            printf("Invalid flags\nExiting...\n");
            exit(1);
        }

        if (strcmp(argv[3], "-d") == 0) {
            (*dataFileName) = argv[4];
        } else {
            printf("Invalid flags\nExiting...\n");
            exit(1);
        }

        if (strcmp(argv[5], "-p") == 0) {
            (*pattern) = argv[6];
        } else {
            printf("Invalid flags\nExiting...\n");
            exit(1);
        }

        if (argc == 8) {
            if (strcmp(argv[7], "-s") == 0) {
                (*skewFlag) = 1;
            } else {
                printf("Invalid flags\nExiting...\n");
                exit(1);
            }
        } else
            (*skewFlag) = 0;
    } else {
        printf("Invalid flags\nExiting...\n");
        exit(1);
    }
}

char stringToEntry(char* s, Entry* entry) {
    char intS[5];
    memcpy(intS, s, 4);

    entry->AM = atoi(intS);
    s+=4;
    if (memcpy(entry->name, s, 15) == NULL)
        return 1;
    s += 15;
    if (memcpy(entry->surname, s, 25) == NULL)
        return 1;
    s += 25;

    char floatS[5];
    memcpy(floatS, s, 4);

    entry->salary = atof(floatS);

    return 0;
}

char readEntryFromFile(FILE* fp, unsigned int entryNum, Entry* entry) {
    // entry num = 5 -> read from position 5*sizeof(Entry)

    fseek(fp, (entryNum * sizeof(Entry)) /* - 1*/, SEEK_SET);

    char* entryS = malloc(sizeof(Entry) + 1);

    fgets(entryS, sizeof(Entry) + 1, fp);

    return stringToEntry(entryS, entry);
}