#pragma once

#include "RW/RenderWare.h"

enum {
	MAX_MATERIALS = 17
};
enum eMaterialType {
	MATERIAL_TYPE_NONE = 0,
	MATERIAL_TYPE_TEXTURE = 1,
	MATERIAL_TYPE_TEXT = 2
};
enum eMaterialSize {
	MATERIAL_SIZE_32X32 = 10,
	MATERIAL_SIZE_64X32 = 20,
	MATERIAL_SIZE_64X64 = 30,
	MATERIAL_SIZE_128X32 = 40,
	MATERIAL_SIZE_128X64 = 50,
	MATERIAL_SIZE_128X128 = 60,
	MATERIAL_SIZE_256X32 = 70,
	MATERIAL_SIZE_256X64 = 80,
	MATERIAL_SIZE_256X128 = 90,
	MATERIAL_SIZE_256256 = 100,
	MATERIAL_SIZE_512X64 = 110,
	MATERIAL_SIZE_512X128 = 120,
	MATERIAL_SIZE_512X256 = 130,
	MATERIAL_SIZE_512X512 = 140
};
enum eMaterialTextAlign {
	MATERIAL_ALIGN_LEFT = 0,
	MATERIAL_ALIGN_CENTER = 1,
	MATERIAL_ALIGN_RIGHT = 2
};

class CObject : public CEntity
{
public:
	MATRIX4X4	  m_matTarget;
	MATRIX4X4	  m_matCurrent;
	uint8_t		  m_byteMoving;
	float		  m_fMoveSpeed;
	bool		  m_bIsPlayerSurfing;
	bool		  m_bNeedRotate;
	
	int			  m_ObjectModel;

	CQuaternion   m_quatTarget;
	CQuaternion   m_quatStart;

	MaterialInfo  m_pMaterials[MAX_MATERIALS];
	bool		  m_bMaterials;

	VECTOR 		  m_vecAttachedOffset;
	VECTOR 		  m_vecAttachedRotation;
	uint16_t 	  m_usAttachedVehicle;
	uint8_t 	  m_bAttachedType;
	bool	      m_bAttached;

	VECTOR 		  m_vecRot;
	VECTOR		  m_vecTargetRot;
	VECTOR		  m_vecTargetRotTar;
	VECTOR		  m_vecRotationTarget;
	VECTOR		  m_vecSubRotationTarget;
	float		  m_fDistanceToTargetPoint;
	
	CObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance);
	~CObject();

	void Process(float fElapsedTime);
	float DistanceRemaining(MATRIX4X4 *matPos, MATRIX4X4 *m_matPositionTarget);
	float RotaionRemaining(VECTOR matPos, VECTOR m_vecRot);

	void SetPos(float x, float y, float z);

	void MoveTo(float x, float y, float z, float speed, float rX, float rY, float rZ);

	void AttachToVehicle(uint16_t usVehID, VECTOR* pVecOffset, VECTOR* pVecRot);
	void ProcessAttachToVehicle(CVehicle* pVehicle);

	void InstantRotate(float x, float y, float z);
	void StopMoving();

	void SetMaterial(int iModel, uint8_t byteMaterialIndex, char *txdname, char *texturename, uint32_t dwColor);
	void SetMaterialText(uint8_t byteMaterialIndex, uint8_t byteMaterialSize, const char *szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char *szText);
	void MaterialTextProcess();
public:
 	uint8_t 		m_byteMaterialIndex;
 	uint8_t 		m_byteMaterialSize;
 	char 			m_szFontName[32];
 	uint8_t 		m_byteFontSize;
 	uint8_t 		m_byteFontBold;
 	uint32_t 		m_dwFontColor;
 	uint32_t 		m_dwBackgroundColor;
 	uint8_t 		m_byteAlign;
 	char 			m_szText[2048];
 	RwTexture* 	    m_MaterialTexture[MAX_MATERIALS];
 	uint32_t		m_dwMaterialColor[MAX_MATERIALS];
 	bool 			m_bHasMaterial;
 	bool			m_bIsMaterialtext;
 	bool			m_bDontCollideWithCamera;
 	uint32_t 		m_dwMoveTick;
};
