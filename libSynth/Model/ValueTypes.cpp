#include "stdafx.h"
#include "ValueTypes.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace Synth::Model;

int ValueType::Clamp(int value) const
{
	return std::max(std::min(_max, value), _min);
}

int ValueType::AddDelta(int val, int delta, bool coarse) const
{
	return val + delta * _deltaMult * (coarse ? _coarseSteps : 1);
}


std::string TimeValueType::ToString(int val) const
{
	std::ostringstream oss;
	oss << val << "ms";
	return oss.str();
}

int TimeValueType::FromString(const std::string& str) const
{
	std::istringstream iss(str);
	int val = 0;
	iss >> val;
	return val;
}


float PercentValueType::ToFloat(int val) const
{
	return val * 0.01f;
}

std::string PercentValueType::ToString(int val) const
{
	std::ostringstream oss;
	if (val < 0)
	{
		oss << '-';
		val = -val;
	}
	oss << val / 100 << '.' << std::setfill('0') << std::setw(2) << val % 100;
	return oss.str();
}

int PercentValueType::FromString(const std::string& str) const
{
	std::istringstream iss(str);
	double val = 0;
	iss >> val;

	return static_cast<int>(std::round(val * 100));
}


std::string IntValueType::ToString(int val) const
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

int IntValueType::FromString(const std::string& str) const
{
	std::istringstream iss(str);
	int val = 0;
	iss >> val;

	return val;
}
