#include "stdafx.h"
#include "Command.h"

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
}

void SetPositionCommand::Undo() 
{
	Module* mod = _graph.FindModule(_modID);
	mod->SetPosition(_oldPos);
}
