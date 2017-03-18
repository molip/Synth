#include "stdafx.h"
#include "Exporter.h"
#include "Model/Graph.h"
#include "Model/Module.h"
#include "Model/ModuleTypes.h"

using namespace Synth;
using namespace Synth::Model;

BufferPtr Exporter::Export(const Graph& graph)
{
	_buffer = std::make_unique<Buffer>();

	const byte polyphony = 8;
	
	byte monoModCount = 0, polyModCount = 0;
	std::map<int, byte> modIndices; // id -> index;
	for (auto& mod : graph.GetSorted())
	{
		byte& index = mod->IsInstanced(graph) ? polyModCount : monoModCount;
		modIndices.insert(std::make_pair(mod->GetID(), index++));
	}

	Add(Engine::CommandType::StartGraph);

	Add(Engine::CommandType::InitGraph);
	Add(monoModCount);
	Add(polyModCount);
	Add(polyphony);

	for (auto& mod : graph.GetSorted())
	{
		Add(mod->IsInstanced(graph) ? Engine::CommandType::AddPolyModule : Engine::CommandType::AddMonoModule);
		Add(mod->GetDef().GetEngineID());
	}

	auto AddPin = [&](const Module& mod, const PinType& pinDef)
	{
		Add(mod.IsInstanced(graph) ? Engine::InstanceType::Poly : Engine::InstanceType::Mono);
		Add(pinDef.IsSigned() ? Engine::PinType::Signed : Engine::PinType::Unsigned);
		Add(modIndices[mod.GetID()]);
		Add(pinDef.GetEngineID());
	};

	for (auto& mod : graph.GetSorted())
	{
		for (auto& conn : mod->GetConnections())
		{
			const PinType& inputDef = mod->GetInputDef(conn.first);
			const PinType& outputDef = mod->GetSourceOutputDef(conn.second, graph);
			const Module& sourceMod = mod->GetSourceModule(conn.second, graph);

			Add(Engine::CommandType::AddConnection);
			Add((inputDef.IsMulti() || outputDef.IsMulti()) ? Engine::ConnectionType::Multi : Engine::ConnectionType::Single);

			AddPin(*mod, inputDef);
			AddPin(sourceMod, outputDef);
		}

		// Set values.
		for (auto& input : mod->GetDef().GetInputs())
		{
			if (input->GetValueType() && !mod->FindConnection(input->GetID()))
			{
				int val = *mod->FindValue(input->GetID());

				Add(mod->IsInstanced(graph) ? Engine::CommandType::SetPolyUnsignedValue : Engine::CommandType::SetMonoUnsignedValue);
				Add(modIndices[mod->GetID()]);
				Add(input->GetEngineID());
				Add(val >> 8);
				Add(val & 0xff);
			}
		}
	}

	Add(Engine::CommandType::EndGraph);

	return std::move(_buffer);
}
