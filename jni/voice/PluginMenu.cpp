#include "main.h"
#include "CSettings.h"
#include "gui/gui.h"
#include "vendor/inih/cpp/INIReader.h"

#include "PluginMenu.h"

//#include "BlackList.h"
#include "SpeakerList.h"
#include "MicroIcon.h"
#include "Playback.h"
#include "Record.h"

extern CGUI *pGUI;
extern CSettings *pSettings;

bool PluginMenu::Init() noexcept
{
    if(PluginMenu::initStatus)
        return false;

    char buff[0x7F];
	sprintf(buff, "%sSAMP/sampvoice/%s", g_pszStorage, SV::kConfigFileName);

	INIReader reader(buff);

	if(reader.ParseError() < 0)
	{
		LogVoice("[sv:err:pluginmenu:init] : can not open config file.");
		PluginMenu::SyncOptions();
	}
    else
    {
        Playback::SetSoundEnable(reader.GetBoolean("svconfig", "SpeakerOn", true));
        Playback::SetSoundVolume(reader.GetInteger("svconfig", "SpeakerVolume", 100));
        Playback::SetSoundBalancer(reader.GetBoolean("svconfig", "SpeakerSoundSmoothing", false));
        Playback::SetSoundFilter(reader.GetBoolean("svconfig", "SpeakerHighPass", false));
        SpeakerList::SetSpeakerIconScale(reader.GetReal("svconfig", "SpeakerSize", 40.0f));

        Record::SetMicroEnable(reader.GetBoolean("svconfig", "MicroOn", true));
        Record::SetMicroVolume(reader.GetInteger("svconfig", "MicroVolume", 75));
		
		/*if(!pSettings->GetReadOnly().bVoiceChatEnable)
		{
			Playback::SetSoundEnable(false);
			Record::SetMicroEnable(false);
		}*/

        PluginMenu::SyncOptions();
    }

    PluginMenu::showStatus = false;

    PluginMenu::initStatus = true;

    return true;
}

void PluginMenu::Free() noexcept
{
    PluginMenu::Hide();

    if(!PluginMenu::initStatus)
        return;

    PluginMenu::initStatus = false;
}

bool PluginMenu::Show() noexcept
{
    if(!PluginMenu::initStatus || PluginMenu::showStatus)
        return false;

    PluginMenu::SyncOptions();
    PluginMenu::showStatus = true;

    return true;
}

bool PluginMenu::IsShowed() noexcept
{
    return PluginMenu::showStatus;
}

void PluginMenu::Hide() noexcept
{
    if(!PluginMenu::initStatus || !PluginMenu::showStatus)
        return;

    PluginMenu::bCheckDevice = false;
    Record::StopChecking();

    PluginMenu::showStatus = false;
}

