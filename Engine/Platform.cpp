#include "Platform.h"

#ifdef _WIN32
#include "../libKernel/Debug.h"

void Engine::SerialPrint(const char* val)
{
	Kernel::Debug::Trace << val;
}

void Engine::SerialPrint(const int val)
{
	Kernel::Debug::Trace << val;
}

void Engine::SerialPrintLn(const char* val)
{
	Kernel::Debug::Trace << val << std::endl;
}

void Engine::SerialPrintLn(const int val)
{
	Kernel::Debug::Trace << val << std::endl;
}

#endif