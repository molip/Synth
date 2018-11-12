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
		enum class DataType { Signed, Unsigned};

		PinType(Tag id, const std::string& name, ConnectionType connType, DataType dataType, int engineID) : _id(id), _name(name), _connType(connType), _dataType(dataType), _engineID(engineID) {}

		void SetValueType(ValueTypePtr valtype) { _valType = std::move(valtype); }
		const ValueType* GetValueType() const { return _valType.get(); }

		bool IsMulti() const { return _connType == ConnectionType::Multi; }
		bool IsSigned() const { return _dataType == DataType::Signed; }

		Tag GetID() const { return _id; }
		const std::string& GetName() const { return _name; }
		ConnectionType GetConnectionType() const { return _connType; }
		DataType GetDataType() const { return _dataType; }
		int GetEngineID() const { return _engineID; }
	
	private:
		Tag _id;
		std::string _name;
		ConnectionType _connType;
		DataType _dataType;
		ValueTypePtr _valType;
		int _engineID;
	};		
	using PinTypePtr = std::unique_ptr<PinType>;

	class ModuleType
	{
	public:
		ModuleType(Tag id, const std::string& name, Engine::ModuleType engineID) : _id(id), _name(name), _engineID(engineID) {}
	
		PinType& AddInput(PinTypePtr input) { _inputs.push_back(std::move(input)); return *_inputs.back(); }
		void AddOutput(PinTypePtr output) { _outputs.push_back(std::move(output)); }

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

		const std::vector<PinTypePtr>& GetInputs() const { return _inputs; }
		const std::vector<PinTypePtr>& GetOutputs() const { return _outputs; }

	private:
		Tag _id;
		std::string _name;
		Engine::ModuleType _engineID;
		std::vector<PinTypePtr> _inputs, _outputs;
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
