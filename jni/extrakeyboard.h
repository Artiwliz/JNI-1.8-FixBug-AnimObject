#pragma once
#include "game/RW/RenderWare.h"
class CExtraKeyBoard
{
public:
	CExtraKeyBoard();
	~CExtraKeyBoard();

	void Render();
	void Clear();
	void Show(bool bShow);
	
private:
	bool 		m_bIsActive;
	bool 		m_bIsExtraShow;
	bool		m_boardplayer;
	bool 		m_passengerUseTexture;
	bool 		m_bDriveByMode;
	RwTexture 	*m_passengerButtonTexture[2];
	uint32_t	m_dwLastTickPassengerHover;
};