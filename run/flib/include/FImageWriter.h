#ifndef _FIMAGEWRITER_H_
#define _FIMAGEWRITER_H_

#include "FIncludes.h"
#include "FImage.h"

#ifdef __cplusplus
extern "C" {
#endif

//要生成图片的格式
typedef enum f_image_writer_formt
{
	FIMG_BMP,
	FIMG_JPEG,
	FIMG_PNG,
} FImgWriterFmt;

//定义生成图片的接口
BOOL FImgWriter_write(const char* fileName, const FImg* image, FImgWriterFmt format);

#ifdef __cplusplus
}
#endif
#endif //_FIMAGEWRITER_H_

