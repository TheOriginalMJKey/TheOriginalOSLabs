#include <stdio.h>
#include "math1.h"

int main() {
    printf("=== Тест программы с линковкой ===\n");

    // Тест производной
    float A = 1.0f;
    float deltaX = 0.01f;
    printf("Производная cos(x) в точке %f: %f\n", A, derivative(A, deltaX));

    // Тест сортировки
    int array[] = {5, 2, 9, 1, 5, 6, 0}; 
    bubble_sort(array);
    printf("Отсортированный массив: ");
    int i = 0;
    while (array[i] != 0) {
        printf("%d ", array[i]);
        i++;
    }
    printf("\n");

    return 0;
}
