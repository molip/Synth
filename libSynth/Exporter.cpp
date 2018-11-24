#include "stdafx.h"
#include "Exporter.h"
#include "Model/Graph.h"
#include "Model/Module.h"
#include "Model/ModuleTypes.h"

using namespace Synth;
using namespace Synth::Model;

Exporter::Exporter(const Graph& graph) : _graph(graph)
{
	_buffer = std::make_unique<Buffer>();

	for (auto& mod : _graph.GetSorted())
	{
		byte& index = mod->IsInstanced(_graph) ? _polyModCount : _monoModCount;
		_modIndices.insert(std::make_pair(mod->GetID(), index++));
	}
}

BufferPtr Exporter::Export(byte polyphony)
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
	}

	Add(Engine::CommandType::EndGraph);

	return std::move(_buffer);
}

BufferPtr Exporter::ExportValues(int moduleID, Tag pinID)
{
	auto& mod = *_graph.FindModule(moduleID);
	WriteValues(mod, mod.GetInputDef(pinID));
	return std::move(_buffer);
}

void Exporter::WriteValues(const Module& mod, const PinType& input)
{
	if (input.GetValueType() && !mod.FindConnection(input.GetID()))
	{
		union
		{
			float f;
			uint32_t i;
		} u;

		u.f = input.GetValueType()->ToFloat(*mod.FindValue(input.GetID()));
		const uint32_t bytes = u.i;

		Add(Engine::CommandType::SetValue);
		Add(mod.IsInstanced(_graph));
		Add(_modIndices[mod.GetID()]);
		Add(input.GetEngineID());
		for (int i = 3; i >= 0; --i)
			Add(bytes >> i * 8 & 0xff);
	}
}
