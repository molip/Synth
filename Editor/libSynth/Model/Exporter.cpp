#include "stdafx.h"
#include "Exporter.h"
#include "Graph.h"
#include "Module.h"
#include "ModuleTypes.h"

using namespace Model;

const Exporter::Buffer& Exporter::Export(const Graph& graph)
{
	_buffer.clear();

	const byte polyphony = 8;
	
	byte monoModCount = 0, polyModCount = 0, valueCount = 0;
	std::map<int, byte> modIndices; // id -> index;
	for (auto& mod : graph.GetSorted())
	{
		byte& index = mod->IsInstanced(graph) ? polyModCount : monoModCount;
		modIndices.insert(std::make_pair(mod->GetID(), index++));
		valueCount += (byte)mod->GetValues().size();
	}

	Add(Engine::CommandType::StartGraph);

	Add(Engine::CommandType::InitGraph);
	Add(monoModCount + valueCount);
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

		for (auto val : mod->GetValues())
		{
			Add(Engine::CommandType::AddMonoModule);
			Add(Engine::ModuleType::UnsignedValue);

			Add(Engine::CommandType::AddConnection);
			Add(Engine::ConnectionType::Single);

			// Input.
			AddPin(*mod, mod->GetInputDef(val.first));

			// Output.
			Add(Engine::InstanceType::Mono);
			Add(Engine::PinType::Unsigned);
			Add(monoModCount);
			Add(0);

			// Set value.
			Add(Engine::CommandType::SetMonoUnsignedValue);
			Add(monoModCount);
			Add(val.second & 0xff);
			Add(val.second >> 8);

			++monoModCount;
		}
	}

	Add(Engine::CommandType::EndGraph);

	return _buffer;
}
