#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../util/armhook.h"
#include "CStats.h"
#include "chatwindow.h"
#include "../scoreboard.h"

#include <jni.h>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow* pChatWindow;
extern CScoreBoard *pScoreBoard;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];
uintptr_t dwCurPlayerActor = 0;
uintptr_t dwCurWeaponProcessingPlayer = 0;
uint8_t byteCurPlayer = 0;
uint8_t byteCurDriver = 0;
uint8_t byteInternalPlayer = 0;
uint16_t wSavedCameraMode2 = 0;
uint8_t byteSavedCameraMode;

float localStatsTypesFloat[MAX_FLOAT_STATS];

extern uintptr_t dwPlayerPedPtrs[PLAYER_PED_SLOTS];

uint16_t(*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
uint16_t CPad__GetPedWalkLeftRight_hook(uintptr_t thiz)
{

	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if ((dwResult == 0xFF80 || dwResult == 0x80) &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x20;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyLR = CPad__GetPedWalkLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t(*CPad__GetPedWalkUpDown)(uintptr_t thiz);
uint16_t CPad__GetPedWalkUpDown_hook(uintptr_t thiz)
{

	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if ((dwResult == 0xFF80 || dwResult == 0x80) &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x20;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyUD = CPad__GetPedWalkUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint32_t (*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_hook(uintptr_t thiz, uint32_t unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

uint32_t (*CPad__JumpJustDown)(uintptr_t thiz);
uint32_t CPad__JumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreJump &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			!RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetJump)(uintptr_t thiz);
uint32_t CPad__GetJump_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetJump(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t (*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_hook()
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t (*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreSwim && 
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreSwim = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_hook(uintptr_t thiz, int unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return 0;
	}
	else
	{
		return CPad__DuckJustDown(thiz, unk);
	}
}

uint32_t (*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_hook(uintptr_t thiz)
{
	/*
		0 - �� ����
		1 - ������� ���� (���)
		2 - ������� ���� (��� + F)
	*/

	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK])
			return 2;

		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = true;

		//if(dwResult == 2) 
		//{
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		//}
		//else if(dwResult == 1)
		//{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = false;
		//}

		return dwResult;
	}
}

uint32_t (*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

int16_t (*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyLR;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyLR = CPad__GetSteeringLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetSteeringUpDown)(uintptr_t thiz);
uint16_t CPad__GetSteeringUpDown_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].wKeyUD;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyUD = CPad__GetSteeringUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint16_t (*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
				return 0;
		}

		// local player
		uint16_t wAccelerate = CPad__GetAccelerate(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
		if (wAccelerate == 0xFF)
		{
			if (pPlayerPed)
			{
				VEHICLE_TYPE* pGtaVehicle = pPlayerPed->GetGtaVehicle();
				if (pGtaVehicle)
				{
					if (pGtaVehicle->m_nVehicleFlags.bEngineOn == 0)
					{
						wAccelerate = 0;
					}
				}
			}
		}

		return wAccelerate;
	}
}

uint16_t (*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
		return wBrake;
	}
}

uint32_t (*CPad__GetHandBrake)(uintptr_t thiz);
uint32_t CPad__GetHandBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint32_t handBrake = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = handBrake;
		return handBrake;
	}
}

uint32_t(*CPad__GetHorn)(uintptr_t thiz, uint8_t a2);
uint32_t CPad__GetHorn_hook(uintptr_t thiz, uint8_t a2)
{
	if (byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
				return 0;
		}

		// local player
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__GetHorn(thiz, a2);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

uint32_t(*CPad__GetNitroFired)(uintptr_t thiz);
uint32_t CPad__GetNitroFired_hook(uintptr_t thiz)
{
	if (byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
				return 0;
		}

		// local player
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetNitroFired(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t(*CPad__GetHydraulicJump)(uintptr_t thiz);
uint32_t CPad__GetHydraulicJump_hook(uintptr_t thiz)
{
	if (byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
				return 0;
		}

		// local player
		uint32_t result = CPad__GetHydraulicJump(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = result;
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

int (*CPad__GetTurretLeft)(uintptr_t thiz);
int CPad__GetTurretLeft_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetTurretLeft(thiz);
				}
			}
		}
	}
	return 0;
}

int (*CPad__GetTurretRight)(uintptr_t thiz);
int CPad__GetTurretRight_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetTurretRight(thiz);
				}
			}
		}
	}
	return 0;
}

