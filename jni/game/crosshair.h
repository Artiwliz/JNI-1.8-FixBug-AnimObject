#pragma once
#include "../game/game.h"

class CCrossHair : private CSprite2d
{
private:

public:
	CCrossHair(/* args */);
	~CCrossHair();

	void ChangeAim(const char* szName);
	void Render();
	
	bool m_UsedCrossHair;
};


