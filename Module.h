#pragma once 

#include "Config.h"

#include <arduino.h>

template <typename T>
class Output
{
public:
	void SetValue(T val)
	{
		_changed = val != _value;
		_value = val;
	}
	T GetValue() const { return _value; }
	void ResetChanged() { _changed = false; }
	bool HasChanged() const { return _changed; }
private:
	T _value = 0;
	bool _changed = true;
};

template <typename T>
class Input
{
public:
	Output<T>* _source = nullptr; // TODO: array.
private:
};

using SignedOutput = Output<int16_t>;
using UnsignedOutput = Output<uint16_t>;

using SignedInput = Input<int16_t>;
using UnsignedInput = Input<uint16_t>;

class Module
{
public:
	using signed_t = int16_t;
	using unsigned_t = uint16_t;
	const unsigned_t UnsignedMax = 0xffff;

	virtual ~Module() = default;
	virtual void Update() = 0;
};
