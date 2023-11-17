#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define NUM_CHARS 26
#define BARLENGTH 100

char *fileList[MAXNAMLEN];

int file_count(char *direct) {
    int result = 0;
    DIR *dir;
    struct dirent *entry;
    dir = opendir(direct);

    if(dir != NULL) {
        while((entry = readdir(dir)) != NULL) {
            if (entry -> d_type == DT_REG) {
                result ++;
                char *file_name = malloc(sizeof(char) * (strlen(entry->d_name) + strlen(direct)) + 1);
                strcpy(file_name, direct);
                strcat(file_name, entry -> d_name);
                fileList[result - 1] = file_name;
            }
        }
        closedir(dir);
    }

    printf("File total count: %d \n", result);
    for (int i = 0; i < result; i++) {
        printf("%dth file : %s \n", i + 1, fileList[i]);
    }
    return result;
}

int main() {
    char directory_name[MAXNAMLEN];
    printf("Enter directory : "); scanf("%s", directory_name);
    int num_files = file_count(directory_name);

    int pipes[num_files][2];

    for (int i = 0; i < num_files; i++) {
        pipe(pipes[i]);
    }

    pid_t pid;
    for (int i = 0; i < num_files; i++) {

        pid = fork();
        if (pid == 0) {
            if (i == 0) {
                int occupancy[NUM_CHARS] = {0};
//                for (int j = 0; j < num_files; j++) {
                    FILE *file = fopen(fileList[i], "r");
                    int c;
                    while ((c = fgetc(file)) != EOF) {
                        occupancy[tolower(c) - 'a']++;
                    }
                    fclose(file);
//                }
                for (int j = 0; j < NUM_CHARS; j++) {
                    write(pipes[i][1], &occupancy[j], sizeof(int));
                }


                int max_count = occupancy[0];
                for (int j = 0; j < NUM_CHARS; j++) {
                    if (max_count < occupancy[j]) {
                        max_count = occupancy[j];
                    }
                }

                for (int j = 0; j < NUM_CHARS; j++) {
                    printf("Process %d --- %c: %.4d : ", i + 1, j + 'a', occupancy[j]);
                    int star_count = occupancy[j] * BARLENGTH / max_count;
                    for (int k = 0; k < star_count; k++) {
                        printf("*");
                    }
                    printf("\n");
                }
            } else {
                int occupancy[NUM_CHARS];
                for (int j = 0; j < NUM_CHARS; j++) {
                    read(pipes[i - 1][0], &occupancy[j], sizeof(int));
                }
//                for (int j = 0; j < NUM_CHARS; j++) {
                    FILE *file = fopen(fileList[i], "r");
                    int c;
                    while ((c = fgetc(file)) != EOF) {
                        occupancy[tolower(c) - 'a']++;
                    }
                    fclose(file);
//                }
                for (int j = 0; j < NUM_CHARS; j++) {
                    write(pipes[i][1], &occupancy[j], sizeof(int));
                }


                int max_count = occupancy[0];
                for (int j = 0; j < NUM_CHARS; j++) {
                    if (max_count < occupancy[j]) {
                        max_count = occupancy[j];
                    }
                }

                for (int j = 0; j < NUM_CHARS; j++) {
                    printf("Process %d --- %c: %.4d : ", i + 1, j + 'a', occupancy[j]);
                    int star_count = occupancy[j] * BARLENGTH / max_count;
                    for (int k = 0; k < star_count; k++) {
                        printf("*");
                    }
                    printf("\n");
                }
            }
            exit(0);
        }
    }

    for (int i = 0; i < num_files - 1; i++) {
        waitpid(pid, NULL, 0);
    }

    int occupancy[NUM_CHARS];
    for (int j = 0; j < NUM_CHARS; j++) {
        read(pipes[num_files - 1][0], &occupancy[j], sizeof(int));
    }

    int max_count = occupancy[0];
    for (int j = 0; j < NUM_CHARS; j++) {
        if (max_count < occupancy[j]) {
            max_count = occupancy[j];
        }
    }

    printf("--------------------------------Total Result-----------------------------\n");
    for (int j = 0; j < NUM_CHARS; j++) {
        printf("%c: %.4d : ", j + 'a', occupancy[j]);
        int star_count = occupancy[j] * BARLENGTH / max_count;
        for (int k = 0; k < star_count; k++) {
            printf("*");
        }
        printf("\n");
    }
    return 0;
}