/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmimageviewer.c
* 摘    要：  提供图片浏览的功能。
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：  2020-9-3
*
*/

#include "wnd_frmimageviewer.h"

#include <dirent.h>
#include <limits.h>

#include "app_frmfilebrowse.h"

#include "FImageLoader.h"
#include "wnd_global.h"


/*********************
* 	所使用的宏定义
*********************/
#define IMGVIEWER_START_X			(2)
#define IMGVIEWER_START_Y			(2)
#define IMGVIEWER_DISP_WIDTH		(636)
#define IMGVIEWER_DISP_HEIGHT		(438)


//目录文件列表
typedef struct _dirctory_file_list {
	struct dirent** dirents;				//扫描结果
	UINT32			count;					//文件个数
	INT32			currIndex;				//当前文件索引
	FILESORT 		sortMethd;				//排序方法
	char 			path[PATH_MAX];			//目录路径
} DirFileList;


/*********************
* 	所需要的GUI资源
*********************/
GUIWINDOW *pFrmImageViewer = NULL;			//窗体

GUIPICTURE* pImageViewerWndBg = NULL;		//窗体背景
GUIPICTURE* pImageViewerImgBg = NULL;		//图片背景
GUIPICTURE*	pImageViewerNameBg = NULL;		//文件名背景

GUIPICTURE* pImageViewerEsc = NULL;			//退出按钮
GUIPICTURE* pImageViewerNext = NULL;		//下一张
GUIPICTURE* pImageViewerPrev = NULL;		//上一张
GUILABEL* pImageViewerFileName = NULL;		//文件名
	
GUIPICTURE* pImageViewerDisableAll = NULL;	//屏蔽控件
GUICHAR *pErrNotSupport = NULL;				//Error Text


/*********************
* 	所需要的其他资源
*********************/
static ImageViewerBackFunc createCallerWindow = NULL;	//Escpe时调用该函数创建跳转到的窗体

static char currImageFile[PATH_MAX + NAME_MAX] = {0};	//文件全路径名缓冲区
static FImgLoader *pImageLoader = NULL;					//图像加载器
static DirFileList imgList = {.sortMethd = TIMEDESC};	//目录下所有图片文件列表

/*********************
* GUI相关内部函数声明
*********************/
static int ImageViewerTextRes_Init(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int ImageViewerTextRes_Exit(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);

static int ImageViewerWndKey_Down(void *pInArg, int iInLen, 
                           		  void *pOutArg, int iOutLen);
static int ImageViewerWndKey_Up(void *pInArg, int iInLen, 
                         		void *pOutArg, int iOutLen);

static int ImageViewerEsc_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int ImageViewerEsc_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);

static int ImageViewerNext_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int ImageViewerNext_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);

static int ImageViewerPrev_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int ImageViewerPrev_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);

static void  ImageViewerEscape(void);


/*********************
*功能相关内部函数声明
*********************/
static void ImageViewerEnter(void);
static void ImageViewerExit(void);

static int ImageVeiwerDisplay(FImg *image);
static void ImageVeiwerDisplayFileName(const char* fileName);
static void ImageVeiwerDisplayError(GUICHAR* errText);
static int ImageVeiwerLoadAndDisplay(const char* imgFile);
static void ImageViewerNextImage();
static void ImageViewerPrevImage();

static int isImageFile(const struct dirent *dir);
static void CreateImageFileList(const char *imgFile, DirFileList* result);

/*********************
* 所有的模块函数实现
*********************/
/***
  * 功能：
        外部调用ImageViewer接口函数
  * 参数：
        1.const char* imgFile:    指向图像文件
        2.GUIWINDOW* currWnd :    当前窗体
        3.ImageViewerBackFunc createCaller 退出时用来创建调用者窗体
  * 返回：
        none
  * 备注：
***/
int ImageViewer(const char* imgFile, GUIWINDOW* currWnd, ImageViewerBackFunc createCaller)
{
	if (NULL == imgFile || NULL == createCaller)
		return -1;
	
	createCallerWindow = createCaller;
	strcpy(currImageFile, imgFile);

	GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
				                   FrmImageViewerInit, FrmImageViewerExit, 
				                   FrmImageViewerPaint, FrmImageViewerLoop, 
							       FrmImageViewerPause, FrmImageViewerResume,
				                   NULL);
	 	
	SendWndMsg_WindowExit(currWnd);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);	//进入imageviewer
	return 0;
}

