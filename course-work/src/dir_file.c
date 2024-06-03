#include "dir_file.h"

//создание папки по пути
void create_dir(const char* path) {
    if(access(path, F_OK) == -1) {
        if(mkdir(path, 0777) == -1) {
        }
    }
}

//создание файла по пути
void create_file(const char* path) {
    if(access(path, F_OK) == -1) {
        FILE* file = fopen(path, "w");

        if(file == NULL) {
        }
        fclose(file);
    }
}

//есть ли папка
int there_is_dir(const char* path) {
    if(access(path, F_OK) == -1) {
        return 0;
    }
    return 1;
}

//есть ли файл
int there_is_file(const char* path) {
    if(access(path, F_OK) == -1) {
        return 0;
    }
    return 1;
}

//удаление папки
void remove_dir(const char* path) {

    DIR* d = opendir(path);
    size_t path_len = strlen(path);
    int r;

    if (!d) {
        return;
    }

    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            char *full_path = malloc(path_len + strlen(dir->d_name) + 2);
            if (!full_path) {
                closedir(d);
                perror("Ошибка выделения памяти");
                return;
            }
            sprintf(full_path, "%s/%s", path, dir->d_name);

            struct stat statbuf;
            if (stat(full_path, &statbuf) == -1) {
                perror("Ошибка при получении информации о файле");
                free(full_path);
                closedir(d);
                return;
            }

            if (S_ISDIR(statbuf.st_mode)) {
                remove_dir(full_path);
            } else {
                r = remove(full_path);
                if (r != 0) {
                    perror("Ошибка при удалении файла");
                    free(full_path);
                    closedir(d);
                    return;
                }
            }
            free(full_path);
        }
    }
    closedir(d);
    r = rmdir(path);
    if (r != 0) {
        perror("Ошибка при удалении директории");
        return;
    }
}
