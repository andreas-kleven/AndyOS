#pragma once

template <class T>
struct Queue_Node
{
public:
	T value;
	Queue_Node<T>* next;
	Queue_Node<T>* prev;

	Queue_Node<T>(T value);
	Queue_Node<T>(T value, Queue_Node<T>* next);
};

template <class T>
inline Queue_Node<T>::Queue_Node(T value)
{
	this->value = value;
	this->next = this;
}

template <class T>
inline Queue_Node<T>::Queue_Node(T value, Queue_Node<T>* next)
{
	this->value = value;
	this->next = next;
}


template <class T>
class Queue
{
public:
	Queue<T>();
	~Queue<T>();

	void Enqueue(T value);
	T Dequeue();
	void Clear();

	int Count() { return count; }

	T& operator[](int index);

private:
	int count;
	Queue_Node<T>* root;
	Queue_Node<T>* last;

	Queue_Node<T>* NodeAt(int index);
};

template<class T>
inline Queue<T>::Queue()
{
	count = 0;
	root = 0;
	last = 0;
}

template<class T>
inline Queue<T>::~Queue()
{
	Clear();
}

template<class T>
void Queue<T>::Enqueue(T value)
{
	if (count == 0)
	{
		root = new Queue_Node<T>(value);
		last = root;
		count = 1;
	}
	else
	{
		last->next = new Queue_Node<T>(value, root);
		last = last->next;
		count++;
	}
}

template<class T>
inline T Queue<T>::Dequeue()
{
	//if (count == 0)
	//	throw "Queue is empty";

	T val = root->value;

	if (count == 1)
	{
		root = 0;
		last = 0;
		count = 0;
	}
	else
	{
		Queue_Node<T>* node = root;
		root = node->next;

		delete node;
		count--;
	}

	return val;
}

template<class T>
inline void Queue<T>::Clear()
{
	if (!root)
		return;

	Queue_Node<T>* node = root;
	while (count--)
	{
		Queue_Node<T>* next = node->next;
		delete node;
		node = next;
	}

	root = 0;
	last = 0;
}


template<class T>
inline T& Queue<T>::operator[](int index)
{
	//if (index >= count)
	//	throw "Index out of range exception";

	Queue_Node<T>* node = NodeAt(index);
	return node->value;
}

template<class T>
inline Queue_Node<T>* Queue<T>::NodeAt(int index)
{
	if (index == count - 1)
		return last;

	Queue_Node<T>* node = root;
	while (index--)
		node = node->next;

	return node;
}
