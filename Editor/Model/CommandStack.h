#pragma once

#include <memory>
#include <vector>

namespace Model
{
	class Command;
	using CommandPtr = std::unique_ptr<Command>;
	class CommandStack
	{
	public:
		void Do(CommandPtr cmd);
		void Undo();
		void Redo();
		bool CanUndo();
		bool CanRedo();
	
	protected:
		std::vector<CommandPtr>_commands;
		int _doneCount = 0;
	};
}
