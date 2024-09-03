#include "../main.h"
#include "../util/armhook.h"
#include "game.h"

CSprite2d::CSprite2d()
{
	((void (*)(CSprite2d *))(SA_ADDR(0x551810 + 1)))(this);
}

CSprite2d::~CSprite2d()
{
	((void (*)(CSprite2d *))(SA_ADDR(0x551838 + 1)))(this);
}

void CSprite2d::Draw(float x, float y, float width, float height, CRGBA const& color)
{
	((void (*)(CSprite2d *, float, float, float, float, CRGBA))(SA_ADDR(0x5525F8 + 1)))(this, x, y, width, height, color);
}

void CSprite2d::Draw(RECT const& rect, CRGBA const& color)
{
	CallFunction<void>(SA_ADDR(0x55265C + 1), this, &rect, &color);
}

void CSprite2d::DrawRotated(RECT const& rect, float d, CRGBA const& color)
{
	CallFunction<void>(SA_ADDR(0x552920 + 1), this, &rect, d, &color);
}

void CSprite2d::Delete()
{
	((void (*)(CSprite2d *))(SA_ADDR(0x551820 + 1)))(this);
}

void CSprite2d::InitPerFrame()
{
	CallFunction<void>(SA_ADDR(0x5519C8 + 1));
}

