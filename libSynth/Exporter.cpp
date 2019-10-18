#include "stdafx.h"
#include "Exporter.h"
#include "Settings.h"
#include "Model/Graph.h"
#include "Model/Module.h"
#include "Model/ModuleTypes.h"

using namespace Synth;
using namespace Synth::Model;

Exporter::Exporter()
{
	_buffer = std::make_unique<Buffer>();
}

void Exporter::AddFloat(float val)
{
	union { float f; uint32_t i; } u;
	u.f = val;
	AddInteger(u.i, 4);
}

void Exporter::AddInteger(uint32_t val, int bytes)
{
	for (int i = bytes - 1; i >= 0; --i)
		Add(byte(val >> i * 8));
}

BufferPtr Exporter::ExportSettings(const Settings& settings)
{
	Add(Engine::CommandType::Settings);
	Add(settings.hold);
	Add(settings.arpEnabled);
	AddInteger(settings.arpPeriod, 2);
	AddInteger(settings.arpDuty, 2);
	AddInteger(settings.arpOctaves, 2);
	return std::move(_buffer);
}


GraphExporter::GraphExporter(const Graph& graph) : _graph(graph)
{
	for (auto& mod : _graph.GetSorted())
	{
		byte& index = mod->IsInstanced(_graph) ? _polyModCount : _monoModCount;
		_modIndices.insert(std::make_pair(mod->GetID(), index++));
	}
}


BufferPtr GraphExporter::Export(byte polyphony)
{
	Add(Engine::CommandType::StartGraph);

	Add(Engine::CommandType::InitGraph);
	Add(_monoModCount);
	Add(_polyModCount);
	Add(polyphony);

	for (auto& mod : _graph.GetSorted())
	{
		Add(mod->IsInstanced(_graph) ? Engine::CommandType::AddPolyModule : Engine::CommandType::AddMonoModule);
		Add(mod->GetDef().GetEngineID());
	}

	auto AddPin = [&](const Module& mod, const PinType& pinDef)
	{
		Add(mod.IsInstanced(_graph) ? Engine::InstanceType::Poly : Engine::InstanceType::Mono);
		Add(_modIndices[mod.GetID()]);
		Add(pinDef.GetEngineID());
	};

	for (auto& mod : _graph.GetSorted())
	{
		for (auto& conn : mod->GetConnections())
		{
			const PinType& inputDef = mod->GetInputDef(conn.first);
			const PinType& outputDef = mod->GetSourceOutputDef(conn.second, _graph);
			const Module& sourceMod = mod->GetSourceModule(conn.second, _graph);

			Add(Engine::CommandType::AddConnection);
			Add((inputDef.IsMulti() || outputDef.IsMulti()) ? Engine::ConnectionType::Multi : Engine::ConnectionType::Single);

			AddPin(*mod, inputDef);
			AddPin(sourceMod, outputDef);
		}

		for (auto& input : mod->GetDef().GetInputs())
			WriteValues(*mod, *input);

		for (auto& output : mod->GetDef().GetOutputs())
		{
			if (output->IsMonitor())
			{
				Add(Engine::CommandType::AddMonitor);
				AddPin(*mod, *output);
				_monitors.emplace_back(mod->GetID(), output->GetID());
			}
		}
	}

	Add(Engine::CommandType::EndGraph);

	return std::move(_buffer);
}

BufferPtr GraphExporter::ExportValues(int moduleID, Tag pinID)
{
	auto& mod = *_graph.FindModule(moduleID);
	WriteValues(mod, mod.GetInputDef(pinID));
	return std::move(_buffer);
}

void GraphExporter::WriteValues(const Module& mod, const PinType& input)
{
	if (input.GetValueType())
	{
		Add(Engine::CommandType::SetInputParams);
		Add(mod.IsInstanced(_graph));
		Add(_modIndices[mod.GetID()]);
		Add(input.GetEngineID());

		const InputParams params = *mod.FindInputParams(input.GetID());
		AddFloat(input.GetValueType()->ToFloat(params.offset));
		AddFloat(input.GetValueType()->ToFloat(params.scale));
	}
}
