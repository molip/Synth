#include "Input.h"
#include "MIDIData.h"

namespace Engine { namespace RemoteInput
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
		SERIAL_PRINT("Data: ");
		SERIAL_PRINTLN(data);
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
		case CommandType::InitGraph:
			SERIAL_PRINTLN("CommandType::InitGraphCommand");
			_command = new InitGraphCommand(_newGraph);
			break;
		case CommandType::AddMonoModule: 
			SERIAL_PRINTLN("CommandType::AddMonoModuleCommand");
			_command = new AddMonoModuleCommand(_newGraph);
			break;
		case CommandType::AddPolyModule: 
			SERIAL_PRINTLN("CommandType::AddPolyModuleCommand");
			_command = new AddPolyModuleCommand(_newGraph);
			break;
		case CommandType::AddConnection: 
			SERIAL_PRINTLN("CommandType::AddConnectionCommand");
			_command = new AddConnectionCommand(_newGraph);
			break;
		case CommandType::AddMonitor:
			SERIAL_PRINTLN("CommandType::AddMonitorCommand");
			_command = new AddMonitorCommand(_newGraph);
			break;

		// Always Graph::GetActive().
		case CommandType::SetMIDIData: 
			SERIAL_PRINTLN("CommandType::SetMIDIDataCommand");
			_command = new SetMIDIDataCommand(Graph::GetActive());
			break;
		case CommandType::StopMIDIPlayback:
			SERIAL_PRINTLN("CommandType::StopMIDIPlayback");
			MIDIData::Instance().ResetData();
			Graph::GetActive()->ResetMIDI();
			break;
		case CommandType::SetAllNotesOn:
			SERIAL_PRINTLN("CommandType::SetAllNotesOn");
			MIDIData::Instance().ResetData();
			Graph::GetActive()->SetAllNotesOn();
			break;
		case CommandType::MIDIInput:
			SERIAL_PRINTLN("CommandType::MIDIInput");
			_command = new MIDIInputCommand(Graph::GetActive());
			break;
		case CommandType::Settings:
			SERIAL_PRINTLN("CommandType::Settings");
			_command = new SettingsCommand(Graph::GetActive());
			break;

			
		// Can be either.
		case CommandType::SetInputParams: 
			SERIAL_PRINTLN("CommandType::SetInputParamsCommand");
			_command = new SetInputParamsCommand(graph);
			break;

		case CommandType::SetFieldParams:
			SERIAL_PRINTLN("CommandType::SetFieldParamsCommand");
			_command = new SetFieldParamsCommand(graph);
			break;

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



void Pin::AddData(byte data) 
{
	if (_modType == InstanceType::None)
		_modType = (InstanceType)data;
	else if (_modIndex < 0)
		_modIndex = data;
	else if (_pinIndex < 0)
		_pinIndex = data, _done = true;
}



Error AddConnectionCommand::AddData(byte data) 
{
	if (_connType == ConnectionType::None)
		_connType = (ConnectionType)data;
	else if (!_input.IsDone())
		_input.AddData(data);
	else if (!_output.IsDone())
		_output.AddData(data);
	else
		return Error::TooManyParameters;

	if (_output.IsDone())
	{
		// TODO: Check pin indices exist in modules.
	}

	return Error::None;
}

bool AddConnectionCommand::Execute() const
{
	if (!_output.IsDone())
		return false;

	_input.ConnectToOutput(*_graph, _output, _connType == ConnectionType::Multi);

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


Error MIDIInputCommand::AddData(byte data)
{
	if (!_dataSize.IsFinished())
		_dataSize.AddData(data);
	else
	{
		if (_dataRead++ == _dataSize)
			return Error::TooManyParameters;

		if (_graph)
			_graph->ProcessMIDI(data);
	}
		
	return Error::None;
}

bool MIDIInputCommand::Execute() const
{
	if (!_dataSize.IsFinished() || _dataRead < _dataSize)
		return false;

	return true;
}


Error SettingsCommand::AddData(byte data)
{
	if (!_hold.IsFinished())
		_hold.AddData(data);
	else if (!_arpEnabled.IsFinished())
		_arpEnabled.AddData(data);
	else if (!_arpPeriod.IsFinished())
		_arpPeriod.AddData(data);
	else if (!_arpDuty.IsFinished())
		_arpDuty.AddData(data);
	else if (!_arpOctaves.IsFinished())
		_arpOctaves.AddData(data);
	else
		return Error::TooManyParameters;

	return Error::None;
}

bool SettingsCommand::Execute() const
{
	if (!_arpOctaves.IsFinished())
		return false;

	if (_graph)
		_graph->SetSettings({ _hold, _arpEnabled, _arpPeriod, _arpDuty, _arpOctaves });

	return true;
}

}} //namespace 
