#include "../../main.h"
#include "../util/armhook.h"
#include "RenderWare.h"

RsGlobalType* RsGlobal;

RwCamera *(*RwCameraBeginUpdate)(RwCamera *camera);
RwCamera *(*RwCameraEndUpdate)(RwCamera *camera);
RwCamera *(*RwCameraShowRaster)(RwCamera *camera, void *pDev, RwUInt32 flags);
RwCamera *(*RwCameraCreate)(void);
RwCamera *(*RwCameraSetFarClipPlane)(RwCamera *camera, float far);
RwCamera *(*RwCameraSetNearClipPlane)(RwCamera *camera, float far);
RwCamera *(*RwCameraSetViewWindow)(RwCamera *camera, RwV2d const& view);
RwCamera *(*RwCameraSetProjection)(RwCamera *, RwCameraProjection);
RwCamera *(*RwCameraClear)(RwCamera *, RwRGBA *, int);

RwFrame* (*RwFrameCreate)(void);
RwFrame* (*RwFrameTranslate)(RwFrame* frame, const RwV3d* v, RwOpCombineType combine);
RwFrame* (*RwFrameRotate)(RwFrame* frame, const RwV3d* axis, RwReal angle, RwOpCombineType combine);
RwFrame* (*RwFrameForAllObjects)(RwFrame* frame, RwObjectCallBack callBack, void* data);

RpMaterial* (*RpMaterialCreate)(void);

RwObject *(*RwObjectHasFrameSetFrame)(void *camera, RwFrame *frame);

RpWorld *(*RpWorldAddCamera)(RpWorld *, RwCamera *);
RpWorld* (*RpWorldAddLight)(RpWorld* world, RpLight* light);
RpWorld* (*RpWorldRemoveLight)(RpWorld* world, RpLight* light);

RpLight* (*RpLightCreate)(RwInt32 type);
RpLight* (*RpLightSetColor)(RpLight* light, const RwRGBAReal* color);

RwRaster *(*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool (*RwRasterDestroy)(RwRaster * raster);
RwRaster *(*RwRasterGetOffset)(RwRaster *raster, RwInt16 *xOffset, RwInt16 *yOffset);
RwInt32 (*RwRasterGetNumLevels)(RwRaster * raster);
RwRaster *(*RwRasterSubRaster)(RwRaster * subRaster, RwRaster * raster, RwRect * rect);
RwRaster *(*RwRasterRenderFast)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster *(*RwRasterRender)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster *(*RwRasterRenderScaled)(RwRaster * raster, RwRect * rect);
RwRaster *(*RwRasterPushContext)(RwRaster * raster);
RwRaster *(*RwRasterPopContext)(void);
RwRaster *(*RwRasterGetCurrentContext)(void);
RwBool (*RwRasterClear)(RwInt32 pixelValue);
RwBool (*RwRasterClearRect)(RwRect * rpRect, RwInt32 pixelValue);
RwRaster *(*RwRasterShowRaster)(RwRaster * raster, void *dev, RwUInt32 flags);
RwUInt8 *(*RwRasterLock)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode);
RwRaster *(*RwRasterUnlock)(RwRaster * raster);
RwUInt8 *(*RwRasterLockPalette)(RwRaster * raster, RwInt32 lockMode);
RwRaster *(*RwRasterUnlockPalette)(RwRaster * raster);
RwImage *(*RwImageCreate)(RwInt32 width, RwInt32 height, RwInt32 depth);
RwBool (*RwImageDestroy)(RwImage * image);
RwImage *(*RwImageAllocatePixels)(RwImage * image);
RwImage *(*RwImageFreePixels)(RwImage * image);
RwImage *(*RwImageCopy)(RwImage * destImage, const RwImage * sourceImage);
RwImage *(*RwImageResize)(RwImage * image, RwInt32 width, RwInt32 height);
RwImage *(*RwImageApplyMask)(RwImage * image, const RwImage * mask);
RwImage *(*RwImageMakeMask)(RwImage * image);
RwImage *(*RwImageReadMaskedImage)(const RwChar * imageName, const RwChar * maskname);
RwImage *(*RwImageRead)(const RwChar * imageName);
RwImage *(*RwImageWrite)(RwImage * image, const RwChar * imageName);
RwImage *(*RwImageSetFromRaster)(RwImage *image, RwRaster *raster);
RwRaster *(*RwRasterSetFromImage)(RwRaster *raster, RwImage *image);
RwRaster *(*RwRasterRead)(const RwChar *filename);
RwRaster *(*RwRasterReadMaskedRaster)(const RwChar *filename, const RwChar *maskname);
RwImage *(*RwImageFindRasterFormat)(RwImage *ipImage, RwInt32 nRasterType, RwInt32 *npWidth, RwInt32 *npHeight, RwInt32 *npDepth, RwInt32 *npFormat);
RwTexture *(*RwTextureCreate)(RwRaster* raster);
RwTexture *(*RwTextureDestroy)(RwTexture *texture);

