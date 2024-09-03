#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../util/armhook.h"
#include <cmath>
#include "materialtext.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CMaterialText *pMaterialText;

RwTexture* g_blanktexture;

float fixAngle(float angle)
{
	if (angle > 180.0f) angle -= 360.0f;
	if (angle < -180.0f) angle += 360.0f;

	return angle;
}

float subAngle(float a1, float a2)
{
	return fixAngle(fixAngle(a2) - a1);
}

CObject::CObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance)
{
	uint32_t dwRetID 	= 0;
	m_pEntity 			= nullptr;
	m_dwGTAId 			= 0;
	m_ObjectModel		= iModel;

	ScriptCommand(&create_object, iModel, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&put_object_at, dwRetID, fPosX, fPosY, fPosZ);

	m_pEntity = GamePool_Object_GetAt(dwRetID);
	m_dwGTAId = dwRetID;
	m_byteMoving = 0;
	m_fMoveSpeed = 0.0;

	m_vecRot = vecRot;
	m_vecTargetRotTar = vecRot;
	
	m_bIsPlayerSurfing = false;
	m_bNeedRotate = false;
	
	m_bAttached = false;
	m_bAttachedType = 0;
	m_usAttachedVehicle = 0xFFFF;

	InstantRotate(vecRot.X, vecRot.Y, vecRot.Z);
	
	m_bMaterials = false;
	for (auto & m_pMaterial : m_pMaterials)
	{
		m_pMaterial.m_bCreated = 0;
		m_pMaterial.pTex = nullptr;
	}

	for(int i = 0; i < 17; ++i)
	{
		m_MaterialTexture[i] = 0;
	}

	m_bHasMaterial = false;
	m_bIsMaterialtext = false;	
}
// todo
CObject::~CObject()
{
	m_bMaterials = false;
	for (auto & m_pMaterial : m_pMaterials)
	{
		if (m_pMaterial.m_bCreated && m_pMaterial.pTex)
		{
			m_pMaterial.m_bCreated = 0;
			RwTextureDestroy(m_pMaterial.pTex);
			m_pMaterial.pTex = nullptr;
		}
	}

	for(int i = 0; i < 17; ++i)
	{
		if(m_MaterialTexture[i] && m_MaterialTexture[i] != g_blanktexture)
		{
			RwTextureDestroy(m_MaterialTexture[i]);
			m_MaterialTexture[i] = 0;
		}
	}	
	
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if(m_pEntity)
		ScriptCommand(&destroy_object, m_dwGTAId);	
}

void CObject::Process(float fElapsedTime)
{
	if (m_bAttachedType == 1 && !m_bAttached)
	{
		CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(m_usAttachedVehicle);
		if (pVehicle)
		{
			if (pVehicle->IsAdded())
			{
				if (m_vecAttachedOffset.X > 10000.0f || m_vecAttachedOffset.Y > 10000.0f || m_vecAttachedOffset.Z > 10000.0f ||
					m_vecAttachedOffset.X < -10000.0f || m_vecAttachedOffset.Y < -10000.0f || m_vecAttachedOffset.Z < -10000.0f)
				{ 
					// пропускаем действие
				}
				else
				{	
					m_bAttached = true;
					ProcessAttachToVehicle(pVehicle);
				}
			}
		}
	}
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if (!m_pEntity) return;
	if (!(m_pEntity->mat)) return;
	if (m_byteMoving & 1)
	{
		MATRIX4X4 matPos;
		GetMatrix(&matPos);


		VECTOR matRot = m_vecRot;

		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matPos, &m_matTarget);
		float remainingRot = RotaionRemaining(m_vecTargetRotTar, m_vecTargetRot);

		if (distance >= remaining)
		{
			m_byteMoving &= ~1;

			m_vecTargetRotTar = m_vecTargetRot;

			TeleportTo(m_matTarget.pos.X, m_matTarget.pos.Y, m_matTarget.pos.Z);
			InstantRotate(m_vecTargetRot.X, m_vecTargetRot.Y, m_vecTargetRot.Z);
		}
		else
		{
			remaining /= distance;
			remainingRot /= distance;

			matPos.pos.X += (m_matTarget.pos.X - matPos.pos.X) / remaining;
			matPos.pos.Y += (m_matTarget.pos.Y - matPos.pos.Y) / remaining;
			matPos.pos.Z += (m_matTarget.pos.Z - matPos.pos.Z) / remaining;
			
			m_vecTargetRotTar.X += (m_vecTargetRot.X - m_vecTargetRotTar.X) / remaining;
			m_vecTargetRotTar.Y += (m_vecTargetRot.Y - m_vecTargetRotTar.Y) / remaining;
			m_vecTargetRotTar.Z += (m_vecTargetRot.Z - m_vecTargetRotTar.Z) / remaining;

			TeleportTo(matPos.pos.X, matPos.pos.Y, matPos.pos.Z);
			InstantRotate(m_vecTargetRotTar.X, m_vecTargetRotTar.Y, m_vecTargetRotTar.Z);
		}
		
		if (m_ObjectModel >= 19332 && m_ObjectModel <= 19338)
		{
			if (pNetGame) 
			{
				CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool) 
				{
					pPlayerPool->GetLocalPlayer()->ProcessSurfing();
				}
			}
		}
	}
}

