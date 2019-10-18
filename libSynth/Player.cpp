#include "stdafx.h"
#include "Player.h"

#include "../libKernel/Debug.h"

#include "../Engine/Common.h"
#include "../Engine/Input.h"

#include <chrono>

using namespace std::chrono_literals;
using namespace Synth;

bool Player::_initialised;

Player::Player(int sampleRate)
{
	if (!_initialised)
		Engine::Config::Init(sampleRate);

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

void Player::ProcessData(const Buffer& buffer)
{
	std::lock_guard<std::mutex> lock(_dataMutex);
	_dataQueue.insert(_dataQueue.end(), buffer.begin(), buffer.end());
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

void Player::StartCapture()
{
	std::lock_guard<std::mutex> lock(_captureMutex);
	_capture.clear();
	_capturing = true;
}

void Player::StopCapture()
{
	std::lock_guard<std::mutex> lock(_captureMutex);
	_capturing = false;
}

Player::AudioBuffer Player::HarvestCapture()
{
	KERNEL_ASSERT(!_capturing);
	return std::move(_capture);
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
	auto& buffer = _buffers[_currentBuffer];

	for (auto& sample : buffer)
	{
		if (Engine::Graph* graph = Engine::Graph::GetActive())
		{
			graph->UpdateKnobsRemote();

			const float level = graph->Update();
			sample = static_cast<int16_t>(level * 0x7fff);
		}
		else
			sample = 0;

		std::lock_guard<std::mutex> lock(_dataMutex);
		if (!_dataQueue.empty())
		{
			const byte data = _dataQueue.front();
			_dataQueue.pop_front();
			Engine::RemoteInput::Process(data);
		}
	}

	{
		std::lock_guard<std::mutex> lock(_monitorMutex);
		if (Engine::Graph* graph = Engine::Graph::GetActive())
			_monitorLevels = graph->GetMonitorLevels();
		else
			_monitorLevels.clear();
	}

	{
		std::lock_guard<std::mutex> lock(_bufferMutex);
		++_submitted;
	}

	Submit(buffer);

	_lastBuffer = &buffer;
	_currentBuffer = (_currentBuffer + 1) % BufferCount;

	std::lock_guard<std::mutex> lock(_captureMutex);
	if (_capturing)
		_capture.insert(_capture.end(), buffer.begin(), buffer.end());
}

std::vector<std::vector<float>> Player::GetMonitorLevels() const
{ 
	std::lock_guard<std::mutex> lock(_monitorMutex);
	return _monitorLevels;
}
