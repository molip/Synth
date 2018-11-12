#pragma once 

namespace Engine
{
template <typename T>
class Array
{
public:
	~Array()
	{
		delete [] _items;
	}

	void Reserve(int size)
	{
		delete [] _items;
		_items = new T[size];
	}

	void SetSize(int size)
	{
		Reserve(size);
		_size = size;
	}

	void Push(const T& item)
	{
		_items[_size++] = item;
	}

	T& operator [] (int i) { return _items[i]; }
	const T& operator [] (int i) const { return _items[i]; }
	
	int GetSize() const { return _size; }

protected:
	int _size = 0;
	T* _items = nullptr;
};

template <typename T>
class PtrArray : public Array<T*>
{
public:
	~PtrArray()
	{
		for (int i = 0; i < Array<T*>::_size; ++i)
			delete Array<T*>::_items[i];
	}
};
}