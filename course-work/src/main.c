#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>

#include "my_string.h"
#include "dirwalk.h"
#include "change.h"
#include "dir_file.h"

#define PATH_TO_BACKUP "/home/pahan/backup"
#define PATH_TO_SETTINGS "/home/pahan/backup/settings"
#define PATH_TO_PROJECTS "/home/pahan/backup/projects"

char* generate_commit() {
    char* commit_name = (char*)malloc(256 * sizeof(char));
    int random_number = rand() % 100000 + 1;
    snprintf(commit_name, 256, "commit_%d", random_number);
    return commit_name;
}

//стандартная пустая рабочая дерриктория если что-то не создано или было удалено или это первый запуск
void standart_working_dir() {
    create_dir(PATH_TO_BACKUP);
    create_dir(PATH_TO_PROJECTS);
    create_dir(PATH_TO_SETTINGS);
}
//создание проекта
void create_project(const char* project_name) {
    char patch[256];
    sprintf(patch, "%s%s%s%s", PATH_TO_SETTINGS, "/", project_name, ".settings");
    create_file(patch);
    sprintf(patch, "%s%s%s", PATH_TO_PROJECTS, "/", project_name);
    create_dir(patch);
}
//cвязать проект с реальным проектом
void bind_project(const char* project_name, const char* real_project_dir_path) {
    char patch_to_dir[256];
    char patch_to_file[256];
    sprintf(patch_to_dir, "%s%s%s", PATH_TO_PROJECTS, "/", project_name);
    sprintf(patch_to_file, "%s%s%s%s", PATH_TO_SETTINGS, "/", project_name, ".settings");
    if(there_is_dir(patch_to_dir) && there_is_dir(patch_to_file)) {
        if(there_is_dir(real_project_dir_path)) {
            FILE* file = fopen(patch_to_file, "w");
            fprintf(file, real_project_dir_path);
            fclose(file);
        } else {
            perror("No such real project");
        }
    } else {
        perror("No such project");
    }
}
//settings
void record_to_settings(const char* patch_to_file, const char* name_commit, char* array_path[], int size) {
    FILE* file = fopen(patch_to_file, "a");
    time_t current_time = time(NULL);
    struct tm* local_time = localtime(&current_time);

    fprintf(file, "\n%s|%s\n", name_commit, asctime(local_time));
    for(int i = 0; i<size; i++) {
        fprintf(file, "%s\n", array_path[i]);
    }
    fclose(file);
}

//сохранение файлов
void commit_real_project(const char* project_name) {
    char patch_to_dir[256];
    char patch_to_file[256];
    char real_project_path[256];

    char* name_commit = generate_commit();

    sprintf(patch_to_dir, "%s%s%s", PATH_TO_PROJECTS, "/", project_name);
    sprintf(patch_to_file, "%s%s%s%s", PATH_TO_SETTINGS, "/", project_name, ".settings");


     if(there_is_dir(patch_to_dir) && there_is_file(patch_to_file)) {
         printf("%s\n", patch_to_file);
        if(there_is_file(patch_to_file)) {

            FILE* file = fopen(patch_to_file, "r");
            if (fgets(real_project_path, sizeof(real_project_path), file) != NULL) {
                char* array_path[MAX_FILES];
                int array_size = 0;
                fclose(file);

                while (strchr(real_project_path, '\n') != NULL) *strchr(real_project_path, '\n') = '\0';
                dirwalk(real_project_path, array_path, &array_size);
                record_to_settings(patch_to_file, name_commit, array_path, array_size);

                struct stat info_dir;
                for(int i = 0; i < array_size; i++) {
                    char path_to_projects_dir[512];
                    stat(array_path[i], &info_dir);
                    if (S_ISDIR(info_dir.st_mode)) {
                        sprintf(path_to_projects_dir, "%s%s", patch_to_dir, remove_substring(array_path[i], real_project_path));
                        create_dir(path_to_projects_dir);
                    }
                }

                struct stat info_file;
                for(int i = 0; i < array_size; i++) {
                    char path_to_projects_file[512];
                    char real_path_to_project[512];
                    stat(array_path[i], &info_file);
                    if (S_ISDIR(info_file.st_mode)) {
                        continue;
                    } else if (S_ISREG(info_file.st_mode)) {
                        sprintf(path_to_projects_file, "%s%s", patch_to_dir, remove_substring(array_path[i], real_project_path));
                        create_file(path_to_projects_file);
                        sprintf(real_path_to_project, "%s%s", real_project_path, array_path[i]);
                        restore_file(path_to_projects_file, "/home/pahan/backup/file.txt", "commit_all");
                        comparison_files("/home/pahan/backup/file.txt", array_path[i], path_to_projects_file, name_commit);
                    }
                }
            }
        } else {
            perror("No such real project");
        }
    } else {
        perror("No such project");
    }
}
//достать изменения
void pull_project(const char* project_name, const char* commit) {
    char patch_to_dir[256];
    char patch_to_file[256];

    sprintf(patch_to_dir, "%s%s%s", PATH_TO_PROJECTS, "/", project_name);
    sprintf(patch_to_file, "%s%s%s%s", PATH_TO_SETTINGS, "/", project_name, ".settings");
    recovery_project(patch_to_file, patch_to_dir, commit);
}
//удаление прокекта
void remove_project(const char* project_name) {
    char path_to_project[256];
    sprintf(path_to_project, "%s/%s", PATH_TO_PROJECTS, project_name);
    if(there_is_dir(path_to_project) == 1) {
        remove_dir(path_to_project);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "standart_working_dir") == 0) {
        standart_working_dir();
    } else if (strcmp(argv[1], "create") == 0 && argc == 3) {
        create_project(argv[2]);
    } else if (strcmp(argv[1], "bind") == 0 && argc == 4) {
        bind_project(argv[2], argv[3]);
    } else if (strcmp(argv[1], "commit") == 0 && argc == 3) {
        commit_real_project(argv[2]);
    } else if (strcmp(argv[1], "pull") == 0 && argc == 4) {
        pull_project(argv[2], argv[3]);
    } else if (strcmp(argv[1], "remove") == 0 && argc == 3) {
        remove_project(argv[2]);
    } else {
        printf("Unknown command or incorrect number of arguments.\n");
        return 1;
    }

    return 0;
}
