/****************************************Copyright (c)****************************************************
**
**                             INNO Instrument(China) Co.Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Skin.h
** Latest Version:             V1.0.0
** Latest modified Date:       2017/02/15
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2017/02/15
** Descriptions:               BMP皮肤包相关接口
** 
*********************************************************************************************************/
#ifndef __SKIN_H
#define __SKIN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void* HSKIN;
typedef void* HPICTURE;

typedef struct _skinset_ {
	int         x;			/** x in skin */
	int         y;			/** y in skin */
	int         w;			/** w in  skin */
	int         h;			/** h in skin */
	int			x_offset;	/** x_offset in skin*/
	int			y_offset;	/** y_offset in skin*/
	char        *path;		/** path in skin */
} skinset;
/** 
 * 从指定的配置文件（INI格式）加载一套皮肤，但暂时不加载皮肤中的图片（避免浪费内存）
 * @param[in]  pszSkinIniPath 指定的配置文件（INI格式）
 * @return     HSKIN          加载的皮肤对象，操作失败则返回NULL
 */
HSKIN SkinCreate(const char* pszSkinIniPath);

/** 
 * 获取皮肤名称，例如“魅蓝幻影”
 * @param[in]  hSkin        皮肤对象
 * @return     const char*  皮肤名称
 */
const char* SkinGetName(HSKIN hSkin);

/** 
 * 获取皮肤描述，用于向用户展示更多的描述信息，从而帮助用户选择合适的皮肤，目前不使用
 * @param[in]  hSkin        皮肤对象
 * @return     const char*  皮肤描述
 */
const char* SkinGetDescription(HSKIN hSkin);

/** 
 * 获取皮肤缩略图，用于向用户展示皮肤效果，从而帮助用户选择合适的皮肤
 * @param[in]  hSkin        皮肤对象
 * @return     const char*  皮肤缩略图
 */
HPICTURE SkinGetThumbnail(HSKIN hSkin);

/** 
 * 根据指定的图片对象名（对应GUI代码中的PICTURE对象名），从皮肤中检索图片
 * @param[in]  hSkin        皮肤对象
 * @param[in]  pszPictureObjectName 图片对象名（对应GUI代码中的PICTURE对象名）
 * @return     HPICTURE     检索得到的图片，已分配内存，如果需要释放可使用GUI库函数直接释放
 * @note       注意此函数如果操作失败，则会返回NULL，
 *             为了方便管理皮肤，需要创建空图片的时候，需在皮肤包中创建一副合适大小的空图像以避免返回NULL
 */
HPICTURE SkinGetPicture(HSKIN hSkin, const char* pszPictureObjectName);

/**
* 根据指定的图片对象名（对应GUI代码中的PICTURE对象名），从皮肤中检索图片
* @param[in]  hSkin        皮肤对象
* @param[in]  pszPictureObjectName 图片对象名（对应GUI代码中的PICTURE对象名）
* @return     HPICTURE     检索得到的图片，已分配内存，如果需要释放可使用GUI库函数直接释放
* @note       注意此函数如果操作失败，则会返回NULL，
*             为了方便管理皮肤，需要创建空图片的时候，需在皮肤包中创建一副合适大小的空图像以避免返回NULL
*/
HPICTURE SkinGetLoopPicture(HSKIN hSkin, const char* pszPictureObjectName);
/** 
 * 销毁已经加载的皮肤对象，但是不会销毁皮肤包中已经加载的图片对象
 * @param[in]  hSkin        皮肤对象
 * @return     void
 */
void SkinDestroy(HSKIN hSkin, HPICTURE hPicture, char* name, char* description);

#ifdef __cplusplus
};
#endif

#endif                                                                  // #ifndef __SKIN_H

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