void PluginMenu::Render() noexcept
{
    if(!PluginMenu::initStatus || !PluginMenu::showStatus)
        return;

    /*ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 20.0);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.94f, 0.94f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.94f, 0.94f, 1.));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.3f, 0.3f, 0.2));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.4));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.5f, 0.5f, 0.5f, 0.6));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.2));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.4));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.6));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.3f, 0.3f, 0.3f, 0.2));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.5f, 0.5f, 0.5f, 0.6));*/

    ImGui::SetNextWindowSize(ImVec2(pGUI->ScaleX(820), pGUI->ScaleY(880)));
    ImGui::SetNextWindowPosCenter();

    if(ImGui::Begin("ConfigWindow", nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize))
    {
        ImGui::Text(kTitleText);

        //ImGui::SameLine(ImGui::GetWindowWidth() - (4 * ImGui::CalcTextSize(kTitleText).y + (vTabWidth -
            //4 * ImGui::CalcTextSize(kTitleText).y) / 2.f + ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding.x));

       // ImGui::Image(PluginMenu::tLogo->GetTexture(), { 4 * ImGui::CalcTextSize(kTitleText).y, ImGui::CalcTextSize(kTitleText).y });

        ImGui::NewLine();

        // Tabs rendering...
        // -------------------------------

        ImVec2 TabSize = ImVec2(pGUI->ScaleX(805 / 2), pGUI->GetFontSize() + (ImGui::GetStyle().ItemSpacing.y * 2.5));
        if(ImGui::Button(kTab1TitleText, TabSize)) PluginMenu::iSelectedMenu = 0;
        ImGui::SameLine(ImGui::GetStyle().WindowPadding.x + TabSize.x + kBaseTabPadding);
        if(ImGui::Button(kTab2TitleText, TabSize)) PluginMenu::iSelectedMenu = 1;
        //ImGui::SameLine(ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding.x + 2 * (vTabWidth + kBaseTabPadding));
        //if(ImGui::Button(kTab3TitleText, { vTabWidth, vTabHeight })) PluginMenu::iSelectedMenu = 2;

        // Description rendering...
        // -------------------------------

        switch(PluginMenu::iSelectedMenu)
        {
            case 0:
            {
                // Common settings rendering...
                // -------------------------------

                ImGui::NewLine();
                ImGui::Text(kTab1Desc1TitleText);
                ImGui::Separator();
                ImGui::NewLine();

                if(ImGui::Checkbox(kTab1Desc1EnableSoundText, &PluginMenu::soundEnable))
                {
                    // Enabling/Disabling sound
                    Playback::SetSoundEnable(PluginMenu::soundEnable);
                }

                if(PluginMenu::soundEnable)
                {
                    if(ImGui::SliderInt(kTab1Desc1VolumeSoundText, &PluginMenu::soundVolume, 0, 100))
                    {
                        // Setting volume
                        Playback::SetSoundVolume(PluginMenu::soundVolume);
                    }
                }

                ImGui::NewLine();
                ImGui::Text(kTab1Desc2TitleText);
                ImGui::Separator();
                ImGui::NewLine();

                if(ImGui::Checkbox(kTab1Desc2BalancerText, &PluginMenu::soundBalancer))
                {
                    // Enabling/Disabling sound balancer
                    Playback::SetSoundBalancer(PluginMenu::soundBalancer);
                }

                if(ImGui::Checkbox(kTab1Desc2FilterText, &PluginMenu::soundFilter))
                {
                    // Enabling/Disabling sound filter
                    Playback::SetSoundFilter(PluginMenu::soundFilter);
                }

                ImGui::NewLine();
                ImGui::Text(kTab1Desc3TitleText);
                ImGui::Separator();
                ImGui::NewLine();

                if(ImGui::SliderFloat(kTab1Desc3SpeakerIconScaleText, &PluginMenu::speakerIconScale, 20.f, 60.f))
                {
                    SpeakerList::SetSpeakerIconScale(PluginMenu::speakerIconScale);
                }

                ImGui::NewLine();
                ImGui::Text(kTab1Desc4TitleText);
                ImGui::Separator();
                ImGui::NewLine();

                if(ImGui::Button(kTab1Desc4ConfigResetText, ImVec2(pGUI->ScaleX(810), TabSize.y)))
                {
                    SpeakerList::ResetConfigs();
                    SpeakerList::SyncConfigs();
                    Playback::ResetConfigs();
                    Playback::SyncConfigs();
                    PluginMenu::SyncOptions();
                }
            } 
            break;
            case 1:
            {
                // Micro settings rendering...
                // -------------------------------

                ImGui::NewLine();
                ImGui::Text(kTab2Desc1TitleText);
                ImGui::Separator();
                ImGui::NewLine();

                const auto& devList = Record::GetDeviceNamesList();

                if(!devList.empty())
                {
                    if(ImGui::Checkbox(kTab2Desc1EnableMicroText, &PluginMenu::microEnable))
                    {
                        // Enabling/Disabling microphone
                        Record::SetMicroEnable(PluginMenu::microEnable);

                        if(!PluginMenu::microEnable && PluginMenu::bCheckDevice)
                            PluginMenu::bCheckDevice = false;
                    }

                    if(PluginMenu::microEnable)
                    {
                        if(ImGui::SliderInt(kTab2Desc1MicroVolumeText, &PluginMenu::microVolume, 0, 100))
                        {
                            // Setting volume micro
                            Record::SetMicroVolume(PluginMenu::microVolume);
                        }

                        if(ImGui::BeginCombo(kTab2Desc1DeviceNameText, devList[PluginMenu::deviceIndex].c_str()))
                        {
                            for(int i { 0 }; i < devList.size(); ++i)
                            {
                                if(ImGui::Selectable(devList[i].c_str(), i == PluginMenu::deviceIndex))
                                    Record::SetMicroDevice(PluginMenu::deviceIndex = i);

                                if(i == PluginMenu::deviceIndex)
                                    ImGui::SetItemDefaultFocus();
                            }

                            ImGui::EndCombo();
                        }

                        if(ImGui::Checkbox(kTab2Desc1CheckDeviceText, &PluginMenu::bCheckDevice))
                        {
                            if(PluginMenu::bCheckDevice) Record::StartChecking();
                            else Record::StopChecking();
                        }
                    }
                }
                else
                {
                    ImGui::TextDisabled(kTab2Desc3MicroNotFoundText);
                }

                ImGui::NewLine();
                ImGui::Text(kTab2Desc2TitleText);
                ImGui::Separator();
                ImGui::NewLine();

                /*if(ImGui::SliderFloat(kTab2Desc2MicroIconScaleText, &PluginMenu::microIconScale, 0.2f, 2.0f))
                {
                    // Setting scale micro icon
                    MicroIcon::SetMicroIconScale(PluginMenu::microIconScale);
                }

                if(float screenWidth { 0.f }, screenHeight { 0.));
                    Render::GetScreenSize(screenWidth, screenHeight))
                {
                    if(ImGui::SliderInt(kTab2Desc2MicroIconPositionXText, &PluginMenu::microIconPositionX, 0, screenWidth))
                        MicroIcon::SetMicroIconPositionX(PluginMenu::microIconPositionX);

                    if(ImGui::SliderInt(kTab2Desc2MicroIconPositionYText, &PluginMenu::microIconPositionY, 0, screenHeight))
                        MicroIcon::SetMicroIconPositionY(PluginMenu::microIconPositionY);
                }*/

                ImGui::NewLine();
                ImGui::Text(kTab1Desc4TitleText);
                ImGui::Separator();
                ImGui::NewLine();

                if(ImGui::Button(kTab1Desc4ConfigResetText, ImVec2(pGUI->ScaleX(810), TabSize.y)))
                {
                    //MicroIcon::ResetConfigs();
                    //MicroIcon::SyncConfigs();
                    Record::ResetConfigs();
                    Record::SyncConfigs();
                    PluginMenu::SyncOptions();
                }
            } 
            break;
        }

        ImGui::End();

        //ImGui::PopStyleColor(10);
        //ImGui::PopStyleVar(12);
    }
}

