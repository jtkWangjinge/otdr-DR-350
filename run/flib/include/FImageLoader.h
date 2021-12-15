#ifndef __IMAGELOADER_H__
#define __IMAGELOADER_H__

#include "FIncludes.h"
#include "FImage.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 错误码 */
typedef enum f_img_loader_error_code 
{
	FIMGLOADER_SUCCESS,				//成功
	FIMGLOADER_IOERROR,				//文件IO错误 fopen fread fwrite...
	FIMGLOADER_UNSUPPORTFMT,		//不支持的文件格式
	FIMGLOADER_UNSUPPORTBMP,		//不支持的bmp文件
	FIMGLOADER_UNSUPPORTJPG,		//不支持的jpeg文件
	FIMGLOADER_UNSUPPORTPNG,		//不支持的png文件
	FIMGLOADER_TOOLARGE,			//图像太大
	FIMGLOADER_OUTOFMEMORY,			//内存不够
	FIMGLOADER_UNKOWNERR,			//未知错误
} FImgLoaderErrCode;

struct f_img_loader;

/* 定义load处理接口 */
typedef BOOL (*FImageLoad)(struct f_img_loader*, const char*);

/* 析构函数 */
typedef void (*FImgLoaderDtor)(struct f_img_loader*);

/* 图像加载器定义基础'类' */
typedef struct f_img_loader {
	FImageLoad 			load;		//加载函数
	FImgLoaderDtor 		dtor;		//析构函数
	FImg*				image;		//加载完成的图像数据
	FImgLoaderErrCode 	error;		//错误码
} FImgLoader;


void FImgLoader_ctor(FImgLoader* me, FImageLoad load, FImgLoaderDtor dtor);

void FImgLoader_dtor(FImgLoader* me);


/* 创建一个图像加载器 */
FImgLoader* FImgLoader_create(void);

/* 销毁一个图像加载器 */
void FImgLoader_destroy(FImgLoader* me);

/* 得到加载完成的图像数据 */
FImg* FImgLoader_image(FImgLoader* me);

/* 得到错误信息 */
const char* FImgLoader_errorMessage(FImgLoader* me);

/* 得到错误码 */
FImgLoaderErrCode FImgLoader_errorCode(FImgLoader* me);

#ifdef __cplusplus
}
#endif
#endif //__IMAGELOADER_H__
