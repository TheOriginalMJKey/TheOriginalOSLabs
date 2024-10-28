#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <complex.h>
#include <semaphore.h>

// Структура данных для передачи в потоки
typedef struct {
    int row;       // Индекс строки матрицы C
    int col;       // Индекс столбца матрицы C
    int size;      // Размер матриц
    double complex **A; // Указатель на матрицу A
    double complex **B; // Указатель на матрицу B
    double complex **C; // Указатель на матрицу C
    sem_t *sem;    // Семафор для ограничения количества потоков
} ThreadData;

// Функция для умножения элементов матриц
void *multiply(void *arg) {
    ThreadData *data = (ThreadData *)arg; // Получаем данные для потока
    int row = data->row; // Индекс строки матрицы C
    int col = data->col; // Индекс столбца матрицы C
    int size = data->size; // Размер матриц
    double complex **A = data->A; // Указатель на матрицу A
    double complex **B = data->B; // Указатель на матрицу B
    double complex **C = data->C; // Указатель на матрицу C

    // Выполняем умножение элементов матриц A и B и добавляем результат в матрицу C
    for (int k = 0; k < size; k++) {
        C[row][col] += A[row][k] * B[k][col];
    }

    sem_post(data->sem); // Освобождаем семафор для следующего потока
    pthread_exit(NULL); // Завершаем поток
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

    int size = atoi(argv[1]); // Размер матриц
    int num_threads = atoi(argv[2]); // Количество потоков
    int max_threads = atoi(argv[3]); // Максимальное количество потоков

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
            A[i][j] = i + j * I; // Инициализация матрицы A
            B[i][j] = i + j * I; // Инициализация матрицы B
            C[i][j] = 0; // Инициализация матрицы C нулями
        }
    }

    // Создание семафора для ограничения количества потоков
    sem_t sem;
    sem_init(&sem, 0, num_threads);

    pthread_t threads[size * size]; // Массив для хранения идентификаторов потоков
    ThreadData data[size * size]; // Массив для хранения данных для каждого потока
    int thread_count = 0; // Счетчик потоков

    // Создание и запуск потоков для умножения элементов
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            sem_wait(&sem); // Ограничение количества потоков
            data[thread_count].row = i; // Устанавливаем индекс строки матрицы C
            data[thread_count].col = j; // Устанавливаем индекс столбца матрицы C
            data[thread_count].size = size; // Устанавливаем размер матриц
            data[thread_count].A = A; // Устанавливаем указатель на матрицу A
            data[thread_count].B = B; // Устанавливаем указатель на матрицу B
            data[thread_count].C = C; // Устанавливаем указатель на матрицу C
            data[thread_count].sem = &sem; // Устанавливаем указатель на семафор

            pthread_create(&threads[thread_count], NULL, multiply, (void *)&data[thread_count]); // Создаем новый поток
            thread_count++; // Увеличиваем счетчик потоков
        }
    }

    // Ожидание завершения всех потоков
    for (int k = 0; k < thread_count; k++) {
        pthread_join(threads[k], NULL); // Основной поток ожидает завершения каждого потока
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
