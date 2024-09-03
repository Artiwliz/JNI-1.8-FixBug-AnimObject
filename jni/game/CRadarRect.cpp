#include "../main.h"
#include "game.h"
#include "CWeaponsOutFit.h"
#include "..//chatwindow.h"
#include "..//util/armhook.h"
#include <cmath>
#include <math.h>

RwTexture* CRadarRect::m_pRectTexture = nullptr;
RwTexture* CRadarRect::m_pDiscTexture = nullptr;
bool CRadarRect::m_bEnabled = false;

extern CChatWindow* pChatWindow;

void CRadarRect::LoadTextures()
{
	m_pDiscTexture = (RwTexture*)LoadTextureFromDB(OBFUSCATE("samp"), OBFUSCATE("radardisc"));
	m_pRectTexture = (RwTexture*)LoadTextureFromDB(OBFUSCATE("samp"), OBFUSCATE("radardisc_r"));
}

void CRadarRect::ChangeTextures(uint8_t RadarType, const char* TextureName)
{
	if (RadarType == 1) 
	{
		SetEnabled(true);
		m_pRectTexture = (RwTexture*)LoadTextureFromDB("samp", TextureName);
		if (!m_pRectTexture) m_pRectTexture = (RwTexture*)LoadTextureFromDB("samp", "radardisc_r");
	}
	else 
	{
		SetEnabled(false);
		m_pDiscTexture = (RwTexture*)LoadTextureFromDB("samp", TextureName);
		if (!m_pDiscTexture) m_pDiscTexture = (RwTexture*)LoadTextureFromDB("samp", "radardisc");
	}
	return;
}

float CRadarRect::CRadar__LimitRadarPoint_hook(float* pos)
{
	float r, angle;

	if (*(uint8_t*)(SA_ADDR(0x63E0B4)))
	{
		r = sqrtf(pos[0] * pos[0] + pos[1] * pos[1]);
		return r;
	}
	r = sqrtf(pos[0] * pos[0] + pos[1] * pos[1]);
	if (r > 1.0)
	{
		if (pos[0] > -1.0f && pos[0] < 1.0f && pos[1] > -1.0f && pos[1] < 1.0f)
			r = 0.99f;
		else
		{
			angle = atan2f(pos[1], pos[0]) * 57.295779513f + 180.0f;
			if (angle <= 45.0f || angle > 315.0f)
			{
				pos[0] = 1.0f;
				pos[1] = sinf(angle / 57.295779513f) * 1.4142135623f;
				//if (pChatWindow) pChatWindow->AddDebugMessage("process 1");
			}
			else if (angle > 45 && angle <= 135)
			{
				pos[0] = cosf(angle / 57.295779513f) * 1.4142135623f;
				pos[1] = 1.0f;
				//if (pChatWindow) pChatWindow->AddDebugMessage("process 2");
			}
			else if (angle > 135 && angle <= 225)
			{
				pos[0] = -1.0f;
				pos[1] = sinf(angle / 57.295779513f) * 1.4142135623f;
				//if (pChatWindow) pChatWindow->AddDebugMessage("process 3");
			}
			else
			{
				pos[0] = cosf(angle / 57.295779513f) * 1.4142135623f;
				pos[1] = -1.0f;
				//if (pChatWindow) pChatWindow->AddDebugMessage("process 1");
			}

			pos[0] *= (-1.0f);
			pos[1] *= (-1.0f);
		}
	}

	return r;
}

bool CRadarRect::IsEnabled()
{
	return m_bEnabled;
}

void CRadarRect::SetEnabled(bool bEnabled)
{
	unProtect(SA_ADDR(0x3DED84));
	m_bEnabled = bEnabled;
	if (!m_bEnabled)
		*(float*)(SA_ADDR(0x3DED84)) = 1.5708f;
	else *(float*)(SA_ADDR(0x3DED84)) = 0.0001f;
}
