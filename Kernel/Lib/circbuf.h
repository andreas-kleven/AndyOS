#pragma once
#include "Lib/debug.h"

template <class T>
class CircularBuffer
{
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

	bool Add(const T& item)
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

	T* Get()
	{
		if (size == 0)
			return 0;

		if (empty)
			return 0;

		T* item = &items[tail];
		tail = (tail + 1) % size;

		empty = tail == head;
		return item;
	}

	bool IsEmpty()
	{
		return empty;
	}

private:
	int head;
	int tail;
	bool empty;
	int size;
	T* items;
};