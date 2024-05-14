#include "dirwalk.h"

DIR* open_directory(const char *dirname) {
    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
    return dir;
}

void close_directory(DIR *dir) {
    if (closedir(dir) != 0) {
        perror("Error closing directory");
        exit(EXIT_FAILURE);
    }
}

void travels_directory(const char* path, char* array_path[], int* array_size) {
    DIR* directory = open_directory(path);
    struct stat info;
    struct dirent* dir;
    while((dir = readdir(directory)) != NULL) {
        char full_path[MAX_FILES];
        sprintf(full_path, "%s/%s", path, dir->d_name);

        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            array_path[*array_size] = strdup(full_path);
            (*array_size)++;
            stat(full_path, &info);

            if(S_ISDIR(info.st_mode)) {
                travels_directory(full_path, array_path, array_size);
            }
        }
    }
    close_directory(directory);
}

void print_files(char* array_path[], int array_size) {
    struct stat info;

    for(int i = 0; i < array_size; i++) {
        stat(array_path[i], &info);
        if (S_ISREG(info.st_mode)) {
            printf("%s\n", array_path[i]);
        } else if (S_ISDIR(info.st_mode)) {
            printf("%s\n", array_path[i]);
        }
    }
}

void dirwalk(char* path, char* array_path[], int* array_size) {
    travels_directory(path, array_path, array_size);
}
