#pragma once

#include "game/rgba.h"

class CFont
{
public:
	static void AsciiToGxtChar(const char* ascii, uint16_t* gxt);
	static void SetOrientation(int alignment);
	static void SetColor(CRGBA* color);
	static void SetColor(uint32_t* dwColor);
	static void SetEdge(short value);
	static void SetBackground(bool enable, bool includeWrap);
	static void SetWrapX(float value);
	static void SetCentreSize(float size);
	static void SetBackgroundColor(CRGBA* color);
	static void SetBackgroundColor(uint32_t* dwColor);
	static void SetScale(float w);
	static void SetScale(float x, float y);
	static void SetFontStyle(short style);
	static void SetProportional(bool on);
	static void PrintString(float X, float Y, uint16_t* sz);
	static void PrintString(float posX, float posY, const char* string);

	static void SetRightJustifyWrap(float v);
	static void SetDropShadowPosition(char a);
	static void SetDropColor(CRGBA* color);
	static void SetDropColor(uint32_t* dwColor);
	static void SetJustify(uint8_t just);
};