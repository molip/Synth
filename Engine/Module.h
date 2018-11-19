#pragma once 

#include "Config.h"
#include "Array.h"
#include "Common.h"

#include "Platform.h"

namespace Engine
{
class Input;

class Output
{
public:
	Output()
	{
		_dests.Reserve(10); //TODO: Upload actual size.
	}

	void AddDest(Input& input) { _dests.Push(&input); }

	void SetValue(float val, bool forceChanged = false);

private:
	float _value = 0;
	Array<Input*> _dests;
};

class Input
{
public:
	Input() = default;
	Input(const Input&) = delete;

	void SetValue(float val)
	{
		_value = val;
		_changed = true; // It's up to the output to only call this when necessary. 
	}

	float GetValue() const { return _value; }
	bool HasChanged() const { return _changed; }
	bool IsConnected() const { return _connected; }
	void Connect(Output& output) { output.AddDest(*this); _connected = true; }
	void ResetChanged() { _changed = false; }

private:
	bool _changed = false;
	bool _connected = false;
	float _value = 0;
};

inline void Output::SetValue(float val, bool forceChanged)
{
	if (forceChanged || val != _value)
	{
		for (int i = 0; i < _dests.GetSize(); ++i)
			_dests[i]->SetValue(val);

		_value = val;
	}
}


using MultiOutput = Array<Output>;
using MultiInput = Array<Input>;

class Module
{
public:
	virtual ~Module() = default;
	virtual void Initialise() {}
	virtual void Update() {}
	virtual bool WantUpdate() const { return false; }

	void SetPolyphony(int polyphony);

	template<typename T> Array<T>& GetPins();
	template<typename T> Array<Array<T>>& GetMultiPins();

protected:
	Array<Output> _outputs;
	Array<MultiOutput> _multiOutputs;

	Array<Input> _inputs;
	Array<MultiInput> _multiInputs;
};
}