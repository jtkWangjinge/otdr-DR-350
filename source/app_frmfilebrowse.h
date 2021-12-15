/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmfilebrowse.h
* 摘    要：  声明文件浏览器中的公用操作函数
*             
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _APP_FRMFILEBROWSE_H
#define _APP_FRMFILEBROWSE_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件			  			  **
*******************************************************************************/

#include "guiglobal.h"


/*******************************************************************************
**									宏定义 				 					  **
*******************************************************************************/	
#define MULTISELECT_LIMIT 			100000
#define MAXPAGEFILENUM  			8          	// 每页显示的最多的文件数目
#define MAXPATHNAME 				512		//当前绝对路径名字的最大长度

#define FRMBUFFSIZE				800*480*2		


/*******************************************************************************
**								类型定义				 					  **
*******************************************************************************/
typedef struct _multiselect
{
    GUILIST *pMsgQueue;     //消息队列，用于保存GUI消息

    GUIMUTEX Mutex;         //互斥锁，控制同步标志、消息队列及注册信息的访问
} MULTISELECT;

typedef struct _cpuInfo
{
    int num;                //CPU 个数
    int size;               //系统页面的大小
    int pageNum;            //系统中物理页数个数
    int avaliablePageNum;   //系统中可用的页面个数
    int totalSize;          //总共物理内存大小(单位:B)
    int freeSize;           //空闲的物理内存大小(单位:B)
}CPUINFO;
/* 定义文件排序方法名的枚举类型 */
typedef enum _file_sort
{
	NAMEASEC = 0,			// 文件名升序排列
	NAMEDESC = 1,			// 文件名降序排列
	TIMEASEC = 2,			// 文件内容修改时间升序排列
	TIMEDESC = 3			// 文件内容修改时间降序排列
} FILESORT;

//文件类型的操作模式
enum _file_operation_mode
{
    FILE_NORMAL_MODE = 0,   // 正常模式
    FILE_COPY_MODE   = 1,   // 复制模式
	FILE_OPEN_MODE   = 6,   // 打开文件操作类型
	FILE_SAVE_MODE   = 7    // 保存文件操作类型
};

//文件选择模式
typedef enum _file_select_mode
{
    FILE_MULTIPLE_MODE = 0,   // 多选模式
    FILE_SINGLE_MODE   = 1    // 单选模式
}FILE_SELECT_MODE;

/* 定义文件扩展名枚举类型 */
typedef enum _file_type
{
	ANY             = 0,
	SOR             = 1,
	BMP             = 2,
	JPG             = 3,
	SOLA            = 4,
	HTML            = 5,
	TXT             = 6,
	GDM             = 7,
	PDF             = 8,	
	BIN             = 10,	
    CUR             = 9,    //文件类型影响FileTypeToFileString/FileBrowseBtnFileType_Up中的字符串匹配，要该需同步改
    SORANDCUR       = 11,
    #ifndef FORC
    INNO            = 14,
    #endif
	OTHER           = 12,
	NONE            = 13
} FILETYPE;


MULTISELECT* CreateQueue(int iQueueLimit);
int DestroyQueue(MULTISELECT **ppMsgObj);
MULTISELECT* GetCurrQueue(void);
int SetCurrQueue(MULTISELECT *pMsgObj);
int ClearSpecialQueue(const char *pFileName, MULTISELECT *pMsgObj);
int CheckSpecialQueue(const char *pFileName, MULTISELECT *pMsgObj);
int WriteQueue(const char *pFileName, MULTISELECT *pMsgObj);
char* ReadQueue(MULTISELECT *pMsgObj);
int ClearQueue(MULTISELECT *pMsgObj);
int ShowQueue(MULTISELECT *pMsgObj);

int DeleteMultiSelect(MULTISELECT *pMsgObj);
int CheckSameName(MULTISELECT *pMsgObj, const char* dir);
int CopyMultiSelect(MULTISELECT *pMsgObj, const char *pDestDirectory);

/***
  * 功能：
        从指定消息对象中，根据给定的消息项，返回该消息项的上一个消息项
  * 参数：
        1.const char *pCurFileName	:		给定的消息项
        2.char *pLastFileName		:		存贮给定的消息项的上一个消息项
        3.GUIMESSAGE *pMsgObj		:  		指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int LastSpecialQueue(const char *pCurFileName, char *pLastFileName, MULTISELECT *pMsgObj);

/***
  * 功能：
        从指定消息对象中，根据给定的消息项，返回该消息项的下一个消息项
  * 参数：
        1.const char *pCurFileName	:		给定的消息项
        2.char *pLastFileName		:		存贮给定的消息项的下一个消息项
        3.GUIMESSAGE *pMsgObj		:  		指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int NextSpecialQueue(const char *pCurFileName, char *pLastFileName, MULTISELECT *pMsgObj);

/***
  * 功能：
        创建实现在文件浏览器中保存图片的线程
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int CreateSavePicture(void);


/***
  * 功能：
        设置当前的文件排序方法
  * 参数：
        1.FILESORT enSort:			将要设置的排序方法
	  2.unsigned int uiMaxFileNum:      当前页最大显示文件个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurSortMethod(FILESORT enSort, unsigned int uiMaxFileNum);


/***
  * 功能：
        获取当前的文件排序方法
  * 参数：
        无
  * 返回：
        返回当前的文件排序方法
  * 备注：
***/
FILESORT GetCurSortMethod(void);


