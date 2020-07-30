#pragma once

#include <types.h>

const char *basename(char *path);
const char *dirname(char *path);
int pathcanon(const char *srcpath, char *dstpath, size_t sz);