//设置排序方式
void SetImageViewerSort(FILESORT enSort)
{
	imgList.sortMethd = enSort;
}

/***
  * 功能：
        窗体ImageViewer的初始化函数
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmImageViewerInit(void *pWndObj)
{
    int iReturn = 0;
    GUIMESSAGE *pMsg = NULL;

	/* 初始化文本 */
	ImageViewerTextRes_Init(NULL, 0, NULL, 0);
	
	/* 初始化GUI资源 */
	pImageViewerWndBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"imageviewerwndbg.bmp");
	pImageViewerImgBg = CreatePicture(0, 0, WINDOW_WIDTH, 440, BmpFileDirectory"imageviewerimgbg.bmp");
	pImageViewerNameBg = CreatePicture(0, 440, 515, 40, BmpFileDirectory"imgviewernamebg.bmp");
	
	pImageViewerNext = CreatePicture(555, 447, 25, 25, BmpFileDirectory"imageviewernext.bmp");
	pImageViewerPrev = CreatePicture(515, 447, 25, 25, BmpFileDirectory"imageviewerprev.bmp");
	pImageViewerEsc = CreatePicture(595, 447, 25, 25, BmpFileDirectory"imagevieweresc.bmp");

	pImageViewerFileName = CreateLabel(8, 440+12, 400, 16, NULL);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pImageViewerFileName);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pImageViewerFileName);

	/* 初始化图像加载器 */
	pImageLoader = FImgLoader_create();
	
	/* 添加事件响应队列 */
    pFrmImageViewer = (GUIWINDOW *)pWndObj;
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmImageViewer, 
                  pFrmImageViewer);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIWINDOW), pImageViewerNext, 
                  pFrmImageViewer);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIWINDOW), pImageViewerPrev, 
                  pFrmImageViewer);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIWINDOW), pImageViewerEsc, 
                  pFrmImageViewer);

	/* 注册注册事件响应消息 */
	pMsg = GetCurrMessage();
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmImageViewer, 
                    ImageViewerWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmImageViewer, 
                    ImageViewerWndKey_Up, NULL, 0, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pImageViewerNext, 
                	ImageViewerNext_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pImageViewerNext, 
                	ImageViewerNext_Up, NULL, 0, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pImageViewerPrev, 
                	ImageViewerPrev_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pImageViewerPrev, 
                	ImageViewerPrev_Up, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pImageViewerEsc, 
                	ImageViewerEsc_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pImageViewerEsc, 
                	ImageViewerEsc_Up, NULL, 0, pMsg);	

	/* 创建以及初始化使能全部的控件 保护耗时操作 */
	pImageViewerDisableAll = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
	pImageViewerDisableAll->Visible.iCursor = 1;
	pImageViewerDisableAll->Visible.iFocus = 1;
	pImageViewerDisableAll->Visible.iLayer = 10;
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIWINDOW), pImageViewerDisableAll, 
                  pFrmImageViewer);
	SetPictureEnable(0, pImageViewerDisableAll);
	
	/* 执行进入动作 */
	ImageViewerEnter();

    return iReturn;
}


/***
  * 功能：
        窗体ImageViewer的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmImageViewerExit(void *pWndObj)
{
    int iReturn = 0;
    GUIMESSAGE *pMsg = NULL;

    pFrmImageViewer = (GUIWINDOW *)pWndObj;

	/* 清除消息队列 */
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);
    ClearWindowComp(pFrmImageViewer);

	/* 销毁GUI资源 */
	DestroyPicture(&pImageViewerWndBg);
	DestroyPicture(&pImageViewerImgBg);
	DestroyPicture(&pImageViewerNameBg);
	DestroyPicture(&pImageViewerNext);
	DestroyPicture(&pImageViewerPrev);
	DestroyPicture(&pImageViewerEsc);
	DestroyLabel(&pImageViewerFileName);
	
	/* 销毁图片加载器 */
	FImgLoader_destroy(pImageLoader);
	pImageLoader = NULL;

	/* 销毁文本资源 */
	ImageViewerTextRes_Exit(NULL, 0, NULL, 0);

	/* 执行退出动作 */
	ImageViewerExit();
	
    return iReturn;
}


