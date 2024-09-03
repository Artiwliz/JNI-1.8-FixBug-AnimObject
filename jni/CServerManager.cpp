#include "CServerManager.h"
#include "dialog.h"

#include "main.h"

extern CDialogWindow *pDialogWindow;

#ifdef FLIN
	const char* g_szServerNames[MAX_SERVERS] = {
		OBFUSCATE("{ff9729}[1]{ffffff} Luxury  RP | Server: 01"),
		OBFUSCATE("{ff9729}[1]{ffffff} Luxury RP | Server: 02")
	};
	const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
		CServerInstance::create(OBFUSCATE("gta-irz.com"), 1, 20, 7777, false), // 1
		CServerInstance::create(OBFUSCATE("gta-irz.com"), 1, 20, 7777, false), // 2

	};
#endif

void CServerInstance::ShowDialog()
{
/*	pDialogWindow->Clear();

	pDialogWindow->m_wDialogID = 0xFFF7;
	pDialogWindow->m_byteDialogStyle = DIALOG_STYLE_LIST;

	cp1251_to_utf8(pDialogWindow->m_utf8Title, "����� �������");
	cp1251_to_utf8(pDialogWindow->m_utf8Button1, "�������");
	cp1251_to_utf8(pDialogWindow->m_utf8Button2, "");

	char szBuff[400];
	memset(szBuff, 0, sizeof(szBuff));
	for (auto & g_szServerName : g_szServerNames)
	{
		strcat(&szBuff[0], g_szServerName);
		strcat(&szBuff[0], OBFUSCATE("\n"));
	}

	pDialogWindow->SetInfo(szBuff, strlen(szBuff));
	pDialogWindow->Show(true);*/
}