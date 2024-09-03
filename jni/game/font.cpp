#include "../main.h"
#include "font.h"

void CFont::AsciiToGxtChar(const char* ascii, uint16_t* gxt)
{
	return (( void (*)(const char*, uint16_t*))(g_libGTASA+0x532D00+1))(ascii, gxt);
}

void CFont::SetOrientation(int alignment)
{
	((int (*)(char))(g_libGTASA + 0x005339E8 + 1))((char)alignment);
}

void CFont::SetColor(CRGBA* color)
{
	((int (*)(CRGBA*))(g_libGTASA + 0x005336F4 + 1))(color);
}

void CFont::SetColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(SA_ADDR(0x5336F4 + 1)))(dwColor);
}

void CFont::SetEdge(short value)
{
	((int (*)(char))(g_libGTASA + 0x0053394C + 1))((char)value);
}

void CFont::SetBackground(bool enable, bool includeWrap)
{
	((int (*)(char, char))(g_libGTASA + 0x00533988 + 1))(enable, includeWrap);
}

void CFont::SetWrapX(float value)
{
	((int (*)(float))(g_libGTASA + 0x0053381C + 1))(value);
}

void CFont::SetCentreSize(float size)
{
	return ((void(*)(float))(SA_ADDR(0x533834 + 1)))(size);
}

void CFont::SetBackgroundColor(CRGBA* color)
{
	((int (*)(CRGBA*))(g_libGTASA + 0x005339A4 + 1))(color);
}

void CFont::SetBackgroundColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(SA_ADDR(0x5339A4 + 1)))(dwColor);
}

void CFont::SetScale(float w)
{
	((int (*)(float))(g_libGTASA + 0x00533694 + 1))(w);
}

void CFont::SetScale(float x, float y)
{
	*(float*)(SA_ADDR(0x99D750)) = x;
	*(float*)(SA_ADDR(0x99D754)) = y;
}

void CFont::SetFontStyle(short style)
{
	((int (*)(char))(g_libGTASA + 0x00533748 + 1))((char)style);
}

void CFont::SetProportional(bool on)
{
	((int (*)(char))(g_libGTASA + 0x00533970 + 1))((char)on);
}

void CFont::PrintString(float X, float Y, uint16_t* sz)
{
	((int (*)(float, float, uint16_t*))(g_libGTASA + 0x005353B4 + 1))(X, Y, sz);
}

void CFont::PrintString(float posX, float posY, const char* string)
{
	uint16_t* gxt_string = new uint16_t[0xFF];
	CFont::AsciiToGxtChar(string, gxt_string);
	((void (*)(float, float, uint16_t*))(SA_ADDR(0x5353B4 + 1)))(posX, posY, gxt_string);
	delete gxt_string;
	((void (*)())(SA_ADDR(0x53411C + 1)))();//53411C ; _DWORD CFont::RenderFontBuffer(CFont *__hidden this)
}

void CFont::SetRightJustifyWrap(float v)
{
	((int (*)(float))(g_libGTASA + 0x0053384C + 1))(v);
}

void CFont::SetDropShadowPosition(char a)
{
	((int (*)(char))(g_libGTASA + 0x005338DC + 1))(a);
}

void CFont::SetDropColor(CRGBA* color)
{
	((int (*)(CRGBA*))(g_libGTASA + 0x0053387C + 1))(color);
}

void CFont::SetDropColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(SA_ADDR(0x53387C + 1)))(dwColor);
}

void CFont::SetJustify(uint8_t justify)
{
	return ((void(*)(uint8_t))(SA_ADDR(0x5339D0 + 1)))(justify);
}