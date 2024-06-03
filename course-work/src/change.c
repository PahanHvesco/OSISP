#include "change.h"


int compare_changes(const void* a, const void* b) {
    const Change* change1 = (const Change*)a;
    const Change* change2 = (const Change*)b;
    return change1->number_line - change2->number_line;
}

void sort_changes(Change* changes, int count) {
    qsort(changes, count, sizeof(Change), compare_changes);
}

//Востановление файла по изменениям
void restore_file(const char* path_to_file_change, const char* path_to_file, const char* commit) {
    char line[1024];

    FILE* file_change = fopen(path_to_file_change, "r");
    if (file_change == NULL) {
        perror("Error opening file");
        return;
    }
    Change* changes = malloc(sizeof(Change));
    int count = 0;
    int commit_search = 0;
    while(fgets(line, sizeof(line), file_change)) {
        if(strstr(line, "commit")) {
            if(commit_search == 0 && strstr(line, commit)) {
                commit_search = 1;
            } else if (commit_search == 1) {
                break;
            }
        } else if(line[0] == '+' || line[0] == '-' || line[0] == '=') {
            char* token = strtok(line+2, "|");
            changes = realloc(changes, (count+1)*sizeof(Change));
            changes[count].action = line[0];
            changes[count].number_line = atoi(token);
            changes[count].line = malloc(256*sizeof(char));
            token = strtok(NULL, "|");
            if (token != NULL) {
                strcpy(changes[count].line, token);
            } else {
                fclose(file_change);
                return;
            }
            token[strcspn(token, "\n")] = '\0';
            strcpy(changes[count].line, token);
            count++;
        }
    }
    fclose(file_change);
    sort_changes(changes, count);
    remove_extra_changes(changes, &count);
    create_file_by_change(path_to_file, changes, count);
}

