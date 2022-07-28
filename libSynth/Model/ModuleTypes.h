#pragma once

#include "Tag.h"
#include "Engine.h"
#include "ValueTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace Synth
{
namespace Model
{
	class PinType
	{
	public:
		enum class ConnectionType { Single, Multi };

		PinType(Tag id, const std::string& name, ConnectionType connType, int engineID) : _id(id), _name(name), _connType(connType), _engineID(engineID) {}

		ValueType& SetValueType(ValueTypePtr valtype) { _valType = std::move(valtype); return *_valType; }
		const ValueType* GetValueType() const { return _valType.get(); }

		void SetMonitor() { _monitor = true; }
		bool IsMonitor() const { return _monitor; }

		bool IsMulti() const { return _connType == ConnectionType::Multi; }

		Tag GetID() const { return _id; }
		const std::string& GetName() const { return _name; }
		ConnectionType GetConnectionType() const { return _connType; }
		int GetEngineID() const { return _engineID; }
	
	private:
		Tag _id;
		std::string _name;
		ConnectionType _connType;
		ValueTypePtr _valType;
		int _engineID;
		bool _monitor{};
	};		
	using PinTypePtr = std::unique_ptr<PinType>;

	class FieldType
	{
	public:
		FieldType(Tag id, int engineID) : _id(id), _engineID(engineID) {}

		Tag GetID() const { return _id; }
		int GetEngineID() const { return _engineID; }
	
	private:
		Tag _id;
		int _engineID;
	};		
	using FieldTypePtr = std::unique_ptr<FieldType>;

	class ModuleType
	{
	public:
		ModuleType(Tag id, const std::string& name, Engine::ModuleType engineID) : _id(id), _name(name), _engineID(engineID) {}
	
		PinType& AddInput(PinTypePtr input) { _inputs.push_back(std::move(input)); return *_inputs.back(); }
		PinType& AddOutput(PinTypePtr output) { _outputs.push_back(std::move(output));  return *_outputs.back(); }
		FieldType& AddField(FieldTypePtr field) { _fields.push_back(std::move(field));  return *_fields.back(); }

		Tag GetID() const { return _id; }
		const std::string& GetName() const { return _name; }
		Engine::ModuleType GetEngineID() const { return _engineID; }

		const PinType* GetInput(Tag id) const 
		{
			for (auto& pin : _inputs)
				if (pin->GetID() == id)
					return pin.get();
			return nullptr;
		}

		const PinType* GetOutput(Tag id) const 
		{
			for (auto& pin : _outputs)
				if (pin->GetID() == id)
					return pin.get();
			return nullptr;
		}

		const FieldType* GetField(Tag id) const
		{
			for (auto& field : _fields)
				if (field->GetID() == id)
					return field.get();
			return nullptr;
		}

		const std::vector<PinTypePtr>& GetInputs() const { return _inputs; }
		const std::vector<PinTypePtr>& GetOutputs() const { return _outputs; }
		const std::vector<FieldTypePtr>& GetFields() const { return _fields; }

	private:
		Tag _id;
		std::string _name;
		Engine::ModuleType _engineID;
		std::vector<PinTypePtr> _inputs, _outputs;
		std::vector<FieldTypePtr> _fields;
	};

	class ModuleTypes
	{
	public:
		ModuleTypes();
		static const ModuleType* Find(Tag id);

	private:
		static const ModuleTypes& Instance();
		std::vector<ModuleType> _types;
	};
}
}
