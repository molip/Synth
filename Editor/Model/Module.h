#pragma once

#include "Defs.h"
#include "Tag.h"
#include <vector>
#include <map>

namespace Model
{
	class Graph;
	class Module;
	class ModuleType;
	class PinType;

	struct PinRef
	{
		void Save(Serial::SaveNode& node) const;
		void Load(Serial::LoadNode& node);

		int moduleID;
		Tag type;
	};

	class Module
	{
	public:
		using ConnectionMap = std::map<Tag, PinRef>;
		using ValueMap = std::map<Tag, int>;

		Module(Tag type = "");

		const ModuleType& GetDef() const;
		const PinType& GetInputDef(Tag type) const;
		const PinType& GetOutputDef(Tag type) const;
		const Module& GetSourceModule(const PinRef& pin, const Graph& graph) const;
		const PinType& GetSourceOutputDef(const PinRef& pin, const Graph& graph) const;

		const ConnectionMap& GetConnections() const { return _connections; }
		void Connect(Tag inputType, int modID, Tag outputType);

		const ValueMap& GetValues() const { return _values; }
		void SetValue(Tag inputType, int value);

		//void AddInput(const std::string& name, Pin::ConnectionType connType, Pin::DataType dataType, int id, int engineID);
		//void AddOutput(const std::string& name, Pin::ConnectionType connType, Pin::DataType dataType, int id, int engineID);

		bool IsInstanced(const Graph& graph) const;
		bool IsDependentOn(int modID, const Graph& graph, bool recurse) const;

		void SetID(int id) { _id = id; }
		int GetID() const { return _id; }

		void Save(Serial::SaveNode& node) const;
		void Load(Serial::LoadNode& node);

	protected:
		int _id;
		Tag _type;
		ConnectionMap _connections;
		ValueMap _values; 
	};
}
