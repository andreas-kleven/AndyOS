#pragma once
#include "Kernel/exceptions.h"

template <class T>
struct Node
{
public:
	T value;
	Node<T>* next;

	Node<T>(T value);
	Node<T>(T value, Node<T>* next);
};

template <class T>
inline Node<T>::Node(T value)
{
	this->value = value;
	this->next = this;
}

template <class T>
inline Node<T>::Node(T value, Node<T>* next)
{
	this->value = value;
	this->next = next;
}


template <class T>
class List
{
public:
	List<T>();
	~List<T>();

	void Add(T value);
	void Insert(T value, int index);
	void RemoveAt(int index);
	void Remove(T rem);
	int IndexOf(T value);
	void Clear();

	int Count() { return count; }

	T& operator[](int index);

private:
	int count;
	Node<T>* root;
	Node<T>* last;

	Node<T>* NodeAt(int index);
};

template<class T>
inline List<T>::List()
{
	count = 0;
	root = 0;
	last = 0;
}

template<class T>
inline List<T>::~List()
{
	Clear();
}

template<class T>
void List<T>::Add(T value)
{
	if (count == 0)
	{
		root = new Node<T>(value);
		last = root;
		count = 1;
	}
	else
	{
		last->next = new Node<T>(value, root);;
		last = last->next;
		count++;
	}
}

template<class T>
inline void List<T>::Insert(T value, int index)
{
	if (index > count)
		OS::ThrowException("Index out of range exception", "List::Insert");

	if (index == count)
		return Add(value);

	if (index == 0)
	{
		last->next = new Node<T>(value, root);
		root = last->next;
		count++;
	}
	else
	{
		Node<T>* node = NodeAt(index - 1);
		node->next = new Node<T>(value, node->next);
		count++;
	}
}

template<class T>
inline void List<T>::RemoveAt(int index)
{
	if (index >= count)
		OS::ThrowException("Index out of range exception", "List::RemoveAt");

	if (index == 0)
	{
		last->next = root->next;
		delete root;
		root = last->next;
		count--;
	}
	else if (index == count - 1)
	{
		Node<T>* node = NodeAt(count - 1);
		Node<T>* new_next = node->next->next;
		node->next = new_next;
		last = node->next;
		count--;
		delete node;
	}
	else
	{
		Node<T>* node = NodeAt(index - 1);
		Node<T>* new_next = node->next->next;
		node->next = new_next;
		count--;
		delete node;
	}
}

template<class T>
inline void List<T>::Remove(T rem)
{
	OS::ThrowException("Not tested exception", "List::Remove");
	Node<T>* node = root;

	for (int i = 0; i < count; i++)
	{
		Node<T>* next = node->next;
		if (next->value == rem)
		{
			node->next = next->next;
			delete next;
		}

		node = next;
	}
}

template<class T>
inline int List<T>::IndexOf(T value)
{
	Node<T>* node = root;

	for (int i = 0; i < count; i++)
	{
		if (node->value == value)
		{
			return i;
		}

		node = node->next;
	}

	return -1;
}

template<class T>
inline void List<T>::Clear()
{
	if (!root)
		return;

	Node<T>* node = root;
	while (count--)
	{
		Node<T>* next = node->next;
		delete node;
		node = next;
	}

	root = 0;
	last = 0;
}


template<class T>
inline T& List<T>::operator[](int index)
{
	if (index >= count)
		Exceptions::ThrowException("Index out of range exception", "List::operator[]");

	Node<T>* node = NodeAt(index);
	return node->value;
}

template<class T>
inline Node<T>* List<T>::NodeAt(int index)
{
	if (index == count - 1)
		return last;

	Node<T>* node = root;
	while (index--)
		node = node->next;

	return node;
}
