#pragma once
template <class T>
class C_Vector
{
public:
	typedef T* iterator;

	C_Vector();
	C_Vector(unsigned int size);
	C_Vector(unsigned int size, const T& initial);
	C_Vector(const C_Vector<T>& v);
	~C_Vector();

	unsigned int capacity() const;
	unsigned int size() const;
	bool empty() const;
	iterator begin();
	iterator end();
	T& front() const;
	T& back() const;
	void push_back(const T& value);
	void pop_back();

	void reserve(unsigned int capacity);
	void resize(unsigned int size);

	T& operator[](unsigned int index);
	C_Vector<T> & operator=(const C_Vector<T> &);
	void clear();
private:
	unsigned int my_size;
	unsigned int my_capacity;
	T* buffer;
};

template<class T>
C_Vector<T>::C_Vector()
{
	my_capacity = 0;
	my_size = 0;
	buffer = 0;
}

template<class T>
C_Vector<T>::C_Vector(const C_Vector<T> & v)
{
	my_size = v.my_size;
	my_capacity = v.my_capacity;
	buffer = new T[my_size];
	for (unsigned int i = 0; i < my_size; i++)
		buffer[i] = v.buffer[i];
}

template<class T>
C_Vector<T>::C_Vector(unsigned int size)
{
	my_capacity = size;
	my_size = size;
	buffer = new T[size];
}

template<class T>
C_Vector<T>::C_Vector(unsigned int size, const T & initial)
{
	my_size = size;
	my_capacity = size;
	buffer = new T[size];
	for (unsigned int i = 0; i < size; i++)
		buffer[i] = initial;
	//T();
}

template<class T>
C_Vector<T> & C_Vector<T>::operator = (const C_Vector<T> & v)
{
	delete[] buffer;
	my_size = v.my_size;
	my_capacity = v.my_capacity;
	buffer = new T[my_size];
	for (unsigned int i = 0; i < my_size; i++)
		buffer[i] = v.buffer[i];
	return *this;
}

template<class T>
typename C_Vector<T>::iterator C_Vector<T>::begin()
{
	return buffer;
}

template<class T>
typename C_Vector<T>::iterator C_Vector<T>::end()
{
	return buffer + size();
}

template<class T>
T& C_Vector<T>::front() const
{
	return buffer[0];
}

template<class T>
T& C_Vector<T>::back() const
{
	return buffer[my_size - 1];
}

template<class T>
void C_Vector<T>::push_back(const T & v)
{
	if (my_size >= my_capacity)
		reserve(my_capacity + 5);
	buffer[my_size++] = v;
}

template<class T>
void C_Vector<T>::pop_back()
{
	my_size--;
}

template<class T>
void C_Vector<T>::reserve(unsigned int capacity)
{
	if (buffer == 0)
	{
		my_size = 0;
		my_capacity = 0;
	}
	T * Newbuffer = new T[capacity];
	//assert(Newbuffer);
	unsigned int l_Size = capacity < my_size ? capacity : my_size;
	//copy (buffer, buffer + l_Size, Newbuffer);

	for (unsigned int i = 0; i < l_Size; i++) {
		Newbuffer[i] = buffer[i];
	}

	my_capacity = capacity;
	delete[] buffer;
	buffer = Newbuffer;
}

template<class T>
unsigned int C_Vector<T>::size()const//
{
	return my_size;
}

template<class T>
void C_Vector<T>::resize(unsigned int size)
{
	reserve(size);
	my_size = size;
}

template<class T>
T& C_Vector<T>::operator[](unsigned int index)
{
	return buffer[index];
}

template<class T>
unsigned int C_Vector<T>::capacity()const
{
	return my_capacity;
}

template <class T>
bool C_Vector<T>::empty() const {
	return my_size == 0;
}

template<class T>
C_Vector<T>::~C_Vector()
{
	delete[] buffer;
}

template <class T>
void C_Vector<T>::clear()
{
	my_capacity = 0;
	my_size = 0;
	buffer = 0;
}