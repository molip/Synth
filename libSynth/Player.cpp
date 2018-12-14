#include "stdafx.h"
#include "Player.h"

#include "../libKernel/Debug.h"

#include "../Engine/Common.h"
#include "../Engine/Input.h"

#include <chrono>

using namespace std::chrono_literals;
using namespace Synth;

bool Player::_initialised;

Player::Player()
{
	if (!_initialised)
		Engine::Config::Init();

	for (int i = 0; i < BufferCount; ++i)
		_buffers[i].resize(Engine::Config::sampleRate / 100); // 10ms per buffer.
}

Player::~Player()
{
	Stop();
}

void Player::Start()
{
	_bufferThread = std::thread(&Player::Go, this);
}

void Player::Stop()
{
	if (!_abort)
	{
		_abort = true;
		_bufferThread.join();
	}
}

bool Player::ProcessData(const Buffer& buffer)
{
	std::lock_guard<std::mutex> lock(_dataMutex);
	_dataQueue.insert(_dataQueue.end(), buffer.begin(), buffer.end());

	while (!_dataQueue.empty())
	{
		const byte data = _dataQueue.front();
		_dataQueue.pop_front();
		Engine::RemoteInput::Process(data);
	}

	return true;
}

void Player::OnBufferFinished()
{
	{
		std::lock_guard<std::mutex> lock(_bufferMutex);
		if (--_submitted <= 0)
			Kernel::Debug::Trace << "[Player::OnBufferFinished] Ran out of buffers!" << std::endl;
	}
	_cvBuffer.notify_one();
}

void Player::Go()
{
	while (!_abort)
	{
		while (_submitted < BufferCount)
		{
			DoBuffer();
		}

		{
			std::unique_lock<std::mutex> lock(_bufferMutex);
			_cvBuffer.wait(lock, [&]() { return _submitted < BufferCount; });
		}
	}
}

void Player::DoBuffer()
{
	std::lock_guard<std::mutex> lock(_dataMutex);

	auto& buffer = _buffers[_currentBuffer];
	if (Engine::Graph* graph = Engine::Graph::GetActive())
	{
		graph->UpdateKnobsRemote();

		for (auto& sample : buffer)
		{
			const float level = graph->Update();
			sample = static_cast<int16_t>(level * 0x7fff);
		}
	}
	else
		for (auto& sample : buffer)
			sample = 0;

	{
		std::lock_guard<std::mutex> lock(_bufferMutex);
		++_submitted;
	}

	Submit(buffer);

	_lastBuffer = &buffer;
	_currentBuffer = (_currentBuffer + 1) % BufferCount;
}