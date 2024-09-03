// -- INCLUDE`S
#include "../main.h"
#include "../chatwindow.h"
#include "../keyboard.h"

#include "CBinder.h"

#include "../vendor/ini/config.h"
#include "../vendor/imgui/imgui_internal.h"

// -- EXTERN`S
extern CGUI *pGUI;
extern CChatWindow *pChatWindow;
extern CKeyBoard *pKeyBoard;

void ChatWindowInputHandler(const char* str);

// -- VARIABLE`S
bool CBinder::m_bIsRender = false;

int CBinder::m_iValuesCount = 0;
int CBinder::m_iSelectedBindID = -1;

std::vector<const char *> CBinder::m_BinderValues;

char CBinder::szNewBindInputBuffer[512];

DataStructures::SingleProducerConsumer<BUFFERED_BIND_BINDER> CBinder::bufferedChat;

// -- METHOD`S
void CBinder::Initialise()
{
	Log(OBFUSCATE("Loading binder values.."));

	char buff[0x7F];
	sprintf(buff, OBFUSCATE("%sSAMP/binder.ini"), g_pszStorage);

	auto config = ini_table_create();
	Log(OBFUSCATE("Opening binder values: %s"), buff);

	if (!ini_table_read_from_file(config, buff))
	{
		Log(OBFUSCATE("Cannot load binder values, exiting..."));
		std::terminate();
	}

	m_iValuesCount = ini_table_get_entry_as_int(config, OBFUSCATE("settings"), OBFUSCATE("count"), 0);
	if (m_iValuesCount == 0)
		return;

	for (int i = 1; i <= m_iValuesCount; i++)
	{
		std::stringstream ss;
		ss << i;

		m_BinderValues.push_back(ini_table_get_entry(config, OBFUSCATE("values"), ss.str().c_str()));
	}
}

void CBinder::Render()
{
	if (!m_bIsRender)
		return;

	ImGuiIO& io = ImGui::GetIO();

	ImVec2 vecWinSize(900, 700);
	ImVec2 vecBtnSize(pGUI->ScaleX(200), pGUI->ScaleY(50));

	char out[512];
	cp1251_to_utf8(out, "Биндер");

	ImGui::Begin(out, nullptr, ImVec2(pGUI->ScaleX(vecWinSize.x), pGUI->ScaleY(vecWinSize.y)), 0.9f, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::BeginChild("#binder_child", ImVec2(-1, pGUI->ScaleY(vecWinSize.y - 108)), false);

	for (int i = 1; i < m_iValuesCount; i++)
	{
		if (ImGui::Button(m_BinderValues.at(m_iSelectedBindID), ImVec2(-1, pGUI->ScaleY(50))))
			m_iSelectedBindID = i - 1;
	}

	ImGui::EndChild();

	ImGui::SetCursorPosX(pGUI->ScaleX((vecWinSize.x - 200 * 4) / 2));

	cp1251_to_utf8(out, "Добавить");
	if (ImGui::Button(out, vecBtnSize))
	{
		if (!pKeyBoard->IsOpen())
			pKeyBoard->Open(&CBinder::NewBindInputHandler);

		// -- TODO - ReWrite config file
	}

	ImGui::SameLine();
	cp1251_to_utf8(out, "Удалить");
	if (ImGui::Button(out, vecBtnSize))
	{
		if (m_iSelectedBindID == -1)
			return;

		m_BinderValues.erase(m_BinderValues.begin() + m_iSelectedBindID);
		m_iValuesCount--;
		// -- TODO - ReWrite config file
	}

	ImGui::SameLine();
	cp1251_to_utf8(out, "Выбрать");
	if (ImGui::Button(out, vecBtnSize))
	{
		if (m_iSelectedBindID == -1)
			return;

		// cp1251_to_utf8(out, m_BinderValues.at(m_iSelectedBindID));
		ChatWindowInputHandler(m_BinderValues.at(m_iSelectedBindID));

		Toggle();
	}

	ImGui::SameLine();
	cp1251_to_utf8(out, "Закрыть");
	if (ImGui::Button(out, vecBtnSize))
		Toggle();

	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(((io.DisplaySize.x - size.x) / 2), ((io.DisplaySize.y - size.y) / 2)));
	ImGui::End();
}

void CBinder::Toggle()
{
	m_iSelectedBindID = -1;

	m_bIsRender = !m_bIsRender;
}

bool CBinder::IsRender()
{
	return m_bIsRender;
}

void CBinder::NewBindInputHandler(const char* str)
{
	if (!str || *str == '\0')
		return;

	m_BinderValues.insert(m_BinderValues.end() - 1, str);
	m_iValuesCount++;
}