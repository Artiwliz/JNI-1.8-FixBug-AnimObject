#include "CFontRenderer.h"
#include "../main.h"

#include <cstring>
#include <cmath>

CFontInstance* CFontRenderer::m_pFonts[MAX_FONTS_RENDERER];

void CFontRenderer::Initialise()
{
	for (auto & m_pFont : m_pFonts)
		m_pFont = nullptr;
}

void CFontRenderer::Shutdown()
{
	for (auto & m_pFont : m_pFonts)
	{
		if (m_pFont)
		{
			delete m_pFont;
			m_pFont = nullptr;
		}
	}
}

CFontInstance* CFontRenderer::AddFont(const char* szPath, int iLineHeight)
{
	CFontInstance* pInstance = nullptr;

	for (auto & m_pFont : m_pFonts)
	{
		if (!m_pFont)
		{
			pInstance = new CFontInstance(szPath, iLineHeight);
			if (pInstance)
			{
				if (!pInstance->IsOk())
				{
					Log(OBFUSCATE("Instance is not ok, deleting"));

					delete pInstance;
					pInstance = nullptr;

					Log(OBFUSCATE("Deleted instance"));
					return pInstance;
				}
			}
			m_pFont = pInstance;
			break;
		}
	}
	return pInstance;
}

void CFontRenderer::RenderText(const char* szText, CFontInstance* pFont, unsigned char* bitmap, int bitWidth, int& x, int& y_max)
{
	x = 0;
	y_max = 0;
	for (int i = 0; i < strlen(szText); ++i)
	{
		// how wide is this character
		int ax;
		int lsb;
		stbtt_GetCodepointHMetrics(&(pFont->m_sFontInfo), szText[i], &ax, &lsb);

		// get bounding box for character (may be offset to account for chars that dip above or below the line 
		int c_x1, c_y1, c_x2, c_y2;
		stbtt_GetCodepointBitmapBox(&(pFont->m_sFontInfo), szText[i], pFont->m_fScale, pFont->m_fScale, &c_x1, &c_y1, &c_x2, &c_y2);

		// compute y (different characters have different heights
		int y = pFont->m_iAscent + c_y1;
		if (y >= y_max)
			y_max = y;

		// render character (stride and offset is important here)
		int byteOffset = x + roundf(lsb * pFont->m_fScale) + (y * bitWidth);
		stbtt_MakeCodepointBitmap(&(pFont->m_sFontInfo), bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitWidth, pFont->m_fScale, pFont->m_fScale, szText[i]);

		// advance x
		x += roundf(ax * pFont->m_fScale);

		// add kerning 
		int kern;
		kern = stbtt_GetCodepointKernAdvance(&(pFont->m_sFontInfo), szText[i], szText[i + 1]);
		x += roundf(kern * pFont->m_fScale);
	}
}
