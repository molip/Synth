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

		virtual float ToFloat(int val) const { return static_cast<float>(val); }
		virtual std::string ToString(int val) const = 0;
		virtual int FromString(const std::string& str) const = 0;
		virtual int AddDelta(int val, int delta) const = 0;

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
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
		virtual int AddDelta(int val, int delta) const override;
	};

	class PercentValueType : public ValueType
	{
	public:
		PercentValueType(int default) : ValueType(DisplayType::Number, default) {}
		virtual float ToFloat(int val) const override;
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
		virtual int AddDelta(int val, int delta) const override;
	};

	class IntValueType : public ValueType
	{
	public:
		IntValueType(int default, int min, int max) : ValueType(DisplayType::Number, default), _min(min), _max(max) {}
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
		virtual int AddDelta(int val, int delta) const override;
	private:
		int _min, _max;
	};

	class BoolValueType : public IntValueType
	{
	public:
		BoolValueType(bool default) : IntValueType(default, 0, 1) {}
	};

}
}
