// client.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <arpa/inet.h>      // inet_pton
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>

#include "common.h"

struct Server {
  char ip[255];
  int port;
};

int main(int argc, char **argv) {
  uint64_t k = (uint64_t)-1;
  uint64_t mod = (uint64_t)-1;
  char servers[255] = {'\0'}; // аргумент обязателен, в задании 1 не используем

  while (true) {
    static struct option options[] = {
      {"k", required_argument, 0, 0},
      {"mod", required_argument, 0, 0},
      {"servers", required_argument, 0, 0},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1) break;

    if (c == 0) {
      switch (option_index) {
        case 0:
          if (!ConvertStringToUI64(optarg, &k)) { fprintf(stderr,"Bad --k\n"); return 1; }
          break;
        case 1:
          if (!ConvertStringToUI64(optarg, &mod)) { fprintf(stderr,"Bad --mod\n"); return 1; }
          break;
        case 2:
          memcpy(servers, optarg, strlen(optarg));
          break;
        default:
          printf("Index %d is out of options\n", option_index);
      }
    } else if (c == '?') {
      printf("Arguments error\n");
      return 1;
    } else {
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
      return 1;
    }
  }

  if (k == (uint64_t)-1 || mod == (uint64_t)-1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n", argv[0]);
    return 1;
  }

  // Задание 1: один сервер 127.0.0.1:20001
  unsigned int servers_num = 1;
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  to[0].port = 20001;
  memcpy(to[0].ip, "127.0.0.1", sizeof("127.0.0.1"));

  for (unsigned i = 0; i < servers_num; i++) {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(to[i].port);

    // ⬇️ вместо gethostbyname используем inet_pton
    if (inet_pton(AF_INET, to[i].ip, &server.sin_addr) != 1) {
      fprintf(stderr, "Bad IP address: %s\n", to[i].ip);
      free(to);
      return 1;
    }

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      free(to);
      return 1;
    }

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed\n");
      close(sck);
      free(to);
      return 1;
    }

    uint64_t begin = 1;
    uint64_t end = k;

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      close(sck);
      free(to);
      return 1;
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
      fprintf(stderr, "Receive failed\n");
      close(sck);
      free(to);
      return 1;
    }

    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));
    printf("answer: %llu\n", (unsigned long long)answer);

    close(sck);
  }

  free(to);
  return 0;
}