uint32_t(*CPad__HornJustDown)(uintptr_t thiz);
uint32_t CPad__HornJustDown_hook(uintptr_t thiz)
{
	uint32_t result = 0;
	if (byteCurDriver != 0)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwPlayerPedPtrs[byteCurDriver];
		if (pPedPlayer && pPedPlayer->pVehicle)
		{
			VEHICLE_TYPE* pVehicle = (VEHICLE_TYPE*)pPedPlayer->pVehicle;
			if (pVehicle)
			{
				if (!pVehicle->m_nVehicleFlags.bSirenOrAlarm)
				{
					*((uint32_t*)(uintptr_t)pVehicle + 328) = RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH] ? 1 : 0;
				}
			}
		}
	}
	else
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
				return 0;
		}

		result = CPad__HornJustDown(thiz);
		PED_TYPE* pPedPlayer = GamePool_FindPlayerPed();
		if (pPedPlayer && pPedPlayer->pVehicle)
		{
			VEHICLE_TYPE* pVehicle = (VEHICLE_TYPE*)pPedPlayer->pVehicle;
			if (pVehicle)
			{
				if (!pVehicle->m_nVehicleFlags.bSirenOrAlarm)
				{
					LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__GetHorn(thiz, 0);
				}
			}
		}
	}
	return result;
}

uint32_t (*CPad__ExitVehicleJustDown)(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec);
uint32_t CPad__ExitVehicleJustDown_hook(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec)
{
	CPlayerPool *pPlayerPool;
	CLocalPlayer *pLocalPlayer;

	if(pNetGame)
	{
		pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
		{
			pLocalPlayer = pPlayerPool->GetLocalPlayer();
			if(pLocalPlayer)
			{
				if( pLocalPlayer->HandlePassengerEntry() )
					return 0;
			}
		}
	}

	return CPad__ExitVehicleJustDown(thiz, a2, vehicle, a4, vec);
}
extern float * pfCameraExtZoom;
void (*CPed__ProcessControl)(uintptr_t thiz);
void CPed__ProcessControl_hook(uintptr_t thiz)
{
	dwCurPlayerActor = thiz;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// REMOTE PLAYER
		uint16_t byteSavedCameraMode;
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);

		/*uint16_t wSavedCameraMode2 = *((uint16_t*)g_libGTASA + 0x8B0FBC);
		*((uint16_t*)g_libGTASA + 0x8B0FBC) = GameGetPlayerCameraMode(byteCurPlayer);
		if(*((uint16_t*)g_libGTASA + 0x8B0FBC) == 4) *((uint16_t*)g_libGTASA + 0x8B0FBC) = 0;*/
		uint16_t wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4)* wCameraMode2 = 0;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		makeNOP(SA_ADDR(0x439B7A), 2);

		*(uint8_t*)(g_libGTASA + 0x8E864C) = byteCurPlayer;

		// Update remote player stats
		CRemoteDataStorage* pData = CRemoteData::GetRemoteDataStorage((PED_TYPE*)thiz);
		if (pData)
		{
			// Store local stats
			memcpy(localStatsTypesFloat, (float*)(SA_ADDR(0x8C41A0)), sizeof(localStatsTypesFloat));

			// Set stats to remote player
			memcpy((float*)(SA_ADDR(0x8C41A0)), pData->GetStatsTypesFloat(), sizeof(localStatsTypesFloat));
		}

		// call original
		CPed__ProcessControl(thiz);
		// restore
		WriteMemory(SA_ADDR(0x439B7A), (uintptr_t)"\xFA\xF7\x1D\xF8", 4);
		*(uint8_t*)(SA_ADDR(0x8E864C)) = 0;
		*pbyteCameraMode = byteSavedCameraMode;
		//*((uint16_t*)g_libGTASA + 0x8B0FBC) = wSavedCameraMode2;
		GameSetLocalPlayerCameraExtZoom();
		GameSetLocalPlayerAim();
		*wCameraMode2 = wSavedCameraMode2;

		if (pData)
		{
			// Restore local stats
			memcpy((float*)(g_libGTASA + 0x8C41A0), localStatsTypesFloat, sizeof(localStatsTypesFloat));
		}
	}
	else
	{
		// LOCAL PLAYER

		WriteMemory(SA_ADDR(0x4BED92), (uintptr_t)"\x10\x60", 2);
		
		(*CPed__ProcessControl)(thiz);

		// Reapply the no ped rots from Cam patch
		WriteMemory(SA_ADDR(0x4BED92), (uintptr_t)"\x00\x46", 2);
	}

	return;
}

