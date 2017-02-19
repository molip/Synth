#pragma once 

#include "Config.h"
#include "Array.h"
#include "Common.h"

#include <arduino.h>

template <typename T>
class OutputT
{
public:
	void SetValue(T val, bool forceChanged = false)
	{
		_changed = forceChanged || val != _value;
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
class InputT
{
public:
	T GetValue() const { return _source->GetValue(); }
	bool HasChanged() const { return _source->HasChanged(); }
	void Connect(OutputT<T>& output) { _source = &output; }
private:
	OutputT<T>* _source = nullptr; 
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

class UnsignedValueModule : public Module
{
public:
	UnsignedValueModule();
	void SetValue(unsigned_t val);
};