/***
  * 功能：
        窗体ImageViewer的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmImageViewerPaint(void *pWndObj)
{
    int iReturn = 0;

	DisplayPicture(pImageViewerWndBg);
	DisplayPicture(pImageViewerNext);
	DisplayPicture(pImageViewerPrev);
	DisplayPicture(pImageViewerEsc);

	ImageVeiwerLoadAndDisplay(currImageFile);
	SetPowerEnable(2,1);
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
        窗体ImageViewer的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmImageViewerLoop(void *pWndObj)
{
    int iReturn = 0;

    /* 禁止并停止窗体循环 */
    SendWndMsg_LoopDisable(pWndObj);
		
    return iReturn;
}


/***
  * 功能：
        窗体ImageViewer的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmImageViewerPause(void *pWndObj)
{
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体ImageViewer的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmImageViewerResume(void *pWndObj)
{
    int iReturn = 0;

    return iReturn;
}

//初始化文本
static int ImageViewerTextRes_Init(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
	pErrNotSupport = TransString("Unsupported image format !");
	return 0;
}

//释放文本
static int ImageViewerTextRes_Exit(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
	free(pErrNotSupport);
	return 0;
}

//按键响应
static int ImageViewerWndKey_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	switch (uiValue)
    {

	case KEYCODE_ESC:
		ImageViewerEsc_Down(NULL, 0, NULL, 0);
		break;
	case KEYCODE_LEFT:
		ImageViewerPrev_Down(NULL, 0, NULL, 0);
		break;
	case KEYCODE_RIGHT:
		ImageViewerNext_Down(NULL, 0, NULL, 0);
		break;
	case KEYCODE_HOME:
		break;
	default:
		break;
	}

    return iReturn;
}

//按键响应
static int ImageViewerWndKey_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	switch (uiValue)
    {
	case KEYCODE_ESC:
		ImageViewerEsc_Up(NULL, 0, NULL, 0);
		break;
	case KEYCODE_HOME:
    #ifdef SAVE_SCREEN
		ScreenShot();
	#endif
        break;
    case KEYCODE_LEFT:
		ImageViewerPrev_Up(NULL, 0, NULL, 0);
        break;
    case KEYCODE_RIGHT:
		ImageViewerNext_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}

    return iReturn;
}

//退出按钮按下
static int ImageViewerEsc_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    int iReturn = 0;

	SetPictureBitmap(BmpFileDirectory"imageviewerescpress.bmp", pImageViewerEsc);
	DisplayPicture(pImageViewerEsc);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
    return iReturn;
}

//退出按钮弹起
static int ImageViewerEsc_Up(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	
	SetPictureBitmap(BmpFileDirectory"imagevieweresc.bmp", pImageViewerEsc);
	DisplayPicture(pImageViewerEsc);	
	ImageViewerEscape();

	return iReturn;
}

//Next按钮按下
static int ImageViewerNext_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    int iReturn = 0;

	SetPictureBitmap(BmpFileDirectory"imageviewernextpress.bmp", pImageViewerNext);
	DisplayPicture(pImageViewerNext);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
    return iReturn;
}

//Next按钮弹起
static int ImageViewerNext_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iReturn = 0;

	SetPictureBitmap(BmpFileDirectory"imageviewernext.bmp", pImageViewerNext);
	DisplayPicture(pImageViewerNext);	
	ImageViewerNextImage();

	return iReturn;
}

//Prev按钮按下
static int ImageViewerPrev_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	SetPictureBitmap(BmpFileDirectory"imageviewerprevpress.bmp", pImageViewerPrev);
	DisplayPicture(pImageViewerPrev);
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//Prev按钮弹起
static int ImageViewerPrev_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	
	SetPictureBitmap(BmpFileDirectory"imageviewerprev.bmp", pImageViewerPrev);
	DisplayPicture(pImageViewerPrev);
	ImageViewerPrevImage();

	return iReturn;
}

//Esc 退出到调用的窗体
static void ImageViewerEscape(void)
{
	GUIWINDOW* dest;
	createCallerWindow(&dest);
	SendWndMsg_WindowExit(pFrmImageViewer);
	SendSysMsg_ThreadCreate(dest);	//返回调用窗体
}


//进入动作，初始化目录等信息
static void ImageViewerEnter(void)
{
	CreateImageFileList(currImageFile, &imgList);
}

//退出动作，释放资源 清空标志
static void ImageViewerExit(void)
{
	int i;
	//释放资源
	for (i = 0; i < imgList.count; ++i) {
		free(imgList.dirents[i]);
	}
	free(imgList.dirents);
	
	imgList.count = 0;
	imgList.currIndex = -1;
	imgList.dirents = NULL;
	imgList.path[0] = '\0';
	imgList.sortMethd = TIMEDESC;
	
	currImageFile[0] = '\0';
}

//linux dir接口的过滤函数，过滤出所有图片文件
static int isImageFile(const struct dirent *dir)
{
	if (dir->d_type != DT_DIR) {
		const char *fileName = dir->d_name;
		const char *p = strchr(fileName, '.');
		if (p && p != fileName) {
			if (
				(!strcasecmp(p, ".BMP"))
				|| (!strcasecmp(p, ".JPG"))
				) {
				return 1;
			}
		}
	}
	return 0;
}

//打印list信息
void Print_DirFileList(DirFileList* fileList, FILE* stream)
{
	char *methd[] = {"NAMEASEC", "NAMEDESC", "TIMEASEC", "TIMEDESC"};
	
	fprintf(stream, "DirFileList(%p):[\n", fileList);
	fprintf(stream, "  Path(%s)\n", fileList->path);
	fprintf(stream, "  File Count(%d)\n", fileList->count);
	fprintf(stream, "  Current(%d)\n", fileList->currIndex);
	fprintf(stream, "  Dirents(%p)\n", fileList->dirents);
	fprintf(stream, "  SortMethd(%s)\n", methd[fileList->sortMethd]);
	fprintf(stream, "]\n");
}

//得到指定目录下的所有图片文件 填充指定的list
static void CreateImageFileList(const char *imgFile, DirFileList* result)
{
	char fileName[NAME_MAX];

	if (NULL == imgFile || NULL == result) {
		return;
	}

	/* 获取文件目录 */
	strcpy(result->path, imgFile);
	char *p = strrchr(result->path, '/');
	if (p) {
		strcpy(fileName, p + 1);
		*p = '\0';
	}
	else {
		strcpy(fileName, imgFile);
		result->path[0] = '.';
		result->path[1] = '\0';
	}
	
	/* 扫描目录获取所有图片文件 */
	int count = scandir(result->path, &result->dirents, isImageFile, alphasort);
	if (count < 0) {
		result->count = 0;
		result->currIndex = -1;
		result->dirents = NULL;
		return;
	}
	result->count = (UINT32)count;

	/* 排序 */
	SortDirectoryFiles(result->dirents, count, result->sortMethd);
	
	/* 查找当前文件所在的索引 */
	for (
		result->currIndex = 0; 
		result->currIndex < count; 
		++result->currIndex
		) {
		if (!strcmp(fileName, result->dirents[result->currIndex]->d_name)) {
			break;
		}
	}
}

