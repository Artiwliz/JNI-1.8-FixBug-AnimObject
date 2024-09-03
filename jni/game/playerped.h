#pragma once

#include "aimstuff.h"
#include "CRemoteData.h"
#include "RW/common.h"

#define FALL_SKYDIVE		1
#define FALL_SKYDIVE_ACCEL	2
#define PARA_DECEL			3
#define PARA_FLOAT			4

enum StuffType 
{
	STUFF_TYPE_NONE,
	STUFF_TYPE_BEER,
	STUFF_TYPE_DYN_BEER,
	STUFF_TYPE_PINT_GLASS,
	STUFF_TYPE_CIGGI
};

class CPlayerPed
	: public CEntity
{
public:
	CPlayerPed();	// local
	CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation); // remote
	~CPlayerPed();

	void Destroy();

	CAMERA_AIM * GetCurrentAim();
	void SetCurrentAim(CAMERA_AIM *pAim);

	uint16_t GetCameraMode();

	void SetCameraMode(uint16_t byteCamMode);

	float GetCameraExtendedZoom();
	void ClumpUpdateAnimations(float step, int flag);
	uint8_t GetExtendedKeys();
	void TogglePlayerControllableWithoutLock(bool bToggle);
	void ApplyCrouch();
	void ResetCrouch();
	bool IsCrouching();
	void ProcessBulletData(BULLET_DATA* btData);

	void SetCameraExtendedZoom(float fZoom);
	void SetDead();
	// 0.3.7
	bool IsInVehicle();
	// 0.3.7
	bool IsAPassenger();
	// 0.3.7
	void ApplyCommandTask(char* a2, int a4, int a5, int a6, VECTOR* a7, char a8, float a9, int a10, int a11, char a12);
	VEHICLE_TYPE* GetGtaVehicle();
	// 0.3.7
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	// 0.3.7
	void SetInitialState();
	// 0.3.7
	void SetHealth(float fHealth);
	void SetArmour(float fArmour);
	// 0.3.7
	float GetHealth();
	float GetArmour();
	// 0.3.7
	void TogglePlayerControllable(bool bToggle);
	// 0.3.7
	void SetModelIndex(unsigned int uiModel);

	bool IsAnimationPlaying(char* szAnimName);
	void ClearAllTasks();

	void SetPlayerSpecialAction(int iAction);
	void ProcessSpecialAction();

	void SetInterior(uint8_t byteID);

	void PutDirectlyInVehicle(int iVehicleID, int iSeat);
	void EnterVehicle(int iVehicleID, bool bPassenger);
	// 0.3.7
	void ExitCurrentVehicle();
	// 0.3.7
	int GetCurrentVehicleID();
	int GetVehicleSeatID();

	ENTITY_TYPE* GetEntityUnderPlayer();

	void GiveWeapon(int iWeaponID, int iAmmo);
	uint8_t GetCurrentWeapon();
	void SetArmedWeapon(int iWeaponID);

	void SetPlayerAimState();
	void ClearPlayerAimState();
	void SetAimZ(float fAimZ);
	float GetAimZ();
	WEAPON_SLOT_TYPE * GetCurrentWeaponSlot();
	//CAMERA_AIM* GetCurrentAim();
	// ��������

	void ClearAllWeapons();
	// ��������
	void DestroyFollowPedTask();
	// ��������
	void ResetDamageEntity();

	// 0.3.7
	void RestartIfWastedAt(VECTOR *vecRestart, float fRotation);
	// 0.3.7
	void ForceTargetRotation(float fRotation);
	// 0.3.7
	uint8_t GetActionTrigger();
	// 0.3.7
	bool IsDead();
	uint16_t GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog);
	void SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);
	// 0.3.7
	void SetMoney(int iAmount);
	// 0.3.7
	void ShowMarker(uint32_t iMarkerColorID);
	// 0.3.7
	void HideMarker();
	// 0.3.7
	void SetFightingStyle(int iStyle);
	// 0.3.7
	void SetRotation(float fRotation);
	// 0.3.7
	void ApplyAnimation( char *szAnimName, char *szAnimFile, float fT, int opt1, int opt2, int opt3, int opt4, int iUnk );
	// 0.3.7
	void GetBonePosition(int iBoneID, VECTOR* vecOut);
	// roflan
	unsigned char FindDeathReasonAndResponsiblePlayer(PLAYERID *nPlayer);
	void SetActionTrigger(uint8_t action);
	PED_TYPE * GetGtaActor() { return m_pPed; };

	void AttachObject(ATTACHED_OBJECT_INFO* pInfo, int iSlot);
	void SetAttachOffset(int iSlot, VECTOR pos, VECTOR rot);
	void DeattachObject(int iSlot);
	bool IsHasAttach();
	void FlushAttach();
	void ProcessAttach();

	void ProcessHeadMatrix();

	bool IsPlayingAnim(int idx);
	int GetCurrentAnimationIndex(float& blendData);
	void PlayAnimByIdx(int idx, float BlendData);

	void SetMoveAnim(int iAnimGroup);

	void FireInstant();
	void GetWeaponInfoForFire(int bLeft, VECTOR* vecBone, VECTOR* vecOut);
	VECTOR* GetCurrentWeaponFireOffset();
	void GetTransformedBonePosition(int iBoneID, VECTOR* vecOut);

	void LockControllable(bool bLock);

	void SetFloatStat(unsigned char ucStat, float fLevel);
	// Support Parachute
	void CheckVehicleParachute();
	void ProcessParachutes();
	void ProcessParachuteSkydiving();
	void ProcessParachuting();	
	// Support Pissing
	void StartPissing();
    void StopPissing();
	bool IsPissing();
	// Support Dancing
	void StartDancing(int iStyle);
	bool IsDancing();
	void StopDancing();
	void ProcessDancing();
	// CustomAnim
	bool IsCheckCustomAnim();
	// Support Drunk 
	void SetDrunkLevel(uint32_t dwLevel) 
	{ 
		if (dwLevel > 50000) dwLevel = 50000;
		else if (dwLevel < 0) dwLevel = 0;
		
		m_stuffData.dwDrunkLevel = dwLevel; 
	}	
	uint32_t GetDrunkLevel() { return m_stuffData.dwDrunkLevel; };
	int GetPedStat();	
	int GetStuff();
	bool ApplyStuff();
	void GiveStuff(int iType);
	void DropStuff();
	void ProcessDrunk();
	// Support CallPhone
	void ToggleCellphone(int iOn);
	int IsCellphoneEnabled();

public:
	PED_TYPE*	m_pPed;
	uint8_t		m_bytePlayerNumber;
	uint32_t	m_dwArrow;

	int 		m_iSpecialAction;

	bool		m_bHaveBulletData;
	BULLET_DATA m_bulletData;
	bool		m_bLockControllable;

	int			m_iParachuteAnim;
	int			m_iParachuteState;
	uint32_t	m_dwParachuteObject;

	bool		m_bPissingState;
	uint32_t	m_dwPissParticlesHandle;

	bool		m_bDanceState;
	int			m_iDanceStyle;
	
	int			m_iCellPhoneEnabled;
	
	struct {
		int				iType;
		uint32_t		dwObject;
		uint32_t		dwDrunkLevel;
		uint32_t		dwLastUpdateTick;
	} m_stuffData;		

	MATRIX4X4 m_HeadBoneMatrix;
	ATTACHED_OBJECT_INFO_INTERNAL m_aAttachedObjects[MAX_ATTACHED_OBJECTS];

	CRemoteDataStorage* m_pData;
};