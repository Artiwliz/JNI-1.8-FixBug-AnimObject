#include "main.h"
#include "CSettings.h"
#include "game/game.h"
#include "game/playerped.h"
#include "net/netgame.h"

#include "SpeakerList.h"

#include "PluginConfig.h"

extern CGUI *pGUI;
extern CNetGame *pNetGame;
extern CSettings *pSettings;

bool SpeakerList::Init() noexcept
{
    if(SpeakerList::initStatus)
        return false;

    try
    {
        SpeakerList::tSpeakerIcon = (RwTexture*)LoadTextureFromDB("samp", "voice_speaker");
    }
    catch (const std::exception& exception)
    {
        LogVoice("[sv:err:speakerlist:init] : failed to create speaker icon");
        SpeakerList::tSpeakerIcon = nullptr;
        return false;
    }

    if(!PluginConfig::IsSpeakerLoaded())
    {
        PluginConfig::SetSpeakerLoaded(true);
        SpeakerList::ResetConfigs();
    }

    SpeakerList::initStatus = true;
    SpeakerList::SyncConfigs();

    return true;
}

void SpeakerList::Free() noexcept
{
    if(!SpeakerList::initStatus)
        return;

    SpeakerList::tSpeakerIcon = nullptr;

    SpeakerList::initStatus = false;
}

void SpeakerList::Show() noexcept
{
    SpeakerList::showStatus = true;
}

bool SpeakerList::IsShowed() noexcept
{
    return SpeakerList::showStatus;
}

void SpeakerList::Hide() noexcept
{
    SpeakerList::showStatus = false;
}

void SpeakerList::Render()
{
    if(!SpeakerList::initStatus || !SpeakerList::IsShowed())
        return;

    if(!pNetGame) return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    if(!pPlayerPool) return;

    int curTextLine;
    char szText[256], szText2[256];
    ImVec2 textPos = ImVec2(pGUI->ScaleX(24), pGUI->ScaleY(480));

    for(PLAYERID playerId { 0 }; playerId < MAX_PLAYERS; ++playerId)
    {
        CRemotePlayer* pPlayer = pPlayerPool->GetAt(playerId);
        if(pPlayer && pPlayer->IsActive())
        {
            CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
            if(pPlayerPed)
            {
                if(const auto playerName = pPlayerPool->GetPlayerName(playerId); playerName)
                {
                    if(!SpeakerList::playerStreams[playerId].empty())
                    {
						//if(!pSettings->GetReadOnly().bVoiceChatEnable) continue;
                        for(const auto& playerStream : SpeakerList::playerStreams[playerId])
                        {
							//if(!pSettings->GetReadOnly().bVoiceChatEnable) continue;
                            if(playerStream.second.GetType() != StreamType::GlobalStream)
                            {
                                if(pPlayerPed->GetDistanceFromLocalPlayerPed() > playerStream.second.GetDistance())
                                    SpeakerList::OnSpeakerStop(*SpeakerList::playerStream[playerId], playerId);
                            }

                            if(playerStream.second.GetType() == StreamType::LocalStreamAtPlayer)
                            {
                                VECTOR VecPos;

                                if(!pPlayerPed->IsAdded()) continue;
                                VecPos.X = 0.0f;
                                VecPos.Y = 0.0f;
                                VecPos.Z = 0.0f;
                                pPlayerPed->GetBonePosition(8, &VecPos);

                                CAMERA_AIM *pCam = GameGetInternalAim();
                                int dwHitEntity = 0;

                                dwHitEntity = ScriptCommand(&get_line_of_sight, VecPos.X, VecPos.Y, VecPos.Z, 
                                    pCam->pos1x, pCam->pos1y, pCam->pos1z, 1, 0, 0, 1, 0);

                                if(dwHitEntity && pPlayerPed->GetDistanceFromLocalPlayerPed() <= 40.0f)
                                    SpeakerList::Draw(&VecPos, pPlayerPed->GetDistanceFromCamera());
                            }
                        }

                        if(curTextLine < 8)
                        {
                            ImVec2 a = ImVec2(textPos.x, textPos.y);
                            ImVec2 b = ImVec2(textPos.x + pGUI->GetFontSize(), textPos.y + pGUI->GetFontSize());
                            ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)SpeakerList::tSpeakerIcon->raster, a, b);

                            float fSpaceFromIconToText = pGUI->GetFontSize() + ImGui::GetStyle().ItemSpacing.x;

							unsigned char RGBcolors[3];
							
							RGBcolors[0] = (pPlayerPool->GetAt(playerId)->GetPlayerColorAsARGB() - 0xFF000000) >> 16;
							RGBcolors[1] = ((pPlayerPool->GetAt(playerId)->GetPlayerColorAsARGB() - 0xFF000000) & 0x00ff00) >> 8;
							RGBcolors[2] = ((pPlayerPool->GetAt(playerId)->GetPlayerColorAsARGB() - 0xFF000000) & 0x0000ff);

                            textPos.x += fSpaceFromIconToText;
                            sprintf(szText, "%s (%d) ", playerName, playerId);
                            pGUI->RenderText(textPos, ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), false, szText);

                            for(const auto& streamInfo : SpeakerList::playerStreams[playerId])
                            {
								//if(!pSettings->GetReadOnly().bVoiceChatEnable) continue;
                                if(streamInfo.second.GetColor() == NULL)
                                    continue;

                                textPos.x += ImGui::CalcTextSize(szText).x;
                                sprintf(szText2, "[%s]", streamInfo.second.GetName().c_str());
                                pGUI->RenderText(textPos, streamInfo.second.GetColor(), false, szText2);
                            }

                            textPos.x -= fSpaceFromIconToText;
                            textPos.x -= ImGui::CalcTextSize(szText).x;
                            textPos.y += pGUI->GetFontSize();

                            curTextLine++;
                        }
                    }
                }
            }
        }
    }
}

