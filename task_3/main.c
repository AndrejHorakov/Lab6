#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void searchInFile(const char *filename, const char *targetBytes) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    int totalBytesRead = 0;

    size_t byteArrayLength = strlen(targetBytes);

    char buffer[byteArrayLength];

     while (!feof(file)) {
        int bytesRead = fread(buffer, 1, sizeof(buffer), file);
        totalBytesRead += bytesRead;

        if (bytesRead > 0) {
            char *found = strstr(buffer, targetBytes);
            if (found != NULL) {
                printf("PID: %d, Filename: %s, Total Bytes Read: %d, Result: Match found: %s\n", getpid(), filename, totalBytesRead, buffer);
                
                if (fclose(file) == EOF) {
                    printf("Error when closing file\n");
                    return;
                }
                return;
            }
        }
    }
    
    printf("PID: %d, Filename: %s, Total Bytes Read: %d, Result: Match not found\n", getpid(), filename, totalBytesRead);
    
     if (fclose(file) == EOF) {
        printf("Error when closing file\n");
        return;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Invalid args count!\n");
        printf("<directory_name> <max_processes> <target bytes>\n");
        return 1;
    }

    char *directoryName = argv[1];
    int maxProcesses = atoi(argv[2]);
    int currentProcesses = 0;
    char *combination = argv[3];
    printf("%s\n", combination);
    DIR *dir = opendir(directoryName);

    if (dir == NULL) {
        printf("Error opening directory");
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { 
            if (currentProcesses < maxProcesses) {

                char filename[1024];
                snprintf(filename, sizeof(filename), "%s/%s", directoryName, entry->d_name);

                pid_t pid = fork(); 

                if (pid == 0) { 
                    searchInFile(filename, combination);
                    exit(0);
                } else if (pid < 0) {
                    perror("fork failed");
                } else {
                    currentProcesses++; 
                }
            } else {
                wait(NULL);
                currentProcesses--;
            }
        }
    }

    closedir(dir);

    while (currentProcesses > 0) {
        wait(NULL);
        currentProcesses--;
    }

    return 0;
}
