#pragma once

#include "Defs.h"
#include "Geometry.h"
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
		PinRef() : moduleID(0) {}
		PinRef(int _moduleID, Tag _type) : moduleID(_moduleID), type(_type) {}
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

		using ConnectionPair = std::pair<Tag, PinRef>;
		using ValuePair = std::pair<Tag, int>;

		Module(Tag type = "");

		const ModuleType& GetDef() const;
		const PinType& GetInputDef(Tag type) const;
		const PinType& GetOutputDef(Tag type) const;
		const Module& GetSourceModule(const PinRef& pin, const Graph& graph) const;
		const PinType& GetSourceOutputDef(const PinRef& pin, const Graph& graph) const;

		const ConnectionMap& GetConnections() const { return _connections; }
		
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

		struct ConnectionUndo
		{
			Tag inputType;
			PinRef source;
		};
		ConnectionUndo AddConnection(Tag inputType, PinRef outputPin);
		ConnectionUndo RemoveConnection(Tag inputType);
		std::vector<ConnectionUndo> RemoveConnectionsToSource(int moduleID);
		void ApplyUndo(const ConnectionUndo& undo);

		void SetPosition(const Point& pos) { _position = pos; }
		const Point& GetPosition() const { return _position; }

	protected:
		int _id;
		Tag _type;
		ConnectionMap _connections;
		ValueMap _values; 
		Point _position;
	};
}
