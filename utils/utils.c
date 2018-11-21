#include "utils.h"

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

void writeRecordToFile(char* recordS, FILE* fileP) {
    const char endOfField[2] = "$";
    const char endOfRecord[2] = "&";
    char* token;

    token = strtok(recordS, endOfField);
    fputs(token, fileP);
    fputs(",", fileP);

    for (unsigned int i = 0; i < 6; i++) {
        token = strtok(NULL, endOfField);
        fputs(token, fileP);
        fputs(",", fileP);
    }

    token = strtok(NULL, endOfRecord);
    fputs(token, fileP);

    fputs("\n", fileP);
    fflush(fileP);
    return;
}

void stringToRecord(char* s, Record* record) {
    char* ptr;
    const char endOfField[2] = "$";
    const char endOfRecord[2] = "&";
    char* token;

    token = strtok(s, endOfField);
    record->AM = (int)strtol(token, &ptr, 10);

    token = strtok(NULL, endOfField);
    strcpy(record->name, token);

    token = strtok(NULL, endOfField);
    strcpy(record->surname, token);

    token = strtok(NULL, endOfField);
    strcpy(record->streetName, token);

    token = strtok(NULL, endOfField);
    record->houseNumber = (int)strtol(token, &ptr, 10);

    token = strtok(NULL, endOfField);
    strcpy(record->cityName, token);

    token = strtok(NULL, endOfField);
    strcpy(record->postCode, token);

    token = strtok(NULL, endOfRecord);
    record->salary = strtof(token, &ptr);

    return;
}

char readRecordFromFile(FILE* fp, unsigned int recordNum, Record* record) {
    unsigned int recordsToSkip = recordNum - 1;

    fseek(fp, (recordsToSkip * sizeof(Record)), SEEK_SET);

    fread(record, sizeof(Record), 1, fp);
    return 0;
}

void readAndSendResultsOfChild(char* childPipeName, int parentPipeDesc, pid_t childPid) {
    char recordS[MAX_STRING_RECORD_SIZE];

    int childPipeDesc = open(childPipeName, O_RDONLY);

    int readReturn;
    readReturn = read(childPipeDesc, recordS, MAX_STRING_RECORD_SIZE);

    while (readReturn != EOF && readReturn > 0) {
        write(parentPipeDesc, recordS, MAX_STRING_RECORD_SIZE);
        readReturn = read(childPipeDesc, recordS, MAX_STRING_RECORD_SIZE);
    }

    close(childPipeDesc);
    unlink(childPipeName);
}