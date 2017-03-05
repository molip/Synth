#include "stdafx.h"
#include "Controller.h"
#include "Command.h"
#include "CommandStack.h"
#include "Exporter.h"

#include "../libKernel/Debug.h"
#include "../libKernel/Serial.h"

using namespace Synth;
using namespace Synth::UI;

Controller::Controller()
{
	_graph = std::make_unique<Model::Graph>();
	_commandStack = std::make_unique<CommandStack>();
	
	_commandStack->Do(std::make_unique<AddModuleCommand>("midi", *_graph));
	_commandStack->Do(std::make_unique<SetPositionCommand>(1, Model::Point(100, 200), true, *_graph), false);

	_commandStack->Do(std::make_unique<AddModuleCommand>("envl", *_graph));
	_commandStack->Do(std::make_unique<SetPositionCommand>(2, Model::Point(600, 200), true, *_graph), false);

	_commandStack->Do(std::make_unique<AddModuleCommand>("oscl", *_graph));
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
		if (_liveConnection)
		{
			_liveConnection->second = point;
		}
		else
		{
			Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
			_commandStack->Do(std::make_unique<SetPositionCommand>(_selection.moduleID, delta, true, *_graph), true);
		}
		return true;
	}
	return false;
}

bool Controller::OnLButtonDown(Model::Point point)
{
	_selection = HitTest(point);

	if (_selection.moduleID == 0)
		return false;

	if (!_selection.pinID.empty())
	{
		if (_selection.isOutput)
			return false;

		auto connectionPoint = ModuleIkon(*_graph->FindModule(_selection.moduleID), *_graph).FindPin(_selection.pinID, false)->connectionPoint;
		_liveConnection = std::make_unique<Connection>(connectionPoint, point);
	}

	_mouseDownPoint = std::make_unique<Model::Point>(point);
	return true;
}

void Controller::OnLButtonUp(Model::Point point)
{
	if (_mouseDownPoint)
	{
		if (_liveConnection)
		{
			_liveConnection.reset();

			Selection sel = HitTest(point);
			const Model::PinRef input(_selection.moduleID, _selection.pinID);
			const Model::PinRef output(sel.moduleID, sel.pinID);

			bool connect = false;
			if (!sel.pinID.empty() && sel.isOutput)
			{
				const auto& valid = _graph->GetValidSourcePins(input);
				connect = std::find(valid.begin(), valid.end(), output) != valid.end();
			}

			if(connect)
				_commandStack->Do(std::make_unique<AddConnectionCommand>(input, output, *_graph));
			else
				_commandStack->Do(std::make_unique<RemoveConnectionCommand>(input, *_graph));
		}
		else
		{
			Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
			_commandStack->Do(std::make_unique<SetPositionCommand>(_selection.moduleID, delta, true, *_graph), false);
		}
		_mouseDownPoint.reset();
	}
}

Controller::Selection Controller::HitTest(Model::Point point) const
{
	Selection sel;
	for (auto& ikon : GetModuleIkons())
	{
		if (ikon.GetRect().Contains(point))
		{
			sel.moduleID = ikon.GetModuleID();
			break;
		}

		auto HitTestPins = [&] (bool output)
		{
			for (auto& pin : output ? ikon.GetOutputPins() : ikon.GetInputPins())
				if (pin.rect.Contains(point))
				{
					sel.pinID = pin.id;
					sel.isOutput = output;
					sel.moduleID = ikon.GetModuleID();
					return true;
				}
			return false;
		};

		if (HitTestPins(false))
			break;

		if (HitTestPins(true))
			break;
	}
		
	return sel;
}

Controller::ModuleIkonRange Controller::GetModuleIkons() const
{
	auto get = [&](size_t i) { return ModuleIkon(*_graph->GetSorted()[i], *_graph); };
	return ModuleIkonRange(get, _graph->GetSorted().size());
}

std::vector<Controller::Connection> Controller::GetConnections() const
{
	std::vector<Connection> connections;

	for (const auto* module : _graph->GetSorted())
	{
		for (const auto& conn : module->GetConnections())
		{
			auto& srcMod = module->GetSourceModule(conn.second, *_graph);

			ModuleIkon srcIkon(srcMod, *_graph);
			ModuleIkon dstIkon(*module, *_graph);

			auto* srcPin = srcIkon.FindPin(conn.second.type, true); 
			auto* dstPin = dstIkon.FindPin(conn.first, false); 
			KERNEL_VERIFY(srcPin && dstPin);
				connections.emplace_back(srcPin->connectionPoint, dstPin->connectionPoint);
		}
	}

	return connections;
}

std::vector<unsigned char> Controller::Export() const
{
	Model::Exporter exporter;
	return exporter.Export(*_graph);
}

bool Controller::Save(const std::wstring& path) const
{
	return Kernel::Serial::SaveClass(path, *_graph);
}

bool Controller::Load(const std::wstring& path)
{
	return Kernel::Serial::LoadClass(path, *_graph);
}
