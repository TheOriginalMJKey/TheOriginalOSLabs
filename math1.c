#include <stdio.h>
#include <math.h>
#include "math1.h"

// Производная методом односторонней разности
float derivative(float A, float deltaX) {
    printf("[math1] Вычисление производной методом односторонней разности...\n");
    return (cos(A + deltaX) - cos(A)) / deltaX;
}

// Пузырьковая сортировка
int* bubble_sort(int *array) {
    printf("[math1] Сортировка пузырьком...\n");
    int size = 0;
    while (array[size] != 0) {
        size++;
    }
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    return array;
}
