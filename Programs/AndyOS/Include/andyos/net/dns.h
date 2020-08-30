#pragma once
#include <sys/types.h>

#define DNS_CACHE_SIZE 64

namespace DNS {
void AddEntry(const char *name, uint32_t addr);
uint32_t Lookup(const char *name);
uint32_t Query(const char *name);
}; // namespace DNS
