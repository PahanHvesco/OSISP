#define _GNU_SOURCE

#include <stdio.h>
#pragma once
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <errno.h>

#define MAX_FILES 65536

typedef struct {
    char* array_patch[MAX_FILES];
    int array_size;
} Output;

DIR* open_directory(const char *dirname);

void close_directory(DIR *dir);

void travels_directory(const char* path, char* array_path[], int* array_size);

void print_files(char* array_path[], int array_size);

void dirwalk();
