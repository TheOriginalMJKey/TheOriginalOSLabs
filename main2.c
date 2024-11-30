#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef float (*derivative_func_t)(float, float);
typedef int* (*sort_func_t)(int*);

void switch_library(void **handle, const char *lib_path) {
    if (*handle) dlclose(*handle);
    *handle = dlopen(lib_path, RTLD_LAZY);
    if (!*handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        exit(1);
    }
}

int main() {
    printf("=== Тест программы с динамической загрузкой ===\n");

    void *handle = NULL;
    derivative_func_t derivative = NULL;
    sort_func_t sort = NULL;

    // Изначальная библиотека
    switch_library(&handle, "./libmath1.so");

    char command;
    while (1) {
        printf("Введите команду: ");
        scanf(" %c", &command);

        if (command == '0') {
            switch_library(&handle, "./libmath2.so");
            printf("Библиотека переключена.\n");
        } else if (command == '1') {
            derivative = (derivative_func_t)dlsym(handle, "derivative");
            if (!derivative) {
                fprintf(stderr, "Ошибка получения функции derivative: %s\n", dlerror());
                continue;
            }
            float A, deltaX;
            printf("Введите A и deltaX: ");
            scanf("%f %f", &A, &deltaX);
            printf("Результат: %f\n", derivative(A, deltaX));
        } else if (command == '2') {
            sort = (sort_func_t)dlsym(handle, "quick_sort");
            if (!sort) {
                sort = (sort_func_t)dlsym(handle, "bubble_sort");
            }
            if (!sort) {
                fprintf(stderr, "Ошибка получения функции сортировки: %s\n", dlerror());
                continue;
            }
            int array[11], n;
            printf("Введите размер массива: ");
            scanf("%d", &n);
            if (n > 10) {
                fprintf(stderr, "Размер массива превышает допустимый предел (10).\n");
                continue;
            }
            printf("Введите массив: ");
            for (int i = 0; i < n; ++i) scanf("%d", &array[i]);
            array[n] = 0; // Добавлен 0 для определения конца массива
            sort(array);
            printf("Отсортированный массив: ");
            for (int i = 0; i < n; ++i) printf("%d ", array[i]);
            printf("\n");
        }
    }

    if (handle) dlclose(handle);
    return 0;
}
