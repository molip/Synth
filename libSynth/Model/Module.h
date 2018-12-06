#pragma once

#include "Defs.h"
#include "Geometry.h"
#include "Tag.h"

#include <vector>
#include <map>
#include <set>

namespace Synth
{
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
		bool operator ==(const PinRef& rhs) const { return moduleID == rhs.moduleID && type == rhs.type; }

		void Save(Serial::SaveNode& node) const;
		void Load(Serial::LoadNode& node);

		int moduleID;
		Tag type;
	};

	struct InputParams
	{
		int offset = 0;
		int scale = 0;

		bool operator ==(const InputParams& rhs) const;
		void Save(Serial::SaveNode& node) const;
		void Load(Serial::LoadNode& node);
	};

	class Module
	{
	public:
		using ConnectionMap = std::map<Tag, PinRef>;
		using InputParamsMap = std::map<Tag, InputParams>;

		Module(Tag type = "");

		const ModuleType& GetDef() const;
		const PinType& GetInputDef(Tag type) const;
		const PinType& GetOutputDef(Tag type) const;
		const Module& GetSourceModule(const PinRef& pin, const Graph& graph) const;
		const PinType& GetSourceOutputDef(const PinRef& pin, const Graph& graph) const;

		const ConnectionMap& GetConnections() const { return _connections; }
		const PinRef* FindConnection(Tag type) const;
		
		void SetInputParams(Tag inputType, InputParams params);
		const InputParams* FindInputParams(Tag type) const;

		bool IsInstanced(const Graph& graph) const;
		bool IsDependentOn(int modID, const Graph& graph) const;

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
		const InputParams* GetDefaultInputParams(Tag type) const;
		bool IsInstanced(const Graph& graph, std::set<int>& done) const;
		bool IsDependentOn(int modID, const Graph& graph, std::set<int>& done) const;

		int _id;
		Tag _type;
		ConnectionMap _connections;
		InputParamsMap _inputParams;
		Point _position;
	};
}
}