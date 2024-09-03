#include "main.h"
#include "gui/gui.h"
#include "gui/CBinder.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "CSettings.h"
#include "game/game.h"
#include "net/netgame.h"
//#include "vendor/bass/bass.h"
#include <dlfcn.h>
#include "scrollbar.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;
extern CAMERA_AIM * pcaInternalAim;
extern CGame * pGame;
extern CChatWindow* pChatWindow;

extern bool bShowDebugLabels;

#define NUM_OF_MESSAGES	100

template<typename T> T GetBASSFunc(void* handle, const char* szFunc)
{
	return (T)dlsym(handle, szFunc);
}

int (*BASS_Init) (uint32_t, uint32_t, uint32_t);
int (*BASS_Free) (void);
int (*BASS_SetConfigPtr) (uint32_t, const char *);
int (*BASS_SetConfig) (uint32_t, uint32_t);
int (*BASS_ChannelStop) (uint32_t);
int (*BASS_StreamCreateURL) (char *, uint32_t, uint32_t, uint32_t);
int (*BASS_StreamCreate) (uint32_t, uint32_t, uint32_t, STREAMPROC *, void *);
int (*BASS_ChannelPlay) (uint32_t, bool);
int (*BASS_ChannelPause) (uint32_t);
int *BASS_ChannelGetTags;
int *BASS_ChannelSetSync;
int *BASS_StreamGetFilePosition;
int (*BASS_StreamFree) (uint32_t);
int (*BASS_ErrorGetCode) (void);
int (*BASS_Set3DFactors) (float, float, float);
int (*BASS_Set3DPosition) (const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
int (*BASS_Apply3D) (void);
int (*BASS_ChannelSetFX) (uint32_t, HFX);
int (*BASS_ChannelRemoveFX) (uint32_t, HFX);
int (*BASS_FXSetParameters) (HFX, const void *);
int (*BASS_IsStarted) (void);
int (*BASS_RecordGetDeviceInfo) (uint32_t, BASS_DEVICEINFO *);
int (*BASS_RecordInit) (int);
int (*BASS_RecordGetDevice) (void);
int (*BASS_RecordFree) (void);
int (*BASS_RecordStart) (uint32_t, uint32_t, uint32_t, RECORDPROC *, void *);
int (*BASS_ChannelSetAttribute) (uint32_t, uint32_t, float);
int (*BASS_ChannelGetData) (uint32_t, void *, uint32_t);
int (*BASS_RecordSetInput) (int, uint32_t, float);
int (*BASS_StreamPutData) (uint32_t, const void *, uint32_t);
int (*BASS_ChannelSetPosition) (uint32_t, uint64_t, uint32_t);
int (*BASS_ChannelIsActive) (uint32_t);
int (*BASS_ChannelSlideAttribute) (uint32_t, uint32_t, float, uint32_t);
int (*BASS_ChannelSet3DAttributes) (uint32_t, int, float, float, int, int, float);
int (*BASS_ChannelSet3DPosition) (uint32_t, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
int (*BASS_SetVolume) (float);

void InitBASSFuncs()
{
	void* handle;

#ifdef GAME_EDITION_CR
	handle = dlopen("/data/data/com.gvn.games/lib/libbass.so", 3);
#else
	handle = dlopen("/data/data/com.gvn.games/lib/libbass.so", 3);
#endif
	if (!handle)
	{
		Log("Cannot load libbass.so");
		return;
	}

	BASS_Init = (int (*)(uint32_t, uint32_t, uint32_t))dlsym(handle, "BASS_Init");
	BASS_Free = (int (*)(void))dlsym(handle, "BASS_Free");
	BASS_SetConfigPtr = (int (*)(uint32_t, const char *))dlsym(handle, "BASS_SetConfigPtr");
	BASS_SetConfig = (int (*)(uint32_t, uint32_t))dlsym(handle, "BASS_SetConfig");
	BASS_ChannelStop = (int (*)(uint32_t))dlsym(handle, "BASS_ChannelStop");
	BASS_StreamCreateURL = (int (*)(char *, uint32_t, uint32_t, uint32_t))dlsym(handle, "BASS_StreamCreateURL");
	BASS_StreamCreate = (int (*)(uint32_t, uint32_t, uint32_t, STREAMPROC *, void *))dlsym(handle, "BASS_StreamCreate");
	BASS_ChannelPlay = (int (*)(uint32_t, bool))dlsym(handle, "BASS_ChannelPlay");
	BASS_ChannelPause = (int (*)(uint32_t))dlsym(handle, "BASS_ChannelPause");
	BASS_ChannelGetTags = (int *)dlsym(handle, "BASS_ChannelGetTags");
	BASS_ChannelSetSync = (int *)dlsym(handle, "BASS_ChannelSetSync");
	BASS_StreamGetFilePosition = (int *)dlsym(handle, "BASS_StreamGetFilePosition");
	BASS_StreamFree = (int (*)(uint32_t))dlsym(handle, "BASS_StreamFree");
	BASS_ErrorGetCode = (int (*)(void))dlsym(handle, "BASS_ErrorGetCode");
	BASS_Set3DFactors = (int (*)(float, float, float))dlsym(handle, "BASS_Set3DFactors");
	BASS_Set3DPosition = (int (*)(const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *))dlsym(handle, "BASS_Set3DPosition");
	BASS_Apply3D = (int (*)(void))dlsym(handle, "BASS_Apply3D");
	BASS_ChannelSetFX = (int (*)(uint32_t, HFX))dlsym(handle, "BASS_ChannelSetFX");
	BASS_ChannelRemoveFX = (int (*)(uint32_t, HFX))dlsym(handle, "BASS_ChannelRemoveFX");
	BASS_FXSetParameters = (int (*)(HFX, const void *))dlsym(handle, "BASS_FXSetParameters");
	BASS_IsStarted = (int (*)(void))dlsym(handle, "BASS_IsStarted");
	BASS_RecordGetDeviceInfo = (int (*)(uint32_t, BASS_DEVICEINFO *))dlsym(handle, "BASS_RecordGetDeviceInfo");
	BASS_RecordInit = (int (*)(int))dlsym(handle, "BASS_RecordInit");
	BASS_RecordGetDevice = (int (*)(void))dlsym(handle, "BASS_RecordGetDevice");
	BASS_RecordFree = (int (*)(void))dlsym(handle, "BASS_RecordFree");
	BASS_RecordStart = (int (*)(uint32_t, uint32_t, uint32_t, RECORDPROC *, void *))dlsym(handle, "BASS_RecordStart");
	BASS_ChannelSetAttribute = (int (*)(uint32_t, uint32_t, float))dlsym(handle, "BASS_ChannelSetAttribute");
	BASS_ChannelGetData = (int (*)(uint32_t, void *, uint32_t))dlsym(handle, "BASS_ChannelGetData");
	BASS_RecordSetInput = (int (*)(int, uint32_t, float))dlsym(handle, "BASS_RecordSetInput");
	BASS_StreamPutData = (int (*)(uint32_t, const void *, uint32_t))dlsym(handle, "BASS_StreamPutData");
	BASS_ChannelSetPosition = (int (*)(uint32_t, uint64_t, uint32_t))dlsym(handle, "BASS_ChannelSetPosition");
	BASS_ChannelIsActive = (int (*)(uint32_t))dlsym(handle, "BASS_ChannelIsActive");
	BASS_ChannelSlideAttribute = (int (*)(uint32_t, uint32_t, float, uint32_t))dlsym(handle, "BASS_ChannelSlideAttribute");
	BASS_ChannelSet3DAttributes = (int (*)(uint32_t, int, float, float, int, int, float))dlsym(handle, "BASS_ChannelSet3DAttributes");
	BASS_ChannelSet3DPosition = (int (*)(uint32_t, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *))dlsym(handle, "BASS_ChannelSet3DPosition");
	BASS_SetVolume = (int (*)(float))dlsym(handle, "BASS_SetVolume");
}

BOOL returnedValue;
#include "util/CJavaWrapper.h"

#ifdef GAME_EDITION_CR
uint32_t g_uiHeadMoveEnabled = 0;
#else
uint32_t g_uiHeadMoveEnabled = 1;
#endif

uint32_t g_uiBorderedText = 1;
#include "CDebugInfo.h"
#include "CLocalisation.h"
#include "scoreboard.h"
#include "game/CCustomPlateManager.h"
#include <fcntl.h>

extern CScoreBoard* pScoreBoard;
bool ProcessCommands(const char* str)
{
	/*if (strcmp(str, "/binder") == 0)
	{
		CBinder::Toggle();

		return true;
	}*/

	if (strcmp(str, "/dl") == 0)
	{
        pGUI->bShowDebugLabels = !pGUI->bShowDebugLabels;
	}	

	if (strstr(str, "/hudeditor"))
	{
		g_pJavaWrapper->ShowClientSettings();
		return true;
	}

	if (strstr(str, "/testdrink")) {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
		pChatWindow->AddInfoMessage(OBFUSCATE("Client: Yse drink"));
		return true;
	}
	return false;
}

void ChatWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	if(!pNetGame) return;

	if (*str == '/')
	{
		if (ProcessCommands(str))
		{
			return;
		}
	}

	auto pCmd = pChatWindow->bufferedChat.WriteLock();

	if (*str == '/')
	{
		pCmd->type = 0;
	}
	else
	{
		pCmd->type = 1;
	}

	strcpy(pCmd->buff, str);

	pChatWindow->bufferedChat.WriteUnlock();

	return;
}


CChatWindow::CChatWindow()
{
	m_uiMaxTimeChatHide = 0;
	m_uiTimeChatHideBegin = 0;
	pScrollbar = nullptr;

	m_uiLastTimePushedMessage = GetTickCount();
	m_bNewMessage = false;
	m_uiTimePushed = GetTickCount();
	m_bPendingReInit = false;
	ReInit();
}

CChatWindow::~CChatWindow()
{
	if (pScrollbar)
	{
		delete pScrollbar;
		pScrollbar = nullptr;
	}
}

#include <mutex>
static std::mutex lDebugMutex;

#include "dialog.h"
#include "gui/CBinder.h"
extern CDialogWindow* pDialogWindow;

bool CChatWindow::OnTouchEvent(int type, bool multi, int x, int y)
{
	if (CBinder::IsRender())
		return true;

	static bool bWannaOpenChat = false;

	switch (type)
	{
		case TOUCH_PUSH:
			if (x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
			{
				bWannaOpenChat = true;
			}
			break;

		case TOUCH_POP:
			if (bWannaOpenChat &&
				x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
			{
				if (pDialogWindow)
				{
					if (pDialogWindow->m_bRendered) return true;
				}
				if (pKeyBoard)
				{
					pKeyBoard->Open(&ChatWindowInputHandler);
				}
				m_uiLastTimePushedMessage = GetTickCount();
			}
			bWannaOpenChat = false;
			break;

		case TOUCH_MOVE:
			break;
	}

	return true;
}

void CChatWindow::Render()
{
	if (m_bPendingReInit)
	{
		ReInit();
		m_bPendingReInit = false;
	}

	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_CHAT)) return;
	if (pScrollbar)
	{
		pScrollbar->m_bDrawState = false;
		if (pKeyBoard && pDialogWindow)
		{
			if (pKeyBoard->IsOpen() && !pDialogWindow->m_bRendered)
			{
				pScrollbar->m_bDrawState = true;
			}
		}
		pScrollbar->Draw();
	}
	if (false)
	{
		ImGui::GetOverlayDrawList()->AddRect(
				ImVec2(m_fChatPosX, m_fChatPosY),
				ImVec2(m_fChatPosX + m_fChatSizeX, m_fChatPosY + m_fChatSizeY),
				IM_COL32_BLACK);
	}

	//ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY);
	if (!pScrollbar)
	{
		if (pScrollbar->GetValue() < 0 || pScrollbar->GetValue() > NUM_OF_MESSAGES + m_iMaxMessages)
		{
			return;
		}
		return;
	}

	if (pKeyBoard)
	{
		if (pKeyBoard->IsOpen())
		{
			m_uiLastTimePushedMessage = GetTickCount();
		}
	}

	uint32_t currentOffset = 1;

	bool firstMessageAppear = false;

	for (int i = 100 - pScrollbar->GetValue(); i < 100 - pScrollbar->GetValue() + m_iMaxMessages + 1; i++)
	{
		if (i >= m_vChatWindowEntries.size())
		{
			break;
		}
		if (!m_vChatWindowEntries[i])
		{
			break;
		}

		ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY);
		float fProgressedAlpha = 1.0f;
		float fY_ = pGUI->GetFontSize() * (float)currentOffset;
		if (m_bNewMessage)
		{
			uint32_t timeSpent = GetTickCount() - m_uiTimePushed;
			if (timeSpent >= NEW_MESSAGE_PUSH_TIME)
			{
				m_bNewMessage = false;
				m_uiTimePushed = 0;
			}
			else
			{
				float fProgress = (float)timeSpent / (float)NEW_MESSAGE_PUSH_TIME;
				fProgressedAlpha = fProgress;
				fY_ = ((pGUI->GetFontSize() * fProgress) + fY_ - pGUI->GetFontSize());
			}
		}
		pos.y -= fY_;
		currentOffset++;
		if (firstMessageAppear)
		{
			fProgressedAlpha = 1.0f;
		}
		if (!firstMessageAppear)
		{
			firstMessageAppear = true;
		}

		auto entry = m_vChatWindowEntries[i];
		switch (entry->eType)
		{
			case CHAT_TYPE_CHAT:
				if (entry->szNick[0] != 0)
				{
					RenderText(entry->szNick, pos.x, pos.y, entry->dwNickColor, fProgressedAlpha);
					pos.x += ImGui::CalcTextSize(entry->szNick).x + ImGui::CalcTextSize(" ").x; //+ pGUI->GetFontSize() * 0.4;
				}
				RenderText(entry->utf8Message, pos.x, pos.y, entry->dwTextColor, fProgressedAlpha);
				break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
				RenderText(entry->utf8Message, pos.x, pos.y, entry->dwTextColor, fProgressedAlpha);
				break;
		}

		pos.x = m_fChatPosX;
		pos.y -= pGUI->GetFontSize();
	}


}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end - start);
	if (hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if (sscanf(hex, "%x", &hexColor) > 0)
		{
			color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;

			if (hexCount == 8)
				color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;

			return true;
		}
	}

	return false;
}

