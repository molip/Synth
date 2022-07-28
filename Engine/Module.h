#pragma once 

#include "Config.h"
#include "Array.h"
#include "Common.h"

#include "Platform.h"

namespace Engine
{

class Settings
{
public:
	bool hold = false;
	bool arpEnabled = false;
	uint16_t arpPeriod = 0;
	uint16_t arpDuty = 0;
	uint16_t arpOctaves = 0;
};
	
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
	float GetValue() const { return _value; }

	void SetConnected() { _connected = true; }
	bool IsConnected() const { return _connected; }

private:
	float _value = 0;
	Array<Input*> _dests;
	bool _connected = false;
};

class Input
{
public:
	Input() = default;
	Input(const Input&) = delete;

	void SetValue(float val)
	{
		_rawValue = val;
		Update();
	}

	void SetParams(float offset, float scale)
	{
		_offset = offset;
		_scale = scale;
		Update();
	}

	float GetValue() const { return _value; }
	bool HasChanged() const { return _changed; }
	bool IsConnected() const { return _connected; }
	void Connect(Output& output) { output.AddDest(*this); output.SetConnected();  _connected = true; }
	void ResetChanged() { _changed = false; }

private:
	void Update()
	{
		_value = _offset + _rawValue * _scale;
		_changed = true; 
	}

	bool _changed = true;
	bool _connected = false;
	float _rawValue = 0;
	float _value = 0;
	float _offset = 0;
	float _scale = 1;
};

class Field
{
public:
	Field() = default;
	Field(const Field&) = delete;

	using DataType = Array<uint8_t>;

	void AttachData(DataType* data)
	{
		delete _data;
		_data = data;
		_changed = true;
	}

	const DataType* GetData() const { return _data; }
	bool HasChanged() const { return _changed; }
	void ResetChanged() { _changed = false; }

private:
	bool _changed = true;
	DataType* _data = nullptr;
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

	template<typename T> const Array<T>& GetPins() const;
	template<typename T> Array<T>& GetPins();
	template<typename T> Array<Array<T>>& GetMultiPins();

	Array<Field>& GetFields() { return _fields; }

protected:
	Array<Output> _outputs;
	Array<MultiOutput> _multiOutputs;

	Array<Input> _inputs;
	Array<MultiInput> _multiInputs;

	Array<Field> _fields;
};
}