#include "main.h"
#include "CSettings.h"
#include "game/game.h"
#include "vendor/ini/config.h"
#include "game/CAdjustableHudColors.h"
#include "game/CAdjustableHudPosition.h"
#include "game/CAdjustableHudScale.h"

static void ClearBackslashN(char* pStr, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		if (pStr[i] == '\n' || pStr[i] == 13)
		{
			pStr[i] = 0;
		}
	}
}

CSettings::CSettings()
{
	LoadSettings(nullptr);
}

CSettings::~CSettings()
{
}

void CSettings::ToDefaults(int iCategory)
{
	char buff[0x7F];
	sprintf(buff, OBFUSCATE("%sSAMP/settings.ini"), g_pszStorage);

	FILE* pFile = fopen(buff, "w");

	fwrite(OBFUSCATE("[gui]"), 1, 6, pFile);

	fclose(pFile);

	Save(iCategory);
	LoadSettings(m_Settings.szNickName, m_Settings.iChatMaxMessages);
}

void CSettings::Save(int iIgnoreCategory)
{
	char buff[0x7F];
	sprintf(buff, OBFUSCATE("%sSAMP/settings.ini"), g_pszStorage);
	remove(buff);

	ini_table_s* config = ini_table_create();
	
	ini_table_create_entry(config, OBFUSCATE("client"), OBFUSCATE("name"), m_Settings.szNickName);
	ini_table_create_entry(config, OBFUSCATE("client"), OBFUSCATE("password"), m_Settings.szPassword);
	ini_table_create_entry_as_int(config, OBFUSCATE("client"), OBFUSCATE("last_server"), m_Settings.last_server);
	ini_table_create_entry(config, OBFUSCATE("gui"), OBFUSCATE("Font"), m_Settings.szFont);

	ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("FontSize"), m_Settings.fFontSize);
	ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("FontOutline"), m_Settings.iFontOutline);

	if (iIgnoreCategory != 1)
	{
		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatPosX"), m_Settings.fChatPosX);
		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatPosY"), m_Settings.fChatPosY);
	}
	ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatSizeX"), m_Settings.fChatSizeX);
	ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatSizeY"), m_Settings.fChatSizeY);
	ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("ChatMaxMessages"), m_Settings.iChatMaxMessages);

	ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("HealthBarWidth"), m_Settings.fHealthBarWidth);
	ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("HealthBarHeight"), m_Settings.fHealthBarHeight);

	if (iIgnoreCategory != 2)
	{

		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("MicrophoneSize"), m_Settings.fButtonMicrophoneSize);
		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("MicrophoneX"), m_Settings.fButtonMicrophoneX);
		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("MicrophoneY"), m_Settings.fButtonMicrophoneY);

		ini_table_create_entry_as_float(config, "gui", "EnterPassengerSize", m_Settings.fButtonEnterPassengerSize);
		ini_table_create_entry_as_float(config, "gui", "EnterPassengerX", m_Settings.fButtonEnterPassengerX);
		ini_table_create_entry_as_float(config, "gui", "EnterPassengerY", m_Settings.fButtonEnterPassengerY);

		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("CameraCycleSize"), m_Settings.fButtonCameraCycleSize);
		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("CameraCycleX"), m_Settings.fButtonCameraCycleX);
		ini_table_create_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("CameraCycleY"), m_Settings.fButtonCameraCycleY);
	}

	ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("fps"), m_Settings.iFPS);

	if (iIgnoreCategory != 1)
	{
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("cutout"), m_Settings.iCutout);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("androidKeyboard"), m_Settings.iAndroidKeyboard);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("fpscounter"), m_Settings.iFPSCounter);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("outfit"), m_Settings.iOutfitGuns);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("radarrect"), m_Settings.iRadarRect);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("nametag"), m_Settings.iNameTag);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("3dtext"), m_Settings.i3DText);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("skybox"), m_Settings.iSkyBox);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("hparmourtext"), m_Settings.iHPArmourText);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("pcmoney"), m_Settings.iPCMoney);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("snow"), m_Settings.iSnow);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("textdraw"), m_Settings.iTextDraw);
		ini_table_create_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("hud"), m_Settings.iHud);
	
	}

	if (iIgnoreCategory != 2)
	{
		for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), OBFUSCATE("hud_color_%d"), i);
			if (CAdjustableHudColors::IsUsingHudColor((E_HUD_ELEMENT)i))
			{
				ini_table_create_entry(config, OBFUSCATE("hud"), buff, CAdjustableHudColors::GetHudColorString((E_HUD_ELEMENT)i).c_str());
			}

		}
	}

	if (iIgnoreCategory != 2)
	{
		for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE-2; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), OBFUSCATE("hud_position_x_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), OBFUSCATE("hud_position_y_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).Y);

		}

		for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE-2; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), OBFUSCATE("hud_scale_x_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), OBFUSCATE("hud_scale_y_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).Y);

		}
	}

	if (iIgnoreCategory != 2)
	{
		for (int i = 10; i < E_HUD_ELEMENT::HUD_SIZE; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), OBFUSCATE("hud_position_x_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), OBFUSCATE("hud_position_y_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).Y);

		}

		for (int i = 10; i < E_HUD_ELEMENT::HUD_SIZE; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), OBFUSCATE("hud_scale_x_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), OBFUSCATE("hud_scale_y_%d"), i);
			ini_table_create_entry_as_int(config, OBFUSCATE("hud"), buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).Y);

		}
	}

	ini_table_write_to_file(config, buff);
	ini_table_destroy(config);
}

