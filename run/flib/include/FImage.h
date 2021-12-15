#ifndef _FIMAGE_H_
#define _FIMAGE_H_

#include "FIncludes.h"



#ifdef __cplusplus
extern "C" {
#endif

#ifndef f_round_up
#define f_round_up(x, align)      (((x) + (align) - 1) & ~((align) - 1))
#endif

//img颜色深度模式
typedef enum f_image_color_mode
{
	FIMG_RGB565 = 16,			//16位色
	FIMG_RGB888 = 24,			//24位色
	FIMG_ARGB8888 = 32,			//32位色
} FImgColorMode;

//image图片缩放模式
typedef enum f_image_zoom_hint
{
	FIMG_SPEED_PRIORITY = 1,	//速度优先
	FIMG_QUALITY_PRIORITY,		//质量优先
	FIMG_BALANCED,				//均衡模式
} FImgZoomHint;

//私有数据
struct f_image_private;

//image结构定义
typedef struct f_image {
	struct f_image_private* d;
} FImg;

void 	FImg_ctor		(FImg* me, U32 colorMode, U32 w, U32 h);

FImg	FImg_copy		(FImg* me);

void 	FImg_dtor		(FImg* me);

U32 	FImg_width		(FImg* me);

U32 	FImg_height		(FImg* me);

U32 	FImg_depth		(FImg* me);

U8* 	FImg_data		(FImg* me);

void 	FImg_setData	(FImg* me, U8* data);

BOOL 	FImg_isValid	(FImg* me);

BOOL	FImg_toRGB565	(FImg* me);

BOOL	FImg_toRGB888	(FImg* me);

BOOL	FImg_toARGB8888	(FImg* me);

void 	FImg_output		(FImg* me, FILE* stream);

void 	FImg_resize(FImg* me, F32 wScale, F32 hScale, U32 zoomHint);

#ifdef __cplusplus
}
#endif

#endif //_FIMAGE_H_