/* rwlpcore.h */
RwReal (*RwIm2DGetNearScreenZ)(void);
RwReal (*RwIm2DGetFarScreenZ)(void);
RwBool (*RwRenderStateGet)(RwRenderState state, void *value);
RwBool (*RwRenderStateSet)(RwRenderState state, void *value);
RwBool (*RwIm2DRenderLine)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwBool (*RwIm2DRenderTriangle)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
RwBool (*RwIm2DRenderPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
RwBool (*RwIm2DRenderIndexedPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices, RwImVertexIndex *indices, RwInt32 numIndices);

/* rtpng.h */
RwImage *(*RtPNGImageWrite)(RwImage* image, const RwChar* imageName);
RwImage *(*RtPNGImageRead)(const RwChar* imageName);

RpGeometry *(*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);

template <typename Func, typename Addr>
void SetFuncAddr(Func func, Addr addr)
{
	*(void **)(func) = (void*)addr;	
}

void InitRenderWareFunctions()
{
	Log(OBFUSCATE("Initializing RenderWare.."));

	/* skeleton.h */
	RsGlobal = (RsGlobalType *)(SA_ADDR(0x95B068));

	/* rwCore.h */
	SetFuncAddr(&RwCameraBeginUpdate			, SA_ADDR(0x1AD6C8 + 1));
	SetFuncAddr(&RwCameraEndUpdate				, SA_ADDR(0x1AD6B8 + 1));
	SetFuncAddr(&RwCameraShowRaster				, SA_ADDR(0x1AD8C4 + 1));
	SetFuncAddr(&RwCameraSetFarClipPlane		, SA_ADDR(0x1AD710 + 1));
	SetFuncAddr(&RwCameraSetNearClipPlane		, SA_ADDR(0x1AD6F4 + 1));
	SetFuncAddr(&RwCameraSetViewWindow			, SA_ADDR(0x1AD924 + 1));
	SetFuncAddr(&RwCameraSetProjection			, SA_ADDR(0x1AD8DC + 1));
	SetFuncAddr(&RwCameraClear					, SA_ADDR(0x1AD8A0 + 1));
	SetFuncAddr(&RwCameraCreate					, SA_ADDR(0x1ADA1C + 1));

	SetFuncAddr(&RwFrameCreate					, SA_ADDR(0x1AE9E0 + 1));
	SetFuncAddr(&RwFrameTranslate				, SA_ADDR(0x1AED7C + 1));
	SetFuncAddr(&RwFrameRotate					, SA_ADDR(0x1AEDC4 + 1));
	SetFuncAddr(&RwFrameForAllObjects			, SA_ADDR(0x1AEE2C + 1));
	
	SetFuncAddr(&RpWorldAddCamera				, SA_ADDR(0x1EB118 + 1));
	SetFuncAddr(&RpWorldAddLight				, SA_ADDR(0x1EB868 + 1));
	SetFuncAddr(&RpWorldRemoveLight				, SA_ADDR(0x1EB8F0 + 1));

	SetFuncAddr(&RpLightCreate					, SA_ADDR(0x1E3860 + 1));
	SetFuncAddr(&RpLightSetColor				, SA_ADDR(0x1E333C + 1));

	SetFuncAddr(&RpMaterialCreate				, SA_ADDR(0x1E3BCC + 1));

	SetFuncAddr(&RwRasterCreate 				, SA_ADDR(0x1B0778 + 1));
	SetFuncAddr(&RwRasterDestroy 				, SA_ADDR(0x1B059C + 1));
	SetFuncAddr(&RwRasterGetOffset 				, SA_ADDR(0x1B0460 + 1));
	SetFuncAddr(&RwRasterGetNumLevels 			, SA_ADDR(0x1B06B4 + 1));
	SetFuncAddr(&RwRasterSubRaster 				, SA_ADDR(0x1B0724 + 1));
	SetFuncAddr(&RwRasterRenderFast				, SA_ADDR(0x1B0500 + 1));
	SetFuncAddr(&RwRasterRender					, SA_ADDR(0x1B054C + 1));
	SetFuncAddr(&RwRasterRenderScaled			, SA_ADDR(0x1B0440 + 1));
	SetFuncAddr(&RwRasterPushContext			, SA_ADDR(0x1B05E4 + 1));
	SetFuncAddr(&RwRasterPopContext				, SA_ADDR(0x1B0674 + 1));
	SetFuncAddr(&RwRasterGetCurrentContext		, SA_ADDR(0x1B0414 + 1));
	SetFuncAddr(&RwRasterClear					, SA_ADDR(0x1B0498 + 1));
	SetFuncAddr(&RwRasterClearRect				, SA_ADDR(0x1B052C + 1));
	SetFuncAddr(&RwRasterShowRaster				, SA_ADDR(0x1B06F0 + 1));
	SetFuncAddr(&RwRasterLock					, SA_ADDR(0x1B0814 + 1));
	SetFuncAddr(&RwRasterUnlock					, SA_ADDR(0x1B0474 + 1));
	SetFuncAddr(&RwRasterLockPalette			, SA_ADDR(0x1B0648 + 1));
	SetFuncAddr(&RwRasterUnlockPalette			, SA_ADDR(0x1B0578 + 1));
	SetFuncAddr(&RwRasterSetFromImage			, SA_ADDR(0x1B0260 + 1));
	SetFuncAddr(&RwRasterRead					, SA_ADDR(0x1B035C + 1));
	SetFuncAddr(&RwRasterReadMaskedRaster		, SA_ADDR(0x1B03CC + 1));

	SetFuncAddr(&RwImageCreate					, SA_ADDR(0x1AF338 + 1));
	SetFuncAddr(&RwImageDestroy					, SA_ADDR(0x1AF44C + 1));
	SetFuncAddr(&RwImageAllocatePixels			, SA_ADDR(0x1AF38C + 1));
	SetFuncAddr(&RwImageFreePixels				, SA_ADDR(0x1AF420 + 1));
	SetFuncAddr(&RwImageCopy					, SA_ADDR(0x1AFA50 + 1));
	SetFuncAddr(&RwImageResize					, SA_ADDR(0x1AF490 + 1));
	SetFuncAddr(&RwImageApplyMask				, SA_ADDR(0x1AFBB0 + 1));
	SetFuncAddr(&RwImageMakeMask				, SA_ADDR(0x1AF5CC + 1));
	SetFuncAddr(&RwImageReadMaskedImage			, SA_ADDR(0x1AFCF8 + 1));
	SetFuncAddr(&RwImageRead					, SA_ADDR(0x1AF74C + 1));
	SetFuncAddr(&RwImageWrite					, SA_ADDR(0x1AF980 + 1));
	SetFuncAddr(&RwImageSetFromRaster			, SA_ADDR(0x1B023C + 1));
	SetFuncAddr(&RwImageFindRasterFormat		, SA_ADDR(0x1B0284 + 1));
	
	SetFuncAddr(&RwTextureCreate				, SA_ADDR(0x1B1B4C + 1));
	SetFuncAddr(&RwTextureDestroy				, SA_ADDR(0x1B1808 + 1));

	/* rwlpcore.h */
	SetFuncAddr(&RwIm2DGetNearScreenZ			, SA_ADDR(0x1B8038 + 1));
	SetFuncAddr(&RwIm2DGetFarScreenZ			, SA_ADDR(0x1B8054 + 1));
	SetFuncAddr(&RwRenderStateGet				, SA_ADDR(0x1B80A8 + 1));
	SetFuncAddr(&RwRenderStateSet				, SA_ADDR(0x1B8070 + 1));
	SetFuncAddr(&RwIm2DRenderLine				, SA_ADDR(0x1B80C4 + 1));
	SetFuncAddr(&RwIm2DRenderTriangle			, SA_ADDR(0x1B80E0 + 1));
	SetFuncAddr(&RwIm2DRenderPrimitive			, SA_ADDR(0x1B80FC + 1));
	SetFuncAddr(&RwIm2DRenderIndexedPrimitive	, SA_ADDR(0x1B8118 + 1));

	/* rtpng.h */
	SetFuncAddr(&RtPNGImageWrite				, SA_ADDR(0x1D6CEC + 1));
	SetFuncAddr(&RtPNGImageRead					, SA_ADDR(0x1D6F84 + 1));

	SetFuncAddr(&RpGeometryForAllMaterials		, SA_ADDR(0x1E284C + 1));
	SetFuncAddr(&RwObjectHasFrameSetFrame		, SA_ADDR(0x1B2988 + 1));

	// increase render memory buffer
	WriteMemory(SA_ADDR(0x1A7EF2), (uintptr_t)"\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
	WriteMemory(SA_ADDR(0x1A7F34), (uintptr_t)"\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
	WriteMemory(SA_ADDR(0x1A7ECE), (uintptr_t)"\x4F\xF0\x01\x00\x00\x46", 6);
}