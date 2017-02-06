#pragma once

#include "Tag.h"
#include <vector>

namespace Model
{
	class Graph;
	class Module;
	class ModuleType;
	class PinType;

	struct PinRef
	{
		int moduleID;
		Tag type;
	};

	struct Connection
	{
		Tag type;
		PinRef source;
	};

	class Module
	{
	public:
		Module(Tag type);

		const ModuleType& GetDef() const;
		const PinType& GetInputDef(Tag type) const;
		const PinType& GetOutputDef(Tag type) const;
		const Module& GetSourceModule(const Connection& conn, const Graph& graph) const;
		const PinType& GetSourceOutputDef(const Connection& conn, const Graph& graph) const;

		const std::vector<Connection>& GetConnections() const { return _connections; }
		void Connect(Tag inputType, int modID, Tag outputType);

		//void AddInput(const std::string& name, Pin::ConnectionType connType, Pin::DataType dataType, int id, int engineID);
		//void AddOutput(const std::string& name, Pin::ConnectionType connType, Pin::DataType dataType, int id, int engineID);

		bool IsInstanced(const Graph& graph) const;
		bool IsDependentOn(int modID, const Graph& graph, bool recurse) const;

		void SetID(int id) { _id = id; }
		int GetID() const { return _id; }

	protected:
		int _id;
		Tag _type;
		std::vector<Connection> _connections;
	};
}