//显示已经加载完成的图片
static int ImageVeiwerDisplay(FImg *image)
{
	UINT32 w = FImg_width(image);
	UINT32 h = FImg_height(image);
	UINT32 depth = FImg_depth(image);

	float xScale = (float)IMGVIEWER_DISP_WIDTH / (float)w;
	float yScale = (float)IMGVIEWER_DISP_HEIGHT / (float)h;

	/* 调整图片适应屏幕 */
	if (xScale < 1.0f || yScale < 1.0f) {
		float scale = xScale < yScale ? xScale : yScale;
		FImg_resize(image, scale, scale, FIMG_BALANCED);
		w = FImg_width(image);
		h = FImg_height(image);
	}
	UINT32 x = (IMGVIEWER_DISP_WIDTH - w) / 2 + IMGVIEWER_START_X;
	UINT32 y = (IMGVIEWER_DISP_HEIGHT - h) / 2+ IMGVIEWER_START_Y;

	/* 创建GUI相关，并显示image */
	GUIIMAGE* guiImage = CreateImage(w, h,  depth);
	if (guiImage) {
		UINT32 size = (round_up(w  * depth, 8) / 8) * h;
		memcpy(guiImage->pImgData, FImg_data(image), size);
		SetImagePlace(x, y, guiImage);
		
		DisplayPicture(pImageViewerImgBg);
		DisplayImage(guiImage);
		
		DestroyImage(&guiImage);
		return 0;
	}

	return -1;
}

