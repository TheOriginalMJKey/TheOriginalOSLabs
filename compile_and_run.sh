#!/bin/bash

# Компилируем дочерний процесс
gcc -o child child.c -lm
if [ $? -ne 0 ]; then
    echo "Ошибка компиляции child.c"
    exit 1
fi

# Компилируем родительский процесс
gcc -o parent parent.c
if [ $? -ne 0 ]; then
    echo "Ошибка компиляции parent.c"
    exit 1
fi

# Запускаем родительский процесс через strace и перенаправляем вывод в файл
strace -o strace_output.txt -f ./parent
