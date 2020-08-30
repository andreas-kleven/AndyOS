#pragma once
#include <debug.h>
#include <errno.h>
#include <stdio.h>
#include <sync.h>

class CircularDataBuffer
{
  private:
    int head;
    int tail;
    bool empty;
    char *buffer;
    int size;

  public:
    CircularDataBuffer()
    {
        this->size = 0;
        this->buffer = 0;
        Clear();
    }

    CircularDataBuffer(int size) : CircularDataBuffer()
    {
        this->size = size;
        this->buffer = new char[size];
    }

    ~CircularDataBuffer() { delete[] buffer; }

    void Clear()
    {
        this->head = 0;
        this->tail = 0;
        this->empty = true;
    }

    int Write(const void *data, size_t length)
    {
        char *ptr = (char *)data;

        for (size_t i = 0; i < length; i++) {
            if (!WriteOne(ptr[i]))
                return i;
        }

        return length;
    }

    int Read(size_t length, void *data)
    {
        char *ptr = (char *)data;

        for (size_t i = 0; i < length; i++) {
            if (!ReadOne(&ptr[i]))
                return i;
        }

        return length;
    }

    int Seek(long offset, int whence)
    {
        switch (whence) {
        case SEEK_SET:
            tail = head + 1 + offset;
            break;

        case SEEK_CUR:
            tail += offset;
            break;

        case SEEK_END:
            tail = head + offset;
            break;

        default:
            return -EINVAL;
        }

        tail = tail % size;

        if (tail < 0)
            tail += size;

        empty = tail == head;
        return 0;
    }

    bool IsEmpty() { return empty; }

    bool IsFull() { return !empty && head == tail; }

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

template <class T> class CircularBuffer
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

    CircularBuffer<T>(int size) : CircularBuffer()
    {
        this->size = size;
        this->items = new T[size];
    }

    ~CircularBuffer<T>() { delete[] items; }

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

    bool IsEmpty() { return empty; }

    bool IsFull() { return !empty && head == tail; }
};
