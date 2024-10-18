#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    int pipe1[2], pipe2[2]; // Массивы для хранения дескрипторов каналов
    pid_t pid; // Переменная для хранения PID дочернего процесса
    char buffer[BUFFER_SIZE]; // Буфер для чтения и записи данных
    FILE *file; // Указатель на файл

    // Создаем два канала для взаимодействия с дочерним процессом
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создаем дочерний процесс
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс
        close(pipe1[1]); // Закрываем записывающий конец pipe1
        close(pipe2[0]); // Закрываем читающий конец pipe2

        // Перенаправляем стандартный ввод на pipe1
        dup2(pipe1[0], STDIN_FILENO);
        // Перенаправляем стандартный вывод на pipe2
        dup2(pipe2[1], STDOUT_FILENO);

        // Запускаем дочернюю программу
        execl("./child", "child", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        // Родительский процесс
        close(pipe1[0]); // Закрываем читающий конец pipe1
        close(pipe2[1]); // Закрываем записывающий конец pipe2

        // Запрашиваем у пользователя имя файла
        printf("Введите имя файла: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Удаляем символ новой строки

        // Открываем файл для записи
        file = fopen(buffer, "w");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        // Передаем имя файла дочернему процессу через pipe1
        write(pipe1[1], buffer, BUFFER_SIZE);

        // Бесконечный цикл для ввода чисел
        while (1) {
            // Запрашиваем у пользователя число
            printf("Введите число: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            // Передаем число дочернему процессу через pipe1
            write(pipe1[1], buffer, BUFFER_SIZE);

            // Читаем ответ от дочернего процесса через pipe2
            read(pipe2[0], buffer, BUFFER_SIZE);
            // Если ответ указывает на завершение работы (число отрицательное или простое)
            if (buffer[0] == '0') {
                break;
            }
            // Записываем число в файл
            fprintf(file, "%s", buffer);
        }

        // Закрываем файл и каналы перед завершением работы
        fclose(file);
        close(pipe1[1]);
        close(pipe2[0]);
        // Ожидаем завершения дочернего процесса
        wait(NULL);
    }

    return 0;
}
