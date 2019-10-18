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
		if (size > _reserved)
		{
			delete[] _items;
			_items = new T[size];
			_reserved = size;
		}
	}

	void SetSize(int size)
	{
		Reserve(size);
		_size = size;
	}

	void Clear()
	{
		_size = 0;
	}

	void Push(const T& item)
	{
		_items[_size++] = item;
	}

	void Sort()
	{
		for (int i = 0; i < _size - 1; ++i)
			for (int j = 0; j < _size - i - 1; ++j)
				if (_items[j] > _items[j + 1])
				{
					float temp = _items[j + 1];
					_items[j + 1] = _items[j];
					_items[j] = temp;
				}
	}

	T& operator [] (int i) { return _items[i]; }
	const T& operator [] (int i) const { return _items[i]; }
	
	int GetSize() const { return _size; }

	bool IsEmpty() const
	{
		return _size == 0;
	}

	bool IsFull() const { return _size == _reserved; }

	bool Contains(const T& item) const
	{
		for (int i = 0; i < _size; ++i)
			if (_items[i] == item)
				return true;
		
		return false;
	}

protected:
	int _size = 0;
	int _reserved = 0;
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