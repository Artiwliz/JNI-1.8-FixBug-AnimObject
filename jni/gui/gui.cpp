#include "../main.h"
#include "gui.h"
#include "CBinder.h"
#include "../game/game.h"
#include "../game/crosshair.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../CSettings.h"
#include "..//scoreboard.h"
#include "../KillList.h"
#include "../CDebugInfo.h"
#include "../GButton.h"
#include "../util/CJavaWrapper.h"
#include "../util/util.h"
#include "../game/vehicle.h"
#include "../extrakeyboard.h"
// voice
#include "voice/MicroIcon.h"
#include "voice/SpeakerList.h"
#include "voice/include/util/Render.h"

extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern KillList *pKillList;
extern CCrossHair *pCrossHair;
extern CGame* pGame;
extern CGButton *pGButton;
extern CExtraKeyBoard *pExtraKeyBoard;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

/*
	Все координаты GUI-элементов задаются
	относительно разрешения 1920x1080
*/
#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080

CGUI::CGUI()
{
	Log(OBFUSCATE("Initializing GUI.."));

	m_bMouseDown = 0;
	m_vTouchPos = ImVec2(-1, -1);
	m_bNextClear = false;
	m_bNeedClearMousePos = false;

	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;
	// font Size
	m_fFontSize = ScaleY( pSettings->GetReadOnly().fFontSize );

	Log(OBFUSCATE("GUI | Scale factor: %f, %f Font size: %f"), m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

	// load fonts
	char path[0xFF];
	sprintf(path, OBFUSCATE("%sSAMP/fonts/%s"), g_pszStorage, pSettings->GetReadOnly().szFont);
	// cp1251 ranges
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x04FF, // Russia
		0x0E00, 0x0E5B, // Thai
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
		0xF020, 0xF0FF, // Half-width characters
        0x2010, 0x205E, // Punctuations
        0
	};

	Log(OBFUSCATE("GUI | Loading font: %s"), pSettings->GetReadOnly().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	Log(OBFUSCATE("GUI | ImFont pointer = 0x%X"), m_pFont);

	m_pFontGTAWeap = LoadFont(OBFUSCATE("gtaweap3.ttf"), 0);
	style.WindowRounding = 0.0f;

	m_pSplashTexture = nullptr;
	m_pSplashTexture = (RwTexture*)LoadTextureFromDB(OBFUSCATE("txd"), OBFUSCATE("splash_icon"));

	CRadarRect::LoadTextures();
	bShowDebugLabels = false;

	m_bKeysStatus = false;
	
   	// voice
	for(const auto& deviceInitCallback : Render::deviceInitCallbacks)
    {
        if(deviceInitCallback != nullptr) 
			deviceInitCallback();
    }
}


void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor);
CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
    // voice 
	for(const auto& deviceFreeCallback : Render::deviceFreeCallbacks)
	{
		if(deviceFreeCallback != nullptr) 
			deviceFreeCallback();
	}
}

extern float g_fMicrophoneButtonPosX;
extern float g_fMicrophoneButtonPosY;
void CGUI::PreProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	io.MousePos = m_vTouchPos;
	io.MouseDown[0] = m_bMouseDown;

	if (!m_bNeedClearMousePos && m_bNextClear)
		m_bNextClear = false;

	if (m_bNeedClearMousePos && m_bNextClear)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}

void CGUI::PostProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (m_bNeedClearMousePos && io.MouseDown[0])
		return;

	if (m_bNeedClearMousePos && !io.MouseDown[0])
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}

extern CGame* pGame;

void CGUI::SetHealth(float fhpcar){
   bHealth = static_cast<int>(fhpcar);
}

int CGUI::GetHealth(){
	return 1;//static_cast<int>(pVehicle->GetHealth());
}

void CGUI::SetDoor(int door){
	bDoor = door;
}

void CGUI::SetEngine(int engine){
	bEngine = engine;
}

void CGUI::SetLights(int lights){
	bLights = lights;
}

void CGUI::SetMeliage(float meliage){
	bMeliage = static_cast<int>(meliage);
}

void CGUI::SetEat(float eate){
	eat = static_cast<int>(eate);
}

int CGUI::GetEat(){
	return eat;
}

