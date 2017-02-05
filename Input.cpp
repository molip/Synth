#include "Input.h"

namespace Input 
{

namespace 
{
	Command* _command;
	Graph* _graph;
	bool _gotError;
}

void SendError(Error error)
{
	Serial.print("Error: ");
	Serial.println((int)error);
}

void Process(byte data)
{
	if (_command)
	{
		Error error = _command->AddData(data);
		if (error != Error::None)
		{
			SendError(error);
			_gotError = true;
		}
	}
	else
	{
		switch (static_cast<CommandType>(data))
		{
		case CommandType::InitGraph: _command = new InitGraphCommand; break;
		case CommandType::AddMonoModule: _command = new AddMonoModuleCommand; break;
		case CommandType::AddPolyModule: _command = new AddPolyModuleCommand; break;
		case CommandType::AddConnection: _command = new AddConnectionCommand; break;
		case CommandType::SetMonoUnsignedValue: _command = new SetMonoUnsignedValueCommand; break;
		case CommandType::SetPolyUnsignedValue: _command = new SetPolyUnsignedValueCommand; break;
		case CommandType::StartGraph: 
		{
			if (_graph)
			{
				SendError(Error::GraphAlreadyStarted);
				_gotError = true;
			}
			else 
				_graph = new Graph;
			break;
		}
		case CommandType::EndGraph: 
			if (_gotError)
				delete _graph;
			else
				_graph->Activate(); 
				
			_gotError = false;
			_graph = nullptr;
			break;
		default:
			SendError(Error::UnknownCommandType);
			_gotError = true;
		}

	}

	if (!_gotError && _command && _command->Execute(*_graph))
	{
		delete _command;
		_command = nullptr;
	}
}



Error InitGraphCommand::AddData(byte data)
{
	if (_modCount < 0)
	{
		if (data == 0)
			return Error::InvalidParameter;

		_modCount = data;
	}
	else if (_polyModCount < 0)
	{
		_polyModCount = data;
	}
	else if (_polyphony < 0)
	{
		_polyphony = data;
	}
	else
		return Error::TooManyParameters;

	return Error::None;
}
bool InitGraphCommand::Execute(Graph& graph) const
{
	if (_polyphony < 0)
		return false;

	_graph->Init(_modCount, _polyModCount, _polyphony);
	return true;
}



Error AddModuleCommand::AddData(byte data) 
{
	if (_moduleType != ModuleType::None)
		return Error::TooManyParameters;

	if (data == (byte)ModuleType::None || data >= (byte)ModuleType::_Count)
		return Error::InvalidParameter;

	_moduleType = static_cast<ModuleType>(data);
	return Error::None;
}

bool AddModuleCommand::Execute(Graph& graph) const
{
	if (_moduleType == ModuleType::None)
		return false;

	if (_poly)
		_graph->AddPolyModule(_moduleType);
	else
		_graph->AddMonoModule(_moduleType);

	return true;
}



void AddConnectionCommand::Connection::AddData(byte data) 
{
	if (_modType == ModType::None)
		_modType = (ModType)data;
	else if (_pinType == PinType::None)
		_pinType = (PinType)data;
	else if (_modIndex < 0)
		_modIndex = data;
	else if (_pinIndex < 0)
		_pinIndex = data, _done = true;
}



Error AddConnectionCommand::AddData(byte data) 
{
	if (!_input._done) 
		_input.AddData(data);
	else if (!_output._done) 
		_output.AddData(data);
	else
		return Error::TooManyParameters;

	if (_output._done)
	{
		if (_input._pinType != _output._pinType)
			return Error::PinTypeMismatch;
		// TODO: Check pin indices exist in modules.
	}

	return Error::None;
}

bool AddConnectionCommand::Execute(Graph& graph) const
{
	if (!_output._done) 
		return false;


	if (_input.IsMono() && _output.IsMono())
	{
		if (_input.IsSigned())
			_input.ConnectToOutputMono<Module::signed_t>(*_graph, _output);
		else
			_input.ConnectToOutputMono<Module::unsigned_t>(*_graph, _output);
	}
	else
	{
		if (_input.IsSigned())
			_input.ConnectToOutputPoly<Module::signed_t>(*_graph, _output);
		else
			_input.ConnectToOutputPoly<Module::unsigned_t>(*_graph, _output);
	}

	return true;
}


Error SetUnsignedValueCommand::AddData(byte data) 
{
	if (_modIndex < 0)
	{
		_modIndex = data;
		//Module* mod = _poly ? _graph->GetPolyModule(_modIndex, 0) : _graph->GetMonoModule(_modIndex);
		//if (dynamic_cast<UnsignedValueModule*>(mod) == nullptr)
		//	return Error::InvalidParameter;
	}
	else
	{
		switch (_valBytes)
		{
		case 0: _val = data; break;
		case 1: _val |= Module::unsigned_t(data) << 8; break;
		default:
			return Error::TooManyParameters;
		}
		++_valBytes;
	}
	return Error::None;
}

bool SetUnsignedValueCommand::Execute(Graph& graph) const
{
	if (_valBytes < 2)
		return false;

	if (_poly)
	{
		for (int i = 0; i < _graph->GetPolyphony(); ++i)
			static_cast<UnsignedValueModule*>(_graph->GetPolyModule(_modIndex, i))->SetValue(_val);
	}
	else
		static_cast<UnsignedValueModule*>(_graph->GetMonoModule(_modIndex))->SetValue(_val);

	return true;
}

} //namespace 