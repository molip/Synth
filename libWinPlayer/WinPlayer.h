#pragma once

#include "../libSynth/Player.h"

#include <xaudio2.h>

class WinPlayer	: public Synth::Player
{
public:
	WinPlayer();
	~WinPlayer();

private:
	HRESULT Init();
	virtual void Submit(const AudioBuffer& buffer) override;

	void OnBufferEnd(void* pBufferContext);

	class VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		VoiceCallback(WinPlayer& player) : _player(player) {}

		void OnBufferEnd(void* pBufferContext);

		void OnStreamEnd() {}
		void OnVoiceProcessingPassEnd() {}
		void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
		void OnBufferStart(void* pBufferContext) {}
		void OnLoopEnd(void* pBufferContext) {}
		void OnVoiceError(void* pBufferContext, HRESULT Error) {}

	private:
		WinPlayer& _player;
	};

	IXAudio2* _pXAudio2 = nullptr;
	IXAudio2SourceVoice* _pSourceVoice = nullptr;
	VoiceCallback _callback;
	int _queueCount = 0;
	std::mutex _queueCountMutex;
};