const stSettings& CSettings::GetReadOnly()
{
	return m_Settings;
}

stSettings& CSettings::GetWrite()
{
	return m_Settings;
}

void CSettings::LoadSettings(const char* szNickName, int iChatLines)
{
	char tempNick[40];
	if (szNickName)
	{
		strcpy(tempNick, szNickName);
	}

	Log(OBFUSCATE("Loading settings.."));

	char buff[0x7F];
	sprintf(buff, OBFUSCATE("%sSAMP/settings.ini"), g_pszStorage);

	ini_table_s* config = ini_table_create();
	Log(OBFUSCATE("Opening settings: %s"), buff);
	if (!ini_table_read_from_file(config, buff))
	{
		Log(OBFUSCATE("Cannot load settings, exiting..."));
		std::terminate();
		return;
	}

	snprintf(m_Settings.szNickName, sizeof(m_Settings.szNickName), OBFUSCATE("__android_%d%d"), rand() % 1000, rand() % 1000);
	memset(m_Settings.szPassword, 0, sizeof(m_Settings.szPassword));
	snprintf(m_Settings.szFont, sizeof(m_Settings.szFont), "Arial.ttf");

	const char* szName = ini_table_get_entry(config, OBFUSCATE("client"), OBFUSCATE("name"));
	const char* szPassword = ini_table_get_entry(config, OBFUSCATE("client"), OBFUSCATE("password"));

	const char* szFontName = ini_table_get_entry(config, OBFUSCATE("gui"), OBFUSCATE("Font"));

	if (szName)
		strcpy(m_Settings.szNickName, szName);

	if (szPassword)
		strcpy(m_Settings.szPassword, szPassword);

	if (szFontName)
		strcpy(m_Settings.szFont, szFontName);

	ClearBackslashN(m_Settings.szNickName, sizeof(m_Settings.szNickName));
	ClearBackslashN(m_Settings.szPassword, sizeof(m_Settings.szPassword));
	ClearBackslashN(m_Settings.szFont, sizeof(m_Settings.szFont));

	if (szNickName)
		strcpy(m_Settings.szNickName, tempNick);

	m_Settings.fFontSize = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("FontSize"), 30.0f);
	m_Settings.iFontOutline = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("FontOutline"), 2);

	m_Settings.fChatPosX = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatPosX"), 325.0f);
	m_Settings.fChatPosY = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatPosY"), 25.0f);
	m_Settings.fChatSizeX = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatSizeX"), 1150.0f);
	m_Settings.fChatSizeY = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ChatSizeY"), 220.0f);
	m_Settings.iChatMaxMessages = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("ChatMaxMessages"), 8);

	m_Settings.fHealthBarWidth = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("HealthBarWidth"), 60.0f);
	m_Settings.fHealthBarHeight = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("HealthBarHeight"), 10.0f);

	m_Settings.fButtonEnterPassengerSize = ini_table_get_entry_as_float(config, "gui", "EnterPassengerSize", 150.0f);
	m_Settings.fButtonEnterPassengerX = ini_table_get_entry_as_float(config, "gui", "EnterPassengerX", 1691.25f);
	m_Settings.fButtonEnterPassengerY = ini_table_get_entry_as_float(config, "gui", "EnterPassengerY", 360.0f);

	m_Settings.fButtonMicrophoneSize = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("MicrophoneSize"), 150.0f);
	m_Settings.fButtonMicrophoneX = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("MicrophoneX"), 1450.0f);
	m_Settings.fButtonMicrophoneY = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("MicrophoneY"), 600.0f);

	m_Settings.fButtonCameraCycleX = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("CameraCycleX"), 1810.0f);
	m_Settings.fButtonCameraCycleY = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("CameraCycleY"), 260.0f);
	m_Settings.fButtonCameraCycleSize = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("CameraCycleSize"), 90.0f);

	m_Settings.fScoreBoardSizeX = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ScoreBoardSizeX"), 1024.0f);
	m_Settings.fScoreBoardSizeY = ini_table_get_entry_as_float(config, OBFUSCATE("gui"), OBFUSCATE("ScoreBoardSizeY"), 768.0f);

	m_Settings.iFPS = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("fps"), 60);
	m_Settings.iFPS = 60;
	m_Settings.last_server = ini_table_get_entry_as_int(config, OBFUSCATE("client"), OBFUSCATE("last_server"), -1);

	m_Settings.iCutout = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("cutout"), 0);
	m_Settings.iAndroidKeyboard = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("androidKeyboard"), 0);

	m_Settings.iFPSCounter = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("fpscounter"), 1);
	m_Settings.iOutfitGuns = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("outfit"), 1);
	m_Settings.iRadarRect = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("radarrect"), 0);
	m_Settings.iHPArmourText = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("hparmourtext"), 0);
	m_Settings.iPCMoney = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("pcmoney"), 0);
	m_Settings.iSkyBox = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("ctimecyc"), 1);
	m_Settings.iSnow = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("snow"), 1);
	m_Settings.iNameTag = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("nametag"), 1);
	m_Settings.i3DText = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("3dtext"), 1);
	m_Settings.iSkyBox = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("skybox"), 1);
	m_Settings.iTextDraw = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("textdraw"), 0);
	m_Settings.iDialog = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("dialog"), 1);
	m_Settings.iHud = ini_table_get_entry_as_int(config, OBFUSCATE("gui"), OBFUSCATE("hud"), 1);
	

	for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
	{
		char buff[30];
		snprintf(buff, sizeof(buff), OBFUSCATE("hud_color_%d"), i);
		const char* szInput = ini_table_get_entry(config, OBFUSCATE("hud"), buff);
		if (szInput)
		{
			strcpy(buff, szInput);

			ClearBackslashN(buff, sizeof(buff));

			std::string szTemp(buff + 1);

			CAdjustableHudColors::SetHudColorFromString((E_HUD_ELEMENT)i, szTemp);
		}
		else
		{
			CAdjustableHudColors::SetHudColorFromRGBA((E_HUD_ELEMENT)i, -1, -1, -1, -1);
		}
	}

	for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
	{
		char buff[30];
		snprintf(buff, sizeof(buff), OBFUSCATE("hud_position_x_%d"), i);
		int valueX = ini_table_get_entry_as_int(config, OBFUSCATE("hud"), buff, -1);

		snprintf(buff, sizeof(buff), OBFUSCATE("hud_position_y_%d"), i);
		int valueY = ini_table_get_entry_as_int(config, OBFUSCATE("hud"), buff, -1);

		CAdjustableHudPosition::SetElementPosition((E_HUD_ELEMENT)i, valueX, valueY);

	}

	for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
	{
		char buff[30];
		snprintf(buff, sizeof(buff), OBFUSCATE("hud_scale_x_%d"), i);
		int valueX = ini_table_get_entry_as_int(config, OBFUSCATE("hud"), buff, -1);

		snprintf(buff, sizeof(buff), OBFUSCATE("hud_scale_y_%d"), i);
		int valueY = ini_table_get_entry_as_int(config, OBFUSCATE("hud"), buff, -1);

		CAdjustableHudScale::SetElementScale((E_HUD_ELEMENT)i, valueX, valueY);

	}

	ini_table_destroy(config);
}
