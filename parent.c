#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define FILE_SIZE 256

volatile sig_atomic_t child_ready = 0;

// Обработчик сигнала от дочернего процесса
void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("[Parent] Получен сигнал SIGUSR1 от дочернего процесса\n");
        child_ready = 1;
    }
}

int main() {
    const char *shared_filename = "shared_memory.dat";
    const char *output_filename = "output.txt";
    int fd;
    char *shared_memory;

    // Устанавливаем обработчик сигнала
    signal(SIGUSR1, signal_handler);

    // Создаем файл для сохранения чисел
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Создаем отображаемый файл и задаем его размер
    fd = open(shared_filename, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        fclose(output_file);
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, FILE_SIZE);

    // Отображаем файл в память
    shared_memory = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        fclose(output_file);
        exit(EXIT_FAILURE);
    }
    printf("[Parent] Создание отображаемого файла %s и отображение его в память\n", shared_filename);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс
        execl("./child", "child", shared_filename, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        // Родительский процесс
        while (1) {
            char buffer[FILE_SIZE];
            printf("Введите число: ");
            fgets(buffer, FILE_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Удаляем символ новой строки
            printf("[Parent] Ожидание ввода числа от пользователя: %s\n", buffer);

            // Записываем в отображаемую память
            strncpy(shared_memory, buffer, FILE_SIZE);
            printf("[Parent] Число записано в отображаемую память: %s\n", buffer);

            // Отправляем сигнал дочернему процессу
            printf("[Parent] Сигнал SIGUSR1 отправлен ребенку (данные готовы)\n");
            kill(pid, SIGUSR1);

            // Ждем ответа от дочернего процесса
            while (!child_ready) {
                pause();
            }
            child_ready = 0;

            // Если дочерний процесс завершил работу, выходим
            if (strcmp(shared_memory, "exit") == 0) {
                break;
            }

            // Сохраняем ответ в файл
            fprintf(output_file, "%s\n", shared_memory);
            printf("[Parent] Результат записан в файл %s\n", output_filename);
            printf("Ответ дочернего процесса: %s\n", shared_memory);
        }
    }

    // Завершаем работу
    munmap(shared_memory, FILE_SIZE);
    close(fd);
    fclose(output_file);
    unlink(shared_filename);
    printf("[Parent] Отображаемый файл закрыт и удален\n");
    return 0;
}
