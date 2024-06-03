#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>

#include "my_string.h"
#include "dir_file.h"
#include "dirwalk.h"

#define MAX_PATH_LENGTH 256
#define MAX_THREADS 8

typedef struct{
    char action;
    int number_line;
    char* line;
} Change;



void create_file_by_change(const char* path_to_file, Change* changes, int count);
int compare_changes(const void* a, const void* b);
void sort_changes(Change* changes, int count);
void remove_extra_changes(Change* change, int *count);
void restore_file(const char* path_to_file_change, const char* path_to_file, const char* commit);
void comparison_files(const char* file1_path, const char* file2_path, const char* output_path, const char* name_comit);
void recovery_project(const char* path_to_settings, const char* path_to_project, const char* commit);
int comparison_project_and_real_dir(const char* real_project_path, const char* project_path);
