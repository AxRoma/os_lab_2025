#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Ошибка: Не переданы аргументы"
    echo "Использование: $0 число1 число2 ..."
    exit 1
fi

sum=0
count=$#

for num in "$@"; do
    sum=$((sum + num))
done

# Вычисляем целую часть и остаток
int_part=$((sum / count))
remainder=$((sum % count))

# Вычисляем дробную часть (примерно)
if [ $remainder -ne 0 ]; then
    # Умножаем остаток на 1000 и делим на count для получения 3 знаков после запятой
    fraction=$(( (remainder * 1000) / count ))
    # Добавляем ведущие нули если нужно
    if [ $fraction -lt 10 ]; then
        fraction="00$fraction"
    elif [ $fraction -lt 100 ]; then
        fraction="0$fraction"
    fi
    average="$int_part.$fraction"
else
    average=$int_part
fi

echo "Количество чисел: $count"
echo "Сумма чисел: $sum"
echo "Среднее арифметическое: $average"
