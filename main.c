#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <complex.h>
#include <semaphore.h>

// Структура данных для передачи в потоки
typedef struct {
    int row;
    int col;
    int size;
    double complex **A;
    double complex **B;
    double complex **C;
    sem_t *sem; // Семафор для ограничения количества потоков
} ThreadData;

// Функция для умножения элементов матриц
void *multiply(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int row = data->row;
    int col = data->col;
    int size = data->size;
    double complex **A = data->A;
    double complex **B = data->B;
    double complex **C = data->C;

    for (int k = 0; k < size; k++) {
        C[row][col] += A[row][k] * B[k][col];
    }

    sem_post(data->sem); // Освобождаем семафор для следующего потока
    pthread_exit(NULL);
}

// Функция для печати матрицы комплексных чисел
void printMatrix(double complex **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("(%f + %fi) ", creal(matrix[i][j]), cimag(matrix[i][j]));
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    // Проверка аргументов командной строки
    if (argc != 4) {
        printf("Usage: %s <matrix_size> <num_threads> <max_threads>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int max_threads = atoi(argv[3]);

    // Ограничение количества потоков
    if (num_threads > max_threads) {
        num_threads = max_threads;
    }

    // Выделение памяти для матриц
    double complex **A = malloc(size * sizeof(double complex *));
    double complex **B = malloc(size * sizeof(double complex *));
    double complex **C = malloc(size * sizeof(double complex *));

    for (int i = 0; i < size; i++) {
        A[i] = malloc(size * sizeof(double complex));
        B[i] = malloc(size * sizeof(double complex));
        C[i] = malloc(size * sizeof(double complex));
        for (int j = 0; j < size; j++) {
            A[i][j] = i + j * I;
            B[i][j] = i + j * I;
            C[i][j] = 0;
        }
    }

    // Создание семафора для ограничения количества потоков
    sem_t sem;
    sem_init(&sem, 0, num_threads);

    pthread_t threads[size * size];
    ThreadData data[size * size];
    int thread_count = 0;

    // Создание и запуск потоков для умножения элементов
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            sem_wait(&sem); // Ограничение количества потоков
            data[thread_count].row = i;
            data[thread_count].col = j;
            data[thread_count].size = size;
            data[thread_count].A = A;
            data[thread_count].B = B;
            data[thread_count].C = C;
            data[thread_count].sem = &sem;

            pthread_create(&threads[thread_count], NULL, multiply, (void *)&data[thread_count]);
            thread_count++;
        }
    }

    // Ожидание завершения всех потоков
    for (int k = 0; k < thread_count; k++) {
        pthread_join(threads[k], NULL);
    }

    // Печать результата
    printMatrix(C, size);

    // Освобождение памяти и уничтожение семафора
    for (int i = 0; i < size; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    sem_destroy(&sem);

    return 0;
}