float CObject::DistanceRemaining(MATRIX4X4 *matPos, MATRIX4X4 *m_matPositionTarget)
{
	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.X - m_matPositionTarget->pos.X) * (matPos->pos.X - m_matPositionTarget->pos.X);
	fSY = (matPos->pos.Y - m_matPositionTarget->pos.Y) * (matPos->pos.Y - m_matPositionTarget->pos.Y);
	fSZ = (matPos->pos.Z - m_matPositionTarget->pos.Z) * (matPos->pos.Z - m_matPositionTarget->pos.Z);
	return (float)sqrt(fSX + fSY + fSZ);
}

float CObject::RotaionRemaining(VECTOR matPos, VECTOR m_vecRot)
{
	float fSX,fSY,fSZ;
	fSX = (matPos.X - m_vecRot.X) * (matPos.X - m_vecRot.X);
	fSY = (matPos.Y - m_vecRot.Y) * (matPos.Y - m_vecRot.Y);
	fSZ = (matPos.Z - m_vecRot.Z) * (matPos.Z - m_vecRot.Z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObject::SetPos(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
		ScriptCommand(&put_object_at, m_dwGTAId, x, y, z);
}

void CObject::StopMoving()
{
	m_byteMoving = 0;
}

void CObject::MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ)
{
	m_matTarget.pos.X = fX;
	m_matTarget.pos.Y = fY;
	m_matTarget.pos.Z = fZ;

	m_vecTargetRot.X = fRotX;
	m_vecTargetRot.Y = fRotY;
	m_vecTargetRot.Z = fRotZ;
	
	m_fMoveSpeed = fSpeed;
	m_byteMoving |= 1;
}

void CObject::AttachToVehicle(uint16_t usVehID, VECTOR* pVecOffset, VECTOR* pVecRot)
{
	m_bAttached = false;
	m_bAttachedType = 1;
	m_usAttachedVehicle = usVehID;
	m_vecAttachedOffset.X = pVecOffset->X;
	m_vecAttachedOffset.Y = pVecOffset->Y;
	m_vecAttachedOffset.Z = pVecOffset->Z;

	m_vecAttachedRotation.X = pVecRot->X;
	m_vecAttachedRotation.Y = pVecRot->Y;
	m_vecAttachedRotation.Z = pVecRot->Z;
}

void CObject::ProcessAttachToVehicle(CVehicle* pVehicle)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
		*(uint32_t*)((uintptr_t)m_pEntity + 28) &= 0xFFFFFFFE;

		if (!ScriptCommand(&is_object_attached, m_dwGTAId))
		{
			ScriptCommand(&attach_object_to_car, m_dwGTAId, pVehicle->m_dwGTAId, m_vecAttachedOffset.X,
				m_vecAttachedOffset.Y, m_vecAttachedOffset.Z, m_vecAttachedRotation.X, m_vecAttachedRotation.Y, m_vecAttachedRotation.Z);
		}
	}
}

void CObject::InstantRotate(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		ScriptCommand(&set_object_rotation, m_dwGTAId, x, y, z);
	}
}

