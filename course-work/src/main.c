#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>

#include "my_string.h"
#include "dirwalk.h"
#include "change.h"
#include "dir_file.h"

#define PATH_TO_BACKUP "/home/pahan/backup"
#define PATH_TO_SETTINGS "/home/pahan/backup/settings"
#define PATH_TO_PROJECTS "/home/pahan/backup/projects"
#define NUM_THREADS 2

//сгенерировать коммит
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
    printf("Creating standart dir finished\n");
}

//создание проекта
void create_project(const char* project_name) {
    char patch[256];
    sprintf(patch, "%s%s%s%s", PATH_TO_SETTINGS, "/", project_name, ".settings");
    create_file(patch);
    sprintf(patch, "%s%s%s", PATH_TO_PROJECTS, "/", project_name);
    create_dir(patch);
    printf("Creating standart project finished\n");
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
            printf("Bind dir(%s) to project(%s) finished\n", real_project_dir_path, project_name);
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

//сохранение файлов и изменений в прокет
void commit_real_project(const char* project_name) {
    char patch_to_dir[256];
    char patch_to_file[256];
    char real_project_path[256];

    char* name_commit = generate_commit();

    sprintf(patch_to_dir, "%s%s%s", PATH_TO_PROJECTS, "/", project_name);
    sprintf(patch_to_file, "%s%s%s%s", PATH_TO_SETTINGS, "/", project_name, ".settings");


     if(there_is_dir(patch_to_dir) && there_is_file(patch_to_file)) {

        if(there_is_file(patch_to_file)) {

            FILE* file = fopen(patch_to_file, "r");
            if (fgets(real_project_path, sizeof(real_project_path), file) != NULL) {
                char* array_path[MAX_FILES];
                int array_size = 0;
                fclose(file);


                int count = 0;
                while (strchr(real_project_path, '\n') != NULL) *strchr(real_project_path, '\n') = '\0';
                dirwalk(real_project_path, array_path, &array_size);

                if(comparison_project_and_real_dir(real_project_path, patch_to_dir) == 0) {

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

                            char dop_file[512];
                            sprintf(dop_file, "%s/file.txt", PATH_TO_BACKUP);

                            restore_file(path_to_projects_file, dop_file, "commit_all");
                            comparison_files(dop_file, array_path[i], path_to_projects_file, name_commit);
                            printf("File commited №%d: %s\n", ++count, array_path[i]);
                        }
                    }

                } else {
                    printf("Project synzronized!!!\n");
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
    printf("Project deleted\n");
}

//структура данных для потока
typedef struct {
    char path_to_project[MAX_FILES];
    char path_to_settings[MAX_FILES];
    char path_to_real_project[MAX_FILES];
    char project_name[MAX_FILES];
} ThreadData;

//функция для проверки проекта в потоке
void* process_project(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    if (strcmp(data->path_to_settings, "") == 0) {
        pthread_exit(NULL);
    }

    FILE* file = fopen(data->path_to_settings, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", data->path_to_settings);
        pthread_exit(NULL);
    }

    fgets(data->path_to_real_project, sizeof(data->path_to_real_project), file);
    while (strchr(data->path_to_real_project, '\n') != NULL) {
        *strchr(data->path_to_real_project, '\n') = '\0';
    }
    fclose(file);

    if (comparison_project_and_real_dir(data->path_to_real_project, data->path_to_project) == 1) {
        printf("%s : synchronized\n", data->project_name);
    } else {
        printf("%s : not synchronized\n", data->project_name);
    }

    pthread_exit(NULL);
}

//проверка всех проетков на синхронизацию
void check_all_projects() {
    DIR* directory = opendir(PATH_TO_PROJECTS);
    if (directory == NULL) {
        printf("Failed to open directory: %s\n", PATH_TO_PROJECTS);
        return;
    }

    struct dirent* dir;
    pthread_t threads[NUM_THREADS];
    int thread_count = 0;

    while ((dir = readdir(directory)) != NULL) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            char path_to_project[MAX_FILES];
            char path_to_settings[MAX_FILES];

            sprintf(path_to_project, "%s/%s", PATH_TO_PROJECTS, dir->d_name);
            sprintf(path_to_settings, "%s/%s.settings", PATH_TO_SETTINGS, dir->d_name);

            ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
            strcpy(data->path_to_project, path_to_project);
            strcpy(data->path_to_settings, path_to_settings);
            strcpy(data->project_name, dir->d_name);

            int result = pthread_create(&threads[thread_count], NULL, process_project, (void*)data);
            if (result != 0) {
                printf("Failed to create thread for project: %s\n", dir->d_name);
                free(data);
            } else {
                thread_count++;
                if (thread_count >= NUM_THREADS) {
                    // Wait for all threads to finish before creating new ones
                    for (int i = 0; i < NUM_THREADS; i++) {
                        pthread_join(threads[i], NULL);
                    }
                    thread_count = 0;
                }
            }
        }
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    closedir(directory);
}

//история коммитов
void commit_history(const char* project_name) {
    char patch_to_file[256];
    char line[256];

    sprintf(patch_to_file, "%s%s%s%s", PATH_TO_SETTINGS, "/", project_name, ".settings");

     if(there_is_file(patch_to_file)) {
        FILE* file = fopen(patch_to_file, "r");
        while(fgets(line, sizeof(line), file) != NULL) {
            if(strstr(line, "commit") != 0) {
                printf("%s", line);
            }
        }
    }
}

//интерфейс
void info() {
    printf("-----------------------------------------------------------------------------------------------------|\n");
    printf("|----> command <----|----> additional parameters <----|-------------> why is it needed <-------------|\n");
    printf("-----------------------------------------------------------------------------------------------------|\n");
    printf("| info              | missing                         | information about the commands               |\n");
    printf("| standart_project  | missing                         | creates a working folder                     |\n");
    printf("| create            | project name                    | creating a project                           |\n");
    printf("| bind              | 1)project name 2)path to dir    | linking a folder to a project                |\n");
    printf("| commit            | project name                    | uploading data about changes to the project  |\n");
    printf("| pull              | 1)project name 2)commit number  | restore data                                 |\n");
    printf("| check             | missing                         | checking projects                            |\n");
    printf("| commit_history    | project name                    | show history commits                         |\n");
    printf("| remove            | project name                    | deleting a project                           |\n");
    printf("-----------------------------------------------------------------------------------------------------|\n");
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "standart_project") == 0 && argc == 2) {
        standart_working_dir();
    } else if (strcmp(argv[1], "info") == 0 && argc == 2) {
        info();
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
    } else if (strcmp(argv[1], "check") == 0 && argc == 2) {
        check_all_projects();
    } else if (strcmp(argv[1], "commit_history") == 0 && argc == 3) {
        commit_history(argv[2]);
    } else {
        printf("Unknown command or incorrect number of arguments.\n");
        return 1;
    }

    return 0;
}
