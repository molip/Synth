#pragma once

#include "Tag.h"

#include <memory>
#include <string>
#include <vector>

namespace Synth
{
namespace Model
{
	class ValueType
	{
	public:
		enum class DisplayType { Number, Select, };

		ValueType(DisplayType type, int default) : _default(default) {}
		virtual std::string ToString(int val) const = 0;
		virtual int FromString(const std::string& str) const = 0;
	
		const int& GetDefault() const { return _default; }

	protected:
		int Clamp(int value) const;

	private:
		int _default = 0;
	};
	using ValueTypePtr = std::unique_ptr<ValueType>;

	class TimeValueType : public ValueType
	{
	public:
		TimeValueType(int default) : ValueType(DisplayType::Number, default) {}
		virtual std::string ToString(int val) const;
		virtual int FromString(const std::string& str) const;
	};

	class PercentValueType : public ValueType
	{
	public:
		PercentValueType(int default) : ValueType(DisplayType::Number, default) {}
		virtual std::string ToString(int val) const;
		virtual int FromString(const std::string& str) const;
	};
}
}
