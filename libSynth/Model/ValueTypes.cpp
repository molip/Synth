#include "stdafx.h"
#include "ValueTypes.h"
#include <algorithm>
#include <sstream>

using namespace Synth::Model;

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

	return int(0xffff * std::min(1.0, val / 100.0));
}
