#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "CSettings.h"
#include "dialog.h"
#include "voice/MicroIcon.h"
#include "extrakeyboard.h"
#include "scoreboard.h"
#include "util/CJavaWrapper.h"
 
extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CSettings *pSettings;
extern CScoreBoard *pScoreBoard;
extern CDialogWindow *pDialogWindow;

CExtraKeyBoard::CExtraKeyBoard() 
{
	m_bIsActive = false;
	m_bIsExtraShow = false;
	m_boardplayer = false;
	Log("Extrakeyboard initialized.");
}
void CExtraKeyBoard::Show(bool bShow) 
{
	m_bIsActive = bShow;
}

void CExtraKeyBoard::Render() 
{
	bool bDontExpand = false;
	if(pDialogWindow->m_bIsActive)
		bDontExpand = true;
	else if(pScoreBoard->m_bToggle)
		bDontExpand = true;

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(pPlayerPed) 
	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
            

		ImGui::Begin("Extrakeyboard", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
	

		if(!bDontExpand)
		{
			ImGui::SameLine(0, 15);
			if (ImGui::ImageButton(!m_bIsExtraShow ? (ImTextureID)MicroIcon::DownIcon->raster : (ImTextureID)MicroIcon::UpIcon->raster, ImVec2(70,70)))
			{
				if (m_bIsExtraShow)
					m_bIsExtraShow = false;
				else m_bIsExtraShow = true;
			}
			if(!m_bIsExtraShow) 
			{
				ImGui::SameLine(0, 15);
				if(ImGui::ImageButton((ImTextureID)MicroIcon::BackpackIcon->raster, ImVec2(70,70)))
					pNetGame->SendChatCommand("/apk1");
				
				ImGui::SameLine(0, 15);
				if(ImGui::ImageButton((ImTextureID)MicroIcon::DataIcon->raster, ImVec2(70,70)))
					pNetGame->SendChatCommand("/apk2");
				
				ImGui::SameLine(0, 15);//ปุ่มALT เกี่ยวกับรถ
				if(ImGui::ImageButton((ImTextureID)MicroIcon::GpsIcon->raster, ImVec2(70,70)))
					pNetGame->SendChatCommand("/apk3");
					
				ImGui::SameLine(0, 15);
				if(ImGui::ImageButton((ImTextureID)MicroIcon::PhoneIcon->raster, ImVec2(70,70)))
					pNetGame->SendChatCommand("/apk4");

				ImGui::SameLine(0, 15);
				if(ImGui::ImageButton((ImTextureID)MicroIcon::AnimationIcon->raster, ImVec2(70,70)))
					pNetGame->SendChatCommand("/apk5");

				ImGui::SameLine(0, 15);
				if(ImGui::ImageButton((ImTextureID)MicroIcon::SettingIcon->raster, ImVec2(70,70)))
					g_pJavaWrapper->ShowClientSettings();
			}
			
		}
		ImGui::SetWindowPos(ImVec2(pGUI->ScaleX(520), pGUI->ScaleY(930)));
		ImGui::End();
		ImGui::PopStyleColor(3);
	}
	return;
}