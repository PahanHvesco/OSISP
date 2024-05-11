#include "my_string.h"


char* remove_substring(const char* str, const char* substr) {
    size_t len = strlen(str);
    size_t substrLen = strlen(substr);
    size_t count = 0;

    // Подсчитываем количество вхождений подстроки
    const char* pos = str;
    while ((pos = strstr(pos, substr)) != NULL) {
        count++;
        pos += substrLen;
    }

    // Вычисляем размер новой строки
    size_t newLen = len - count * substrLen;

    // Выделяем память для новой строки
    char* newStr = (char*)malloc((newLen + 1) * sizeof(char));
    if (newStr == NULL) {
        printf("Ошибка выделения памяти\n");
        return NULL;
    }

    // Копируем символы из исходной строки, пропуская подстроку
    pos = str;
    char* newPos = newStr;
    while ((pos = strstr(pos, substr)) != NULL) {
        size_t segmentLen = pos - str;
        strncpy(newPos, str, segmentLen);
        newPos += segmentLen;
        pos += substrLen;
        str = pos;
    }

    // Копируем оставшийся фрагмент строки
    strcpy(newPos, str);

    return newStr;
}

char* copyString(const char* source) {
    size_t length = strlen(source);
    char* destination = (char*)malloc((length + 1) * sizeof(char)); // Выделение памяти для новой строки

    strcpy(destination, source); // Копирование строки

    return destination; // Возвращение новой строки
}
