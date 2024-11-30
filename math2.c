#include <stdio.h>
#include <math.h>
#include "math2.h"

// Производная методом центральной разности
float derivative(float A, float deltaX) {
    printf("[math2] Вычисление производной методом центральной разности...\n");
    return (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
}

// Объявление вспомогательной функции
void quick_sort_helper(int *array, int left, int right);

// Быстрая сортировка (сортировка Хоара)
int* quick_sort(int *array) {
    printf("[math2] Быстрая сортировка...\n");
    int size = 0;
    while (array[size] != 0) {
        size++;
    }
    quick_sort_helper(array, 0, size - 1);
    return array;
}

void quick_sort_helper(int *array, int left, int right) {
    if (left >= right) return;
    int pivot = array[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (array[i] < pivot) i++;
        while (array[j] > pivot) j--;
        if (i <= j) {
            int temp = array[i];
            array[i++] = array[j];
            array[j--] = temp;
        }
    }
    quick_sort_helper(array, left, j);
    quick_sort_helper(array, i, right);
}
