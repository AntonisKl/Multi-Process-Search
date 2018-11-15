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

// char stringFromFileToEntry(char* s, Entry* entry) {
//     char* ptr;
//     char intS[5];
//     memcpy(intS, s, 4);

//     entry->AM = (int)strtol(intS, &ptr, 10);
//     s += 4;
//     if (memcpy(entry->name, s, 15) == NULL)
//         return 1;
//     s += 15;
//     if (memcpy(entry->surname, s, 25) == NULL)
//         return 1;
//     s += 25;

//     char floatS[5];
//     memcpy(floatS, s, 4);

//     entry->salary = strtof(floatS, &ptr);

//     return 0;
// }

void writeEntryToFile(char* entryS, FILE* fileP) {
    // char* ptr;
    const char endOfField[2] = "$";
    // const char endOfEntry[2] = "&";
    char* token;

    token = strtok(entryS, endOfField);
    fputs(token, fileP);
    fputs(",", fileP);

    for (unsigned int i = 0; i < 8; i++) {
        token = strtok(NULL, endOfField);
        fputs(token, fileP);
        fputs(",", fileP);
    }
    fputs("\n", fileP);

    return;
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

    token = strtok(NULL, endOfField);
    strcpy(entry->streetName, token);

    token = strtok(NULL, endOfField);
    entry->houseNumber = (int)strtol(token, &ptr, 10);

    token = strtok(NULL, endOfField);
    strcpy(entry->cityName, token);

    token = strtok(NULL, endOfField);
    strcpy(entry->postCode, token);

    token = strtok(NULL, endOfEntry);
    entry->salary = strtof(token, &ptr);

    return;
}

char entryToString(Entry entry, char s[73]) {
    char int1S[MAX_STRING_INT_SIZE], int2S[MAX_STRING_INT_SIZE];
    char floatS[MAX_STRING_FLOAT_SIZE];
    if (sprintf(int1S, "%d", entry.AM) < 0)
        return 1;
    if (sprintf(floatS, "%f", entry.salary) < 0)
        return 1;

    if (sprintf(int2S, "%d", entry.houseNumber < 0))
        return 1;

    strcpy(s, int1S);
    strcat(s, "$");  // end of field
    strcat(s, entry.name);
    strcat(s, "$");
    strcat(s, entry.surname);
    strcat(s, "$");
    strcat(s, entry.streetName);
    strcat(s, "$");
    strcat(s, int2S);
    strcat(s, "$");
    strcat(s, entry.cityName);
    strcat(s, "$");
    strcat(s, entry.postCode);
    strcat(s, "$");
    strcat(s, floatS);
    strcat(s, "&");  // end of entry

    return 0;
}

char readEntryFromFile(FILE* fp, unsigned int entryNum, Entry* entry) {
    // entry num = 5 -> read from position 5*sizeof(Entry)
    unsigned int entriesToSkip = entryNum - 1;

    fseek(fp, (entriesToSkip * sizeof(Entry)), SEEK_SET);

    // unsigned char entryS[sizeof(Entry) + 1];

    if (fread(&entry, sizeof(entry), 1, fp) == sizeof(entry)) {
        printf("Read entry: AM->%d, name->%s, surname->%s, streetName->%s, houseNumber->%d, cityName->%s, postCode->%s, salary->%f\n",
               entry->AM, entry->name, entry->surname, entry->streetName, entry->houseNumber, entry->cityName, entry->postCode, entry->salary);
        return 0;
    } else
        return 1;

    // return stringFromFileToEntry(entryS, entry);
}

void readAndSendResultsOfChild(int childFileDesc[2], int parentPipeDesc[2], pid_t childPid) {
    char entryS[MAX_STRING_ENTRY_SIZE];
    char metadata[MAX_STRING_METADATA_SIZE];

    // close(childFileDesc[0]);  // Close reading end of first pipe

    // // Write input string and close writing end of first
    // // pipe.
    // write(fd1[1], input_str, strlen(input_str)+1);
    // close(fd1[1]);

    // // Wait for child to send a string
    waitpid(childPid, NULL, 0);
    printf("Child with pid %d exited, so this process with pid %d can now read from the pipe", childPid, getpid());

    close(childFileDesc[1]);  // Close writing end of second pipe

    // Read string from child, print it and close
    // reading end.
    read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
    // printf("Concatenated string %s\n", concat_str);
    close(childFileDesc[0]);

    while (strcmp(entryS, "m") != 0 /*&& strcmp(entryS, "no_result") != 0*/) {
        // we got an entry !

        close(parentPipeDesc[0]);
        write(parentPipeDesc[1], entryS, /*strlen(entryS) + 1*/ MAX_STRING_ENTRY_SIZE);
        close(parentPipeDesc[1]);

        // read from child
        close(childFileDesc[1]);  // Close writing end of second pipe
        read(childFileDesc[0], entryS, MAX_STRING_ENTRY_SIZE);
        // printf("Concatenated string %s\n", concat_str);
        close(childFileDesc[0]);
    }

    // if (strcmp(entryS, "no_result") == 0) {
    //     close(parentPipeDesc[0]);
    //     write(parentPipeDesc[1], "no_result", /*strlen(entryS) + 1*/ 10);
    //     close(parentPipeDesc[1]);
    //     return;
    // } else {
    close(parentPipeDesc[0]);
    write(parentPipeDesc[1], "m", /*strlen(entryS) + 1*/ 2);
    close(parentPipeDesc[1]);

    // get metadata and send to parent
    // read from child
    close(childFileDesc[1]);  // Close writing end of second pipe
    read(childFileDesc[0], metadata, MAX_STRING_METADATA_SIZE);
    // printf("Concatenated string %s\n", concat_str);
    close(childFileDesc[0]);

    close(parentPipeDesc[0]);
    write(parentPipeDesc[1], metadata, /*strlen(entryS) + 1*/ MAX_STRING_METADATA_SIZE);
    close(parentPipeDesc[1]);
    // }
}