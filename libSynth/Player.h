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
		Player();
		~Player();
		bool ProcessData(const Buffer& buffer);

		using AudioBuffer = std::vector<int16_t>;

		const AudioBuffer* GetLastBuffer() const { return _lastBuffer; }

	protected:
		void Start();
		void Stop();
		void OnBufferFinished();

	private:
		void Go();
		void GoData();
		void DoBuffer();

		virtual void Submit(const AudioBuffer& buffer) = 0;

		static bool _initialised;
		std::deque<byte> _dataQueue;
		std::thread _bufferThread;
		std::mutex _bufferMutex, _dataMutex;
		std::condition_variable _cvBuffer, _cvData;
		bool _abort = false;
		//bool _request = false;
		
		static const int BufferCount = 3;
		int _currentBuffer = 0;
		int _submitted = 0;
		AudioBuffer _buffers[BufferCount];
		const AudioBuffer* _lastBuffer = nullptr;
	};
}
