#pragma once
#include "font.h"
#include <string>

class CAdjustableHudPosition
{
public:
	static CVector2D GetElementPosition(E_HUD_ELEMENT id);

	static void SetElementPosition(E_HUD_ELEMENT id, int x, int y);

private:
	static CVector2D m_aPosition[E_HUD_ELEMENT::HUD_SIZE];
};