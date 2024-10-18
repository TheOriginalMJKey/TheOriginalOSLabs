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
    FILE *file;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс
        close(pipe1[1]); // Закрываем записывающий конец pipe1
        close(pipe2[0]); // Закрываем читающий конец pipe2

        dup2(pipe1[0], STDIN_FILENO); // Перенаправляем стандартный ввод на pipe1
        dup2(pipe2[1], STDOUT_FILENO); // Перенаправляем стандартный вывод на pipe2

        execl("./child", "child", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        // Родительский процесс
        close(pipe1[0]); // Закрываем читающий конец pipe1
        close(pipe2[1]); // Закрываем записывающий конец pipe2

        printf("Введите имя файла: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Удаляем символ новой строки

        file = fopen(buffer, "w");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        write(pipe1[1], buffer, BUFFER_SIZE); // Передаем имя файла дочернему процессу

        while (1) {
            printf("Введите число: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            write(pipe1[1], buffer, BUFFER_SIZE);

            read(pipe2[0], buffer, BUFFER_SIZE);
            if (buffer[0] == '0') {
                break;
            }
            fprintf(file, "%s", buffer);
        }

        fclose(file);
        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    }

    return 0;
}
