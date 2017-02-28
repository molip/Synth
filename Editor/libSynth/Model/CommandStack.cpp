#include "stdafx.h"
#include "CommandStack.h"
#include "Command.h"

using namespace Model;

void CommandStack::Do(CommandPtr cmd, bool preview)
{
	ClearPreview();

	cmd->Do();

	if (preview)
	{
		_preview = std::move(cmd);
	}
	else
	{
		_commands.resize(_doneCount++);
		_commands.push_back(std::move(cmd));
	}
}

void CommandStack::ClearPreview()
{
	if (_preview)
	{
		_preview->Undo();
		_preview.reset();
	}
}

void CommandStack::Undo()
{
	if (CanUndo())
		_commands[--_doneCount]->Undo();
}

void CommandStack::Redo()
{
	if (CanRedo())
		_commands[_doneCount++]->Redo();
}

bool CommandStack::CanUndo()
{
	return _doneCount > 0;
}

bool CommandStack::CanRedo()
{
	return _doneCount < (int)_commands.size();
}
