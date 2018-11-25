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

		ValueType(DisplayType type, int default, int min, int max, int deltaMult = 1) : _default(default), _min(min), _max(max), _deltaMult(deltaMult) {}

		virtual float ToFloat(int val) const { return static_cast<float>(val); }
		virtual std::string ToString(int val) const = 0;
		virtual int FromString(const std::string& str) const = 0;
		
		int AddDelta(int val, int delta) const;
		const int& GetDefault() const { return _default; }

	protected:
		int Clamp(int value) const;

	private:
		int _default, _min, _max, _deltaMult;
	};
	using ValueTypePtr = std::unique_ptr<ValueType>;

	class TimeValueType : public ValueType
	{
	public:
		TimeValueType(int default) : ValueType(DisplayType::Number, default, 0, 100000, 10) {}
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
	};

	class PercentValueType : public ValueType
	{
	public:
		PercentValueType(int default, int min, int max, int deltaMult = 1) : ValueType(DisplayType::Number, default, min, max, deltaMult) {}
		virtual float ToFloat(int val) const override;
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
	};

	class PitchValueType : public PercentValueType
	{
	public:
		PitchValueType() : PercentValueType(6000, 0, 12700, 100) {}
	};

	class IntValueType : public ValueType
	{
	public:
		IntValueType(int default, int min, int max) : ValueType(DisplayType::Number, default, min, max) {}
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
	};

	class BoolValueType : public IntValueType
	{
	public:
		BoolValueType(bool default) : IntValueType(default, 0, 1) {}
	};

}
}
