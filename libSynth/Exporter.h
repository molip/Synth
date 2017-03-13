#pragma once

#include "Model/Engine.h"
#include "Types.h"

#include <vector>

namespace Synth
{
namespace Model
{
	class Graph;
	class Exporter
	{
	public:
		BufferPtr Export(const Graph& graph);

	private:
		void Add(byte data) { (*_buffer).push_back(data); }
		void Add(Engine::CommandType data) { Add((byte)data); }
		void Add(Engine::InstanceType data) { Add((byte)data); }
		void Add(Engine::ConnectionType data) { Add((byte)data); }
		void Add(Engine::ModuleType data) { Add((byte)data); }
		void Add(Engine::PinType data) { Add((byte)data); }

		BufferPtr _buffer;
	};

}
}