//显示图片名字
static void ImageVeiwerDisplayFileName(const char* fileName)
{
    int len = strlen(MntUpDirectory);
	if (0 == strncmp(fileName, MntUpDirectory, len)) {
		fileName += len;
	}

	/* 计算页数 */
	char number[36];
	sprintf(number, "(%d/%d) ", imgList.currIndex+1, imgList.count);
	char* nameString = (char*)malloc(strlen(number) + strlen(fileName) + 1);
	sprintf(nameString, "%s%s", number, fileName);

	GUICHAR* fileNameText = TransString((nameString));
	SetLabelText(fileNameText, pImageViewerFileName);
	DisplayPicture(pImageViewerNameBg);
	DisplayLabel(pImageViewerFileName);

	free(fileNameText);
	free(nameString);
}

//显示错误信息
static void ImageVeiwerDisplayError(GUICHAR* errText)
{
	UINT32 x = (IMGVIEWER_DISP_WIDTH - 200) / 2 + IMGVIEWER_START_X;
	UINT32 y = (IMGVIEWER_DISP_HEIGHT - 16) / 2+ IMGVIEWER_START_Y;
	
	GUILABEL *errorLbl = CreateLabel(x, y, 200, 16, errText);
	
	SetLabelFont(getGlobalFnt(EN_FONT_RED), errorLbl);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, errorLbl);
	DisplayPicture(pImageViewerImgBg);
	DisplayLabel(errorLbl);
	DestroyLabel(&errorLbl);
	free(errText);
}

//加载并显示图片
static int ImageVeiwerLoadAndDisplay(const char* imgFile)
{
	int iErr = 0;
	
	/* 保护耗时操作 */
	SetPictureEnable(1, pImageViewerDisableAll);

	/* 如果加载失败显示错误信息 */
	if ((!pImageLoader->load(pImageLoader, imgFile))) {
		const char* errMsg = FImgLoader_errorMessage(pImageLoader);
		ImageVeiwerDisplayError(TransString((char*)errMsg));
	}
	else if (-1 == ImageVeiwerDisplay(FImgLoader_image(pImageLoader))) {
		ImageVeiwerDisplayError(TransString("Unsupported image format!"));
	}
	ImageVeiwerDisplayFileName(imgFile);
	
	/* 取消保护 */
	SetPictureEnable(0, pImageViewerDisableAll);
	
	return iErr;
}

//下一张图片
static void ImageViewerNextImage()
{
	if ((imgList.currIndex != -1) && (imgList.count > 0)) {
		if (imgList.currIndex < imgList.count - 1) {
			imgList.currIndex++;
			sprintf(currImageFile, "%s/%s", imgList.path, 
				    imgList.dirents[imgList.currIndex]->d_name);
			ImageVeiwerLoadAndDisplay(currImageFile);
		}
	}
	RefreshScreen(__FILE__, __func__, __LINE__);
}

//上一张图片
static void ImageViewerPrevImage()
{
	if ((imgList.currIndex != -1) && (imgList.count > 0)) {
		if (imgList.currIndex > 0) {
			imgList.currIndex--;
			sprintf(currImageFile, "%s/%s", imgList.path, 
				    imgList.dirents[imgList.currIndex]->d_name);
			ImageVeiwerLoadAndDisplay(currImageFile);
		}
	}
	RefreshScreen(__FILE__, __func__, __LINE__);
}
