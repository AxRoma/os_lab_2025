#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

// Глобальные переменные
long long result = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Структура для передачи данных в поток
typedef struct {
    int start;
    int end;
    int mod;
} thread_data_t;

// Функция, которую выполняет каждый поток
void* calculate_partial_factorial(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    long long partial_result = 1;
    
    printf("Thread calculating from %d to %d\n", data->start, data->end);
    
    // Вычисляем частичный факториал
    for (int i = data->start; i <= data->end; i++) {
        partial_result = (partial_result * i) % data->mod;
    }
    
    // Синхронизируем доступ к общему результату
    pthread_mutex_lock(&mutex);
    result = (result * partial_result) % data->mod;
    pthread_mutex_unlock(&mutex);
    
    printf("Partial result from %d to %d: %lld\n", data->start, data->end, partial_result);
    
    return NULL;
}

// Функция для разбора аргументов командной строки
void parse_arguments(int argc, char* argv[], int* k, int* pnum, int* mod) {
    // Значения по умолчанию
    *k = 10;
    *pnum = 4;
    *mod = 1000000;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            *k = atoi(argv[++i]);
        } else if (strstr(argv[i], "--pnum=") == argv[i]) {
            *pnum = atoi(argv[i] + 7);
        } else if (strstr(argv[i], "--mod=") == argv[i]) {
            *mod = atoi(argv[i] + 6);
        }
    }
}

int main(int argc, char* argv[]) {
    int k, pnum, mod;
    
    // Разбираем аргументы командной строки
    parse_arguments(argc, argv, &k, &pnum, &mod);
    
    printf("Calculating %d! mod %d using %d threads\n", k, mod, pnum);
    
    // Проверка входных данных
    if (k <= 0 || pnum <= 0 || mod <= 0) {
        printf("Error: All parameters must be positive integers\n");
        return 1;
    }
    
    if (pnum > k) {
        pnum = k;
        printf("Warning: pnum reduced to %d (cannot exceed k)\n", pnum);
    }
    
    // Создаем потоки
    pthread_t threads[pnum];
    thread_data_t thread_data[pnum];
    
    // Распределяем работу между потоками
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    for (int i = 0; i < pnum; i++) {
        int current_end = current_start + numbers_per_thread - 1;
        
        // Распределяем остаток
        if (remainder > 0) {
            current_end++;
            remainder--;
        }
        
        // Заполняем данные для потока
        thread_data[i].start = current_start;
        thread_data[i].end = current_end;
        thread_data[i].mod = mod;
        
        printf("Thread %d: numbers from %d to %d\n", i, current_start, current_end);
        
        // Создаем поток
        if (pthread_create(&threads[i], NULL, calculate_partial_factorial, &thread_data[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
        
        current_start = current_end + 1;
    }
    
    // Ждем завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }
    
    // Выводим результат
    printf("\nFinal result: %d! mod %d = %lld\n", k, mod, result);
    
    // Очищаем мьютекс
    pthread_mutex_destroy(&mutex);
    
    return 0;
}