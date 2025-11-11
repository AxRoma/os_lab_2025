#include "common.h"
#include <errno.h>
#include <stdlib.h>

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a %= mod;
  while (b > 0) {
    if (b & 1) result = (result + a) % mod;
    a = (a * 2) % mod;
    b >>= 1;
  }
  return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  errno = 0;
  char *end = NULL;
  unsigned long long x = strtoull(str, &end, 10);
  if (errno == ERANGE || errno != 0 || end == str || *end != '\0') return false;
  *val = (uint64_t)x;
  return true;
}
