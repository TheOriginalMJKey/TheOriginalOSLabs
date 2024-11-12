#!/bin/bash

# Компилируем программу main.c с использованием gcc и библиотеки pthread
gcc main.c -o main -lpthread

# Функция для измерения времени выполнения программы
function get_exec_time {
    # Записываем текущее время в миллисекундах
    local start_time=$(date +%s%3N)

    # Запускаем программу main с переданными аргументами и перенаправляем вывод в файл output.txt
    ./main $1 $2 $3 > output.txt

    # Записываем текущее время в миллисекундах после завершения программы
    local end_time=$(date +%s%3N)

    # Вычисляем разницу между начальным и конечным временем и выводим результат
    echo $((end_time - start_time))
}

# Функция для отслеживания максимального количества потоков, используемых программой
function check_max_threads {
    # Запускаем программу main с переданными аргументами в фоновом режиме и перенаправляем вывод в /dev/null
    ./main $1 $2 $3 > /dev/null &

    # Записываем PID запущенной программы
    PID=$!

    # Инициализируем переменную для отслеживания максимального количества потоков
    max_threads=0

    # Цикл для отслеживания количества потоков, пока программа выполняется
    while kill -0 $PID 2> /dev/null; do
        # Извлекаем количество потоков из файла /proc/$PID/status
        THREADS=$(grep Threads /proc/$PID/status | awk '{print $2}')

        # Если количество потоков больше текущего максимального значения, обновляем максимальное значение
        [ "$THREADS" -gt "$max_threads" ] && max_threads=$THREADS

        # Задержка на 0.1 секунду перед следующей проверкой
        sleep 0.1
    done

    # Вычитаем один из общего количества потоков, чтобы исключить оригинальный поток
    echo $((max_threads - 1))
}

# Функция для запуска тестов с различными параметрами
function run_test {
    # Записываем переданные аргументы в локальные переменные
    local size=$1
    local num_threads=$2
    local max_threads=$3

    # Выводим информацию о текущем тесте
    echo "Running test: Matrix size = $size, Num threads = $num_threads, Max threads = $max_threads"

    # Измеряем время выполнения программы в однопоточном режиме
    single_thread_time=$(get_exec_time $size 1 1)
    echo "Single-thread execution time: $single_thread_time ms"

    # Измеряем время выполнения программы в многопоточном режиме
    multi_thread_time=$(get_exec_time $size $num_threads $max_threads)
    echo "Multi-thread execution time: $multi_thread_time ms"

    # Отслеживаем максимальное количество потоков, используемых программой
    actual_max_threads=$(check_max_threads $size $num_threads $max_threads)
    echo "Actual max threads used: $actual_max_threads"

    # Проверяем, ускоряет ли многопоточность выполнение программы
    if (( multi_thread_time < single_thread_time )); then
        echo "PASS: Multi-threading is faster than single-threading."
    else
        echo "FAIL: Multi-threading did not improve performance."
    fi

    # Проверяем, не превышает ли количество потоков заданное ограничение
    if (( actual_max_threads <= max_threads )); then
        echo "PASS: Thread count did not exceed limit."
    else
        echo "FAIL: Thread count exceeded the limit."
    fi
    echo ""
}

# Запускаем тесты с различными параметрами
run_test 1000 4 4
run_test 1000 8 8
run_test 1000 16 16
