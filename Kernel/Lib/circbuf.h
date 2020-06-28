#pragma once
#include "Lib/debug.h"
#include "sync.h"

class CircularDataBuffer
{
private:
	int head;
	int tail;
	bool empty;
	int size;
	char *buffer;

public:
	CircularDataBuffer()
	{
		this->size = 0;
		this->buffer = 0;
		Clear();
	}

	CircularDataBuffer(int size)
		: CircularDataBuffer()
	{
		this->size = size;
		this->buffer = new char[size];
	}

	~CircularDataBuffer()
	{
		delete[] buffer;
	}

	void Clear()
	{
		this->head = 0;
		this->tail = 0;
		this->empty = true;
	}

	int Write(const char *data, size_t length)
	{
		for (size_t i = 0; i < length; i++)
		{
			if (!WriteOne(data[i]))
				return i;
		}

		return length;
	}

	int Read(int length, char *data)
	{
		for (int i = 0; i < length; i++)
		{
			if (!ReadOne(&data[i]))
				return i;
		}

		return length;
	}

	bool IsEmpty()
	{
		return empty;
	}

	bool IsFull()
	{
		return !empty && head == tail;
	}

private:
	bool WriteOne(char c)
	{
		if (size == 0)
			return false;

		if (!empty && head == tail)
			return false;

		buffer[head] = c;
		head = (head + 1) % size;

		empty = false;
		return 1;
	}

	bool ReadOne(char *c)
	{
		if (size == 0)
			return false;

		if (empty)
			return false;

		*c = buffer[tail];
		tail = (tail + 1) % size;

		empty = tail == head;
		return true;
	}
};

template <class T>
class CircularBuffer
{
private:
	int head;
	int tail;
	bool empty;
	int size;
	T *items;

public:
	CircularBuffer()
	{
		this->head = 0;
		this->tail = 0;
		this->empty = true;
		this->size = 0;
	}

	CircularBuffer<T>(int size)
		: CircularBuffer()
	{
		this->size = size;
		this->items = new T[size];
	}

	~CircularBuffer<T>()
	{
		delete items;
	}

	bool Add(const T &item)
	{
		if (size == 0)
			return false;

		if (!empty && head == tail)
			return false;

		items[head] = item;
		head = (head + 1) % size;

		empty = false;
		return 1;
	}

	T *Pop()
	{
		if (size == 0)
			return 0;

		if (empty)
			return 0;

		T *item = Get();
		tail = (tail + 1) % size;
		empty = tail == head;
		return item;
	}

	T *Get()
	{
		if (size == 0)
			return 0;

		if (empty)
			return 0;

		return &items[tail];
	}

	bool IsEmpty()
	{
		return empty;
	}

	bool IsFull()
	{
		return !empty && head == tail;
	}
};