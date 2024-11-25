#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define FILE_SIZE 256

volatile sig_atomic_t parent_signal = 0;

// Обработчик сигнала от родительского процесса
void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("[Child] Получен сигнал SIGUSR1 от родительского процесса\n");
        parent_signal = 1;
    }
}

// Функция проверки числа на простоту
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char *shared_memory = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Устанавливаем обработчик сигнала
    signal(SIGUSR1, signal_handler);

    while (1) {
        // Ждем сигнала от родительского процесса
        while (!parent_signal) {
            pause();
        }
        parent_signal = 0;

        char buffer[FILE_SIZE];
        strncpy(buffer, shared_memory, FILE_SIZE);
        printf("[Child] Число прочитано из отображаемой памяти: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0) {
            printf("[Child] Завершение работы\n");
            break;
        }

        int num = atoi(buffer);
        if (num <= 0 || is_prime(num)) {
            printf("[Child] Проверка числа на простоту: простое или отрицательное\n");
            strncpy(shared_memory, "exit", FILE_SIZE);
            printf("[Child] Результат записан в отображаемую память: exit\n");
            kill(getppid(), SIGUSR1);
            break;
        } else {
            printf("[Child] Проверка числа на простоту: составное\n");
            snprintf(shared_memory, FILE_SIZE, "%d - составное число", num);
            printf("[Child] Результат записан в отображаемую память: %d - составное число\n", num);
            kill(getppid(), SIGUSR1);
        }
    }

    munmap(shared_memory, FILE_SIZE);
    close(fd);
    return 0;
}
