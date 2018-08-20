#pragma once
#include "definitions.h"
#include "Drivers/driver.h"
#include "circbuf.h"

#define PIPE_BUF_SIZE 4096

class Pipe : public FileIO
{
private:
    CircularDataBuffer buffer;

public:
	Pipe(int buf_size = PIPE_BUF_SIZE);

	int Read(FILE* file, char* buf, size_t size);
    int Write(FILE* file, const char* buf, size_t size);
    int Seek(FILE* file, long offset, int origin);
};