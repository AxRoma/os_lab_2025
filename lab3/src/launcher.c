// launcher.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <seed> <array_size>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        return 1;
    }
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Child process: Launching sequential_min_max with seed=%s, array_size=%s\n", 
               argv[1], argv[2]);
        
        // Подготавливаем аргументы для exec
        char *args[] = {"./sequential_min_max", argv[1], argv[2], NULL};
        
        // Запускаем sequential_min_max
        execvp(args[0], args);
        
        // Если execvp вернул управление - произошла ошибка
        perror("execvp failed");
        exit(1);
    } else {
        // Родительский процесс
        int status;
        waitpid(pid, &status, 0);  // Ждем завершения дочернего процесса
        
        if (WIFEXITED(status)) {
            printf("Parent process: sequential_min_max completed with exit code %d\n", 
                   WEXITSTATUS(status));
        } else {
            printf("Parent process: sequential_min_max terminated abnormally\n");
        }
    }
    
    return 0;
}