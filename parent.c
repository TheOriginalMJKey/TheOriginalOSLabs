#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // Создаем два канала (pipe) для взаимодействия между процессами
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

        // Запускаем дочерний процесс
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

        // Передаем имя файла дочернему процессу через pipe1
        write(pipe1[1], buffer, strlen(buffer) + 1);

        while (1) {
            // Запрашиваем у пользователя число
            printf("Введите число: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            // Передаем число дочернему процессу через pipe1
            write(pipe1[1], buffer, strlen(buffer) + 1);

            // Читаем ответ от дочернего процесса через pipe2
            read(pipe2[0], buffer, BUFFER_SIZE);
            // Если ответ равен '0', завершаем работу
            if (buffer[0] == '0') {
                break;
            }
        }

        // Закрываем каналы перед завершением программы
        close(pipe1[1]);
        close(pipe2[0]);
        // Ожидаем завершения дочернего процесса
        wait(NULL);
    }

    return 0;
}
