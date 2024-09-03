#pragma once

/*#include <atomic>

class CAudioStream
{
private:
	std::string					m_strAudioStreamURL;
	VECTOR						m_vecAudioStreamPosition;
	float						m_fAudioStreamRadius;

	bool						m_bUsePosition;
	HSTREAM						m_bassStreamSample;

	bool						m_bPlaying;
	bool						m_bPaused;

public:
	CAudioStream();
	~CAudioStream();

	bool PlayByURL(const char* url, float fX, float fY, float fZ, float fRadius, bool bUsePosition);
	bool Stop();
	void Process();

	void AsyncAudioStreamSampleProcessing();
};*/


class CAudioStream
{
public:
	// Check active
	bool m_bPlayingAudio;

public:
	CAudioStream();

	void Initialize();
	void Play(char const*, float, float, float, float, bool);
	void Stop(bool);
	void Process();

private:
	bool m_bChannelPause;
};
