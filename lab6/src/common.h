#pragma once
#include <stdbool.h>
#include <stdint.h>

bool     ConvertStringToUI64(const char *str, uint64_t *val);
uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);
