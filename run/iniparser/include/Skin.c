#include "Skin.h"
#include "iniparser.h"
#include <string.h>
/**
* 从指定的配置文件（INI格式）加载一套皮肤，但暂时不加载皮肤中的图片（避免浪费内存）
* @param[in]  pszSkinIniPath 指定的配置文件（INI格式）
* @return     HSKIN          加载的皮肤对象，操作失败则返回NULL
*/
HSKIN SkinCreate(const char* pszSkinIniPath)
{
	dictionary *dict = NULL;
	dict = (dictionary *)malloc(sizeof(dictionary));
	dict = iniparser_load(pszSkinIniPath);
	return dict;
}

/**
* 获取皮肤名称，例如“魅蓝幻影”
* @param[in]  hSkin        皮肤对象
* @return     const char*  皮肤名称
*/
const char* SkinGetName(HSKIN hSkin)
{
	dictionary *dict = (dictionary*)hSkin;
	char* name = NULL;
	name = (char*)malloc(sizeof(char));
	if (dict == NULL)
	{
		return NULL;
	}
	else
	{
		name = (char*)iniparser_getstring(dict, "Main:Name", NULL);
		return name;
	}	
}

/**
* 获取皮肤描述，用于向用户展示更多的描述信息，从而帮助用户选择合适的皮肤，目前不使用
* @param[in]  hSkin        皮肤对象
* @return     const char*  皮肤描述
*/
const char* SkinGetDescription(HSKIN hSkin)
{
	dictionary *dict = (dictionary*)hSkin;
	if (dict == NULL)
	{
		return NULL;
	}
	char* decription = NULL;
	decription = (char*)malloc(sizeof(char));
	decription = (char*)iniparser_getstring(dict, "Main:Desc", NULL);
	return decription;
}

/**
* 获取皮肤缩略图，用于向用户展示皮肤效果，从而帮助用户选择合适的皮肤
* @param[in]  hSkin        皮肤对象
* @return     const char*  皮肤缩略图
*/
HPICTURE SkinGetThumbnail(HSKIN hSkin)
{
	dictionary *dict = (dictionary*)hSkin;
	if (dict == NULL)
	{
		return NULL;
	}
	skinset *skin = NULL;
	skin = (skinset *)malloc(sizeof(skinset));
	skin->x = iniparser_getint(dict, "Main:Thumbnail_X", -1);
	skin->y = iniparser_getint(dict, "Main:Thumbnail_Y", -1);
	skin->w = iniparser_getint(dict, "Main:Thumbnail_W", -1);
	skin->h = iniparser_getint(dict, "Main:Thumbnail_H", -1);
	skin->path = (char*)iniparser_getstring(dict, "Main:Thumbnail_Path", NULL);
	return skin;
}

/**
* 根据指定的图片对象名（对应GUI代码中的PICTURE对象名），从皮肤中检索图片
* @param[in]  hSkin        皮肤对象
* @param[in]  pszPictureObjectName 图片对象名（对应GUI代码中的PICTURE对象名）
* @return     HPICTURE     检索得到的图片，已分配内存，如果需要释放可使用GUI库函数直接释放
* @note       注意此函数如果操作失败，则会返回NULL，
*             为了方便管理皮肤，需要创建空图片的时候，需在皮肤包中创建一副合适大小的空图像以避免返回NULL
*/
HPICTURE SkinGetPicture(HSKIN hSkin, const char* pszPictureObjectName)
{
	dictionary *dict = (dictionary*)hSkin;
	if (dict == NULL)
	{
		return NULL;
	}
	skinset *skin = NULL;
	skin = (skinset *)malloc(sizeof(skinset));
	char s1[300] = "Picture:";
	char* s2 = strcat(s1, pszPictureObjectName);
	char temp[300];
	strcpy(temp,s2);
	char* stringName[5] = { "_X", "_Y", "_W", "_H", "_Path" };
	int* intName[4] = { &skin->x, &skin->y, &skin->w, &skin->h};
	int i;
	for (i = 0; i < 5; i++)
	{
		strcat(s2, stringName[i]);
		if (i == 4)
		{
			skin->path = (char*)iniparser_getstring(dict, s2, NULL);
			break;
		}
		*intName[i] = iniparser_getint(dict, s2, -1);
		strcpy(s2,temp);
	}
	return skin;
}
/**
* 根据指定的图片对象名（对应GUI代码中的PICTURE对象名），从皮肤中检索图片
* @param[in]  hSkin        皮肤对象
* @param[in]  pszPictureObjectName 图片对象名（对应GUI代码中的PICTURE对象名）
* @return     HPICTURE     检索得到的图片，已分配内存，如果需要释放可使用GUI库函数直接释放
* @note       注意此函数如果操作失败，则会返回NULL，
*             为了方便管理皮肤，需要创建空图片的时候，需在皮肤包中创建一副合适大小的空图像以避免返回NULL
*/
HPICTURE SkinGetLoopPicture(HSKIN hSkin, const char* pszPictureObjectName)
{
	dictionary *dict = (dictionary*)hSkin;
	if (dict == NULL)
	{
		return NULL;
	}
	skinset *skin = NULL;
	skin = (skinset *)malloc(sizeof(skinset));
	char s1[200] = "LoopPicture:";
	char* s2 = strcat(s1, pszPictureObjectName);
	char temp[200];
	strcpy(temp, s2);
	char* stringName[7] = { "_X", "_Y", "_W", "_H", "_XOffset", "_YOffset", "_Path" };
	int* intName[6] = { &skin->x, &skin->y, &skin->w, &skin->h, &skin->x_offset, &skin->y_offset};
	int i;
	for (i = 0; i < 7; i++)
	{
		strcat(s2, stringName[i]);
		if (i == 6)
		{
			skin->path = (char*)iniparser_getstring(dict, s2, NULL);
			break;
		}
		*intName[i] = iniparser_getint(dict, s2, -1);
		strcpy(s2, temp);
	}
	return skin;
}
/**
* 销毁已经加载的皮肤对象，但是不会销毁皮肤包中已经加载的图片对象
* @param[in]  hSkin        皮肤对象
* @return     void
*/
void SkinDestroy(HSKIN hSkin, HPICTURE hPicture, char* name, char* description)
{
	dictionary *dict = (dictionary*)hSkin;
	skinset *skin = (skinset*)hPicture;
	if (dict != NULL)
	{
		iniparser_freedict(dict);
	}
	if (skin != NULL)
	{
		free(skin);
	}
	if (name != NULL)
	{
		free(name);
	}
	if (description != NULL)
	{
		free(description);
	}	
}
