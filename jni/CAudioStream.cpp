/*#include "main.h"
#include "game/game.h"

#include <pthread.h>
#include <mutex>
#include <chrono>
#include <thread>

#include "vendor/bass/bass.h"
#include "CAudioStream.h"

extern CGame* pGame;

CAudioStream::CAudioStream()
{
	m_strAudioStreamURL.clear();

	m_fAudioStreamRadius = -1;
	m_bassStreamSample = NULL;

	m_bUsePosition = false;
	m_bPlaying = false;
	m_bPaused = false;

	memset(&m_vecAudioStreamPosition, 0, sizeof(m_vecAudioStreamPosition));
}

CAudioStream::~CAudioStream()
{
	if (m_bPlaying) Stop();
	m_strAudioStreamURL.clear();
}

bool CAudioStream::PlayByURL(const char* url, float fX, float fY, float fZ, float fRadius, bool bUsePosition)
{
	if (m_bPlaying && m_bassStreamSample)
	{
		if (!Stop())
			return false;
	}

	m_strAudioStreamURL = url;

	m_vecAudioStreamPosition.X = fX;
	m_vecAudioStreamPosition.Y = fY;
	m_vecAudioStreamPosition.Z = fZ;

	m_fAudioStreamRadius = fRadius;
	m_bUsePosition = bUsePosition;

	m_bPaused = false;

	m_bassStreamSample = BASS_StreamCreateURL(m_strAudioStreamURL.c_str(), 0, 9699328, nullptr, nullptr);
	if (m_bassStreamSample)
	{
		BASS_ChannelPlay(m_bassStreamSample, true);
		m_bPlaying = true;
	}

	return true;
}

bool CAudioStream::Stop()
{
	if (m_bPlaying && m_bassStreamSample)
	{
		BASS_StreamFree(m_bassStreamSample);
		m_bassStreamSample = NULL;
		m_bPlaying = false;
	}
	return true;
}

void CAudioStream::Process()
{
	if (!m_bPlaying) return;

	if (!m_bPaused)
	{
		if (pGame->IsGamePaused())
		{
			if (m_bassStreamSample)
			{
				BASS_ChannelPause(m_bassStreamSample);
				m_bPaused = true;
			}
		}
		else if (m_bUsePosition && (m_fAudioStreamRadius != -1))
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				MATRIX4X4 playerMatrix;
				pPlayerPed->GetMatrix(&playerMatrix);

				if (GetDistanceBetween3DPoints(&m_vecAudioStreamPosition, &playerMatrix.pos) > m_fAudioStreamRadius)
				{
					if (m_bassStreamSample)
					{
						BASS_ChannelPause(m_bassStreamSample);
						m_bPaused = true;
					}
				}
			}
		}
	}
	else
	{
		if (!pGame->IsGamePaused())
		{
			if (m_bassStreamSample)
			{
				BASS_ChannelPlay(m_bassStreamSample, false);
				m_bPaused = false;
			}
		}
		else if (m_bUsePosition && (m_fAudioStreamRadius != -1))
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				MATRIX4X4 playerMatrix;
				pPlayerPed->GetMatrix(&playerMatrix);

				if (GetDistanceBetween3DPoints(&m_vecAudioStreamPosition, &playerMatrix.pos) <= m_fAudioStreamRadius)
				{
					if (m_bassStreamSample)
					{
						BASS_ChannelPlay(m_bassStreamSample, false);
						m_bPaused = false;
					}
				}
			}
		}
	}
}*/

#include <pthread.h>
#include <dlfcn.h>

#include "main.h"
#include "game/game.h"
#include "CAudiostream.h"

extern CGame* pGame;

char g_szAudioStreamUrl[256];
float g_fAudioStreamX;
float g_fAudioStreamY;
float g_fAudioStreamZ;
float g_fAudioStreamRadius;
bool g_audioStreamUsePos;
bool g_bAudioStreamStop;
pthread_t g_bAudioStreamThreadWorked;
uintptr_t bassStream;

CAudioStream::CAudioStream()
{

}
void CAudioStream::Initialize()
{
	bassStream = 0;
	BASS_SetConfigPtr(16, "SA-MP/0.3");
	BASS_Free();
	if (BASS_Init(-1, 44100, 0))
	{
		BASS_SetConfigPtr(16, "SA-MP/0.3");
		BASS_SetConfig(21, 1);
		BASS_SetConfig(11, 10000);
	}
}

void CAudioStream::Process()
{
	MATRIX4X4 mat;
	pGame->FindPlayerPed()->GetMatrix(&mat);

	VECTOR vecPlayerPos = { mat.pos.X, mat.pos.Y, mat.pos.Z };
	VECTOR vecAudioPos = { g_fAudioStreamX, g_fAudioStreamY, g_fAudioStreamZ };

    if (m_bPlayingAudio && m_bChannelPause && bassStream)
    {
        if (pGame->IsGamePaused())
        {
            BASS_ChannelPause(bassStream);
            m_bChannelPause = true;
        }
        else
        {
            if (g_audioStreamUsePos)
            {
                if (GetDistanceFromVectorToVector(&vecPlayerPos, &vecAudioPos) <= g_fAudioStreamRadius)
                {
                    BASS_ChannelPlay(bassStream, false);
                    m_bChannelPause = false;
                }
                else
                {
                    BASS_ChannelPause(bassStream);
                    m_bChannelPause = true;
                }
            }
            else
            {
                BASS_ChannelPlay(bassStream, false);
                m_bChannelPause = false;
            }
        }
    }
}

void* audioStreamThread(void* p)
{
	g_bAudioStreamThreadWorked = 1;
	if (bassStream)
	{
		BASS_ChannelStop(bassStream);
		bassStream = 0;
	}
	bassStream = BASS_StreamCreateURL(g_szAudioStreamUrl, 0, 9699328, 0);
	BASS_ChannelPlay(bassStream, false);
	while (!g_bAudioStreamStop) usleep(2000);
	BASS_ChannelStop(bassStream);
	bassStream = 0;
	g_bAudioStreamThreadWorked = 0;
	pthread_exit(nullptr);
}

void CAudioStream::Play(char const* szURL, float X, float Y, float Z, float Radius, bool bUsePos)
{
    if (g_bAudioStreamThreadWorked)
	{
		g_bAudioStreamStop = true;

		do
			usleep(2000);
		while (g_bAudioStreamThreadWorked);

		BASS_StreamFree(bassStream);
		bassStream = 0;
	}
	if (bassStream)
	{
		BASS_StreamFree(bassStream);
		bassStream = 0;
	}
	memset(g_szAudioStreamUrl, 0, 256);
	strncpy(g_szAudioStreamUrl, szURL, 256);

	g_fAudioStreamX = X;
	g_fAudioStreamY = Y;
	g_fAudioStreamZ = Z;

	g_fAudioStreamRadius = Radius;
	g_audioStreamUsePos = bUsePos;
	g_bAudioStreamStop = false;

	m_bPlayingAudio = true;
	m_bChannelPause = false;

	pthread_create(&g_bAudioStreamThreadWorked, nullptr, audioStreamThread, nullptr);
}

void CAudioStream::Stop(bool bStop)
{
	if (g_bAudioStreamThreadWorked)
	{
		g_bAudioStreamStop = true;
		if (bStop)
		{
			do
				usleep(200);
			while (g_bAudioStreamThreadWorked);
		}

		BASS_StreamFree(bassStream);
		bassStream = 0;

		m_bPlayingAudio = false;
		m_bChannelPause = false;
	}
}