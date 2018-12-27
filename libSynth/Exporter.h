#pragma once

#include "Model/Engine.h"
#include "Model/Tag.h"
#include "Types.h"

#include <map>

namespace Synth
{
class Settings;

namespace Model
{
	class Graph;
	class Module;
	class PinType;

	class Exporter
	{
	public:
		Exporter();
		BufferPtr ExportSettings(const Settings& settings);
	protected:
		void Add(byte data) { (*_buffer).push_back(data); }
		void Add(Engine::CommandType data) { Add((byte)data); }
		void AddFloat(float val);
		void AddInteger(uint32_t val, int bytes);

		BufferPtr _buffer;
	};
	
	class GraphExporter : public Exporter
	{
	public:
		GraphExporter(const Graph& graph);
		BufferPtr Export(byte polyphony);
		BufferPtr ExportValues(int moduleID, Tag pinID);

	private:
		using Exporter::Add;
		void Add(Engine::InstanceType data) { Add((byte)data); }
		void Add(Engine::ConnectionType data) { Add((byte)data); }
		void Add(Engine::ModuleType data) { Add((byte)data); }

		void WriteValues(const Module& mod, const PinType& input);

		const Graph& _graph;
		byte _monoModCount = 0;
		byte _polyModCount = 0;
		std::map<int, byte> _modIndices; // id -> index;
	};

}
}
