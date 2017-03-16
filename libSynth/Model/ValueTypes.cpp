#include "stdafx.h"
#include "ValueTypes.h"
#include <algorithm>
#include <sstream>

using namespace Synth::Model;

int Synth::Model::ValueType::Clamp(int value) const
{
	return std::max(std::min(value, 0xffff), 0);
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
	return Clamp(val);
}


std::string PercentValueType::ToString(int val) const
{
	std::ostringstream oss;
	oss << int(std::round(100.0 * val / 0xffff)) << "%";
	return oss.str();
}

int PercentValueType::FromString(const std::string& str) const
{
	std::istringstream iss(str);
	double val = 0;
	iss >> val;

	return Clamp(int(0xffff * val / 100.0));
}
