#pragma once

#include "Tag.h"
#include "Engine.h"

#include <memory>
#include <string>
#include <vector>

namespace Model
{
	class PinType
	{
	public:
		enum class ConnectionType { Single, Multi };
		enum class DataType { Signed, Unsigned };

		PinType(Tag id, const std::string& name, ConnectionType connType, DataType dataType, int engineID) : _id(id), _name(name), _connType(connType), _dataType(dataType), _engineID(engineID) {}

		bool IsMulti() const { return _connType == ConnectionType::Multi; }

		Tag GetID() const { return _id; }
		const std::string& GetName() const { return _name; }
		ConnectionType GetConnectionType() const { return _connType; }
		DataType GetDataType() const { return _dataType; }
	
	private:
		Tag _id;
		std::string _name;
		ConnectionType _connType;
		DataType _dataType;
		int _engineID;
	};		

	class ModuleType
	{
	public:
		ModuleType(Tag id, const std::string& name, Engine::ModuleType engineID) : _id(id), _name(name), _engineID(engineID) {}
	
		void AddInput(const PinType& input) { _inputs.push_back(input); }
		void AddOutput(const PinType& output) { _outputs.push_back(output); }

		Tag GetID() const { return _id; }
		const std::string& GetName() const { return _name; }
		Engine::ModuleType GetEngineID() const { return _engineID; }

		const PinType* GetInput(Tag id) const 
		{
			for (auto& pin : _inputs)
				if (pin.GetID() == id)
					return &pin;
			return nullptr;
		}

		const PinType* GetOutput(Tag id) const 
		{
			for (auto& pin : _outputs)
				if (pin.GetID() == id)
					return &pin;
			return nullptr;
		}

		const std::vector<PinType>& GetInputs() const { return _inputs; }
		const std::vector<PinType>& GetOutputs() const { return _outputs; }

	private:
		Tag _id;
		std::string _name;
		Engine::ModuleType _engineID;
		std::vector<PinType> _inputs, _outputs;
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