void PluginMenu::SyncOptions() noexcept
{
    PluginMenu::soundEnable = Playback::GetSoundEnable();
    PluginMenu::soundVolume = Playback::GetSoundVolume();
    PluginMenu::soundBalancer = Playback::GetSoundBalancer();
    PluginMenu::soundFilter = Playback::GetSoundFilter();

    PluginMenu::speakerIconScale = SpeakerList::GetSpeakerIconScale();

    PluginMenu::microEnable = Record::GetMicroEnable();
    PluginMenu::microVolume = Record::GetMicroVolume();
    PluginMenu::deviceIndex = Record::GetMicroDevice();
    /*PluginMenu::microEchoCancel = Record::GetMicroEchoCancel();
    PluginMenu::microAGC = Record::GetMicroAGC();*/

    /*PluginMenu::microIconScale = MicroIcon::GetMicroIconScale();
    PluginMenu::microIconPositionX = MicroIcon::GetMicroIconPositionX();
    PluginMenu::microIconPositionY = MicroIcon::GetMicroIconPositionY();*/

    // help me :( i dont have include INIWriter.

    char buff[0x7F];
	sprintf(buff, "%sSAMP/sampvoice/%s", g_pszStorage, SV::kConfigFileName);

    static FILE *fileConfig { nullptr };
    if(fileConfig == nullptr)
        fileConfig = fopen(buff, "w");

    fprintf(fileConfig, "[svconfig]\nSpeakerOn = %d\nSpeakerVolume = %d\n"
        "SpeakerSoundSmoothing = %d\nSpeakerHighPass = %d\nSpeakerSize = %1.f\n"
        "\nMicroOn = %d\nMicroVolume = %d\n", 
            PluginMenu::soundEnable,
            PluginMenu::soundVolume,
            PluginMenu::soundBalancer,
            PluginMenu::soundFilter,
            PluginMenu::speakerIconScale,
            PluginMenu::microEnable,
            PluginMenu::microVolume
    );

	fclose(fileConfig);
}

bool PluginMenu::initStatus { false };
bool PluginMenu::showStatus { false };

bool PluginMenu::soundEnable { false };
int PluginMenu::soundVolume { 0 };
bool PluginMenu::soundBalancer { false };
bool PluginMenu::soundFilter { false };

float PluginMenu::speakerIconScale { 0.f };

bool PluginMenu::microEnable { false };
int PluginMenu::microVolume { 0 };
int PluginMenu::deviceIndex { 0 };
bool PluginMenu::microEchoCancel { false };
bool PluginMenu::microAGC { false };

float PluginMenu::microIconScale { 0.f };
int PluginMenu::microIconPositionX { 0 };
int PluginMenu::microIconPositionY { 0 };

int PluginMenu::iSelectedMenu { 0 };
bool PluginMenu::bCheckDevice { false };
