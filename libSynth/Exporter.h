#pragma once

#include "Model/Engine.h"
#include "Model/Tag.h"
#include "Types.h"

#include <map>

namespace Synth
{
namespace Model
{
	class Graph;
	class Module;
	class PinType;

	class Exporter
	{
	public:
		Exporter(const Graph& graph);
		BufferPtr Export();
		BufferPtr ExportValues(int moduleID, Tag pinID);

	private:
		void Add(byte data) { (*_buffer).push_back(data); }
		void Add(Engine::CommandType data) { Add((byte)data); }
		void Add(Engine::InstanceType data) { Add((byte)data); }
		void Add(Engine::ConnectionType data) { Add((byte)data); }
		void Add(Engine::ModuleType data) { Add((byte)data); }
		void Add(Engine::PinType data) { Add((byte)data); }
		
		bool WriteValues(const Module& mod, const PinType& input);

		const Graph& _graph;
		BufferPtr _buffer;
		byte _monoModCount = 0;
		byte _polyModCount = 0;
		std::map<int, byte> _modIndices; // id -> index;
	};

}
}