RwTexture* LoadFromTxdSlot(const char* szSlot, const char* szTexture, RwRGBA* rgba)
{	
	RwTexture* tex;
	if (strncmp(szSlot, "none", 5u))
	{	
		uintptr_t v10 = ((int (*)(const char*))(g_libGTASA + 0x55BB85))(szSlot);
		CallFunction<void>(g_libGTASA + 0x55BD6C + 1);
   		CallFunction<void>(g_libGTASA + 0x55BD6C + 1, v10, 0);
    	tex = CallFunction<RwTexture*>(g_libGTASA + 0x1B2558 + 1, szTexture, 0);
    	CallFunction<void>(g_libGTASA + 0x55BDA8 + 1);
	}

	if(!tex && strncmp(szTexture, "none", 5u))
	{
		if(!tex) tex = (RwTexture*)LoadTexture(std::string(std::string(szTexture) + "_" + szSlot).c_str());
		if(!tex) tex = (RwTexture*)LoadTexture(std::string(std::string(szSlot) + "_" + szTexture).c_str());
		if(!tex) tex = (RwTexture*)LoadTexture(szTexture);
		if(!tex)
		{
			std::string str = szTexture;
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			tex = (RwTexture*)LoadTexture(str.c_str());
		}
		if(!tex)
		{
			std::string str = szTexture;
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			tex = (RwTexture*)LoadTexture(std::string(str + "_" + szSlot).c_str());
		}
	}

	return tex;
}

void CObject::SetMaterial(int iModel, uint8_t byteMaterialIndex, char *txdname, char *texturename, uint32_t dwColor)
{
	if(byteMaterialIndex < 17)
	{
		if(m_MaterialTexture[byteMaterialIndex])
		{
			RwTextureDestroy(m_MaterialTexture[byteMaterialIndex]);
			m_MaterialTexture[byteMaterialIndex] = 0;
		}

		if(iModel < 1)
		{
			m_MaterialTexture[byteMaterialIndex] = g_blanktexture;
			if(!g_blanktexture)
				g_blanktexture = (RwTexture*)LoadTextureFromDB("samp", "blanktex");
		}		

		m_MaterialTexture[byteMaterialIndex] = (RwTexture*)LoadFromTxdSlot(txdname, texturename);
		if(!m_MaterialTexture[byteMaterialIndex]) {
			return;
		}
		m_dwMaterialColor[byteMaterialIndex] = dwColor;
		Log("color of material: 0x%X", m_dwMaterialColor[byteMaterialIndex]);
		m_bHasMaterial = true;
	}
}

void CObject::SetMaterialText(uint8_t byteMaterialIndex, uint8_t byteMaterialSize, const char *szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char *szText)
{
	if(byteMaterialIndex < 17)
	{
		if(!szText || !strlen(szText)) return;

		if(m_MaterialTexture[byteMaterialIndex])
		{
			RwTextureDestroy(m_MaterialTexture[byteMaterialIndex]);
			m_MaterialTexture[byteMaterialIndex] = 0;
		}

		m_byteMaterialIndex = byteMaterialIndex;
		m_byteMaterialSize = byteMaterialSize;
		memset(m_szFontName, 0, 32);
		strncpy(m_szFontName, szFontName, 32);
		m_szFontName[32] = 0;
		m_byteFontSize = byteFontSize;
		m_byteFontBold = byteFontBold;
		m_dwFontColor = dwFontColor;
		m_dwBackgroundColor = dwBackgroundColor;
		m_byteAlign = byteAlign;
		memset(m_szText, 0, 2048);
		strncpy(m_szText, szText, 2048);
		m_szText[2048] = 0;

		m_bHasMaterial = false;
		m_bIsMaterialtext = true;
	}
}

void CObject::MaterialTextProcess()
{
	if(m_bHasMaterial || !m_bIsMaterialtext)
		return;

	if(!m_szText || !strlen(m_szText)) return;

	// materialsize
	if(m_byteMaterialSize < 10)
		m_byteMaterialSize = 10;
	else if(m_byteMaterialSize > 140)
		m_byteMaterialSize = 140;

	// align
	if(m_byteAlign < 0 || m_byteAlign > 2)
		m_byteAlign = 1;

	// get material size XY
	static uint16_t sizes[14][2] = {
		{ 32, 32 } , { 64, 32 }, { 64, 64 }, { 128, 32 }, { 128, 64 }, { 128,128 }, { 256, 32 },
		{ 256, 64 } , { 256, 128 } , { 256, 256 } , { 512, 64 } , { 512,128 } , { 512,256 } , { 512,512 }
	};
	int del = (m_byteMaterialSize / 10) - 1;
	int iSizeX = sizes[del][0];
	int iSizeY = sizes[del][1];

	// correct font size
	m_byteFontSize = m_byteFontSize * 0.75;

	// set object material text
	m_MaterialTexture[m_byteMaterialIndex] = pMaterialText->Generate(iSizeX, iSizeY, m_szFontName, m_byteFontSize, m_byteFontBold, m_dwFontColor, m_dwBackgroundColor, m_byteAlign, m_szText);;
	m_bHasMaterial = true;
	m_bIsMaterialtext = false;
}
