#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <errno.h>

#define MAX_FILES 1024

typedef struct {
    int l;
    int d;
    int f;
    int s;
    char* dir;
} Options;

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

Options get_options(int argc, char* argv[]) {
    Options options = {0, 0, 0, 0, NULL};
    int opt;
    while((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch(opt) {
            case 'l':
                options.l = 1;
                break;
            case 'd':
                options.d = 1;
                break;
            case 'f':
                options.f = 1;
                break;
            case 's':
                options.s = 1;
                break;
        }
    }
    if(options.l == 0 && options.d == 0 && options.f == 0) {
        options.l = options.d = options.f = 1;
    }
    if(optind < argc){
        options.dir = strdup(argv[optind]);
    } else {
        options.dir = ".";
    }

    return options;
}

void travels_directory(const char* path, char* array_path[], int* array_size) {
    DIR* directory = open_directory(path);
    struct stat info;
    struct dirent* dir;

    while((dir = readdir(directory)) != NULL) {
        char full_path[PATH_MAX];
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

int collate_compare(const void *a, const void *b) {
    return strcoll(*(const char **)a, *(const char **)b);
}

void print_files(Options options, char* array_path[], int array_size) {
    struct stat info;
    int file_count = 0, dir_count = 0, lnk_count = 0;

    if(options.s) {
        setlocale(LC_COLLATE, "");
        qsort(array_path, array_size, sizeof(char *), collate_compare);
    }

    for(int i = 0; i < array_size; i++) {
        stat(array_path[i], &info);
        if (options.f && S_ISREG(info.st_mode)) {
            printf("%s\n", array_path[i]);
            file_count++;
        } else if (options.d && S_ISDIR(info.st_mode)) {
            printf("%s\n", array_path[i]);
            dir_count++;
        } else if (options.l && S_ISLNK(info.st_mode)) {
            printf("%s\n", array_path[i]);
            lnk_count++;
        }
    }

    if (options.f && !options.l && !options.d && file_count == 0) {
        printf("No files found.\n");
    } else if (!options.f && options.l && !options.d && lnk_count == 0) {
        printf("No links found.\n");
    } else if (!options.f && !options.l && options.d && dir_count == 0) {
        printf("No directories found.\n");
    }
}

int main(int argc, char* argv[]) {
    Options options;
    char* array_path[MAX_FILES];
    int array_size = 0;

    options = get_options(argc, argv);
    printf("Directory: %s\n", options.dir);
    travels_directory(options.dir, array_path, &array_size);
    print_files(options, array_path, array_size);

    for (int i = 0; i < array_size; i++) {
        free(array_path[i]);
    }

    return 0;
}