void CGUI::SetFuel(float fuel){
   m_fuel = static_cast<int>(fuel);
}

void CGUI::ShowSpeed(){
	if (!pGame || !pNetGame || !pGame->FindPlayerPed()->IsInVehicle()) {
		g_pJavaWrapper->HideSpeed();
		bMeliage =0;
		m_fuel = 0;
		return;
	}
	if (pGame->FindPlayerPed()->IsAPassenger()) {
		g_pJavaWrapper->HideSpeed();
		bMeliage =0;
		m_fuel = 0;
		return;
	}

	int i_speed = 0;
	bDoor =0;
	bEngine = 0;
	bLights = 0;
	float fHealth = 0;
	CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
    VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
    pVehicle = pVehiclePool->GetAt(id);
    
    if(pPlayerPed)
    {
        if(pVehicle)
        {
            VECTOR vecMoveSpeed;
            pVehicle->GetMoveSpeedVector(&vecMoveSpeed);
            i_speed = sqrt((vecMoveSpeed.X * vecMoveSpeed.X) + (vecMoveSpeed.Y * vecMoveSpeed.Y) + (vecMoveSpeed.Z * vecMoveSpeed.Z)) * 180;
            bHealth = pVehicle->GetHealth();
            bDoor = pVehicle->GetDoorState();
            bEngine = pVehicle->GetEngineState();
            bLights = pVehicle->GetLightsState();
        }
    }
	g_pJavaWrapper->ShowSpeed();
	g_pJavaWrapper->UpdateSpeedInfo(i_speed, m_fuel, bHealth, bMeliage, bEngine, bLights, 0, bDoor);
}

void* ButtonPressed(void* g_button)
{
    for (int i = 0; i < 50; i++)
    {   
    	usleep(5000);
    	*(bool*)g_button = true;  
    }
	pthread_exit(0);
	return (void*)0;
}
pthread_t threadBut;