void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)thiz;
	uintptr_t this_vtable = pVehicle->entity.vtable;
	this_vtable -= g_libGTASA;

	uintptr_t call_addr = 0;

	switch(this_vtable)
	{
		// CAutomobile
		case 0x5CC9F0:
		call_addr = 0x4E314C;
		break;

		// CBoat
		case 0x5CCD48:
		call_addr = 0x4F7408;
		break;

		// CBike
		case 0x5CCB18:
		call_addr = 0x4EE790;
		break;

		// CPlane
		case 0x5CD0B0:
		call_addr = 0x5031E8;
		break;

		// CHeli
		case 0x5CCE60:
		call_addr = 0x4FE62C;
		break;

		// CBmx
		case 0x5CCC30:
		call_addr = 0x4F3CE8;
		break;

		// CMonsterTruck
		case 0x5CCF88:
		call_addr = 0x500A34;
		break;

		// CQuadBike
		case 0x5CD1D8:
		call_addr = 0x505840;
		break;

		// CTrain
		case 0x5CD428:
		call_addr = 0x50AB24;
		break;
	}

	if(pVehicle && pVehicle->pDriver)
	{
		byteCurDriver = FindPlayerNumFromPedPtr((uintptr_t)pVehicle->pDriver);
	}

	if(pVehicle->pDriver && pVehicle->pDriver->dwPedType == 0 &&
		pVehicle->pDriver != GamePool_FindPlayerPed() && 
		*(uint8_t*)(SA_ADDR(0x8E864C)) == 0) // CWorld::PlayerInFocus
	{
		*(uint8_t*)(SA_ADDR(0x8E864C)) = 0;

		pVehicle->pDriver->dwPedType = 4;
		//CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(SA_ADDR(0x364B64 + 1)))(thiz + 0x138);
		pVehicle->pDriver->dwPedType = 0;
	}
	else
	{
		(( void (*)(uintptr_t))(SA_ADDR(0x364B64 + 1)))(thiz + 0x138);
	}

	// Tyre burst fix
	if (pVehicle->pDriver)
	{
		if (pVehicle->m_nVehicleFlags.bTyresDontBurst)
		{
			pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
		}
		if(!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
	}
	else
	{
		if (!pVehicle->m_nVehicleFlags.bTyresDontBurst)
		{
			pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
		}
		if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
	}

	// VEHTYPE::ProcessControl()
    (( void (*)(VEHICLE_TYPE*))(SA_ADDR(call_addr + 1)))(pVehicle);
}


uint32_t(*CPad__GetWeapon)(uintptr_t thiz, uintptr_t ped, bool unk);
uint32_t CPad__GetWeapon_hook(uintptr_t thiz, uintptr_t ped, bool unk)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint8_t old = *(uint8_t*)(SA_ADDR(0x8E864C));
		*(uint8_t*)(SA_ADDR(0x8E864C)) = byteCurPlayer;
		uintptr_t value = CPad__GetWeapon(thiz, ped, unk);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = value;
		*(uint8_t*)(SA_ADDR(0x8E864C)) = old;
		return value;
	}
}

