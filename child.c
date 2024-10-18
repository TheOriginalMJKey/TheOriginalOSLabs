#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define BUFFER_SIZE 256

// Функция для проверки, является ли число простым
int is_prime(int num) {
    if (num <= 1) return 0; // Числа меньше или равные 1 не являются простыми
    if (num == 2) return 1; // 2 - простое число
    if (num % 2 == 0) return 0; // Четные числа не являются простыми
    for (int i = 3; i <= sqrt(num); i += 2) { // Проверяем делимость на нечетные числа до корня из num
        if (num % i == 0) return 0; // Если число делится без остатка, оно не простое
    }
    return 1; // Если ни одно из условий не выполнено, число простое
}

int main() {
    char buffer[BUFFER_SIZE];
    FILE *file;

    // Читаем имя файла из стандартного ввода (pipe1)
    read(STDIN_FILENO, buffer, BUFFER_SIZE);

    // Открываем файл для записи
    file = fopen(buffer, "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Читаем число из стандартного ввода (pipe1)
        read(STDIN_FILENO, buffer, BUFFER_SIZE);
        int num = atoi(buffer); // Преобразуем строку в целое число

        // Проверяем, является ли число отрицательным или простым
        if (num <= 0 || is_prime(num)) {
            // Если число отрицательное или простое, завершаем работу
            write(STDOUT_FILENO, "0", 1);
            break;
        } else {
            // Если число составное, записываем его в файл
            fprintf(file, "%d\n", num);
            // Отправляем сигнал родительскому процессу, что число составное
            write(STDOUT_FILENO, "1", 1);
        }
    }

    // Закрываем файл
    fclose(file);
    return 0;
}
