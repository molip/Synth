#include "Input.h"
#include "MIDIData.h"

namespace Input 
{

namespace 
{
	Command* _command;
	Graph* _newGraph;
	bool _gotError;
}

void SendError(Error error)
{
	SERIAL_PRINT("Error: ");
	SERIAL_PRINTLN((int)error);
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
		Graph* graph = _newGraph ? _newGraph : Graph::GetActive();

		switch (static_cast<CommandType>(data))
		{
		// Always _newGraph.
		case CommandType::InitGraph: _command = new InitGraphCommand(_newGraph); break;
		case CommandType::AddMonoModule: _command = new AddMonoModuleCommand(_newGraph); break;
		case CommandType::AddPolyModule: _command = new AddPolyModuleCommand(_newGraph); break;
		case CommandType::AddConnection: _command = new AddConnectionCommand(_newGraph); break;

		// Always Graph::GetActive().
		case CommandType::SetMIDIData: _command = new SetMIDIDataCommand(Graph::GetActive()); break;
		case CommandType::StopMIDIPlayback:
			MIDIData::Instance().ResetData();
			Graph::GetActive()->ResetMIDI();
			break;
			
		// Can be either.
		case CommandType::SetUnsignedValue: _command = new SetUnsignedValueCommand(graph); break;
		case CommandType::SetSignedValue: _command = new SetSignedValueCommand(graph); break;

		case CommandType::StartGraph: 
		{
			SERIAL_PRINTLN("CommandType::StartGraph");
			if (_newGraph)
			{
				SendError(Error::GraphAlreadyStarted);
				_gotError = true;
			}
			else 
				_newGraph = new Graph;
			break;
		}
		case CommandType::EndGraph: 
			if (_gotError)
				delete _newGraph;
			else
			{
				_newGraph->Activate();
				SERIAL_PRINTLN("Graph activated.");
			}

			_gotError = false;
			_newGraph = nullptr;
			break;
		default:
			SendError(Error::UnknownCommandType);
			_gotError = true;
		}
	}

	if (!_gotError && _command && _command->Execute())
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
bool InitGraphCommand::Execute() const
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

bool AddModuleCommand::Execute() const
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
	if (_modType == InstanceType::None)
		_modType = (InstanceType)data;
	else if (_pinType == PinType::None)
		_pinType = (PinType)data;
	else if (_modIndex < 0)
		_modIndex = data;
	else if (_pinIndex < 0)
		_pinIndex = data, _done = true;
}



Error AddConnectionCommand::AddData(byte data) 
{
	if (_connType == ConnectionType::None)
		_connType = (ConnectionType)data;
	else if (!_input._done) 
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

bool AddConnectionCommand::Execute() const
{
	if (!_output._done) 
		return false;

	if (_input.IsSigned())
		_input.ConnectToOutput<Module::signed_t>(*_graph, _output, _connType == ConnectionType::Multi);
	else
		_input.ConnectToOutput<Module::unsigned_t>(*_graph, _output, _connType == ConnectionType::Multi);

	return true;
}


Error SetMIDIDataCommand::AddData(byte data)
{
	if (!_division.IsFinished())
		_division.AddData(data);
	else if (!_dataSize.IsFinished())
		_dataSize.AddData(data);
	else
	{
		if (_dataRead == _dataSize)
			return Error::TooManyParameters;

		if (!_data)
			_data = new byte[_dataSize];

		_data[_dataRead++] = data;
	}
		
	return Error::None;
}

bool SetMIDIDataCommand::Execute() const
{
	if (!_dataSize.IsFinished() || _dataRead < _dataSize)
		return false;

	MIDIData::Instance().SetData(_data, _dataSize, _division); // MIDIData now owns _data.
	_graph->ResetMIDI();

	return true;
}

} //namespace 