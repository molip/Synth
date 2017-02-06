#pragma once

#include <algorithm>
#include <mutex>
#include <thread>
#include <vector>

#undef min
#undef max

class Serial
{
public:
	Serial();
	~Serial();

	bool Open();
	bool Close();
	bool IsOpen() const { return _file != nullptr; }

	std::wstring GetPortName() const { return _portName; }

private:
	void Read();
	std::wstring FindPortName() const;

	HANDLE _file;
	std::wstring _portName;
};
