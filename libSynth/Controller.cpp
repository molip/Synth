#include "Controller.h"
#include "Controller.h"
#include "stdafx.h"
#include "Controller.h"
#include "Command.h"
#include "CommandStack.h"
#include "Exporter.h"
#include "MIDIExporter.h"
#include "Player.h"
#include "View.h"

#include "Model/Module.h"
#include "Model/ModuleTypes.h"
#include "Model/Notification.h"

#include "../libKernel/Debug.h"
#include "../libKernel/Serial.h"

using namespace Synth;
using namespace Synth::UI;

Settings Controller::_settings;

Controller::Controller(const MIDIKeyboard::KeyCodes& keyCodes) : _midiKeyboard(keyCodes)
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
			Model::InputParams params = *module.FindInputParams(_selection.pinID);
			const int value = GetInputParamsValue(params, _selection.element);

			std::string str = module.GetInputDef(_selection.pinID).GetValueType()->ToString(value);
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

void Synth::UI::Controller::SetInputParams(Selection& sel, std::function<int(const Model::ValueType&, int)> fn)
{
	KERNEL_ASSERT(sel.element == Selection::Element::Offset || sel.element == Selection::Element::Scale);

	auto& module = *_graph->FindModule(sel.moduleID);
	const auto* valDef = module.GetInputDef(sel.pinID).GetValueType();

	Model::InputParams params = *module.FindInputParams(sel.pinID);
	int& oldVal = GetInputParamsValue(params, sel.element);
	int newVal = fn(*valDef, oldVal);
	if (sel.element == Selection::Element::Offset)
		newVal = valDef->Clamp(newVal);

	if (newVal != oldVal) // TODO: Command::IsNull.
	{
		oldVal = newVal;
		_commandStack->Do(std::make_unique<SetInputParamsCommand>(sel.moduleID, sel.pinID, params, *_graph)); // TODO: Consolidate.
	}
}

void Synth::UI::Controller::OnMouseWheel(Model::Point point, bool negative, bool coarse)
{
	auto sel = HitTest(point);
	if (sel.element != Selection::Element::Offset && sel.element != Selection::Element::Scale)
		return;

	// TODO: Consolidate.
	SetInputParams(sel, [&](const Model::ValueType& valDef, int oldVal) 
	{
		return valDef.AddDelta(oldVal, negative ? -1 : 1, coarse); 
	});
}

void Synth::UI::Controller::OnKeyDown(char key)
{
	if (BufferPtr buffer = _midiKeyboard.OnKeyStateChanged(key, true))
		SendData(*buffer);
}

void Synth::UI::Controller::OnKeyUp(char key)
{
	if (BufferPtr buffer = _midiKeyboard.OnKeyStateChanged(key, false))
		SendData(*buffer);
}

void Controller::ExportRawMIDI(Buffer&& buffer)
{
	SendData(*MIDIExporter().ExportRawMIDI(std::move(buffer)));
}

void Synth::UI::Controller::CommitValueEdit(const std::string& text)
{
	SetInputParams(_selection, [&](const Model::ValueType& valDef, int oldVal)
	{
		return valDef.FromString(text);
	});

	_view->InvalidateAll();
}

