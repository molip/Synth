#include "stdafx.h"
#include "Controller.h"
#include "Command.h"
#include "CommandStack.h"
#include "Exporter.h"
#include "MIDIExporter.h"
#include "View.h"

#include "Model/Module.h"
#include "Model/ModuleTypes.h"
#include "Model/Notification.h"

#include "../libKernel/Debug.h"
#include "../libKernel/Serial.h"

using namespace Synth;
using namespace Synth::UI;

Controller::Controller()
{
	_graph = std::make_unique<Model::Graph>();
	_commandStack = std::make_unique<CommandStack>();
	
	_graph->AddObserver(*this);
	//_commandStack->Do(std::make_unique<AddModuleCommand>("midi", *_graph));
	//_commandStack->Do(std::make_unique<SetPositionCommand>(1, Model::Point(100, 100), true, *_graph), false);

	//_commandStack->Do(std::make_unique<AddModuleCommand>("envl", *_graph));
	//_commandStack->Do(std::make_unique<SetPositionCommand>(2, Model::Point(500, 100), true, *_graph), false);

	//_commandStack->Do(std::make_unique<AddModuleCommand>("oscl", *_graph));
	//_commandStack->Do(std::make_unique<SetPositionCommand>(3, Model::Point(900, 100), true, *_graph), false);

	//_commandStack->Do(std::make_unique<AddModuleCommand>("pmix", *_graph));
	//_commandStack->Do(std::make_unique<SetPositionCommand>(4, Model::Point(300, 400), true, *_graph), false);

	//_commandStack->Do(std::make_unique<AddModuleCommand>("trgt", *_graph));
	//_commandStack->Do(std::make_unique<SetPositionCommand>(5, Model::Point(700, 400), true, *_graph), false);

	//_graph->FindModule(2)->SetValue("sust", 0xffff);
	//_graph->FindModule(2)->SetValue("rels", 1000);
}

Controller::~Controller()
{
	_graph->RemoveObserver(*this);
}

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

void Controller::InsertModule(const std::string& type)
{
	_commandStack->Do(std::make_unique<AddModuleCommand>(type, *_graph));
}

void Controller::DeleteModule()
{
	_commandStack->Do(std::make_unique<RemoveModuleCommand>(_selection.moduleID, *_graph));
	_selection = Selection(); // TODO: What about undo? Need notifications! 
}

bool Controller::CanDeleteModule() const
{
	return _selection.moduleID && _graph->FindModule(_selection.moduleID); 
}

void Controller::OnMouseMove(Model::Point point)
{
	if (_mouseDownPoint)
	{
		if (_liveConnection)
		{
			_liveConnection->second = point;
			_view->InvalidateAll();
		}
		else
		{
			Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
			_commandStack->Do(std::make_unique<SetPositionCommand>(_selection.moduleID, delta, true, *_graph), true);
		}
	}
}

void Controller::OnLButtonDown(Model::Point point)
{
	Model::Rect elementRect;
	_selection = HitTest(point, &elementRect);

	_view->InvalidateAll();

	if (_selection.moduleID == 0 || _selection.element == Selection::Element::Output)
		return;

	if (!_selection.pinID.empty())
	{
		const auto& module = *_graph->FindModule(_selection.moduleID);

		if (_selection.element == Selection::Element::Input)
		{
			auto connectionPoint = ModuleIkon(module, false, *_graph).FindPin(_selection.pinID, false)->GetConnectionPoint();
			_liveConnection = std::make_unique<Connection>(connectionPoint, point);
		}
		else
		{
			KERNEL_ASSERT(_selection.element == Selection::Element::Value);
			
			std::string str = module.GetInputDef(_selection.pinID).GetValueType()->ToString(*module.FindValue(_selection.pinID));
			_view->StartValueEdit(elementRect, str);

			return;
		}
	}

	_mouseDownPoint = std::make_unique<Model::Point>(point);
	_view->SetCapture(true);
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
			if (!sel.pinID.empty() && sel.element == Selection::Element::Output)
			{
				const auto& valid = _graph->GetValidSourcePins(input);
				connect = std::find(valid.begin(), valid.end(), output) != valid.end();
			}

			if(connect)
				_commandStack->Do(std::make_unique<AddConnectionCommand>(input, output, *_graph));
			else
				_commandStack->Do(std::make_unique<RemoveConnectionCommand>(input, *_graph)); // TODO: don't do if null.
		}
		else
		{
			Model::Point delta(point.x - _mouseDownPoint->x, point.y - _mouseDownPoint->y);
			_commandStack->Do(std::make_unique<SetPositionCommand>(_selection.moduleID, delta, true, *_graph), false);
		}
		_mouseDownPoint.reset();
	}
	_view->SetCapture(false);
}

