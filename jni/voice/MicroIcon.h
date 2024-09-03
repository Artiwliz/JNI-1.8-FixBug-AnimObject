#pragma once
#include "gui/gui.h"

class MicroIcon {
    friend class CGUI;
    
    MicroIcon() = delete;
    ~MicroIcon() = delete;
    MicroIcon(const MicroIcon&) = delete;
    MicroIcon(MicroIcon&&) = delete;
    MicroIcon& operator=(const MicroIcon&) = delete;
    MicroIcon& operator=(MicroIcon&&) = delete;

public:
    static constexpr float kBaseIconSize = 30.f;

public:
    static bool Init() noexcept;
    static void Free() noexcept;

    static void Show() noexcept;
    static bool IsShowed() noexcept;
    static void Hide() noexcept;

public:
    static RwTexture* tPassiveIcon;
    static RwTexture* tActiveIcon;
    static RwTexture* tMutedIcon;
    static RwTexture* BackpackIcon;
    static RwTexture* DataIcon;
    static RwTexture* GpsIcon;
    static RwTexture* PhoneIcon;
    static RwTexture* AnimationIcon;
    static RwTexture* SettingIcon;
    static RwTexture* UpIcon;
    static RwTexture* DownIcon;

    static bool hasShowed;

private:
    static bool initStatus;
    static bool showStatus;
};
