#pragma once

class PluginMenu {
    PluginMenu() = delete;
    ~PluginMenu() = delete;
    PluginMenu(const PluginMenu&) = delete;
    PluginMenu(PluginMenu&&) = delete;
    PluginMenu& operator=(const PluginMenu&) = delete;
    PluginMenu& operator=(PluginMenu&&) = delete;

private:
    static constexpr auto kTitleText                       = "Voice chat settings";
    static constexpr auto kTab1TitleText                   = "General";
    static constexpr auto kTab1Desc1TitleText              = "Sound";
    static constexpr auto kTab1Desc1EnableSoundText        = "Turn on sound";
    static constexpr auto kTab1Desc1VolumeSoundText        = "Sound volume";
    static constexpr auto kTab1Desc2TitleText              = "Effects";
    static constexpr auto kTab1Desc2BalancerText           = "Volume smoothing";
    static constexpr auto kTab1Desc2FilterText             = "High pass filter";
    static constexpr auto kTab1Desc3TitleText              = "Icon above players";
    static constexpr auto kTab1Desc3SpeakerIconScaleText   = "Scale";
    static constexpr auto kTab1Desc3SpeakerIconOffsetXText = "Offset by X";
    static constexpr auto kTab1Desc3SpeakerIconOffsetYText = "Offset by Y";
    static constexpr auto kTab1Desc4TitleText              = "Reset";
    static constexpr auto kTab1Desc4ConfigResetText        = "Reset all settings";
    static constexpr auto kTab2TitleText                   = "Microphone";
    static constexpr auto kTab2Desc1TitleText              = "Device";
    static constexpr auto kTab2Desc1EnableMicroText        = "Turn on microphone";
    static constexpr auto kTab2Desc1MicroVolumeText        = "Microphone volume";
    static constexpr auto kTab2Desc1DeviceNameText         = "Input device";
    static constexpr auto kTab2Desc1CheckDeviceText        = "Check device";
    static constexpr auto kTab2Desc2TitleText              = "Microphone icon";
    static constexpr auto kTab2Desc2MicroIconScaleText     = "Scale";
    static constexpr auto kTab2Desc2MicroIconPositionXText = "Position by X";
    static constexpr auto kTab2Desc2MicroIconPositionYText = "Position by Y";
    static constexpr auto kTab2Desc2MicroIconMoveText      = "Move";
    static constexpr auto kTab2Desc3MicroNotFoundText      = "No microphones available";
    static constexpr auto kTab3TitleText                   = "Black list";
    static constexpr auto kTab3Desc1TitleText              = "Filter";
    static constexpr auto kTab3Desc1InputPlaceholderText   = "Enter Player ID or Nickname...";
    static constexpr auto kTab3Desc2PlayerListText         = "Players online";
    static constexpr auto kTab3Desc3BlackListText          = "Blocked players";

    static constexpr float kBaseMenuWidth                  = 0.6f * 640.f;
    static constexpr float kBaseMenuHeight                 = 0.7f * 480.0f;
    static constexpr float kBaseMenuPaddingX               = 20.f;
    static constexpr float kBaseMenuPaddingY               = 10.f;
    static constexpr float kBaseMenuFramePaddingX          = 10.f;
    static constexpr float kBaseMenuFramePaddingY          = 0.5f;
    static constexpr float kBaseMenuItemSpacingX           = 20.f;
    static constexpr float kBaseMenuItemSpacingY           = 2.f;
    static constexpr float kBaseMenuItemInnerSpacingX      = 10.f;
    static constexpr float kBaseMenuItemInnerSpacingY      = 10.f;
    static constexpr float kBaseMenuRounding               = 10.f;
    static constexpr float kBaseFontTitleSize              = 20.f;
    static constexpr float kBaseFontTabSize                = 14.f;
    static constexpr float kBaseFontDescSize               = 12.f;
    static constexpr float kBaseFontSize                   = 10.f;
    static constexpr int   kTabsCount                      = 3;
    static constexpr float kBaseTabPadding                 = 4.f;
    static constexpr float kBaseTabWidth                   = (kBaseMenuWidth - (2 * kBaseMenuPaddingX +
                                                             (kTabsCount - 1) * kBaseTabPadding)) / kTabsCount;
    static constexpr float kBaseTabHeight                  = kBaseTabWidth / 6.f;
    static constexpr float kBlurLevelIncrement             = 5.f;
    static constexpr float kBlurLevelDecrement             = -5.f;

public:
    static bool Init() noexcept;
    static void Free() noexcept;

    static bool Show() noexcept;
    static bool IsShowed() noexcept;
    static void Hide() noexcept;

    static void Render() noexcept;

private:
    static void SyncOptions() noexcept;

private:
    static bool initStatus;
    static bool showStatus;

    // Configs
    // ------------------------------------------------------------------------------------------

    static bool soundEnable;
    static int soundVolume;
    static bool soundBalancer;
    static bool soundFilter;

    static float speakerIconScale;
    static int speakerIconOffsetX;
    static int speakerIconOffsetY;

    static bool microEnable;
    static int microVolume;
    static int deviceIndex;
    static bool microEchoCancel;
    static bool microAGC;

    static float microIconScale;
    static int microIconPositionX;
    static int microIconPositionY;

    // Internal options
    // ------------------------------------------------------------------------------------------

    static int iSelectedMenu;
    static bool bCheckDevice;
};