Controller::Selection Controller::HitTest(Model::Point point, Model::Rect* elementRect) const
{
	Selection sel;
	for (auto& ikon : GetModuleIkons(true))
	{
		auto HitTestPins = [&] (bool output)
		{
			for (auto& pin : output ? ikon.GetOutputPins() : ikon.GetInputPins())
			{
				const bool conn = pin.connectionRect.Contains(point);
				const bool offset = pin.showOffset && pin.offsetRect.Contains(point);
				const bool scale = pin.showScale && pin.scaleRect.Contains(point);

				if (conn || offset || scale)
				{
					sel.pinID = pin.id;
					sel.moduleID = ikon.GetModuleID();

					Model::Rect rect;
					if (conn)
					{
						sel.element = output ? Selection::Element::Output : Selection::Element::Input;
						rect = pin.connectionRect;
					}
					else
					{
						sel.element = offset ? Selection::Element::Offset : Selection::Element::Scale;
						rect = offset ? pin.offsetRect : pin.scaleRect;
					}

					if (elementRect)
						*elementRect = rect;

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

Controller::ModuleIkonRange Controller::GetModuleIkons(bool reverse) const
{
	auto get = [=](size_t i) 
	{
		const Model::Module& mod = *_graph->GetSorted()[i];
		return ModuleIkon(mod, mod.GetID() == _selection.moduleID, *_graph);
	};
	return ModuleIkonRange(get, _graph->GetSorted().size(), reverse);
}

MonitorIkon Controller::GetMonitorIkon(const ModuleIkon::MonitorArea& monitorArea) const
{
	if (!_player)
		return {};

	for (size_t i = 0; i < _monitors.size(); ++i)
	{
		if (_monitors[i] == monitorArea.pinRef)
		{
			const auto levels = _player->GetMonitorLevels();
			if (i >= levels.size())
				return {};

			MonitorIkon ikon{ monitorArea.rect };
			ikon.activeWidths.reserve(levels[i].size());
			for (float level : levels[i])
				ikon.activeWidths.push_back(int(monitorArea.rect.Width() * level));
			return ikon;
		}
	}
	return {};
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

bool Controller::SendData(const Buffer& buffer) const
{
	if (_player)
		_player->ProcessData(buffer);

	return _view->UploadData(buffer);
}

bool Controller::Export() const
{
	return DoExport(_settings.polyphony);
}

bool Controller::DoExport(byte polyphony) const
{
	_syncState = SyncState::None;

	Model::GraphExporter exporter(*_graph);
	if (BufferPtr buffer = exporter.Export(polyphony))
	{
		_syncState = SendData(*buffer) ? SyncState::All : SyncState::Local;
		_monitors = exporter.GetMonitors();
	}

	ExportSettings();

	return _syncState == SyncState::All;
}

bool Controller::ExportPolyTest()
{
	if (!DoExport(16))
		return false;

	return SendData(*MIDIExporter().ExportSetAllNotesOn());
}

bool Controller::ExportMIDIFile(const std::wstring& path) const
{
	MIDIExporter exporter;
	if (BufferPtr buffer = exporter.Export(path))
		return SendData(*buffer);
	
	return false;
}

void Controller::StopMIDIFilePlayback() const
{
	MIDIExporter exporter;
	if (BufferPtr buffer = exporter.ExportStopMIDI())
		SendData(*buffer);
}

void Controller::ExportSettings() const
{
	Model::Exporter exporter;
	if (BufferPtr buffer = exporter.ExportSettings(_settings))
		SendData(*buffer);
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
		_syncState = SyncState::None;
	}
	else if (auto* vcn = dynamic_cast<const Model::ValueChangedNotification*>(&notification))
	{
		if (_syncState != SyncState::None)
		{
			Model::GraphExporter exporter(*_graph);
			BufferPtr buffer = exporter.ExportValues(vcn->modID, vcn->pinID);
			
			if (_syncState == SyncState::All)
				if (!_view->UploadData(*buffer))
					_syncState = SyncState::Local;

			if (_player)
				_player->ProcessData(*buffer);
		}
	}
}

int& Controller::GetInputParamsValue(Model::InputParams& params, Selection::Element element) const 
{
	KERNEL_ASSERT(element == Selection::Element::Offset || element == Selection::Element::Scale);
	return element == Selection::Element::Offset ? params.offset : params.scale;
}

void Controller::SetSettings(const Settings& settings)
{
	const bool needExport = settings.polyphony != _settings.polyphony;

	_settings = settings;

	if (needExport)
		Export();
	else
		ExportSettings();
}
