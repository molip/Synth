#pragma once

#include "Tag.h"
#include "Module.h"

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
		ValueType() {}

		virtual std::string ToString(int val) const = 0;
		virtual int FromString(const std::string& str) const = 0;
	};
	using ValueTypePtr = std::unique_ptr<ValueType>;

	class NumberValueType : public ValueType
	{
	public:
		enum class DisplayType { Number, Select, };

		NumberValueType(DisplayType type, int defaultOffset, int defaultScale, int min, int max, int deltaMult = 1, int coarseSteps = 10) : _inputParams{ defaultOffset, defaultScale }, _min(min), _max(max), _deltaMult(deltaMult), _coarseSteps(coarseSteps) {}

		virtual float ToFloat(int val) const { return static_cast<float>(val); }
		
		int AddDelta(int val, int delta, bool coarse) const;
		const InputParams& GetDefault() const { return _inputParams; }
		int Clamp(int value) const;

	private:
		int _min, _max, _deltaMult, _coarseSteps;
		InputParams _inputParams;
	};
	using NumberValueTypePtr = std::unique_ptr<NumberValueType>;

	class TimeValueType : public NumberValueType
	{
	public:
		TimeValueType(int defaultOffset) : NumberValueType(DisplayType::Number, defaultOffset, defaultOffset, 0, 100000, 10) {}
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
	};

	class PercentValueType : public NumberValueType
	{
	public:	
		PercentValueType(int defaultOffset, int defaultScale, int min, int max, int deltaMult = 1, int coarseSteps = 10) : NumberValueType(DisplayType::Number, defaultOffset, defaultScale, min, max, deltaMult, coarseSteps) {}
		virtual float ToFloat(int val) const override;
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
	};

	class PitchValueType : public PercentValueType
	{
	public:
		PitchValueType() : PercentValueType(0, 100, -12700, 12700, 100, 12) {}
	};

	class IntValueType : public NumberValueType
	{
	public:
		IntValueType(int defaultOffset, int defaultScale, int min, int max) : NumberValueType(DisplayType::Number, defaultOffset, defaultScale, min, max) {}
		virtual std::string ToString(int val) const override;
		virtual int FromString(const std::string& str) const override;
	};

	class BoolValueType : public IntValueType
	{
	public:
		BoolValueType(bool default) : IntValueType(default, 1, 0, 1) {}
	};

}
}
