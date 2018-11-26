#include "stdafx.h"
#include "Command.h"
#include "Model/Notification.h"

using namespace Synth;
using namespace Synth::Model;

void AddModuleCommand::Do() 
{
	_undo = _graph.AddModule(_type);
}

void AddModuleCommand::Undo() 
{
	_graph.ApplyUndo(_undo);
}



void RemoveModuleCommand::Do() 
{
	_undo = _graph.RemoveModule(_moduleID);
}

void RemoveModuleCommand::Undo() 
{
	_graph.ApplyUndo(_undo);
}


void AddConnectionCommand::Do() 
{
	_undo = _graph.AddConnection(_inputPin, _outputPin);
}

void AddConnectionCommand::Undo() 
{
	_graph.ApplyUndo(_undo);
}



void RemoveConnectionCommand::Do() 
{
	_undo = _graph.RemoveConnection(_inputPin);
}

void RemoveConnectionCommand::Undo() 
{
	_graph.ApplyUndo(_undo);
}



void SetPositionCommand::Do() 
{
	Module* mod = _graph.FindModule(_modID);
	_oldPos = mod->GetPosition();
	mod->SetPosition(_relative ? _pos + _oldPos : _pos);
	_graph.SendNotification(GenericChangedNotification());
}

void SetPositionCommand::Undo() 
{
	Module* mod = _graph.FindModule(_modID);
	mod->SetPosition(_oldPos);
	_graph.SendNotification(GenericChangedNotification());
}


void SetInputParamsCommand::Do()
{
	Module* mod = _graph.FindModule(_modID);
	_oldValue = *mod->FindInputParams(_pinID);
	mod->SetInputParams(_pinID, _value);
	_graph.SendNotification(ValueChangedNotification(_modID, _pinID));
}

void SetInputParamsCommand::Undo()
{
	Module* mod = _graph.FindModule(_modID);
	mod->SetInputParams(_pinID, _oldValue);
	_graph.SendNotification(ValueChangedNotification(_modID, _pinID));
}