uintptr_t g_playerPed;
uintptr_t(*ProcessPlayerWeapon)(uintptr_t thiz, uintptr_t playerPed);
uintptr_t ProcessPlayerWeapon_hook(uintptr_t thiz, uintptr_t playerPed)
{
	g_playerPed = playerPed;
	uintptr_t toRetn = ProcessPlayerWeapon(thiz, playerPed);

	g_playerPed = 0;
	return toRetn;
}

uint32_t(*GetTarget)(uintptr_t thiz, int a2);
uint32_t GetTarget_hook(uintptr_t thiz, int a2)
{
	if (!g_playerPed)
	{
		return GetTarget(thiz, a2);
	}
	if (!(*(uint32_t*)(g_playerPed + 1088)))
	{
		return GetTarget(thiz, a2);
	}

	return *(uint8_t*)(*(uint32_t*)(g_playerPed + 1088) + 52) & 0b00001000;
}

uint32_t(*CPad__GetEnterTargeting)(uintptr_t thiz);
uint32_t CPad__GetEnterTargeting_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE];
	}
	else
	{
		uint8_t old = *(uint8_t*)(SA_ADDR(0x8E864C));
		*(uint8_t*)(SA_ADDR(0x8E864C)) = byteCurPlayer;
		uintptr_t result = CPad__GetEnterTargeting(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = result;


		*(uint8_t*)(SA_ADDR(0x8E864C)) = old;
		return result;
	}
}
uint32_t TaskUseGun(uintptr_t thiz, uintptr_t ped)
{
    dwCurPlayerActor = ped;
    byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

    uint32_t result = 0;

    if (dwCurPlayerActor &&
        (byteCurPlayer != 0)) // not local player and local player's keys set.
    {
        uint16_t byteSavedCameraMode = *pbyteCameraMode;
        *pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

        uint16_t wSavedCameraMode2 = *wCameraMode2;
        *wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
        if (*wCameraMode2 == 4)* wCameraMode2 = 0;

        // save the camera zoom factor, apply the context
        GameStoreLocalPlayerCameraExtZoom();
        GameSetRemotePlayerCameraExtZoom(byteCurPlayer);

        // aim switching
        GameStoreLocalPlayerAim();
        GameSetRemotePlayerAim(byteCurPlayer);
        *(uint8_t*)(SA_ADDR(0x8E864C)) = byteCurPlayer;

        result = ((uint32_t(*)(uintptr_t, uintptr_t))(SA_ADDR(0x46D6AC + 1)))(thiz, ped);

        // restore the camera modes, internal id and local player's aim
        *pbyteCameraMode = byteSavedCameraMode;
        //*((uint16_t*)g_libGTASA + 0x8B0FBC) = wSavedCameraMode2;

        // remote the local player's camera zoom factor
        GameSetLocalPlayerCameraExtZoom();

        *(uint8_t*)(SA_ADDR(0x8E864C)) = 0;
        GameSetLocalPlayerAim();
        *wCameraMode2 = wSavedCameraMode2;
    }
    else
    {
        result = ((uint32_t(*)(uintptr_t, uintptr_t))(SA_ADDR(0x46D6AC + 1)))(thiz, ped);
    }

    return result;
}

uint32_t TaskProcess(uintptr_t thiz, uintptr_t ped, int unk, int unk1)
{
	dwCurPlayerActor = ped;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	uint8_t old = *(uint8_t*)(SA_ADDR(0x8E864C));
	*(uint8_t*)(SA_ADDR(0x8E864C)) = byteCurPlayer;

	uint32_t result =  ((uint32_t(*)(uintptr_t, uintptr_t, int, int))(SA_ADDR(0x4C2F7C + 1)))(thiz, ped, unk, unk1);
	*(uint8_t*)(SA_ADDR(0x8E864C)) = old;
	return result;
}

