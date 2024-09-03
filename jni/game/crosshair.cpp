#include "../main.h"
#include "game.h"
#include "crosshair.h"

#include "../gui/gui.h"
#include "../net/netgame.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;

RwTexture* pCrossHairTexture = nullptr;

CCrossHair::CCrossHair()
{
	SetTexture(LoadTextureFromDB("txd", "siteM16"));
}

void CCrossHair::ChangeAim(const char* szName)
{
	Delete();
	pCrossHairTexture = nullptr;
	if (strstr(szName, "crosshair_0")) 
	{
		pCrossHairTexture = (RwTexture*)LoadTextureFromDB("txd", "siteM16");
		
		SetTexture(pCrossHairTexture);
	}
	else
	{
		pCrossHairTexture = (RwTexture*)LoadTextureFromDB("samp", szName);
		if (!pCrossHairTexture) pCrossHairTexture = (RwTexture*)LoadTextureFromDB("txd", "siteM16");
		
		SetTexture(pCrossHairTexture);
	}
}

void CCrossHair::Render()
{
	if (!pGame || !pNetGame) return;
	if (!pNetGame->GetPlayerPool()->GetLocalPlayer()) return;
	
	CPlayerPed* pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	if (!pPlayerPed) return;
	
	int iCamState = *((uint16_t*)(SA_ADDR(0x8B0808)) + 264 * *(int8_t*)(SA_ADDR(0x8B085F)) + 191);
	
	int v1 = iCamState & 0xFFFD;
	if(iCamState == 53 || iCamState == 39 || v1 == 40)
	{
		//float fFixChair1 = (RsGlobal->maximumWidth - (RsGlobal->maximumHeight / 9 * 16)) / 2;
		//float fFixChair2 = ((RsGlobal->maximumHeight / 9 * 16) * 0.524);
		float fCHairScreenMultX = (RsGlobal->maximumWidth - (RsGlobal->maximumHeight / 9 * 16)) / 2 + ((RsGlobal->maximumHeight / 9 * 16) * 0.524);
		Log("fCHairScreenMultX: %f", fCHairScreenMultX);
		if (fCHairScreenMultX <= 1080 || fCHairScreenMultX >= 1920) // fix linked limits
		{
			m_UsedCrossHair = false;
			return;
		}
		
		m_UsedCrossHair = true;
		float fFixedOffset = RsGlobal->maximumWidth / (RsGlobal->maximumWidth - (RsGlobal->maximumHeight / 9 * 16)) * 2.0;

		if(fFixedOffset < 0)
			fFixedOffset = 0;

		float fCHairScreenMultY = (RsGlobal->maximumHeight / 9 * 16) / 10 * 6 * 0.4 + fFixedOffset;
		Log("fCHairScreenMultY: %f", fCHairScreenMultY);
		
		// GetWeaponRadiusOnScreen
		float fRadiusWeap = ((float (*)(PED_TYPE*))(SA_ADDR(0x454F60 + 1)))(GamePool_FindPlayerPed());

		float f1 = ((RsGlobal->maximumHeight / 448.0) * 64.0) * fRadiusWeap;

		float fPosX1 = ((f1 * 0.5) + fCHairScreenMultX) - f1;
		float fPosY1 = ((f1 * 0.5) + fCHairScreenMultY) - f1;

		float fPosX2 = (f1 * 0.5) + fPosX1;
		float fPosY2 = (f1 * 0.5) + fPosY1;

		Draw(RECT{fPosX1, fPosY2, fPosX2, fPosY1}, CRGBA(255, 255, 255, 255));
		Draw(RECT{fPosX1 + f1, fPosY2, fPosX2, fPosY1}, CRGBA(255, 255, 255, 255));
		Draw(RECT{fPosX1, fPosY2, fPosX2, fPosY1 + f1}, CRGBA(255, 255, 255, 255));
		Draw(RECT{fPosX1 + f1, fPosY2, fPosX2, fPosY1 + f1}, CRGBA(255, 255, 255, 255));
	}
}