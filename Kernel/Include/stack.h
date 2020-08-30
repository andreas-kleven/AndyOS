#pragma once
#include <panic.h>

template <class T> struct Stack_Node
{
  public:
    T value;
    Stack_Node<T> *next;
    Stack_Node<T> *prev;

    Stack_Node<T>(T value);
    Stack_Node<T>(T value, Stack_Node<T> *next);
};

template <class T> inline Stack_Node<T>::Stack_Node(T value)
{
    this->value = value;
    this->next = this;
}

template <class T> inline Stack_Node<T>::Stack_Node(T value, Stack_Node<T> *next)
{
    this->value = value;
    this->next = next;
}

template <class T> class Stack
{
  public:
    Stack<T>()
    {
        count = 0;
        root = 0;
        last = 0;
    }

    ~Stack<T>() { Clear(); }

    void Push(T value)
    {
        if (count == 0) {
            root = new Stack_Node<T>(value);
            last = root;
            count = 1;
        } else {
            last->next = new Stack_Node<T>(value, 0);
            last = last->next;
            count++;
        }
    }

    T Pop()
    {
        if (count == 0)
            panic("Stack is empty", "Stack::Pop");

        T val = last->value;

        if (count == 1) {
            root = 0;
            last = 0;
            count = 0;
        } else {
            Stack_Node<T> *node = last;
            last = NodeAt(count - 2);

            delete node;
            count--;
        }

        return val;
    }

    void Clear()
    {
        if (!root)
            return;

        Stack_Node<T> *node = root;
        while (count--) {
            Stack_Node<T> *next = node->next;
            delete node;
            node = next;
        }

        root = 0;
        last = 0;
    }

    int Count() { return count; }

    T &operator[](int index)
    {
        if (index >= count)
            panic("Index out of range exception", "Stack::operator[]");

        Stack_Node<T> *node = NodeAt(index);
        return node->value;
    }

  private:
    int count;
    Stack_Node<T> *root;
    Stack_Node<T> *last;

    Stack_Node<T> *NodeAt(int index)
    {
        if (index == count - 1)
            return last;

        Stack_Node<T> *node = root;
        while (index--)
            node = node->next;

        return node;
    }
};