#include "main.h"
#include "game/RW/common.h"
#include "game/util.h"

#include "MicroIcon.h"

#include "PluginConfig.h"

bool MicroIcon::Init() noexcept
{
    if(MicroIcon::initStatus)
        return false;

    try
    {
        MicroIcon::tPassiveIcon = (RwTexture*)LoadTextureFromDB("samp", "micro_passive");
        MicroIcon::tActiveIcon = (RwTexture*)LoadTextureFromDB("samp", "micro_active");
        MicroIcon::tMutedIcon = (RwTexture*)LoadTextureFromDB("samp", "micro_passive");
        MicroIcon::BackpackIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_backpack");
        MicroIcon::DataIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_data");
        MicroIcon::GpsIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_gps");
        MicroIcon::PhoneIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_phone");
        MicroIcon::AnimationIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_animation");
        MicroIcon::SettingIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_setting");
        MicroIcon::UpIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_up");
        MicroIcon::DownIcon = (RwTexture*)LoadTextureFromDB("samp", "ic_down");
    }
    catch(const std::exception& exception)
    {
        LogVoice("[sv:err:microicon:init] : failed to create icons");
        MicroIcon::tPassiveIcon = nullptr;
        MicroIcon::tActiveIcon = nullptr;
        MicroIcon::tMutedIcon = nullptr;
        MicroIcon::BackpackIcon = nullptr;
        MicroIcon::DataIcon = nullptr;
        MicroIcon::GpsIcon = nullptr;
        MicroIcon::PhoneIcon = nullptr;
        MicroIcon::AnimationIcon = nullptr;
        MicroIcon::SettingIcon = nullptr;
        MicroIcon::UpIcon = nullptr;
        MicroIcon::DownIcon = nullptr;
        return false;
    }

    if(!PluginConfig::IsMicroLoaded())
    {
        PluginConfig::SetMicroLoaded(true);
    }

    MicroIcon::initStatus = true;

    return true;
}

void MicroIcon::Free() noexcept
{
    if(!MicroIcon::initStatus)
        return;

    MicroIcon::tPassiveIcon = nullptr;
    MicroIcon::tActiveIcon = nullptr;
    MicroIcon::tMutedIcon = nullptr;

    MicroIcon::initStatus = false;
}

void MicroIcon::Show() noexcept
{
    MicroIcon::hasShowed = true;
    MicroIcon::showStatus = true;
}

bool MicroIcon::IsShowed() noexcept
{
    return MicroIcon::showStatus;
}

void MicroIcon::Hide() noexcept
{
    MicroIcon::showStatus = false;
}

bool MicroIcon::initStatus { false };
bool MicroIcon::showStatus { false };
bool MicroIcon::hasShowed { false };

RwTexture* MicroIcon::tPassiveIcon { nullptr };
RwTexture* MicroIcon::tActiveIcon { nullptr };
RwTexture* MicroIcon::tMutedIcon { nullptr };
RwTexture* MicroIcon::BackpackIcon { nullptr };
RwTexture* MicroIcon::DataIcon { nullptr };
RwTexture* MicroIcon::GpsIcon { nullptr };
RwTexture* MicroIcon::PhoneIcon { nullptr };
RwTexture* MicroIcon::AnimationIcon { nullptr };
RwTexture* MicroIcon::SettingIcon { nullptr };
RwTexture* MicroIcon::UpIcon { nullptr };
RwTexture* MicroIcon::DownIcon { nullptr };