void CChatWindow::ReInit()
{
	m_fChatPosX = pGUI->ScaleX(pSettings->GetReadOnly().fChatPosX);
	m_fChatPosY = pGUI->ScaleY(pSettings->GetReadOnly().fChatPosY);
	m_fChatSizeX = pGUI->ScaleX(pSettings->GetReadOnly().fChatSizeX);
	m_fChatSizeY = pGUI->ScaleY(pSettings->GetReadOnly().fChatSizeY);
	m_iMaxMessages = pSettings->GetReadOnly().iChatMaxMessages;

	m_fChatSizeY = m_iMaxMessages * pGUI->GetFontSize();

	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xBEBEBEFF;

	if (!pScrollbar)
	{
		pScrollbar = new CScrollbar();
	}
	pScrollbar->m_fX = m_fChatPosX - pGUI->ScaleX(50.0f);
	pScrollbar->m_fY = m_fChatPosY - pGUI->ScaleY(10.0f);

	pScrollbar->m_fWidthBox = pGUI->ScaleX(30.0f);
	pScrollbar->m_fHeightBox = pGUI->ScaleY(80.0f);

	pScrollbar->m_fUpY = m_fChatPosY - pGUI->ScaleY(10.0f);
	pScrollbar->m_fDownY = m_fChatSizeY + m_fChatPosY + pGUI->ScaleY(20.0f);

	pScrollbar->m_iMaxValue = NUM_OF_MESSAGES;
	pScrollbar->SetOnPos(NUM_OF_MESSAGES);

	m_uiLastTimePushedMessage = GetTickCount();

	m_fPosBeforeBoundChat = m_fChatPosY;
	m_fOffsetBefore = 0.0f;
}

