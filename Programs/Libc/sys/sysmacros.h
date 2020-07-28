#pragma once

#define major(dev) ((dev >> 16) & 0xffff)
#define minor(dev) (dev & 0xffff)
