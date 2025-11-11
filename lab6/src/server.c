// server.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>

#include "common.h"

struct FactorialArgs {
  uint64_t begin;  // включительно
  uint64_t end;    // включительно
  uint64_t mod;
};

static uint64_t Factorial(const struct FactorialArgs *args) {
  if (args->mod == 1) return 0;
  if (args->begin > args->end) return 1;
  uint64_t ans = 1;
  for (uint64_t x = args->begin; x <= args->end; ++x) {
    ans = MultModulo(ans, x, args->mod);
  }
  return ans % args->mod;
}

static void *ThreadFactorial(void *args) {
  struct FactorialArgs *fargs = (struct FactorialArgs *)args;
  uint64_t *res = (uint64_t *)malloc(sizeof(uint64_t));
  *res = Factorial(fargs);
  return res;
}

int main(int argc, char **argv) {
  int tnum = -1;
  int port = -1;

  while (true) {
    static struct option options[] = {
      {"port", required_argument, 0, 0},
      {"tnum", required_argument, 0, 0},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1) break;

    if (c == 0) {
      switch (option_index) {
        case 0: port = atoi(optarg); break;
        case 1: tnum = atoi(optarg); break;
        default: printf("Index %d is out of options\n", option_index);
      }
    } else if (c == '?') {
      printf("Unknown argument\n");
      return 1;
    } else {
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
      return 1;
    }
  }

  if (port == -1 || tnum <= 0) {
    fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) { fprintf(stderr, "Can not create server socket!"); return 1; }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) { fprintf(stderr, "Can not bind to socket!"); return 1; }

  err = listen(server_fd, 128);
  if (err < 0) { fprintf(stderr, "Could not listen on socket\n"); return 1; }

  printf("Server listening at %d\n", port);

  while (true) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
    if (client_fd < 0) { fprintf(stderr, "Could not establish new connection\n"); continue; }

    while (true) {
      unsigned int buffer_size = sizeof(uint64_t) * 3;
      char from_client[buffer_size];
      int readn = recv(client_fd, from_client, buffer_size, 0);

      if (!readn) break;
      if (readn < 0) { fprintf(stderr, "Client read failed\n"); break; }
      if (readn < (int)buffer_size) { fprintf(stderr, "Client send wrong data format\n"); break; }

      uint64_t begin = 0, end = 0, mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      fprintf(stdout, "Receive: %llu %llu %llu\n",
              (unsigned long long)begin,
              (unsigned long long)end,
              (unsigned long long)mod);

      pthread_t threads[tnum];
      struct FactorialArgs args[tnum];

      // делим [begin; end] на tnum частей
      uint64_t n = (end >= begin) ? (end - begin + 1) : 0;
      uint64_t base = (tnum > 0) ? (n / (uint64_t)tnum) : 0;
      uint64_t rem  = (tnum > 0) ? (n % (uint64_t)tnum) : 0;

      uint64_t cur = begin;
      for (int i = 0; i < tnum; i++) {
        uint64_t len = base + (i < (int)rem ? 1 : 0);
        args[i].begin = (len ? cur : 1);
        args[i].end   = (len ? (cur + len - 1) : 0);
        args[i].mod   = mod;
        cur += len;

        if (pthread_create(&threads[i], NULL, ThreadFactorial, (void *)&args[i])) {
          printf("Error: pthread_create failed!\n");
          close(client_fd); close(server_fd); return 1;
        }
      }

      uint64_t total = 1;
      for (int i = 0; i < tnum; i++) {
        uint64_t *result_ptr = NULL;
        pthread_join(threads[i], (void **)&result_ptr);
        uint64_t result = result_ptr ? *result_ptr : 1;
        free(result_ptr);
        total = MultModulo(total, result, mod);
      }

      printf("Total: %llu\n", (unsigned long long)total);

      char buffer[sizeof(total)];
      memcpy(buffer, &total, sizeof(total));
      err = send(client_fd, buffer, sizeof(total), 0);
      if (err < 0) { fprintf(stderr, "Can't send data to client\n"); break; }
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }

  return 0;
}