void SpeakerList::Draw(VECTOR* vec, float fDist)
{
	VECTOR TagPos;

	TagPos.X = vec->X;
	TagPos.Y = vec->Y;
	TagPos.Z = vec->Z;
	TagPos.Z += 0.25f + (fDist * 0.0475f);

	VECTOR Out;
	// CSprite::CalcScreenCoors
	(( void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_libGTASA+0x54EEC0+1))(&TagPos, &Out, 0, 0, 0, 0);

	if(Out.Z < 1.0f)
		return;

    ImVec2 pos = ImVec2(Out.X, Out.Y);
	pos.x -= SpeakerList::GetSpeakerIconScale() / 2;
    pos.y -= pGUI->GetFontSize() * 1.5;

    ImVec2 a = ImVec2(pos.x, pos.y);
    ImVec2 b = ImVec2(pos.x + SpeakerList::GetSpeakerIconScale(), pos.y + SpeakerList::GetSpeakerIconScale());
    ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)SpeakerList::tSpeakerIcon->raster, a, b);
}

void SpeakerList::OnSpeakerPlay(const Stream& stream, const uint16_t speaker) noexcept
{
    uint16_t wSpeaker = speaker;
    if(speaker < 0) wSpeaker = 0;
    else if(speaker > MAX_PLAYERS - 1) wSpeaker = MAX_PLAYERS - 1;
    if(speaker != wSpeaker) return;

    SpeakerList::playerStream[speaker] = &stream;
    SpeakerList::playerStreams[speaker][(Stream*)(&stream)] = stream.GetInfo();
}

void SpeakerList::OnSpeakerStop(const Stream& stream, const uint16_t speaker) noexcept
{
    uint16_t wSpeaker = speaker;
    if(speaker < 0) wSpeaker = 0;
    else if(speaker > MAX_PLAYERS - 1) wSpeaker = MAX_PLAYERS - 1;
    if(speaker != wSpeaker) return;

    SpeakerList::playerStreams[speaker].erase((Stream*)(&stream));
}

float SpeakerList::GetSpeakerIconScale() noexcept
{
    return PluginConfig::GetSpeakerIconScale();
}

void SpeakerList::SetSpeakerIconScale(const float speakerIconScale) noexcept
{
    float iSpeakerIconScale = speakerIconScale;
    if(speakerIconScale < 20.f) iSpeakerIconScale = 20.f;
    else if(speakerIconScale > 60.f) iSpeakerIconScale = 60.f;

    PluginConfig::SetSpeakerIconScale(iSpeakerIconScale);
}

void SpeakerList::SyncConfigs() noexcept
{
    SpeakerList::SetSpeakerIconScale(PluginConfig::GetSpeakerIconScale());
}

void SpeakerList::ResetConfigs() noexcept
{
    PluginConfig::SetSpeakerIconScale(PluginConfig::kSpeakerIconSize);
}

std::array<std::unordered_map<Stream*, StreamInfo>, MAX_PLAYERS> SpeakerList::playerStreams;
std::array<const Stream*, MAX_PLAYERS> SpeakerList::playerStream;

bool SpeakerList::initStatus { false };
bool SpeakerList::showStatus { false };

RwTexture* SpeakerList::tSpeakerIcon { nullptr };