//Создание файла по изменениям
void create_file_by_change(const char* path_to_file, Change* changes, int count) {
    FILE* file = fopen(path_to_file, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int j = 1;
    for (int i = 0; i < count; i++) {
        if (changes[i].action == '+' || changes[i].action == '=') {
            for (; j < changes[i].number_line; j++) {
                fputs("\n", file);
            }
            fputs(changes[i].line, file);
        }
    }
    fputs("\n", file);

    fclose(file);
}

//удаление лишних изменений
void remove_extra_changes(Change* change, int *count) {
    for(int i = 0; i < *count; i++) {
        if((i+1) <= *count) {
            if(change[i].number_line == change[i+1].number_line && change[i].action == '+' && change[i+1].action == '=') {
                change[i+1].action = '+';
                for(int j = i; j < *count-1; j++) {
                    change[j] = change[j+1];
                }
                (*count)--;
                i--;
            } else if(change[i].number_line == change[i+1].number_line && change[i].action == '=' && change[i+1].action == '=') {
                for(int j = i; j < *count-1; j++) {
                    change[j] = change[j+1];
                }
                (*count)--;
                i--;
            } else if(change[i].number_line == change[i+1].number_line && change[i].action == '-' && change[i+1].action == '+') {
                for(int j = i; j < *count-1; j++) {
                    change[j] = change[j+1];
                }
                (*count)--;
                i--;
            } else if(change[i].number_line == change[i+1].number_line && change[i].action == '+' && change[i+1].action == '-') {
                for(int j = i; j < *count-1; j++) {
                    change[j] = change[j+1];
                }
                (*count)--;
                i--;
            }
        }
    }
}
//..................................................................
//добавления изменений в файл изменений
void comparison_files(const char* file1_path, const char* file2_path, const char* output_path, const char* name_comit) {
    FILE* file1 = fopen(file1_path, "r");
    FILE* file2 = fopen(file2_path, "r");
    FILE* output_file = fopen(output_path, "a");

    if (file1 == NULL || file2 == NULL || output_file == NULL) {
        printf("Ошибка при открытии файлов.\n");
        return;
    }

    char line1[256];
    char line2[256];
    int line_num = 1;

    fprintf(output_file, "%s\n", name_comit);
    while (1) {
        if(fgets(line1, sizeof(line1), file1) != NULL) {
            if(fgets(line2, sizeof(line2), file2) != NULL) {
                if(strcmp(line1, line2) == 0) {
                    line_num++;
                    continue;
                } else if (strcmp(line1, line2) != 0 && strstr(line1, "\n") != NULL) {
                    fprintf(output_file, "=|%d|%s", line_num, line2);
                    if(strstr(line1, "\n") == NULL) {
                        fprintf(output_file, "\n");
                    }
                    line_num++;
                }
            } else {
                fprintf(output_file, "-|%d|%s", line_num, line1);
                if(strstr(line1, "\n") == NULL) {
                    fprintf(output_file, "\n");
                }
                line_num++;
            }
        } else {
            while(fgets(line2, sizeof(line2), file2) != NULL ) {
                if(feof(file2)) {
                    break;
                }
                if(strstr(line2, "\n") == NULL) {
                        fprintf(output_file, "\n");
                    }
                fprintf(output_file, "+|%d|%s", line_num, line2);
                line_num++;
            }
            break;
        }
    }

    fclose(file1);
    fclose(file2);
    fclose(output_file);
}

//восстановление проекта
void recovery_project(const char* path_to_settings, const char* path_to_project, const char* commit) {
    char path_to_real_dir[256];
    char line[256];
    char last_commit[256];

    FILE* file1 = fopen(path_to_settings, "r");

    if(strcmp(commit, "commit_all") == 0) {
        while(fgets(line, sizeof(line), file1)) {
            if(strstr(line, "commit")) {
                strcpy(last_commit, line);
            }
        }
    } else {
        strcpy(last_commit, commit);
    }

    fclose(file1);

    file1 = fopen(path_to_settings, "r");

    fgets(path_to_real_dir, sizeof(path_to_real_dir), file1);
    while (strchr(path_to_real_dir, '\n') != NULL) *strchr(path_to_real_dir, '\n') = '\0';

    remove_dir(path_to_real_dir);
    create_dir(path_to_real_dir);

    int commit_search = 0;
    while(fgets(line, sizeof(line), file1)) {
        if(strstr(line, last_commit) || strstr(line, "commit")) {
            if(commit_search == 0 && strstr(line, last_commit)) {
                commit_search = 1;
            } else if(commit_search == 1 && strstr(line, "commit")) {
                break;
            }
        } else if(commit_search == 1 && strcmp(line, "\n")){
            while (strchr(line, '\n') != NULL) *strchr(line, '\n') = '\0';
            while (strchr(path_to_real_dir, '\n') != NULL) *strchr(path_to_real_dir, '\n') = '\0';

            char real_path_to_project_file[256];
            sprintf(real_path_to_project_file, "%s", line);

            char path[256];
            sprintf(path, "%s%s", path_to_project, remove_substring(line, path_to_real_dir));

            struct stat info;
            stat(path, &info);
            if (S_ISDIR(info.st_mode)) {
                create_dir(line);
                //printf("Dir  pull %s\n", line);
            } else if (S_ISREG(info.st_mode)) {
                restore_file(path, real_path_to_project_file, commit);
                printf("File pull %s\n", real_path_to_project_file);
            }
        }
    }
}

//проверка проекта и реальной дирректории на синхронизацию
int comparison_project_and_real_dir(const char* real_project_path, const char* project_path) {
    char* array_real_project[MAX_FILES];
    int array_size_real_project = 0;

    char* array_project[MAX_FILES];
    int array_size_project = 0;

    dirwalk(real_project_path, array_real_project, &array_size_real_project);
    dirwalk(project_path, array_project, &array_size_project);

    if(array_size_project<1) {
        return 0;
    }

    for(int i = 0; i<array_size_real_project; i++) {
        char str1[256];
        char str2[256];
        sprintf(str1, "%s", remove_substring(array_real_project[i], real_project_path));
        for(int j = 0; j<array_size_project; j++) {
            sprintf(str2, "%s", remove_substring(array_project[j], project_path));
            if(strcmp(str1, str2) == 0) {
                struct stat info_file;
                stat(array_real_project[i], &info_file);

                if (S_ISDIR(info_file.st_mode)) {
                    break;
                } else if (S_ISREG(info_file.st_mode)) {
                    restore_file(array_project[j], "/home/pahan/backup/file.txt", "commit_all");

                    FILE* fp1 = fopen("/home/pahan/backup/file.txt", "r");
                    FILE* fp2 = fopen(array_real_project[i], "r");

                    if (fp1 == NULL || fp2 == NULL) {
                        return 0;
                    }

                    int ch1, ch2;
                    while ((ch1 = fgetc(fp2)) != EOF && (ch2 = fgetc(fp1)) != EOF) {
                        if (ch1 != ch2) {
                            fclose(fp1);
                            fclose(fp2);

                            return 0; //проект не синхрогизированн
                        }
                    }

                    if (ch1 != EOF || ch2 != EOF) {
                        fclose(fp1);
                        fclose(fp2);
                        if(ch1 == -1 && ch2 == 10) {
                            break;
                        }
                        return 0; //проект не синхрогизированн
                    }

                    break;
                }
            } else if(j + 1 == array_size_project) {
                return 0; //проект не синхрогизированн
            }
        }
    }
    return 1;
}
