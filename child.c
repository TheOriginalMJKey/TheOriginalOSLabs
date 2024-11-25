#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define BUFFER_SIZE 256

// Функция для проверки, является ли число простым
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main() {
    char buffer[BUFFER_SIZE];
    FILE *file;

    // Читаем имя файла из стандартного ввода (pipe1)
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    buffer[strcspn(buffer, "\n")] = 0; // Удаляем символ новой строки

    // Открываем файл для записи
    file = fopen(buffer, "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Читаем число из стандартного ввода (pipe1)
        read(STDIN_FILENO, buffer, BUFFER_SIZE);
        int num = atoi(buffer);

        // Проверяем, является ли число простым или отрицательным
        if (num <= 0 || is_prime(num)) {
            // Если число простое или отрицательное, завершаем работу
            write(STDOUT_FILENO, "0", 1);
            break;
        } else {
            // Если число составное, записываем его в файл
            fprintf(file, "%d\n", num);
            // Отправляем сигнал родительскому процессу, что число составное
            write(STDOUT_FILENO, "1", 1);
        }
    }

    // Закрываем файл перед завершением программы
    fclose(file);
    return 0;
}
