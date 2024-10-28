#!/bin/bash

# Компиляция программы
gcc main.c -o main -lpthread

# Функция для замера времени выполнения программы
function get_exec_time {
    local start_time=$(date +%s%3N)
    ./main $1 $2 $3 > output.txt
    local end_time=$(date +%s%3N)
    echo $((end_time-start_time))
}

# Функция для проверки количества потоков
function check_threads {
    ./main $1 $2 $3 > /dev/null &
    PID=$!

    max=0

    # Проверяем количество потоков
    while kill -0 $PID 2> /dev/null; do
        # Считываем количество потоков, если файл /proc/$PID/status существует
        if [ -e /proc/$PID/status ]; then
            THREADS=$(grep Threads /proc/$PID/status | awk '{print $2}')

            # Обновляем максимальное количество потоков, если значение увеличилось
            if [ -n "$THREADS" ] && [ $THREADS -gt $max ]; then
                max=$THREADS
            fi
        fi
        sleep 0.1
    done

    echo $(($max - 1))
}

# Функция для запуска тестов
function run_test {
    echo "Running test: $1"
    echo "Matrix size: $2, Number of threads: $3, Max threads: $4"

    # Замер времени выполнения
    Tp=$(get_exec_time $2 $3 $4)
    echo "Execution time: $Tp ms"

    # Проверка количества потоков
    max_threads=$(check_threads $2 $3 $4)
    echo "Maximum number of threads used: $max_threads"

    # Проверка корректности результата
    expected_output=$(cat expected_output_$2_$3_$4.txt)
    actual_output=$(cat output.txt)
    if [ "$expected_output" == "$actual_output" ]; then
        echo "Test passed"
    else
        echo "Test failed"
    fi

    echo ""
}

# Тесты
run_test "Test 1: Small matrix 2x2" 2 2 2
run_test "Test 2: Medium matrix 5x5" 5 4 4
run_test "Test 3: Large matrix 10x10" 10 8 8
