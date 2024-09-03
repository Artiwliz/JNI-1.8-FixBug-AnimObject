#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"
#include "materialtext.h"
#include "gui/gui.h"
#include "../gui/UIUtils.h"

extern CGame *pGame;
extern CGUI *pGUI;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_CreateDeviceObjects();
void ImGui_ImplRenderWare_DestroyDeviceObjects();

CMaterialText::CMaterialText()
{
    m_camera = 0;
    m_frame = 0;

    SetUpScene();
}

void CMaterialText::SetUpScene()
{
    m_camera = RwCameraCreate();
    m_frame = RwFrameCreate();
    if(!m_camera || !m_frame) return;

    RwObjectHasFrameSetFrame(m_camera, m_frame);

    RwCameraSetFarClipPlane(m_camera, 300.0f);
    RwCameraSetNearClipPlane(m_camera, 0.01f);

    RwV2d view = { 0.5f, 0.5f };
    RwCameraSetViewWindow(m_camera, view);
    RwCameraSetProjection(m_camera, rwPERSPECTIVE);

    // RpWorldAddCamera
    RpWorld* rpWorld = *(RpWorld**)(g_libGTASA+0x95B060);
    if(rpWorld)
        RpWorldAddCamera(rpWorld, m_camera);
}

RwTexture* CMaterialText::Generate(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char* szText)
{
    // RwRasterCreate
    RwRaster *raster = RwRasterCreate(iSizeX, iSizeY, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
    if(!raster) return 0;

    // RwTextureCreate
    RwTexture *bufferTexture = RwTextureCreate(raster);
    if(!bufferTexture) return 0;
    //proverki
    if(!raster || !bufferTexture) return 0;

    // set camera frame buffer
    m_camera->frameBuffer = raster;

    // CVisibilityPlugins::SetRenderWareCamera
    ((void(*)(RwCamera*))(g_libGTASA + 0x55CFA4 + 1))(m_camera);

    // background color
    int b = (dwBackgroundColor) & 0xFF;
    int g = (dwBackgroundColor >> 8) & 0xFF;
    int r = (dwBackgroundColor >> 16) & 0xFF;
    int a = (dwBackgroundColor >> 24) & 0xFF;
    unsigned int dwBackgroundABGR = (r | (g << 8) | (b << 16) | (a << 24));

    // RwCameraClear
    RwCameraClear(m_camera, (RwRGBA*)&dwBackgroundABGR, 3);

    RwCameraBeginUpdate((RwCamera*)m_camera);

    // DefinedState
    ((void(*) (void))(g_libGTASA + 0x559008 + 1))();

    if(pGUI)
        Render(iSizeX, iSizeY, szFontName, byteFontSize, byteFontBold, dwFontColor, byteAlign, szText);

    RwCameraEndUpdate((RwCamera*)m_camera);
    return bufferTexture;
}

void CMaterialText::Render(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint8_t byteAlign, const char* szText)
{
    // load fonts
    ImFont *pFont = pGUI->GetFont();

    ImGuiIO &io = ImGui::GetIO();

    // make font to bold

    // setup frame
    ImGui_ImplRenderWare_NewFrame();
    ImGui::NewFrame();

    // cp1251 to utf8
    char utf8[2048];
    cp1251_to_utf8(utf8, szText, 2048);

    // text color
    uint8_t b = (dwFontColor) & 0xFF;
    uint8_t g = (dwFontColor >> 8) & 0xFF;
    uint8_t r = (dwFontColor >> 16) & 0xFF;
    uint8_t a = (dwFontColor >> 24) & 0xFF;

    // text align
    ImVec2 _veecPos;
    ImColor lastMaterialTextColor = ImColor(r, g, b, a);
    switch(byteAlign)
    {
        case OBJECT_MATERIAL_TEXT_ALIGN_LEFT:
        {
            // render text with left align
            ImVec2 veecPos = ImVec2(pGUI->ScaleX(0), pGUI->ScaleY(0));

            // TextWithColors aleardy filter \n (newline)
            lastMaterialTextColor = RenderTextAndGetLastColor(pFont, byteFontSize, false, _veecPos, lastMaterialTextColor, utf8);
            break;
        }
        case OBJECT_MATERIAL_TEXT_ALIGN_CENTER:
        {
            bool bIsFirstTextLine = true;

            std::string strText = utf8;
            std::stringstream ssLine(strText);
            std::string tmpLine;
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                if(bIsFirstTextLine)
                {
                    _veecPos.y = (iSizeY - CalcTextSizeWithoutTags(pFont, byteFontSize, tmpLine.c_str()).y) * 0.5;
                    bIsFirstTextLine = false;
                }
                else
                {
                    _veecPos.y -= byteFontSize / 2;
                }
            }

            std::string strTexts = utf8;
            std::stringstream ssLines(strTexts);
            std::string tmpLines;
            while(std::getline(ssLines, tmpLines, '\n'))
            {
                _veecPos.x = (iSizeX - CalcTextSizeWithoutTags(pFont, byteFontSize, tmpLines.c_str()).x) * 0.5;
                lastMaterialTextColor = RenderTextAndGetLastColor(pFont, byteFontSize, false, _veecPos, lastMaterialTextColor, tmpLines.c_str());
                _veecPos.y += byteFontSize;
            }

            break;
        }
        case OBJECT_MATERIAL_TEXT_ALIGN_RIGHT:
        {
            std::string strText = utf8;
            std::stringstream ssLine(strText);
            std::string tmpLine;
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                _veecPos.x = (iSizeX - CalcTextSizeWithoutTags(pFont, byteFontSize, tmpLine.c_str()).x);
                lastMaterialTextColor = RenderTextAndGetLastColor(pFont, byteFontSize, false, _veecPos, lastMaterialTextColor, tmpLine.c_str());
                _veecPos.y += byteFontSize;
            }

            break;
        }
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

    // back font to normal
}
