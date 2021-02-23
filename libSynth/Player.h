#pragma once

#include "Types.h"

#include <deque>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace Synth
{
	class Player
	{
	public:
		Player(int sampleRate);
		~Player();
		void ProcessData(const Buffer& buffer);

		using AudioBuffer = std::vector<int16_t>;
		using MonitorLevels = std::vector<std::vector<float>>; // Values per channel per module.
		using MonitorLevelsBuffer = std::vector<MonitorLevels>; // MonitorLevels per sample.
		const AudioBuffer* GetLastBuffer() const { return _lastBuffer; }

		struct Capture
		{
			AudioBuffer audio;
			MonitorLevelsBuffer monitors;
		};

		void StartCapture();
		void StopCapture();
		Capture HarvestCapture();
		bool IsCapturing() const { return _capturing; }

		MonitorLevels GetMonitorLevels() const;

	protected:
		void Start();
		void Stop();
		void OnBufferFinished();

	private:
		void Go();
		void DoBuffer();

		virtual void Submit(const AudioBuffer& buffer) = 0;

		static bool _initialised;
		std::deque<byte> _dataQueue;
		std::thread _bufferThread;
		std::mutex _bufferMutex, _dataMutex, _captureMutex;
		mutable std::mutex _monitorMutex;
		std::condition_variable _cvBuffer, _cvData;
		bool _abort = false;
		//bool _request = false;
		
		static const int BufferCount = 3;
		int _currentBuffer = 0;
		int _submitted = 0;
		AudioBuffer _buffers[BufferCount];
		const AudioBuffer* _lastBuffer = nullptr;
		
		Capture _capture;
		bool _capturing = false;

		MonitorLevels _monitorLevels;
	};
}