/***
  * 功能：
        设置当前的文件过滤类型
  * 参数：
        1.FILETYPE enFilter:	将要设置的文件过滤类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurFileType(FILETYPE enFilter);


/***
  * 功能：
        获取当前的文件过滤类型
  * 参数：
        无
  * 返回：
        返回当前的文件过滤类型
  * 备注：
***/
FILETYPE GetCurFileType(void);


/***
  * 功能：
        遍历指定路径的目录下的文件
  * 参数：
        1.const char *pFilePath:      需要遍历的路径
        2.unsigned int uiMaxFileNum:  当前页最大显示文件个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ScanDirectory(const char *pFilePath, unsigned int uiMaxFileNum);


/***
  * 功能：
        返回当前的文件数目
  * 参数：
        无
  * 返回：
        返回当前的文件数目
  * 备注：
***/
int GetCurFileNum(void);


/***
  * 功能：
        返回当前的文件目录结构体数组
  * 参数：
        无
  * 返回：
        返回当前的文件目录结构体数组
  * 备注：
***/
struct dirent ** GetCurFileNamelist(void);


/***
  * 功能：
        获取文件的扩展名
  * 参数：
        1、const char *pFileName:	文件名
  * 返回：
		成功返回扩展名的枚举
  * 备注：
***/
FILETYPE GetFileExtension(const char *pFileName);


/***
  * 功能：
        返回存放每页文件结构的指针数组
  * 参数：
        无
  * 返回：
        返回存放每页文件结构的指针数组
  * 备注：
***/
struct dirent ** GetCurPageFileNamelist(void);


/***
  * 功能：
        返回当前目录下每页能实际显示的项目数
  * 参数：
        无
  * 返回：
        返回当前目录下每页能实际显示的项目数
  * 备注：
***/
int GetActulPageFileNum(void);


/***
  * 功能：
        向前移动一个条目
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int LastItem(int *pSelectItem);


/***
  * 功能：
        向后移动一个条目
  * 参数：
        无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int NextItem(int *pSelectItem);

/***
  * 功能：
        向后翻页
  * 参数：
        1、int *pSelectItem:			当前选中项
	  2、unsigned int uiMaxFileNum:	每页最大显示个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int PageDown1(int *pSelectItem, unsigned int uiMaxFileNum);


/***
  * 功能：
        向前翻页
  * 参数：
        1、int *pSelectItem:			当前选中项
	  2、unsigned int uiMaxFileNum:	每页最大显示个数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int PageUp1(int *pSelectItem, unsigned int uiMaxFileNum);


/***
  * 功能：
        进入图片浏览器之前，遍历当前目录下的图片名字，并存入链表中
  * 参数：
        无
  * 返回：
        无
  * 备注：
  		根据当前选择的文件类型，若是bmp，则只读入bmp图片，
  		若是jpg，则只读入jpg图片，若是*类型，则读入bmp和jpg图片
***/
void FillPictureQueue(void);


/***
  * 功能：
        检测是否挂载SD卡或U盘
  * 参数：
        无
  * 返回：
        0:	不需要重新打开/mnt目录				
        -1:	需要重新打开/mnt目录				
  * 备注：
  		此函数一般放在loop当中，循环检测SD或USB是否插拔；
  		对于插入事件，如果在/mnt目录下，则要重新打开/mnt目录
  		如果不在/mnt目录下，则不需要重新打开/mnt目录；
  		对于拔出事件，则要重新打开/mnt目录
***/
int DetectionSD(void);


/***
  * 功能：
        每次进入文件浏览器时检测SD卡或U盘是否挂载成功
  * 参数：
        无
  * 返回：
		无			
  * 备注：
  		有时SD卡或U盘拔出，却没有删除/mnt目录下的文件夹，所以每次打开目录时，
  		通过分析执行mount命令的结果来判断是否挂载
***/	
int FirstCheckMount(void);


/***
  * 功能：
        每次进入文件浏览器时检测SD卡或U盘是否挂载成功
  * 参数：
        无
  * 返回：
        0:	挂载成功				
        -1:	挂载失败		
  * 备注：
  		有时SD卡或U盘拔出，却没有删除/mnt目录下的文件夹，所以每次打开目录时，
  		通过分析执行mount命令的结果来判断是否挂载
***/	
int FirstCheckMount(void);