void CGUI::Render()
{
	PreProcessInput();

	ProcessPushedTextdraws();
	if (pChatWindow)
		pChatWindow->ProcessPushedCommands();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	RenderVersion();
	//RenderRakNetStatistics();

	if (pCrossHair)
		pCrossHair->Render();

	if (pKeyBoard)
		pKeyBoard->ProcessInputCommands();

	if (pPlayerTags) 
		pPlayerTags->Render();
	
	if(pNetGame && pNetGame->GetLabelPool())
		pNetGame->GetLabelPool()->Draw();

    if(pKillList)
        pKillList->Render();

    ImVec2 ppp(10, 600);

	if (pChatWindow)
		pChatWindow->Render();
	else RenderText(ppp, IM_COL32_WHITE, true, "FUCK YOU");

	if (pScoreBoard)
		pScoreBoard->Draw();

	if (pKeyBoard)
		pKeyBoard->Render();

	if (pDialogWindow)
		pDialogWindow->Render();
	
	if(pGame) CGUI::ShowSpeed();

	if(pNetGame)
	{
		if(pDialogWindow->m_bIsActive || pScoreBoard->m_bToggle)
		{
			SpeakerList::Hide();
			MicroIcon::Hide();
		}
		else 
		{
			if(MicroIcon::hasShowed)
			{
				SpeakerList::Show();
				MicroIcon::Show();
			}
		}

		for(const auto& renderCallback : Render::renderCallbacks)
		{
			if(renderCallback != nullptr) 
				renderCallback();
		}
	}

	if (bShowDebugLabels)
	{
		if (pNetGame)
		{
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool)
			{
				for (VEHICLEID x = 0; x < MAX_VEHICLES; x++)
				{
					CVehicle *pVehicle = pVehiclePool->GetAt(x);
					if (pVehicle)
					{
						if (pVehicle->GetDistanceFromLocalPlayerPed() <= 15.0f)
						{
							MATRIX4X4 matVehicle;
							pVehicle->GetMatrix(&matVehicle);

							RwV3d rwPosition;
							rwPosition.x = matVehicle.pos.X;
							rwPosition.y = matVehicle.pos.Y;
							rwPosition.z = matVehicle.pos.Z;

							RwV3d rwOutResult;

							// CSPrite::CalcScreenCoors
							((void (*)(RwV3d const&, RwV3d *, float *, float *, bool, bool))(g_libGTASA + 0x54EEC0 + 1))(rwPosition, &rwOutResult, 0, 0, 0, 0);
							if (rwOutResult.z < 1.0f)
								break;

							char szTextLabel[256];
							sprintf(szTextLabel, "[ID: %d | Model: %d | Health: %.1f]\nDistance: %.2fm\ncPos: %.3f, %.3f, %.3f\nsPos: %.3f, %.3f, %.3f",
								x, pVehicle->GetModelIndex(), pVehicle->GetHealth(),
								pVehicle->GetDistanceFromLocalPlayerPed(),
								matVehicle.pos.X, matVehicle.pos.Y, matVehicle.pos.Z,
								pVehiclePool->m_vecSpawnPos[x].X, pVehiclePool->m_vecSpawnPos[x].Y, pVehiclePool->m_vecSpawnPos[x].Z 
							);

							ImVec2 vecRealPos = ImVec2(rwOutResult.x, rwOutResult.y);
							Render3DLabel(vecRealPos, szTextLabel, 0xFFFF99FF/*0x358BD4FF*/);
						}
					}
				}
			}
		}
	}


	if (pNetGame && !pDialogWindow->m_bIsActive && pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
	{
	ImGuiIO& io = ImGui::GetIO();
		ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.2, ImGui::GetFontSize() * 2.4);
		ImGui::SetNextWindowPos(ImVec2(2.0f, io.DisplaySize.y / 2.4 - vecButSize.x / 1.8));
		ImGui::Begin(OBFUSCATE("###keys"), nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button(m_bKeysStatus ? OBFUSCATE("<<") : OBFUSCATE(">>"), vecButSize))
		{
			if (m_bKeysStatus)
				m_bKeysStatus = false;
			else m_bKeysStatus = true;
		}

		ImGui::SameLine(0,12);
		if (!pScoreBoard->m_bToggle) 
		{
			if (ImGui::Button(OBFUSCATE("TAB"), vecButSize))
			{
				pScoreBoard->Toggle();
			}
		}
		else 
		{
			if (ImGui::Button(OBFUSCATE("X"), vecButSize))
			{
				pScoreBoard->Toggle();
			}
		}

		ImGui::SameLine(0,12);
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		if (pVehiclePool) 
		{
			if (pGButton->p_GButtonTexture) pGButton->RenderButton();
			else
			{
				CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool)
				{
					CLocalPlayer* pLocalPlayer;
					if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
					{
						VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
						if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
						{
							CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
							if (pVehicle)
							{
								if (pVehicle->GetDistanceFromLocalPlayerPed() < 5.0f)
								{
									if (ImGui::Button(OBFUSCATE("G"), vecButSize))
									{
										if (pNetGame)
										{
											pLocalPlayer = pPlayerPool->GetLocalPlayer();
											if (pLocalPlayer)
												pLocalPlayer->HandlePassengerEntryEx();
										}
									}
									ImGui::SameLine(0,12);
								}
							}
						}
					}
				}
			}
		}
		
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
		{
			if (ImGui::Button(OBFUSCATE("L. Ctrl"), vecButSize))
			LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
		}

		ImGui::SameLine(0,12);

		if (m_bKeysStatus)
		{
			ImGui::SameLine(0,12);
			if (ImGui::Button("ESC", vecButSize))
	        {
	         	RakNet::BitStream bsClick;
	         	bsClick.Write(0xFFFF);
	         	pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bsClick, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
			}		

			ImGui::SameLine(0,12);
			if (ImGui::Button(OBFUSCATE("ALT"), vecButSize))
			{
				CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
				{
					if(pPlayerPed->IsInVehicle()) pthread_create(&threadBut, nullptr, ButtonPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]);
					else pthread_create(&threadBut, nullptr, ButtonPressed, (void*)&LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]);
				}
			}
					
			ImGui::SameLine(0,12);
			if (ImGui::Button(OBFUSCATE("F"), vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;

			ImGui::SameLine(0,12);
			if (ImGui::Button(OBFUSCATE("H"), vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;

			ImGui::SameLine(0,12);
			if (ImGui::Button(OBFUSCATE("Y"), vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;

			ImGui::SameLine(0,12);
			if (ImGui::Button(OBFUSCATE("N"), vecButSize))
				LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
		}

		ImGui::End();
	}

	CDebugInfo::Draw();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	PostProcessInput();
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	if(!pKeyBoard->OnTouchEvent(type, multi, x, y))
		return false;

	bool bFalse = true;
	if (pNetGame)
	{
		if (pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y))
		{
			if (!pChatWindow->OnTouchEvent(type, multi, x, y))
				return false;
		}
		else bFalse = false;
	}

	switch(type)
	{
		case TOUCH_PUSH:
		{
			m_vTouchPos = ImVec2(x, y);
			m_bMouseDown = true;
			m_bNeedClearMousePos = false;
			break;
		}

		case TOUCH_POP:
		{
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
			break;
		}

		case TOUCH_MOVE:
		{
			m_bNeedClearMousePos = false;
			m_vTouchPos = ImVec2(x, y);
			break;
		}
	}

	if (!bFalse)
		return false;

	return true;
}

#include "../str_obfuscator_no_template.hpp"
void CGUI::RenderVersion()
{
	ImGui::GetBackgroundDrawList()->AddText(
		ImVec2(ScaleX(10), ScaleY(10)), 
		/*ImColor(IM_COL32_BLACK)*/ImColor(122, 2, 158), cryptor::create("", 10).decrypt());
}

void CGUI::ProcessPushedTextdraws()
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = nullptr;
	while (pCmd = m_BufferedCommandTextdraws.ReadLock())
	{
		RakNet::BitStream bs;
		bs.Write(pCmd->textdrawId);
		pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		m_BufferedCommandTextdraws.ReadUnlock();
	}
}

void CGUI::RenderRakNetStatistics()
{
		//StatisticsToString(rss, message, 0);

		/*ImGui::GetOverlayDrawList()->AddText(
			ImVec2(ScaleX(10), ScaleY(400)),
			ImColor(IM_COL32_BLACK), message);*/
}

extern uint32_t g_uiBorderedText;
void CGUI::RenderTextForChatWindow(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	auto colOutline = ImColor(IM_COL32_BLACK);
	auto colDef = ImColor(col);

	colOutline.Value.w = colDef.Value.w;

	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
			if (colOutline.Value.w <= 0.4)
				co.Value.w = colOutline.Value.w;

			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());
			ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::PushToBufferedQueueTextDrawPressed(uint16_t textdrawId)
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = m_BufferedCommandTextdraws.WriteLock();

	pCmd->textdrawId = textdrawId;

	m_BufferedCommandTextdraws.WriteUnlock();
}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());
			if (m_pSplashTexture)
			{
				ImColor co(1.0f, 1.0f, 1.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pSplashTexture->raster, posCur, b, ImVec2(0, 0), ImVec2(1, 1), co);
			}
			else
			{
				ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
			}
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::RenderTextWithSize(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->GetReadOnly().iFontOutline;

    if (bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
}

ImFont* CGUI::LoadFont(char *font, float fontsize)
{
    ImGuiIO &io = ImGui::GetIO();

    // load fonts
    char path[0xFF];
    sprintf(path, OBFUSCATE("%sSAMP/fonts/%s"), g_pszStorage, font);

    // ranges
    static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x04FF, // Russia
		0x0E00, 0x0E7F, // Thai
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0xF020, 0xF0FF, // Half-width characters
		0
	};

    ImFont* pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
    return pFont;
}

void CGUI::RenderTextDeathMessage(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size, ImFont *font, bool bOutlineUseTextColor)
{
    int iOffset = bOutlineUseTextColor ? 1 : pSettings->GetReadOnly().iFontOutline;
    if(bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, col, text_begin, text_end);
}

void CGUI::AddText(ImFont* font, ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->GetReadOnly().iFontOutline;

    if (bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
}

void CGUI::RenderMaterialText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size, ImFont *font, bool bOutlineUseTextColor)
{
	int iOffset = bOutlineUseTextColor ? 1 : pSettings->GetReadOnly().iFontOutline;
	if(bOutline)
	{
		// left
		posCur.x -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right
		posCur.x += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetBackgroundDrawList()->AddText(font, font_size == 0.0f ? GetFontSize() : font_size, posCur, col, text_begin, text_end);
}
