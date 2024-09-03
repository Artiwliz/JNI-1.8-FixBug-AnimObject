#include "CDebugInfo.h"

#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "chatwindow.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "CSettings.h"
#include "util/armhook.h"

extern CGUI* pGUI;
extern CChatWindow* pChatWindow;

uint32_t CDebugInfo::uiStreamedPeds = 0;
uint32_t CDebugInfo::uiStreamedVehicles = 0;
uint32_t CDebugInfo::m_uiDrawDebug = 0;
uint32_t CDebugInfo::m_uiDrawFPS = 0;
uint32_t CDebugInfo::m_dwSpeedMode = 0;
uint32_t CDebugInfo::m_dwSpeedStart = 0;

void CDebugInfo::ToggleDebugDraw()
{
	m_uiDrawDebug ^= 1;
}

void CDebugInfo::SetDrawFPS(uint32_t bDraw)
{
	m_uiDrawFPS = bDraw;
}

void CDebugInfo::Draw()
{
	char szStr[30];
	ImVec2 pos;
	if (m_uiDrawDebug || m_uiDrawFPS)
	{ 	
		float* pFPS = (float*)(SA_ADDR(0x608E00));
		snprintf(&szStr[0], 30, "%.1f", *pFPS);
		pos = ImVec2(pGUI->ScaleX(10.0f), pGUI->ScaleY(10.0f));

		pGUI->RenderText(pos, (ImU32)0xFFFFFFFF, true, &szStr[0]);
	}

#ifdef DEBUG_INFO_ENABLED

	if (!m_uiDrawDebug)
	{
		return;
	}

#endif
}

void CDebugInfo::ApplyDebugPatches()
{
#ifdef DEBUG_INFO_ENABLED

	unProtect(SA_ADDR(0x8B8018));
	*(uint8_t*)(SA_ADDR(0x8B8018)) = 1;
	makeNOP(SA_ADDR(0x399EDA), 2);
	makeNOP(SA_ADDR(0x399F46), 2);
	makeNOP(SA_ADDR(0x399F92), 2);

#endif
}

void CDebugInfo::ToggleSpeedMode()
{
	m_dwSpeedMode ^= 1;
	if (m_dwSpeedMode)
	{
		pChatWindow->AddDebugMessage(OBFUSCATE("Speed mode enabled"));
	}
	else
	{
		pChatWindow->AddDebugMessage(OBFUSCATE("Speed mode disabled"));
	}
}

void CDebugInfo::ProcessSpeedMode(VECTOR* pVecSpeed)
{
	if (!m_dwSpeedMode)
	{
		return;
	}
	static uint32_t m_dwState = 0;
	float speed = sqrt((pVecSpeed->X * pVecSpeed->X) + (pVecSpeed->Y * pVecSpeed->Y) + (pVecSpeed->Z * pVecSpeed->Z)) * 2.0f * 100.0f;
	if (speed >= 1.0f)
	{
		if (!m_dwSpeedStart)
		{
			m_dwSpeedStart = GetTickCount();
			m_dwState = 0;
			pChatWindow->AddDebugMessage("Start");
		}
		if ((speed >= 119.0f) && (speed <= 121.0f) && (m_dwState == 0))
		{
			pChatWindow->AddDebugMessage("1 to 100: %d", GetTickCount() - m_dwSpeedStart);
			m_dwSpeedStart = GetTickCount();
			m_dwState = 1;
		}
		if ((speed >= 230.0f) && (speed <= 235.0f) && (m_dwState == 1))
		{
			pChatWindow->AddDebugMessage("100 to 200: %d", GetTickCount() - m_dwSpeedStart);
			m_dwSpeedStart = 0;
			m_dwState = 0;
		}
		// process for 100 and 200
	}
	else
	{
		if (m_dwSpeedStart)
		{
			m_dwSpeedStart = 0;
			m_dwState = 0;
			pChatWindow->AddDebugMessage(OBFUSCATE("Reseted"));
			return;
		}
	}
}
