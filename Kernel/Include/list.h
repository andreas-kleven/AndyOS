#pragma once
#include <panic.h>

template <class T> struct Node
{
  public:
    T value;
    Node<T> *next;

    Node<T>(T value);
    Node<T>(T value, Node<T> *next);
};

template <class T> inline Node<T>::Node(T value)
{
    this->value = value;
    this->next = this;
}

template <class T> inline Node<T>::Node(T value, Node<T> *next)
{
    this->value = value;
    this->next = next;
}

template <class T> class List
{
  public:
    List<T>()
    {
        count = 0;
        root = 0;
        last = 0;
    }

    ~List<T>() { Clear(); }

    void Add(const T &value)
    {
        if (count == 0) {
            root = new Node<T>(value);
            last = root;
            count = 1;
        } else {
            last->next = new Node<T>(value, root);
            last = last->next;
            count++;
        }
    }

    void Insert(const T &value, int index)
    {
        if (index > count)
            panic("Index out of range", "List::Insert");

        if (index == count)
            return Add(value);

        if (index == 0) {
            last->next = new Node<T>(value, root);
            root = last->next;
            count++;
        } else {
            Node<T> *node = NodeAt(index - 1);
            node->next = new Node<T>(value, node->next);
            count++;
        }
    }

    void RemoveAt(int index)
    {
        if (index > count)
            panic("Index out of range", "List::RemoveAt");

        if (index == 0) {
            Node<T> *next = root->next;
            last->next = root->next;
            delete root;
            root = next;
            count--;
        } else if (index == count - 1) {
            Node<T> *node = NodeAt(count - 1);
            Node<T> *new_next = node->next->next;
            node->next = new_next;
            last = node->next;
            count--;
            delete node;
        } else {
            Node<T> *node = NodeAt(index - 1);
            Node<T> *new_next = node->next->next;
            node->next = new_next;
            count--;
            delete node;
        }
    }

    void Remove(const T &rem)
    {
        Node<T> *node = root;

        for (int i = 0; i < count; i++) {
            Node<T> *next = node->next;
            if (next->value == rem) {
                RemoveAt(i);
                return;
            }

            node = next;
        }
    }

    int IndexOf(const T &value)
    {
        Node<T> *node = root;

        for (int i = 0; i < count; i++) {
            if (node->value == value) {
                return i;
            }

            node = node->next;
        }

        return -1;
    }

    void Clear()
    {
        if (!root)
            return;

        Node<T> *node = root;
        while (count--) {
            Node<T> *next = node->next;
            delete node;
            node = next;
        }

        root = 0;
        last = 0;
    }

    int Count() { return count; }

    inline T &First() { return root->value; }

    inline T &Last() { return last->value; }

    T &operator[](int index)
    {
        if (index > count)
            panic("Index out of range", "List::operator[]");

        Node<T> *node = NodeAt(index);
        return node->value;
    }

  private:
    int count;
    Node<T> *root;
    Node<T> *last;

    Node<T> *NodeAt(int index)
    {
        if (index == count - 1)
            return last;

        Node<T> *node = root;
        while (index--)
            node = node->next;

        return node;
    }
};