/***
  * 功能：
        设置需要截屏的OTDR窗体对象
  * 参数：
        1.unsigned char ucWndtype:   设置需要截屏的窗体
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetSaveOtdrWin(int ucWndtype);


/***
  * 功能：
        设置指定区域内控件与设定的颜色进行透明显示
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SaveFrmBuff(void);


/***
  * 功能：
        获得OTDR界面保存的帧缓冲
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
unsigned short *GetOTDRFrmBuff(void);


/***
  * 功能：
        获得进入过OTDR界面的标志
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		如果用户没有进入过OTDR界面，而此时进入文件浏览器进行截屏操作，
  		则为无效操作，此时提示用户需先进入OTDR界面
***/
unsigned char GetEnterOtdrFlg(void);


/***
  * 功能：
        将文件类型枚举变量转换为相应的字符串
  * 参数：
        1、FILETYPE emFileType	:	文件类型枚举变量	
        2、char *pFileTypeString:	存贮转换后的文件类型字符串		
  * 返回：
		0:		成功
		非0:	失败
  * 备注：
***/
int FileTypeToFileString(FILETYPE emFileType, char *pFileTypeString);


/***
  * 功能：
        将文件类型字符转转换为相应的文件类型枚举变量
  * 参数：
        1、FILETYPE *emFileType	:	文件类型枚举变量	
        2、char *pFileTypeString:	文件类型字符串		
  * 返回：
		0:		成功
		非0:	失败
  * 备注：
***/
int FileStringToFileType(FILETYPE *emFileType, char *pFileTypeString);


/***
  * 功能：
        检测SD卡是否挂载成功
  * 参数：
        无
  * 返回：
        0	挂载成功			
        -1	挂载失败			
  * 备注：
  		通过分析执行mount命令的结果来判断是否挂载
***/
int CheckMountSD(void);


/***
  * 功能：
        检测USB是否挂载成功
  * 参数：
        无
  * 返回：
        0	挂载成功			
        -1	挂载失败			
  * 备注：
  		通过分析执行mount命令的结果来判断是否挂载
***/
int CheckMountUSB(void);


/***
  * 功能：
        检测存储器的占用信息
  * 参数：
        1.char *pDiskPath   :		存储器的挂载路径
        2.int iFormat	    :		存储信息的格式(0:B/B, 1:KB/KB, 2:MB/MB, 3:GB/GB)
  * 返回：
        成功返回存储信息字符串，失败返回NULL			
  * 备注：
  		
***/
char* GetDiskInformation(char* DiskPath, int iFormat);

/***
  * 功能：
        获取CPU相关信息
  * 参数：
        无
  * 返回：
        失败为NULL,成功获得CPU信息
***/
CPUINFO* GetCPUInformation(void);

/***
  * 功能：
        判断内存是否不足
  * 参数：
        1.int fd        :   文件描述符
        2.char* buf     :   将要写入文件的内容
        3.int size      :   写入文件内容的大小
  * 返回：
        -1:代表内存不足，-2:代表写入的文件内容为空, 0:代表内存足够
***/
int CheckRemainSpace(int fd, char* buf, int size);
/***
  * 功能：
        全选专用函数，全部选中所选文件类型
  * 参数：
  		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		
***/
int FullSelectQueue(void);


/***
  * 功能：
        判断是否已经全选了
  * 参数：
  		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		
***/
int IsFullSelect(void);


/***
  * 功能：
        判断是否是顶层目录
  * 参数：
  		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		
***/
int IsUpDir(void);

/***
  * 功能：
        排序
  * 参数：
  		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		
***/
int SortDirectoryFiles(struct dirent** dirents, int count, FILESORT enSort);
/***
  * 功能：
        判断文件是否为目录
  * 参数：
  		char *pPath 文件路径
  * 返回：
        是返回1，不是返回0
  * 备注：
  		
***/
int isDirectory(const char *pPath);

/***
  * 功能：
        判断指定路径是否存在，不存在则创建
  * 参数：
  		char *pPath 文件路径
  * 返回：
        成功返回0，失败返回-1
  * 备注：
  		
***/
int isExist(const char *pPath);

/***
  * 功能：
        从队列中获取文件列表
  * 参数：
        1.MULTISELECT *pMsgObj:     队列指针(传入)
        1.char** selectedList:      文件列表(传入)
  * 返回：
        无
  * 备注：
  		文件列表通过参数传出，传入的文件列表选项个数大于队列中选项个数
***/
void GetFileListFromQueue(MULTISELECT *pMsgObj, char** selectedList);
/***
  * 功能：
        获取当前目录的文件个数
  * 参数:
  * 返回：
        无
  * 备注：
***/
int GetCurrentDirFileNum();

/***
  * 功能：
		获取当前文件位置
  * 参数:
  * 返回：
		无
  * 备注：用于向上翻页
***/
int GetCurFilePos(void);
#endif  
