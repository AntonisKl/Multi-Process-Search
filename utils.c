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

void read_from_pipe(int file) {
    FILE* stream;
    int c;
    stream = fdopen(file, "r");
    while ((c = fgetc(stream)) != EOF)
        putchar(c);
    fclose(stream);
}

/* Write some random text to the pipe. */

void write_to_pipe(int file) {
    FILE* stream;
    stream = fdopen(file, "w");
    fprintf(stream, "hello, world!\n");
    fprintf(stream, "goodbye, world!\n");
    fclose(stream);
}

char stringFromFileToEntry(char* s, Entry* entry) {
    char* ptr;
    char intS[5];
    memcpy(intS, s, 4);

    entry->AM = (int)strtol(intS, &ptr, 10);
    s += 4;
    if (memcpy(entry->name, s, 15) == NULL)
        return 1;
    s += 15;
    if (memcpy(entry->surname, s, 25) == NULL)
        return 1;
    s += 25;

    char floatS[5];
    memcpy(floatS, s, 4);

    entry->salary = strtof(floatS, &ptr);

    return 0;
}

void stringToEntry(char* s, Entry* entry) {
    char* ptr;
    const char endOfField[2] = "$";
    const char endOfEntry[2] = "&";
    char* token;

    token = strtok(s, endOfField);
    entry->AM = (int)strtol(token, &ptr, 10);

    token = strtok(NULL, endOfField);
    strcpy(entry->name, token);

    token = strtok(NULL, endOfField);
    strcpy(entry->surname, token);

    token = strtok(NULL, endOfEntry);
    entry->salary = strtof(token, &ptr);

    return;
}

char entryToString(Entry entry, char s[73]) {
    char intS[12];
    char floatS[20];
    if (sprintf(intS, "%d", entry.AM) < 0)
        return 1;
    if (sprintf(floatS, "%f", entry.salary) < 0)
        return 1;

    strcat(s, intS);
    strcat(s, "$");  // end of field
    strcat(s, entry.name);
    strcat(s, "$");
    strcat(s, entry.surname);
    strcat(s, "$");
    strcat(s, floatS);
    strcat(s, "&");  // end of entry
}

char readEntryFromFile(FILE* fp, unsigned int entryNum, Entry* entry) {
    // entry num = 5 -> read from position 5*sizeof(Entry)
    unsigned int entriesToSkip = entryNum - 1;

    fseek(fp, (entriesToSkip * sizeof(Entry)), SEEK_SET);

    unsigned char entryS[sizeof(Entry) + 1];

    fread(entryS, sizeof(entryS), 1, fp);

    return stringFromFileToEntry(entryS, entry);
}