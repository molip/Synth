#pragma once

#include <memory>
#include <vector>

namespace Synth
{
	using byte = unsigned char;
	using Buffer = std::vector<byte>;
	using BufferPtr = std::unique_ptr<Buffer>;

	class Exception : public std::runtime_error
	{
	public:
		explicit Exception(const std::string& msg) : std::runtime_error(msg) {}
	};
}