void CChatWindow::RenderText(const char* u8Str, float posX, float posY, uint32_t dwColor, float alphaNewMessage)
{
	const char* textStart = u8Str;
	const char* textCur = u8Str;
	const char* textEnd = u8Str + strlen(u8Str);

	uint8_t bAlpha = GetAlphaFromLastTimePushedMessage();

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	if (bAlpha != 255)
	{
		colorCur.Value.w = (float)bAlpha / 255.0f;
	}
	ImVec4 col;

	if (alphaNewMessage != 1.0f)
	{
		colorCur.Value.w = alphaNewMessage;
	}

	while (*textCur)
	{
		// {BBCCDD}
		// '{' e '}' niioaaonoao?o ASCII eiae?iaea
		if (textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			// Auaiaei oaeno ai oeao?iie neiaee
			if (textCur != textStart)
			{
				// Auaiaei ai oaeouaai neiaiea
				pGUI->RenderTextForChatWindow(posCur, colorCur, true, textStart, textCur);

				// Aun?eouaaai iiaia niauaiea
				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Iieo?aai oaao
			if (ProcessInlineHexColor(textCur + 1, textCur + 7, col))
			{
				colorCur = col;
				if (bAlpha != 255)
				{
					colorCur.Value.w = (float)bAlpha / 255.0f;
				}
				if (alphaNewMessage != 1.0f)
				{
					colorCur.Value.w = alphaNewMessage;
				}
			}

			// Aaeaaai niauaiea
			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	if (textCur != textStart)
		pGUI->RenderTextForChatWindow(posCur, colorCur, true, textStart, textCur);

	return;
}

void CChatWindow::SetChatDissappearTimeout(uint32_t uiTimeoutStart, uint32_t uiTimeoutEnd)
{
	m_uiMaxTimeChatHide = uiTimeoutEnd;
	m_uiTimeChatHideBegin = uiTimeoutStart;
}

void CChatWindow::ProcessPushedCommands()
{
	BUFFERED_COMMAND_CHAT* pCmd = nullptr;
	while (pCmd = bufferedChat.ReadLock())
	{
		if (pCmd->buff[0] == '/')
		{
			pNetGame->SendChatCommand(pCmd->buff);
		}
		else
		{
			pNetGame->SendChatMessage(pCmd->buff);
		}

		bufferedChat.ReadUnlock();
	}
}

void CChatWindow::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

void CChatWindow::AddDebugMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddDebugMessageNonFormatted(char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, szStr, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void CChatWindow::SetLowerBound(int bound)
{
	ImGuiIO& io = ImGui::GetIO();

	m_fChatPosY = m_fPosBeforeBoundChat;
	pScrollbar->m_fY += m_fOffsetBefore;
	pScrollbar->m_fDownY += m_fOffsetBefore;

	float y = io.DisplaySize.y - (float)bound;

	m_fOffsetBefore = 0.0f;

	while (m_fChatPosY + m_fChatSizeY + pGUI->GetFontSize() * 2.0f >= y)
	{
		m_fChatPosY -= pGUI->GetFontSize();
		m_fOffsetBefore += pGUI->GetFontSize();
	}

	pScrollbar->m_fY -= m_fOffsetBefore;
	pScrollbar->m_fDownY -= m_fOffsetBefore;
}

void CChatWindow::PushBack(CHAT_WINDOW_ENTRY& entry)
{
	if (m_vChatWindowEntries.size() >= NUM_OF_MESSAGES + m_iMaxMessages)
	{
		auto it = m_vChatWindowEntries.back();
		delete it;
		m_vChatWindowEntries.pop_back();
	}

	CHAT_WINDOW_ENTRY* pEntry = new CHAT_WINDOW_ENTRY;
	memcpy(pEntry, &entry, sizeof(CHAT_WINDOW_ENTRY));

	m_bNewMessage = true;
	m_uiTimePushed = GetTickCount();

	m_vChatWindowEntries.insert(std::begin(m_vChatWindowEntries), pEntry);
	return;
}

uint8_t CChatWindow::GetAlphaFromLastTimePushedMessage()
{
	if (!m_uiMaxTimeChatHide || !m_uiTimeChatHideBegin)
	{
		return 0xFF;
	}

	if ((GetTickCount() - m_uiLastTimePushedMessage) >= TIME_CHAT_HIDE_BEGIN)
	{
		uint32_t time =  GetTickCount() - m_uiLastTimePushedMessage - TIME_CHAT_HIDE_BEGIN;
		uint32_t distance = MAX_TIME_CHAT_HIDE - TIME_CHAT_HIDE_BEGIN;
		if (!distance)
		{
			return 0xFF;
		}
		float res = (float)time / (float)distance;
		if (res >= 1.0f)
		{
			return 0;
		}
		return (255 - (uint8_t)(res * 255.0f));
	}
	return 0xFF;
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType type, char* szString, char* szNick,
										uint32_t dwTextColor, uint32_t dwNickColor)
{
	m_uiLastTimePushedMessage = GetTickCount();

	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if(szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else
		entry.szNick[0] = '\0';

	if(type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		// ������� ������ ������ � �����
		while(szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		// ���� ��������� ����� ������ 12 ��������
		if((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			// ������� �� MAX_MESSAGE_LENGTH/2
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			// ������� ����� MAX_MESSAGE_LENGTH/2
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			// ������� �� �������
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			// ������� ����� �������
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+(iBestLineLength+1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}

	return;
}

void CChatWindow::FilterInvalidChars(char *szString)
{
	while(*szString)
	{
		if(*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}