void Synth::UI::Controller::OnMouseWheel(Model::Point point, bool negative, bool coarse)
{
	auto sel = HitTest(point);
	if (sel.element != Selection::Element::Value)
		return;

	auto& module = *_graph->FindModule(sel.moduleID);
	const auto& valDef = module.GetInputDef(sel.pinID).GetValueType();

	const int oldVal = *module.FindValue(sel.pinID);
	const int newVal = valDef->AddDelta(oldVal, (coarse ? 10 : 1) * (negative ? -1 : 1));
	
	if (newVal != oldVal) // TODO: Command::IsNull.
	{
		_commandStack->Do(std::make_unique<SetValueCommand>(sel.moduleID, sel.pinID, newVal, *_graph)); // TODO: Consolidate.
	}
}

void Synth::UI::Controller::CommitValueEdit(const std::string& text)
{
	KERNEL_ASSERT(_selection.element == Selection::Element::Value);
	auto& module = *_graph->FindModule(_selection.moduleID);

	int val = module.GetInputDef(_selection.pinID).GetValueType()->FromString(text);
	if (val != *module.FindValue(_selection.pinID)) // TODO: Command::IsNull.
		_commandStack->Do(std::make_unique<SetValueCommand>(_selection.moduleID, _selection.pinID, val, *_graph));

	_view->InvalidateAll();
}

Controller::Selection Controller::HitTest(Model::Point point, Model::Rect* elementRect) const
{
	Selection sel;
	for (auto& ikon : GetModuleIkons())
	{
		auto HitTestPins = [&] (bool output)
		{
			for (auto& pin : output ? ikon.GetOutputPins() : ikon.GetInputPins())
			{
				if (pin.connectionRect.Contains(point))
				{
					sel.pinID = pin.id;
					sel.element = output ? Selection::Element::Output : Selection::Element::Input;
					sel.moduleID = ikon.GetModuleID();
					if (elementRect)
						*elementRect = pin.connectionRect;
					return true;
				}

				if (!output && pin.showValue && pin.valueRect.Contains(point))
				{
					sel.pinID = pin.id;
					sel.element = Selection::Element::Value;
					sel.moduleID = ikon.GetModuleID();
					if (elementRect)
						*elementRect = pin.valueRect;
					return true;
				}
			}
			return false;
		};

		if (HitTestPins(false))
			break;
		
		if (HitTestPins(true))
			break;

		if (ikon.GetRect().Contains(point))
		{
			sel.moduleID = ikon.GetModuleID();
			break;
		}
	}
		
	return sel;
}

Controller::ModuleIkonRange Controller::GetModuleIkons() const
{
	auto get = [&](size_t i) 
	{
		const Model::Module& mod = *_graph->GetSorted()[i];
		return ModuleIkon(mod, mod.GetID() == _selection.moduleID, *_graph);
	};
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

			ModuleIkon srcIkon(srcMod, false, *_graph);
			ModuleIkon dstIkon(*module, false, *_graph);

			auto* srcPin = srcIkon.FindPin(conn.second.type, true); 
			auto* dstPin = dstIkon.FindPin(conn.first, false); 
			KERNEL_VERIFY(srcPin && dstPin);
			connections.emplace_back(dstPin->GetConnectionPoint(), srcPin->GetConnectionPoint());
		}
	}

	return connections;
}

bool Controller::Export() const
{
	Model::Exporter exporter(*_graph);
	if (BufferPtr buffer = exporter.Export())
	{
		if (_view->UploadData(*buffer))
		{
			_inSync = true;
			return true;
		}
	}
	
	return false;
}

bool Controller::ExportMIDIFile(const std::wstring& path) const
{
	MIDIExporter exporter;
	if (BufferPtr buffer = exporter.Export(path))
		return _view->UploadData(*buffer);
	
	return false;
}

bool Controller::Save(const std::wstring& path) const
{
	if (Kernel::Serial::SaveClass(path, *_graph))
	{
		_view->SetModified(false);
		return true;
	}
	return false;
}

bool Controller::Load(const std::wstring& path)
{
	return Kernel::Serial::LoadClass(path, *_graph);
}

void Controller::OnGraphNotification(const Model::Notification& notification) 
{
	_view->SetModified(true);
	_view->InvalidateAll();

	if (dynamic_cast<const Model::StructureChangedNotification*>(&notification))
	{
		_inSync = false;
	}
	else if (auto* vcn = dynamic_cast<const Model::ValueChangedNotification*>(&notification))
	{
		if (_inSync)
		{
			Model::Exporter exporter(*_graph);
			BufferPtr buffer = exporter.ExportValues(vcn->modID, vcn->pinID);
			if (!_view->UploadData(*buffer)) // Now out of sync! 
				_inSync = false;
		}
	}
}
