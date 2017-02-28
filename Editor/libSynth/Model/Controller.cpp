#include "stdafx.h"
#include "Controller.h"
#include "Command.h"
#include "CommandStack.h"

using namespace Synth;

Controller::Controller()
{
	_graph = std::make_unique<Model::Graph>();
	_commandStack = std::make_unique<CommandStack>();
	
	_commandStack ->Do(std::make_unique<AddModuleCommand>("midi", *_graph));
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

void Controller::OnMouseMove(Model::Point point)
{
	if (_mouseDownPoint)
	{
		Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
		_commandStack->Do(std::make_unique<SetPositionCommand>(1, delta, true, *_graph), true);
	}
}

void Controller::OnLButtonDown(Model::Point point)
{
	if (_selectedModule = HitTest(point))
		_mouseDownPoint = std::make_unique<Model::Point>(point);
}

void Controller::OnLButtonUp(Model::Point point)
{
	Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
	_commandStack->Do(std::make_unique<SetPositionCommand>(1, delta, true, *_graph), false);
	_mouseDownPoint.reset();
}

const std::vector<Model::Module*>& Synth::Controller::GetSorted() const
{
	return _graph->GetSorted();
}

Model::Rect Synth::Controller::GetModuleRect(const Model::Module& mod) const
{
	Model::Point pos = mod.GetPosition();
	return Model::Rect(pos, pos + Model::Point(300, 200));
}

Model::Module* Controller::HitTest(Model::Point point) const
{
	for (auto& mod : _graph->GetSorted())
		if (GetModuleRect(*mod).Contains(point))
			return mod;

	return nullptr;
}
