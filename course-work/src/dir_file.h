#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>


void create_dir(const char* path);
void create_file(const char* path);
int there_is_dir(const char* path);
int there_is_file(const char* path);
void remove_dir(const char* path);