bool bWeaponClicked;
extern "C" {
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onWeaponChanged(JNIEnv *pEnv, jobject thiz) 
	{
		if (!pGame->FindPlayerPed()) return;
		if (!bWeaponClicked) 
		{
			bWeaponClicked = true;
		}
		else
		{ 
			bWeaponClicked = false;
		}
	}
}
extern "C" {
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_showTab(JNIEnv *pEnv, jobject thiz) {
		pScoreBoard->Toggle();
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onTabClose(JNIEnv *pEnv, jobject thiz) { 
		pScoreBoard->Close();
	}
}
uint32_t (*CPad__CycleWeaponRightJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponRightJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0)) return 0;

	return CPad__CycleWeaponRightJustDown(thiz);
}

uint32_t(*CPad__CycleWeaponLeftJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponLeftJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0)) return 0;
	
	return CPad__CycleWeaponLeftJustDown(thiz);
}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	// fix aiming problems
	//memcpy((void*)(g_libGTASA + 0x008C3E20), , )

	// CPed::ProcessControl
	installHook(SA_ADDR(0x45A280), (uintptr_t) CPed__ProcessControl_hook, (uintptr_t *) &CPed__ProcessControl);
	// all vehicles ProcessControl
	InstallMethodHook(SA_ADDR(0x5CCA1C), (uintptr_t)AllVehicles__ProcessControl_hook); // CAutomobile::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CCD74), (uintptr_t)AllVehicles__ProcessControl_hook); // CBoat::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CCB44), (uintptr_t)AllVehicles__ProcessControl_hook); // CBike::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CD0DC), (uintptr_t)AllVehicles__ProcessControl_hook); // CPlane::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CCE8C), (uintptr_t)AllVehicles__ProcessControl_hook); // CHeli::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CCC5C), (uintptr_t)AllVehicles__ProcessControl_hook); // CBmx::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CCFB4), (uintptr_t)AllVehicles__ProcessControl_hook); // CMonsterTruck::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CD204), (uintptr_t)AllVehicles__ProcessControl_hook); // CQuadBike::ProcessControl
	InstallMethodHook(SA_ADDR(0x5CD454), (uintptr_t)AllVehicles__ProcessControl_hook); // CTrain::ProcessControl
	InstallMethodHook(SA_ADDR(0x5C8610), (uintptr_t)TaskUseGun);
	//InstallMethodHook(g_libGTASA + 0x005CC1D4, (uintptr_t)TaskProcess);

	// lr/ud (onfoot)
	installHook(SA_ADDR(0x39D08C), (uintptr_t) CPad__GetPedWalkLeftRight_hook, (uintptr_t *) &CPad__GetPedWalkLeftRight);
	installHook(SA_ADDR(0x39D110), (uintptr_t) CPad__GetPedWalkUpDown_hook, (uintptr_t *) &CPad__GetPedWalkUpDown);

	// Nitro/Hydraulic sync support
	installHook(SA_ADDR(0x39D54C), (uintptr_t) CPad__GetNitroFired_hook, (uintptr_t *) &CPad__GetNitroFired);
	installHook(SA_ADDR(0x39D528), (uintptr_t) CPad__GetHydraulicJump_hook, (uintptr_t *) &CPad__GetHydraulicJump);
	
	// Turn buttons in flight
	installHook(SA_ADDR(0x39D344), (uintptr_t) CPad__GetTurretLeft_hook, (uintptr_t *) &CPad__GetTurretLeft);
	installHook(SA_ADDR(0x39D368), (uintptr_t) CPad__GetTurretRight_hook, (uintptr_t *) &CPad__GetTurretRight);

	// sprint/jump stuff
	installHook(SA_ADDR(0x39EAA4), (uintptr_t) CPad__GetSprint_hook, (uintptr_t *) &CPad__GetSprint);
	installHook(SA_ADDR(0x39E9B8), (uintptr_t) CPad__JumpJustDown_hook, (uintptr_t *) &CPad__JumpJustDown);
	installHook(SA_ADDR(0x39E96C), (uintptr_t) CPad__GetJump_hook, (uintptr_t *) &CPad__GetJump);
	installHook(SA_ADDR(0x39E824), (uintptr_t) CPad__GetAutoClimb_hook, (uintptr_t *) &CPad__GetAutoClimb);
	installHook(SA_ADDR(0x39E8C0), (uintptr_t) CPad__GetAbortClimb_hook, (uintptr_t *) &CPad__GetAbortClimb);

	// swimm
	installHook(SA_ADDR(0x39EA0C), (uintptr_t) CPad__DiveJustDown_hook, (uintptr_t *) &CPad__DiveJustDown);
	installHook(SA_ADDR(0x39EA4C), (uintptr_t) CPad__SwimJumpJustDown_hook, (uintptr_t *) &CPad__SwimJumpJustDown);

	installHook(SA_ADDR(0x39DD9C), (uintptr_t) CPad__MeleeAttackJustDown_hook, (uintptr_t *) &CPad__MeleeAttackJustDown);

	installHook(SA_ADDR(0x39E038), (uintptr_t) CPad__GetWeapon_hook, (uintptr_t *) &CPad__GetWeapon);
	installHook(SA_ADDR(0x39E498), (uintptr_t) CPad__GetEnterTargeting_hook, (uintptr_t *) &CPad__GetEnterTargeting);
	installHook(SA_ADDR(0x39E418), (uintptr_t) GetTarget_hook, (uintptr_t *) &GetTarget);
	installHook(SA_ADDR(0x4C1748), (uintptr_t) ProcessPlayerWeapon_hook, (uintptr_t *) &ProcessPlayerWeapon);

	installHook(SA_ADDR(0x39E7B0), (uintptr_t) CPad__DuckJustDown_hook, (uintptr_t *) &CPad__DuckJustDown);
	installHook(SA_ADDR(0x39DB50), (uintptr_t) CPad__GetBlock_hook, (uintptr_t *) &CPad__GetBlock);

	// steering lr/ud (incar)
	installHook(SA_ADDR(0x39C9E4), (uintptr_t) CPad__GetSteeringLeftRight_hook,
			(uintptr_t *) &CPad__GetSteeringLeftRight);
	installHook(SA_ADDR(0x39CBF0), (uintptr_t) CPad__GetSteeringUpDown_hook, (uintptr_t *) &CPad__GetSteeringUpDown);

	installHook(SA_ADDR(0x39DB7C), (uintptr_t) CPad__GetAccelerate_hook, (uintptr_t *) &CPad__GetAccelerate);
	installHook(SA_ADDR(0x39D938), (uintptr_t) CPad__GetBrake_hook, (uintptr_t *) &CPad__GetBrake);
	installHook(SA_ADDR(0x39D754), (uintptr_t) CPad__GetHandBrake_hook, (uintptr_t *) &CPad__GetHandBrake);
	installHook(SA_ADDR(0x39D4F8), (uintptr_t) CPad__HornJustDown_hook, (uintptr_t *) &CPad__HornJustDown);
	installHook(SA_ADDR(0x39D4C8), (uintptr_t) CPad__GetHorn_hook, (uintptr_t *) &CPad__GetHorn);
	installHook(SA_ADDR(0x39DA1C), (uintptr_t) CPad__ExitVehicleJustDown_hook, (uintptr_t *) &CPad__ExitVehicleJustDown);
	installHook(SA_ADDR(0x39DD30), (uintptr_t) CPad__CycleWeaponRightJustDown_hook, (uintptr_t*)&CPad__CycleWeaponRightJustDown);
	installHook(SA_ADDR(0x39DCC4), (uintptr_t) CPad__CycleWeaponLeftJustDown_hook, (uintptr_t*)&CPad__CycleWeaponLeftJustDown);
}