#include "stdafx.h"
#include "ValueTypes.h"
#include <algorithm>
#include <sstream>

using namespace Synth::Model;

int ValueType::Clamp(int value) const
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

int TimeValueType::AddDelta(int val, int delta) const
{
	return Clamp(val + delta * 10);
}


float PercentValueType::ToFloat(int val) const
{
	return val / float(0xffff);
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

int PercentValueType::AddDelta(int val, int delta) const
{
	return Clamp(val + (int)std::round(delta * 0xffff / 100.0));
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

	return std::min(_max, std::max(_min, val));
}

int IntValueType::AddDelta(int val, int delta) const
{
	return std::min(_max, std::max(_min, val + delta));
}
