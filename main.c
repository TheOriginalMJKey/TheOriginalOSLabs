#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <complex.h>

// Структура для передачи данных в поток
typedef struct {
    int start_row;  // Начальная строка для обработки
    int end_row;    // Конечная строка для обработки
    int size;       // Размер матрицы
    double complex **A;  // Указатель на матрицу A
    double complex **B;  // Указатель на матрицу B
    double complex **C;  // Указатель на матрицу C (результат)
} ThreadData;

// Функция, выполняемая каждым потоком
void *multiply(void *arg) {
    // Преобразование аргумента в структуру ThreadData
    ThreadData *data = (ThreadData *)arg;
    int start_row = data->start_row;
    int end_row = data->end_row;
    int size = data->size;
    double complex **A = data->A;
    double complex **B = data->B;
    double complex **C = data->C;

    // Вывод сообщения о начале работы потока
    printf("Thread started: start_row=%d, end_row=%d\n", start_row, end_row);

    // Перемножение матриц
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                // Умножение элементов матриц A и B и добавление результата в матрицу C
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Вывод сообщения о завершении работы потока
    printf("Thread finished: start_row=%d, end_row=%d\n", start_row, end_row);

    // Завершение потока
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Проверка количества аргументов
    if (argc != 4) {
        printf("Usage: %s <matrix_size> <num_threads> <max_threads>\n", argv[0]);
        return 1;
    }

    // Получение аргументов командной строки
    int size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int max_threads = atoi(argv[3]);

    // Проверка, чтобы количество потоков не превышало максимальное значение
    if (num_threads > max_threads) {
        num_threads = max_threads;
    }

    // Инициализация матриц
    double complex **A = malloc(size * sizeof(double complex *));
    double complex **B = malloc(size * sizeof(double complex *));
    double complex **C = malloc(size * sizeof(double complex *));
    if (A == NULL || B == NULL || C == NULL) {
        printf("ERROR: Failed to allocate memory for matrices.\n");
        return 1;
    }

    // Выделение памяти для строк матриц
    for (int i = 0; i < size; i++) {
        A[i] = malloc(size * sizeof(double complex));
        B[i] = malloc(size * sizeof(double complex));
        C[i] = calloc(size, sizeof(double complex));
        if (A[i] == NULL || B[i] == NULL || C[i] == NULL) {
            printf("ERROR: Failed to allocate memory for matrix rows.\n");
            return 1;
        }
        // Инициализация элементов матриц A и B
        for (int j = 0; j < size; j++) {
            A[i][j] = i + j * I;
            B[i][j] = i + j * I;
        }
    }

    // Массив для хранения дескрипторов потоков
    pthread_t threads[num_threads];
    // Массив для хранения данных для каждого потока
    ThreadData data[num_threads];
    // Количество строк, обрабатываемых каждым потоком
    int rows_per_thread = size / num_threads;

    // Создание потоков
    for (int i = 0; i < num_threads; i++) {
        data[i].start_row = i * rows_per_thread;

        if (i == num_threads - 1) {
            data[i].end_row = size;
        } else {
            data[i].end_row = (i + 1) * rows_per_thread;
        }

        data[i].size = size;
        data[i].A = A;
        data[i].B = B;
        data[i].C = C;

        // Создание нового потока
        int rc = pthread_create(&threads[i], NULL, multiply, (void *)&data[i]);
        if (rc) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Ожидание завершения всех потоков
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Освобождение памяти
    for (int i = 0; i < size; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    return 0;
}
