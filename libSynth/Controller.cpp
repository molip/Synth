#include "stdafx.h"
#include "Controller.h"
#include "Command.h"
#include "CommandStack.h"

using namespace Synth;
using namespace Synth::UI;

Controller::Controller()
{
	_graph = std::make_unique<Model::Graph>();
	_commandStack = std::make_unique<CommandStack>();
	
	_commandStack ->Do(std::make_unique<AddModuleCommand>("midi", *_graph));
	_commandStack->Do(std::make_unique<SetPositionCommand>(1, Model::Point(100, 200), true, *_graph), false);

	_commandStack ->Do(std::make_unique<AddModuleCommand>("envl", *_graph));
	_commandStack->Do(std::make_unique<SetPositionCommand>(2, Model::Point(600, 200), true, *_graph), false);

	_commandStack ->Do(std::make_unique<AddModuleCommand>("oscl", *_graph));
	_commandStack->Do(std::make_unique<SetPositionCommand>(3, Model::Point(1100, 200), true, *_graph), false);
}

Controller::~Controller() = default;

bool Controller::CanUndo() const 
{
	return _commandStack->CanUndo();
}

bool Controller::CanRedo() const
{
	return _commandStack->CanRedo();
}

void Controller::Undo()
{
	_commandStack->Undo();
}

void Controller::Redo()
{
	_commandStack->Redo();
}

bool Controller::OnMouseMove(Model::Point point)
{
	if (_mouseDownPoint)
	{
		Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
		_commandStack->Do(std::make_unique<SetPositionCommand>(_selectedModuleID, delta, true, *_graph), true);
		return true;
	}
	return false;
}

void Controller::OnLButtonDown(Model::Point point)
{
	if (_selectedModuleID = HitTest(point))
		_mouseDownPoint = std::make_unique<Model::Point>(point);
}

void Controller::OnLButtonUp(Model::Point point)
{
	if (_mouseDownPoint)
	{
		Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
		_commandStack->Do(std::make_unique<SetPositionCommand>(_selectedModuleID, delta, true, *_graph), false);
		_mouseDownPoint.reset();
	}
}

int Controller::HitTest(Model::Point point) const
{
	for (auto& ikon : GetModuleIkons())
		if (ikon.GetRect().Contains(point))
			return ikon.GetModuleID();

	return 0;
}

Controller::ModuleIkonRange Controller::GetModuleIkons() const
{
	auto get = [&](size_t i) { return ModuleIkon(*_graph->GetSorted()[i], *_graph); };
	return ModuleIkonRange(get, _graph->GetSorted().size());
}
