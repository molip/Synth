#pragma once 

#include "Config.h"
#include "Array.h"
#include "Common.h"

#include <arduino.h>

template <typename T> class InputT;

template <typename T>
class OutputT
{
public:
	OutputT()
	{
		_dests.Reserve(10); //TODO: Upload actual size.
	}

	void AddDest(InputT<T>& input) { _dests.Push(&input); }

	void SetValue(T val, bool forceChanged = false)
	{
		if (forceChanged || val != _value)
		{
			for (size_t i = 0; i < _dests.GetSize(); ++i)
				_dests[i]->SetValue(val);
			
			_value = val;
		}
	}

private:
	T _value = 0;
	Array<InputT<T>*> _dests;
};

template <typename T>
class InputT
{
public:
	void SetValue(T val)
	{
		_value = val;
		_changed = true; // It's up to the output to only call this when necessary. 
	}

	T GetValue() const { return _value; }
	bool HasChanged() const { return _changed; }
	void Connect(OutputT<T>& output) { output.AddDest(*this); }
	void ResetChanged() { _changed = false; }

private:
	bool _changed = false;
	T _value = 0;
};

using SignedOutput = OutputT<int16_t>;
using UnsignedOutput = OutputT<uint16_t>;
using SignedMultiOutput = Array<OutputT<int16_t>>;
using UnsignedMultiOutput = Array<OutputT<uint16_t>>;

using SignedInput = InputT<int16_t>;
using UnsignedInput = InputT<uint16_t>;
using SignedMultiInput = Array<InputT<int16_t>>;
using UnsignedMultiInput = Array<InputT<uint16_t>>;

class Module
{
public:
	using signed_t = int16_t;
	using unsigned_t = uint16_t;
	const unsigned_t UnsignedMax = 0xffff;

	virtual ~Module() = default;
	virtual void Update() {}
	virtual bool WantUpdate() const { return false; }

	void SetPolyphony(int polyphony);

	template<typename T> Array<T>& GetPins();
	template<typename T> Array<Array<T>>& GetMultiPins();

protected:
	Array<SignedOutput> _signedOutputs;
	Array<UnsignedOutput> _unsignedOutputs;
	Array<SignedMultiOutput> _signedMultiOutputs;
	Array<UnsignedMultiOutput> _unsignedMultiOutputs;

	Array<SignedInput> _signedInputs;
	Array<UnsignedInput> _unsignedInputs;
	Array<SignedMultiInput> _signedMultiInputs;
	Array<UnsignedMultiInput> _unsignedMultiInputs;
};
