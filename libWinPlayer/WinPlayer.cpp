#include "stdafx.h"
#include "WinPlayer.h"

#include "../libKernel/Debug.h"

void WinPlayer::VoiceCallback::OnBufferEnd(void* pBufferContext)
{
	_player.OnBufferEnd(pBufferContext);
}


WinPlayer::WinPlayer() : _callback(*this)
{
	HRESULT hr = Init();
	KERNEL_ASSERT(SUCCEEDED(hr));
}

WinPlayer::~WinPlayer()
{
	Stop();

	if (_pSourceVoice)
		_pSourceVoice->DestroyVoice();

	if (_pXAudio2)
		_pXAudio2->StopEngine();
}

HRESULT WinPlayer::Init()
{
	CoInitialize(nullptr);

	HRESULT hr;
	if (FAILED(hr = XAudio2Create(&_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		return hr;

	IXAudio2MasteringVoice* pMasterVoice = NULL;
	if (FAILED(hr = _pXAudio2->CreateMasteringVoice(&pMasterVoice)))
		return hr;

	WAVEFORMATEX wfx{};
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 48000;
	wfx.nAvgBytesPerSec = 96000;
	wfx.nBlockAlign = 2;
	wfx.wBitsPerSample = 16;
	wfx.cbSize = 0;

	if (FAILED(hr = _pXAudio2->CreateSourceVoice(&_pSourceVoice, &wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &_callback)))
		return hr;

	if (FAILED(hr = _pSourceVoice->Start(0)))
		return hr;

	Start();

	return 0;
}

void WinPlayer::Submit(const AudioBuffer& buffer)
{
	XAUDIO2_BUFFER buffer2{};
	buffer2.AudioBytes = static_cast<UINT32>(buffer.size() * 2);
	buffer2.pAudioData = reinterpret_cast<const BYTE*>(&buffer[0]);
	HRESULT hr = _pSourceVoice->SubmitSourceBuffer(&buffer2);
	if (SUCCEEDED(hr))
	{
		std::lock_guard<std::mutex> lock(_queueCountMutex);
		++_queueCount;
		//Kernel::Debug::Trace << "Submitted - queue count = " <<  << std::endl;
	}
}

void WinPlayer::OnBufferEnd(void* pBufferContext)
{
	OnBufferFinished();

	std::lock_guard<std::mutex> lock(_queueCountMutex);
	--_queueCount;
	//Kernel::Debug::Trace << "Finished - queue count = " << --_queueCount << std::endl;
}
