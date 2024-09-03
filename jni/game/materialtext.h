#pragma once

#include "gui/gui.h"

#define OBJECT_MATERIAL_TEXT_ALIGN_LEFT     0
#define OBJECT_MATERIAL_TEXT_ALIGN_CENTER   1
#define OBJECT_MATERIAL_TEXT_ALIGN_RIGHT    2

class CMaterialText
{
public:
    CMaterialText();

    RwTexture* Generate(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char* szText);
    //RwTexture* GenerateNumberPlate(const char* szPlate) { return Generate(70, 70, "", 40, 0, 0xffff6759, 0x0, OBJECT_MATERIAL_TEXT_ALIGN_CENTER, szPlate); }
private:
    void SetUpScene();
    void Render(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint8_t byteAlign, const char* szText);

    RwCamera*   m_camera;
    RwFrame*    m_frame;
};
