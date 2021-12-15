/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmain.c
* 摘    要：  实现主窗体frmmain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
*
******************************************************************************/
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/i2c.h>  
#include <linux/i2c-dev.h> 
#include <linux/rtc.h>
#include <fcntl.h>
#include <termios.h>

#include "drvoptic.h"
#include "common_opm.h"
#include "crc32.h"

#include "wnd_frmmainboard.h"
#include "wnd_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmkeycheck.h"

/****************************************
* 为实现窗体frmmain而需要引用的其他头文件
****************************************/

#define EDEVBUSY 					1		//设备无响应
#define EPAGEFAULT 					2		//页校验错误
#define PAGESIZE 					128		// Eeprom中每页的字节数
#define I2C_SLAVE 					0x0703
#define I2C_RDWR 					0x0707
#define I2C_EEPROM_WP 				0x0709
#define PAGECOUNT					85		// 当前系统占用Eeprom总的页数
#define EEPROM_PAGE_SIZE			128
#define EEPROMFILE					"/dev/i2c-1"
#define EEPROMADDR					0x50

static F32 fadcValue[20];
static int 	iE2prom = -1;			//定义静态全局变量，保存EEPROM设备文件的设备描述符
static struct i2c_rdwr_ioctl_data EepromData;	//定义静态全局变量，保存IIC通讯单元


typedef struct _page {
    unsigned char data[EEPROM_PAGE_SIZE];
}PAGE;

typedef struct _eeprom						
{
	unsigned int	iIfSelectFlag;					//0=没选中
	unsigned char	ucReserved[120];					//保留区，用于扩展和页对齐
	unsigned int	ulCrc32;
} MAINEEPROM,*PMAINEEPROM;
#define MAINEEPROMVALIDBYTE 124    					

struct mtd_info_user {
	U08 type;
	U32 flags;
	U32 size;	 // Total size of the MTD
	U32 erasesize;
	U32 writesize;
	U32 oobsize;   // Amount of OOB data per block (e.g. 16)
	/* The below two fields are obsolete and broken, do not use them
	 * (TODO: remove at some point) */
	U32 ecctype;
	U32 eccsize;
};

struct mtd_user_input {
	U08 type;
	U32 flags;
	U32 size;	 // Total size of the MTD
	U32 erasesize;
	U32 writesize;
	U32 oobsize;   // Amount of OOB data per block (e.g. 16)
	/* The below two fields are obsolete and broken, do not use them
	 * (TODO: remove at some point) */
	U32 ecctype;
	U32 eccsize;
};

struct mtd_user_output {
	U08 type;
	U32 flags;
	U32 size;	 // Total size of the MTD
	U32 erasesize;
	U32 writesize;
	U32 oobsize;   // Amount of OOB data per block (e.g. 16)
	/* The below two fields are obsolete and broken, do not use them
	 * (TODO: remove at some point) */
	U32 ecctype;
	U32 eccsize;
};

struct erase_info_user {
	U32 start;
	U32 length;
};

struct mtd_oob_buf {
	U32 start;
	U32 length;
	unsigned char *ptr;
};
struct mtd_info_user meminfo;
//nand
#define NANDFILE			"/dev/mtd0"
#define MEMGETINFO			_IOR('M', 1, struct mtd_info_user)
#define MEMERASE			_IOW('M', 2, struct erase_info_user)
#define MEMINPUT			_IOW('M', 3, struct mtd_user_input)
#define MEMOUTPUT			_IOR('M', 4, struct mtd_user_output)
#define MEMSETBADBLOCK		_IOW('M', 12, __kernel_loff_t)

/************************************
* 声明与RTC处理相关的结构及中间层函数;
************************************/
typedef struct rtc_time RTCVAL;     //rtc_time的定义在<linux/rtc.h>中;
/************************
* 窗体frmmain中的字体资源
************************/
//桌面上的字体
static GUIFONT *pMainBoardFntWrong = NULL;     //标题按钮的字体
static GUIFONT *pMainBoardFntRight = NULL;     //标题按钮的字体

//按钮区的字体
static GUICHAR *pMainBoardStrTitle = NULL;
static GUILABEL *pMainBoardLblTitle = NULL;

static GUICHAR *pMainBoardStrDataLine[20];
static GUILABEL *pMainBoardLblDataLine[20];

/************************
* 窗体frmmain中的文本资源
************************/

//桌面上的文本
static GUICHAR *pMainBoardStrF1 = NULL;    //窗体图标1的文本
static GUICHAR *pMainBoardStrF2 = NULL;    //窗体图标2的文本

static GUICHAR *pMainBoardStrF11 = NULL;    //窗体图标1的文本
static GUICHAR *pMainBoardStrF21 = NULL;    //窗体图标2的文本
static GUICHAR *pMainBoardStrF31 = NULL;    //窗体图标3的文本
static GUICHAR *pMainBoardStrF41 = NULL;    //窗体图标4的文本
static GUICHAR *pMainBoardStrF51 = NULL;    //窗体图标5的文本
static GUICHAR *pMainBoardStrF61 = NULL;    //窗体图标6的文本
static GUICHAR *pMainBoardStrF71 = NULL;    //窗体图标7的文本

//信息栏上文本
static GUICHAR *pMainBoardStrInfo = NULL;    //提示信息的文本


/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmMainBoard = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pMainBoardBarStatus = NULL;
static GUIPICTURE *pMainBoardBgDesk = NULL;
static GUIPICTURE *pMainBoardBarInfo = NULL;

//状态栏上的控件

//桌面上的控件
static GUIPICTURE *pMainBoardBtnF1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pMainBoardBtnF2 = NULL;     //窗体图标2图形框

static GUILABEL *pMainBoardLblF1 = NULL;       //窗体图标1标签
static GUILABEL *pMainBoardLblF2 = NULL;       //窗体图标2标签

static GUIPICTURE *pMainBoardBtnF11 = NULL;     //窗体图标1图形框
static GUIPICTURE *pMainBoardBtnF21 = NULL;     //窗体图标2图形框
static GUIPICTURE *pMainBoardBtnF31 = NULL;     //窗体图标3图形框
static GUIPICTURE *pMainBoardBtnF41 = NULL;     //窗体图标4图形框
static GUIPICTURE *pMainBoardBtnF51 = NULL;     //窗体图标5图形框
static GUIPICTURE *pMainBoardBtnF61 = NULL;     //窗体图标6图形框
static GUIPICTURE *pMainBoardBtnF71 = NULL;     //窗体图标7图形框

static GUILABEL *pMainBoardLblF11 = NULL;       //窗体图标1标签
static GUILABEL *pMainBoardLblF21 = NULL;       //窗体图标2标签
static GUILABEL *pMainBoardLblF31 = NULL;       //窗体图标3标签
static GUILABEL *pMainBoardLblF41 = NULL;       //窗体图标4标签
static GUILABEL *pMainBoardLblF51 = NULL;       //窗体图标5标签
static GUILABEL *pMainBoardLblF61 = NULL;       //窗体图标6标签
static GUILABEL *pMainBoardLblF71 = NULL;       //窗体图标7标签

//按钮区的控件
//...
//数据显示区
static GUIPICTURE *pMainBoardPicDataDispaly = NULL;
static GUILABEL *pMainBoardLblDataDispaly = NULL;
static GUICHAR *pMainBoardStrDataDispaly = NULL;    

//信息栏上的控件
static GUILABEL *pMainBoardLblInfo = NULL;       //提示信息标签

static GUIPICTURE *pMainBoardBtnBack = NULL;

/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int MainBoardTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int MainBoardTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int MainBoardWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int MainBoardWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int MainBoardBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainBoardBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int MainBoardBtnF11_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainBoardBtnF21_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int MainBoardBtnF31_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainBoardBtnF41_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainBoardBtnF51_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
                            
static int MainBoardBtnF61_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainBoardBtnF71_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainBoardBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainBoardBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int MainBoardBtnF11_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnF21_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnF31_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnF41_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnF51_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnF61_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnF71_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainBoardBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int MainBoardErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


static int MainBoardFCTShow(void);
static int EepromCheck(void);
static int UsbCheck(void);
static RTCVAL* Rtc_GetTime(int iRtcFd);
static int NandCheck(void);
static int ConnectMCU();

static int E2prom_open(void);
static int E2prom_close(void);
static int E2prom_polling(void);
static int Poll_E2prom(void);
static int E2prom_write_page(unsigned short pagenum, unsigned char  *buff);
static int E2prom_read_page(unsigned short pagenum, unsigned char *buff);
static int E2prom_write_check(unsigned short pagenum, unsigned char  *pBuff);

//enter键响应处理函数
static void KeyEnterCallBack(int iKeyFlag);

/***
  * 功能：
        窗体frmmain的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMainBoardInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	int iLineGap = 20;
	int iLineY = 0;
	int iLineWidth1 = 312;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	
    //得到当前窗体对象
    pFrmMainBoard = (GUIWINDOW *) pWndObj;
	
	for(i = 0; i < 20; i++)
	{
		pMainBoardStrDataLine[i] = NULL;
		pMainBoardLblDataLine[i] = NULL;
	}

    //初始化字体资源
    pMainBoardFntWrong = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);
    pMainBoardFntRight = CreateFont(FNTFILE_STD, 16, 16, 0x00ffffff, 0xFFFFFFFF);
    

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    MainBoardTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pMainBoardBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, 
                                   BmpFileDirectory"bar_status.bmp");
	pMainBoardBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420,
									 BmpFileDirectory "bg_desk.bmp");
	pMainBoardBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30,
									  BmpFileDirectory "bar_info.bmp");
	pMainBoardLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pMainBoardStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblTitle);
	pMainBoardLblInfo = CreateLabel(20, 458, 300, 16, pMainBoardStrInfo);
    //建立状态栏上的控件

    //建立桌面上的控件
    pMainBoardBtnF1 = CreatePicture(424, 35, 80, 53, 
                                 BmpFileDirectory"btn_enable.bmp");//connect MCU
	pMainBoardBtnF2 = CreatePicture(424, 215, 80, 53,
									BmpFileDirectory "btn_enable.bmp"); //clear nandflash

	pMainBoardBtnF11 = CreatePicture(520, 35, 80, 53,
									 BmpFileDirectory "btn_disable.bmp"); //voltage check
	pMainBoardBtnF21 = CreatePicture(520, 95, 80, 53,
									 BmpFileDirectory "btn_enable.bmp"); //eeprom check
	pMainBoardBtnF31 = CreatePicture(520, 155, 80, 53,
									 BmpFileDirectory "btn_enable.bmp"); //usb check
	pMainBoardBtnF41 = CreatePicture(520, 215, 80, 53,
									 BmpFileDirectory "btn_enable.bmp"); //nandflash check
	pMainBoardBtnF51 = CreatePicture(520, 215, 80, 53,
									 BmpFileDirectory "btn_enable.bmp"); //key check
	pMainBoardBtnF61 = CreatePicture(520, 275, 80, 53,
									 BmpFileDirectory "btn_enable.bmp"); //wifi check
	pMainBoardBtnF71 = CreatePicture(520, 275, 80, 53,
									 BmpFileDirectory "btn_enable.bmp"); //rtc check

	pMainBoardLblF1 = CreateLabel(428, 44, 80, 53, pMainBoardStrF1);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF1);
	pMainBoardLblF2 = CreateLabel(428, 224, 80, 53, pMainBoardStrF2);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF2);

	pMainBoardLblF11 = CreateLabel(524, 44, 80, 53, pMainBoardStrF11);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF11);
	pMainBoardLblF21 = CreateLabel(524, 104, 80, 53, pMainBoardStrF21);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF21);
	pMainBoardLblF31 = CreateLabel(524, 164, 80, 53, pMainBoardStrF31);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF31);
	pMainBoardLblF41 = CreateLabel(524, 224, 80, 53, pMainBoardStrF41);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF41);
	pMainBoardLblF51 = CreateLabel(524, 224, 80, 53, pMainBoardStrF51);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF51);
	pMainBoardLblF61 = CreateLabel(524, 284, 80, 53, pMainBoardStrF61);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF61);
	pMainBoardLblF71 = CreateLabel(524, 284, 80, 53, pMainBoardStrF71);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblF71);

	pMainBoardPicDataDispaly = CreatePicture(20, 35, 320, 400, BmpFileDirectory"frm_dataInfo.bmp");
	pMainBoardLblDataDispaly = CreateLabel(20, 230, 320, 16, pMainBoardStrDataDispaly);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainBoardLblDataDispaly);
    //数据框中的数据
    for(i = 0; i < 20; i++)
   	{
   		iLineY = 35 + iLineGap * i;
		pMainBoardLblDataLine[i] = CreateLabel(25, iLineY, iLineWidth1, iLineGap, pMainBoardStrDataLine[i]);
	}

	pMainBoardBtnBack = CreatePicture(520, 454, 60, 25, BmpFileDirectory"btn_back_unpress.bmp");

    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmMainBoard->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmMainBoard, 
                  pFrmMainBoard);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF1, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF2, 
                  pFrmMainBoard);
   
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF11, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF21, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF31, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF41, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF51, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF61, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnF71, 
                  pFrmMainBoard);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainBoardBtnBack, 
                  pFrmMainBoard);

    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmMainBoard->Mutex));    //注意，必须释放锁


    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmMainBoard, 
                    MainBoardWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmMainBoard, 
                    MainBoardWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF1, 
                    MainBoardBtnF1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF2, 
                    MainBoardBtnF2_Down, NULL, 0, pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF11, 
                    MainBoardBtnF11_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF21, 
                    MainBoardBtnF21_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF31, 
                    MainBoardBtnF31_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF41, 
                    MainBoardBtnF41_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF51, 
                    MainBoardBtnF51_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF61, 
                    MainBoardBtnF61_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnF71, 
                    MainBoardBtnF71_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainBoardBtnBack, 
                    MainBoardBtnBack_Down, NULL, 0, pMsg);
	
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF1, 
                    MainBoardBtnF1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF2, 
                    MainBoardBtnF2_Up, NULL, 0, pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF11, 
                    MainBoardBtnF11_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF21, 
                    MainBoardBtnF21_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF31, 
                    MainBoardBtnF31_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF41, 
                    MainBoardBtnF41_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF51, 
                    MainBoardBtnF51_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF61, 
                    MainBoardBtnF61_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnF71, 
                    MainBoardBtnF71_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainBoardBtnBack, 
                    MainBoardBtnBack_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmMainBoard, 
                    MainBoardErrProc_Func, NULL, 0, pMsg);
    //释放消息队列的互斥锁
    MutexUnlock(&(pMsg->Mutex));    //注意，必须释放锁

	SetPictureEnable(0, pMainBoardBtnF2);
	SetLabelEnable(0, pMainBoardLblF2);
	SetPictureEnable(0, pMainBoardBtnF41);
	SetLabelEnable(0, pMainBoardLblF41);

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMainBoardExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmMainBoard = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmMainBoard->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmMainBoard);
    MutexUnlock(&(pFrmMainBoard->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pMainBoardBarStatus);
    DestroyPicture(&pMainBoardBgDesk);
    DestroyPicture(&pMainBoardBarInfo);
    DestroyLabel(&pMainBoardLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyPicture(&pMainBoardBtnF1);
    DestroyPicture(&pMainBoardBtnF2);

    DestroyPicture(&pMainBoardBtnF11);
    DestroyPicture(&pMainBoardBtnF21);
    DestroyPicture(&pMainBoardBtnF31);
    DestroyPicture(&pMainBoardBtnF41);
    DestroyPicture(&pMainBoardBtnF51);
    DestroyPicture(&pMainBoardBtnF61);
    DestroyPicture(&pMainBoardBtnF71);
    DestroyLabel(&pMainBoardLblF1);
    DestroyLabel(&pMainBoardLblF2);
    
    DestroyLabel(&pMainBoardLblF11);
    DestroyLabel(&pMainBoardLblF21);
    DestroyLabel(&pMainBoardLblF31);
    DestroyLabel(&pMainBoardLblF41);
    DestroyLabel(&pMainBoardLblF51);
    DestroyLabel(&pMainBoardLblF61);
    DestroyLabel(&pMainBoardLblF71);
    for(i = 0; i < 10; i++)
   	{
		DestroyLabel(&pMainBoardLblDataLine[i]);
	}
   
    DestroyPicture(&pMainBoardPicDataDispaly);
    DestroyLabel(&pMainBoardLblDataDispaly);
    //销毁信息栏上的控件
    DestroyLabel(&pMainBoardLblInfo);
	
    DestroyPicture(&pMainBoardBtnBack);

    //释放文本资源
    //***************************************************************/
    MainBoardTextRes_Exit(NULL, 0, NULL, 0);

    //释放字体资源
    //***************************************************************/
    //释放按钮区的字体
    //...
	SerialClose();
	
    return iReturn;
}


/***
  * 功能：
        窗体frmmain的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMainBoardPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //得到当前窗体对象
    pFrmMainBoard = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pMainBoardBarStatus);
    DisplayPicture(pMainBoardBgDesk);
    DisplayPicture(pMainBoardBarInfo);
	DisplayLabel(pMainBoardLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayPicture(pMainBoardBtnF1);
    DisplayPicture(pMainBoardBtnF2);
    
    DisplayPicture(pMainBoardBtnF11);
    DisplayPicture(pMainBoardBtnF21);
    DisplayPicture(pMainBoardBtnF31);
    DisplayPicture(pMainBoardBtnF41);
    DisplayPicture(pMainBoardBtnF51);
    //DisplayPicture(pMainBoardBtnF61);
    SetPictureEnable(0, pMainBoardBtnF61);
    DisplayPicture(pMainBoardBtnF71);
    DisplayLabel(pMainBoardLblF1);
    DisplayLabel(pMainBoardLblF2);
    
    DisplayLabel(pMainBoardLblF11);
    DisplayLabel(pMainBoardLblF21);
    DisplayLabel(pMainBoardLblF31);
    DisplayLabel(pMainBoardLblF41);
    DisplayLabel(pMainBoardLblF51);
    //DisplayLabel(pMainBoardLblF61);

    DisplayLabel(pMainBoardLblF71);
    for(i = 0; i < 20; i++)
   	{
		DisplayLabel(pMainBoardLblDataLine[i]);
	}
    //显示按钮区的控件
    //...
    DisplayPicture(pMainBoardPicDataDispaly);
    DisplayLabel(pMainBoardLblDataDispaly);
    //显示信息栏上的控件
    DisplayLabel(pMainBoardLblInfo);
    DisplayPicture(pMainBoardBtnBack);

	GUIPEN *pPen = NULL;
    unsigned int uiColor;
    pPen = GetCurrPen();
    uiColor = pPen->uiPenColor;
	pPen->uiPenColor = 0x00FFFFFF;
	DrawLine(509, 62, 520, 62);
    pPen->uiPenColor = uiColor;
	
	SetPictureEnable(0, pMainBoardBtnF11);

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMainBoardLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendMsg_DisableLoop(pWndObj);
	
    //更新状态栏显示

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMainBoardPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMainBoardResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int MainBoardTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //初始化状态栏上的文本
    pMainBoardStrTitle = TransString("**Mainboard FCT**");
    //初始化桌面上的文本
    pMainBoardStrF1 = TransString("Connect MCU");
    pMainBoardStrF2 = TransString("Clear Nand");

    pMainBoardStrF11 = TransString("Voltage Check");
    pMainBoardStrF21 = TransString("EEPROM Check");
    pMainBoardStrF31 = TransString("USB Check");
    pMainBoardStrF41 = TransString("Nand Check");
    pMainBoardStrF51 = TransString("Key Check");
    pMainBoardStrF61 = TransString("Wifi Check");
    pMainBoardStrF71 = TransString("RTC");
    for(i = 0; i < 20; i++)
   	{
		pMainBoardStrDataLine[i] = TransString("");
	}

	//初始化信息栏上的文本
    pMainBoardStrInfo = TransString("");
	pMainBoardStrDataDispaly = TransString("");
	
    return iReturn;
}


//释放文本资源
static int MainBoardTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //释放状态栏上的文本
    free(pMainBoardStrTitle);

    //释放桌面上的文本
    free(pMainBoardStrF1);
    free(pMainBoardStrF2);

    free(pMainBoardStrF11);
    free(pMainBoardStrF21);
    free(pMainBoardStrF31);
    free(pMainBoardStrF41);
    free(pMainBoardStrF51);
    free(pMainBoardStrF61);
    free(pMainBoardStrF71);
    for(i = 0; i < 20; i++)
   	{
		free(pMainBoardStrDataLine[i]);
	}
    //释放按钮区的文本
    //...
    free(pMainBoardStrDataDispaly);
    //释放信息栏上的文本
    free(pMainBoardStrInfo);

    return iReturn;
}


static int MainBoardWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;

    switch (uiValue)
    {
    case KEYCODE_OTDR:
        break;
    case KEYCODE_BACK:
    case KEYCODE_HOME:
		MainBoardBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    default:
        break;
    }

    return iReturn;
}


static int MainBoardWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	static int iKeyFlag = 0;

    switch (uiValue)
    {
	case KEYCODE_up:
		if (iKeyFlag > 1 && iKeyFlag <= 5)
		{
			iKeyFlag--;
		}
		break;
	case KEYCODE_down:
		if (iKeyFlag >= 1 && iKeyFlag < 5)
		{
			iKeyFlag++;
		}
		break;
	case KEYCODE_left:
		if (iKeyFlag == 1)
		{
			iKeyFlag--;
		}
		break;
	case KEYCODE_right:
		if (iKeyFlag == 0)
		{
			iKeyFlag++;
		}
		break;
	case KEYCODE_enter:
		KeyEnterCallBack(iKeyFlag);
		break;
	case KEYCODE_BACK:
		iKeyFlag = 0;
		MainBoardBtnBack_Up(NULL, 0, NULL, 0);
		break;
	default:
        break;
    }

    return iReturn;
}

static int MainBoardBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF1, pMainBoardLblF1);

    return iReturn;
}


static int MainBoardBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF2, pMainBoardLblF2);

    return iReturn;
}


static int MainBoardBtnF11_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF11, pMainBoardLblF11);

    return iReturn;
}


static int MainBoardBtnF21_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF21, pMainBoardLblF21);

    return iReturn;
}


static int MainBoardBtnF31_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF31, pMainBoardLblF31);

	return iReturn;
}


static int MainBoardBtnF41_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF41, pMainBoardLblF41);

    return iReturn;
}


static int MainBoardBtnF51_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF51, pMainBoardLblF51);
	SerialClose();
    return iReturn;
}


static int MainBoardBtnF61_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF61, pMainBoardLblF61);

    return iReturn;
}


static int MainBoardBtnF71_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pMainBoardBtnF71, pMainBoardLblF71);

    return iReturn;
}



static int MainBoardBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	SetPictureEnable(0, pMainBoardBtnBack);

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	pMainBoardStrDataDispaly = TransString("Please wait ......");
	SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
	SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);
	
	ConnectMCU();

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	pMainBoardStrDataDispaly = TransString("");
	SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
	SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);
	
    ShowBtn_FuncUp(pMainBoardBtnF1, pMainBoardLblF1);

	SetPictureEnable(1, pMainBoardBtnBack);
	DisplayPicture(pMainBoardBtnBack);
	
    return iReturn;
}


static int MainBoardBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;   
   
    ShowBtn_FuncUp(pMainBoardBtnF2, pMainBoardLblF2);

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	pMainBoardStrDataDispaly = TransString("Please wait ......");
	SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
	SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);
	
	system("flash_eraseall /dev/mtd0");
	system("flash_eraseall /dev/mtd1");
	system("flash_eraseall /dev/mtd2");
	system("flash_eraseall /dev/mtd3");
	system("flash_eraseall /dev/mtd4");
	system("flash_eraseall /dev/mtd5");
	system("flash_eraseall /dev/mtd6");
	system("flash_eraseall /dev/mtd7");
	system("flash_eraseall /dev/mtd8");
	system("flash_eraseall /dev/mtd9");
	system("flash_eraseall /dev/mtd10");

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	pMainBoardStrDataDispaly = TransString("Clear Nand Over!");
	SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
	SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);
	
    return iReturn;
}


static int MainBoardBtnF11_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    ShowBtn_FuncUp(pMainBoardBtnF11, pMainBoardLblF11);

	MainBoardFCTShow();
	
    return iReturn;
}



static int MainBoardBtnF21_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    ShowBtn_FuncUp(pMainBoardBtnF21, pMainBoardLblF21);

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	if(EepromCheck() == 0)
   	{

   		pMainBoardStrDataDispaly = TransString("Eeprom Check OK!");
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
   		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
   	}
    else
   	{
   		pMainBoardStrDataDispaly = TransString("Eeprom Check Error!");
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
   		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataDispaly);   
   	}
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);
	
    return iReturn;
}


static int MainBoardBtnF31_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	ShowBtn_FuncUp(pMainBoardBtnF31, pMainBoardLblF31);

	if(pMainBoardStrDataDispaly != NULL)
    {
	    free(pMainBoardStrDataDispaly);
	    pMainBoardStrDataDispaly = NULL;
    }
	    
	if((iReturn = UsbCheck()) == 0)
   	{
		pMainBoardStrDataDispaly = TransString("Usb Check OK!");
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);
	}
	else
	{
		pMainBoardStrDataDispaly = TransString("Usb Check Error!");
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataDispaly);
	}
   	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);

	return iReturn;
}

static int MainBoardBtnF41_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncUp(pMainBoardBtnF41, pMainBoardLblF41);

	iReturn = NandCheck();

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	if(iReturn == 0)
	{

		pMainBoardStrDataDispaly = TransString("NAND OK!");
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
   		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
	}
	else
	{
		if(iReturn == -1)
		{
			pMainBoardStrDataDispaly = TransString("NAND Inexistence!");
		}
		else if(iReturn == -2)
		{
			pMainBoardStrDataDispaly = TransString("NAND ERROR!");
		}
		else if(iReturn == -3)
		{
			pMainBoardStrDataDispaly = TransString("NAND ERROR1!");
		}
		else if(iReturn == -4)
		{
			pMainBoardStrDataDispaly = TransString("NAND ERROR2!");
		}
		else if(iReturn == -5)
		{
			pMainBoardStrDataDispaly = TransString("NAND ERROR3!");
		}
		else if(iReturn == -6)
		{
			pMainBoardStrDataDispaly = TransString("NAND ERROR4!");
		}
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
	   	SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataDispaly);   
	}
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);
	
    return iReturn;
}

static int MainBoardBtnF51_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    ShowBtn_FuncUp(pMainBoardBtnF51, pMainBoardLblF51);
	
    GUIWINDOW *pWnd = NULL;

    //调度frmotdr
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmKeyCheckInit, FrmKeyCheckExit, 
                        FrmKeyCheckPaint, FrmKeyCheckLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainBoard);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd); 
  
    return iReturn;
}


static int MainBoardBtnF61_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    ShowBtn_FuncUp(pMainBoardBtnF61, pMainBoardLblF61);

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	pMainBoardStrDataDispaly = TransString("WiFi Check......");
	SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
	SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);

	MsecSleep(3000);

	if(pMainBoardStrDataDispaly != NULL)

	{
		free(pMainBoardStrDataDispaly);
		pMainBoardStrDataDispaly = NULL;
	}
	
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);
	
    return iReturn;
}


static int MainBoardBtnF71_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
	
    ShowBtn_FuncUp(pMainBoardBtnF71, pMainBoardLblF71);

	int iRtcDevFd = 0;
    RTCVAL *pRtc = NULL;
    char Time[9] = {'x','x',':','x','x',':','x','x',0};
    char Date[11] = {'x','x','x','x','/','x','x','/','x','x',0};
	char buff[50];
    //临时变量定义
	
	iRtcDevFd = open("/dev/rtc0", O_RDWR);
	pRtc = Rtc_GetTime(iRtcDevFd);
	if (NULL == pRtc)
	{
        sprintf((char *)buff, "%s", "RTC ERROR!");
		pMainBoardStrDataDispaly = TransString(buff);
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);	
		DisplayPicture(pMainBoardPicDataDispaly);
		DisplayLabel(pMainBoardLblDataDispaly);
	}
	else
	{
		printf("time %d, %d, %d", pRtc->tm_hour, pRtc->tm_min, pRtc->tm_sec);
        sprintf(Time, "%02d:%02d:%2d", pRtc->tm_hour, pRtc->tm_min, pRtc->tm_sec);
        sprintf(Date, "%4d/%02d/%02d", pRtc->tm_year + 1900, 
                                       pRtc->tm_mon + 1, 
                                       pRtc->tm_mday);
        sprintf((char *)buff, "RTC %s %s", Time, Date);
        pMainBoardStrDataDispaly = TransString(buff);
		SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);	
		DisplayPicture(pMainBoardPicDataDispaly);
		DisplayLabel(pMainBoardLblDataDispaly);
	}

	close(iRtcDevFd);

    return iReturn;
}

static int MainBoardBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
	SerialClose();

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pMainBoardBtnBack);
    DisplayPicture(pMainBoardBtnBack);
	
    return iReturn;
}

static int MainBoardBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
	SerialClose();


	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pMainBoardBtnBack);
    DisplayPicture(pMainBoardBtnBack);
	
    GUIWINDOW *pWnd = NULL; 
	
    pWnd = CreateWindow(0, 0, 320, 240, 
                        FrmMainInit, FrmMainExit, 
                        FrmMainPaint, FrmMainLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainBoard);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

static int MainBoardErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}


static int UsbCheck()
{
	int iReturn = -1;

	pMainBoardStrDataDispaly = TransString("Usb Check ......");
	SetLabelText(pMainBoardStrDataDispaly,pMainBoardLblDataDispaly);
	SetLabelFont(pMainBoardFntRight, pMainBoardLblDataDispaly);   
	DisplayPicture(pMainBoardPicDataDispaly);
	DisplayLabel(pMainBoardLblDataDispaly);

	MsecSleep(1000);

	DIR *dirp;
	int num = 0;

	dirp = opendir("/proc/scsi/usb-storage/");

	while(dirp)
		{
			if(readdir(dirp) != NULL)
				++num;
			else
				break;
		}

	closedir(dirp);

	if(num == 3)
	{
		iReturn = 0;
		return iReturn;
	}
	else
	{
		return iReturn;
	}
}

static int EepromCheck(void)
{
	int iReturn = 0;
	MAINEEPROM pEeprom;
	
	pEeprom.iIfSelectFlag = 13;
	pEeprom.ulCrc32 = crc32c((unsigned char *)&pEeprom, MAINEEPROMVALIDBYTE);

	if(E2prom_open() < 0)
	{
		return -1;
	}
	
	iReturn = E2prom_write_check(20, (unsigned char *)&pEeprom);
	if(iReturn < 0)
	{
		E2prom_close();
		return -2;
	}
	
	pEeprom.iIfSelectFlag = 0;

	if(E2prom_read_page(20, (unsigned char *)&pEeprom) < 0)
	{
		E2prom_close();
		return -3;
	}
	
	if(pEeprom.iIfSelectFlag == 13)
	{
		E2prom_close();
		return 0;
	}
	else
	{
		E2prom_close();
		return -4;
	}
}

/*	主板要测的点的电压对应
 *	单片机的点--------------治具电路图对应的点-----------F216电路图对应的点----TP
 *
 *	ADC_VSYS_TP97--------------VSYS(TP97)----------------LCD_LED+----------TP18
 *	ADC_VSYS_1V8_TP103---------VSYS_1V8(TP103)-----------VSYS_1V8----------TP217
 *	ADC_VSYSIO_3V3_TP109-------VSYSIO_3V3(TP109)---------LCD_AVDD----------TP36
 *  ADC_OPM_PWR_5V_TP113-------OPM_PWR_5V(TP113)---------ARM_CORE_1V0------TP223
 *	ADC_5V_A0------------------+5V_A0--------------------VDDIOM0-----------TP11
 *	ADC_MCU_3V3_TP127----------MCU_3V3(TP127)------------LCD_3V3-----------TP37
 *	ADC_OPM_7V_TP111-----------OPM_+7V(TP111)------ -----VGH---------------TP40
 *	ADC_5V_A1------------------+5V_A1--------------------USBHOST_VBUS------TP27
 *	ADC_3_3V_A_TP14------------3.3_A(TP14)---------------WiFi_3V3----------TP28
 *	ADC_OPM_7V_TP114-----------OPM_-7V(TP114)------------VGL---------------TP39
 *	
 *	总共是10个点
*/
static int MainBoardFCTShow(void)
{
    int ret = 0;	
	char buff[50];
	int i = 0;
	char *flag = NULL;
	int num = 0;
	
	for(i = 0; i < 10; i++)
	{
		fadcValue[i] = 99.99999f;
	}

	if(flag == NULL)
	{
		flag = (char *)malloc(sizeof(char) * 10);
	}

    DisplayPicture(pMainBoardPicDataDispaly);

	MsecSleep(500);

	//-1--ADC_VSYS_TP97--------------VSYS(TP97)----------------LCD_LED+----------TP18
	ret = OPMGetZhijuAdcValue(ADC_VSYS_TP97, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP18", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-2--ADC_VSYS_1V8_TP103---------VSYS_1V8(TP103)-----------VSYS_1V8----------TP217
	ret = OPMGetZhijuAdcValue(ADC_VSYS_1V8_TP103, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP217", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-3--ADC_VSYSIO_3V3_TP109--------VSYSIO_3V3(TP109)----------LCD_AVDD--------TP36
	ret = OPMGetZhijuAdcValue(ADC_VSYSIO_3V3_TP109, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP36", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-4--ADC_OPM_PWR_5V_TP113-------OPM_PWR_5V(TP113)-------ARM_CORE_1V0------TP223
	ret = OPMGetZhijuAdcValue(ADC_OPM_PWR_5V_TP113, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP223", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-5--ADC_5V_A0-------------+5V_A0---------------VDDIOM0------------------TP11
	ret = OPMGetZhijuAdcValue(ADC_5V_A0, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP11", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-6--ADC_MCU_3V3_TP127--------MCU_3V3(TP127)----------LCD_3V3----------TP37
	ret = OPMGetZhijuAdcValue(ADC_MCU_3V3_TP127, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP37", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-7--ADC_OPM_7V_TP111-----------OPM_+7V(TP111)-----------VGH-----------TP40
	ret = OPMGetZhijuAdcValue(ADC_MCU_3V3_TP127, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP40", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-8--ADC_5V_A1------------+5V_A1--------------USBHOST_VBUS------------TP27
	ret = OPMGetZhijuAdcValue(ADC_5V_A1, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP27", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);


	MsecSleep(100);
	num++;
	//-9--ADC_3_3V_A_TP14---------3.3_A(TP14)-----------WiFi_3V3-------------TP28
	ret = OPMGetZhijuAdcValue(ADC_3_3V_A_TP14, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP28", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);

	MsecSleep(100);
	num++;
	//-10--ADC_OPM_7V_TP114----------OPM_-7V(TP114)------------VGL----------TP39
	ret = OPMGetZhijuAdcValue(ADC_OPM_7V_TP114, 0x07, &fadcValue[num], NULL, GET_ZHIJU_ADC_VALUE, COMMAND_LEN);
	if((fadcValue[num] < 1.15) && (fadcValue[num] > 0.85))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pMainBoardFntRight, pMainBoardLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblDataLine[num]);
	}
	sprintf((char *)buff, "%s  %s: %7.5fV", flag, "TP39", fadcValue[num]);
	pMainBoardStrDataLine[num] = TransString(buff);
	UpdateLabelRes(pMainBoardStrDataLine[num], pMainBoardLblDataLine[num], NULL);
	
	free(flag);
	flag = NULL;
	
    return ret;
}

static int NandCheck()
{
	int iReturn = 0;
	
	int ret;
	struct erase_info_user er;
	struct mtd_oob_buf oob;
	unsigned char *oobbuf = NULL;
	unsigned char *buff = NULL;
	unsigned char *buff1 = NULL;
    //临时变量定义
    int iNand = 0;
	iNand = open(NANDFILE, O_RDWR);
	if(iNand < 0)
	{
		iReturn = -1;
	}
	if(!iReturn)
	{	
		if(ioctl(iNand, MEMGETINFO, &meminfo))
		{
			iReturn = -2;
		}
	}
	if(!iReturn)
	{
		er.start = 0;
		er.length = meminfo.erasesize;
		if(ioctl(iNand, MEMERASE, &er))
		{
			iReturn = -3;
		}
	}
	if(!iReturn)
	{
		oob.length = meminfo.oobsize;
		
		oobbuf = malloc((meminfo.erasesize / meminfo.writesize) * meminfo.oobsize);
		buff = malloc((meminfo.erasesize / meminfo.writesize) * meminfo.oobsize);
		buff1 = malloc((meminfo.erasesize / meminfo.writesize) * meminfo.oobsize);
		
		oob.ptr = oobbuf;
		oob.start = 0;

		buff[0] = 'a';
		if ((ret = pwrite(iNand, buff, meminfo.writesize, oob.start)) < 0) 
		{
			iReturn = -4;
		}
		if ((ret = pread(iNand, buff1, meminfo.writesize, oob.start)) < 0) 
		{
			iReturn = -5;
		}
		printf("read = %c, read = %d\n", buff1[0],buff1[0]);
		free(oobbuf);
		oobbuf = NULL;
		if(buff1[0] == 'a')
		{
			iReturn = 0;
		}
		else
		{
			iReturn = -6;
		}
		
		free(buff);
		buff = NULL;
		free(buff1);
		buff1 = NULL;
	}
	close(iNand);
	return iReturn;
}

static int ConnectMCU()
{
	static S32 ret = 0;
    S32 tryTimes = 3;
    U08 SendBuffer[BUFFER_SIZE];
	U08 RecvBuffer[BUFFER_SIZE];
	
    ret = SerialInit();

    if (!ret)
    { 
        SendBuffer[0] = 2;
        SendBuffer[1] = CONNECT;
        SendBuffer[2] = '#';
        while (tryTimes--)
        {
            SerialSendData(SendBuffer, SendBuffer[0]+1);
            ret = SerialRecvData(RecvBuffer, sizeof(RecvBuffer), COMMAND_LEN);
			
            if (ret)
            {
            	printf("--mainboard connect ---ret = %d---\n",ret);
                if(pMainBoardStrInfo != NULL)

				{
					free(pMainBoardStrInfo);
					pMainBoardStrInfo = NULL;
				}
				pMainBoardStrInfo = TransString("Connect MCU Time Out!");
				SetLabelText(pMainBoardStrInfo,pMainBoardLblInfo);
	   			SetLabelFont(pMainBoardFntWrong, pMainBoardLblInfo);
				DisplayPicture(pMainBoardBarInfo);
				DisplayPicture(pMainBoardBtnBack);
				DisplayLabel(pMainBoardLblInfo);
                MsecSleep(50);
                continue;
            }
            else
            {
                if (RecvBuffer[1] == CONNECT)
                {
                    break;
                }
                else
                {
                    MsecSleep(50);
                    continue;
                }
            }
         }
         if (tryTimes >= 0)
         {
         	if(pMainBoardStrInfo != NULL)

			{
				free(pMainBoardStrInfo);
				pMainBoardStrInfo = NULL;
			}
			pMainBoardStrInfo = TransString("Connected Success......Oh yeah!");
			SetLabelText(pMainBoardStrInfo,pMainBoardLblInfo);
   			SetLabelFont(pMainBoardFntRight, pMainBoardLblInfo);
			DisplayPicture(pMainBoardBarInfo);
			DisplayPicture(pMainBoardBtnBack);
			DisplayLabel(pMainBoardLblInfo);

			SetPictureEnable(1, pMainBoardBtnF11);
			SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pMainBoardBtnF11);
			DisplayPicture(pMainBoardBtnF11);
			DisplayLabel(pMainBoardLblF11);
         }
         else
         {   
        	 if (serialFD != -1)
             {
                 SerialClose();
             }
             if(pMainBoardStrInfo != NULL)

			 {
				 free(pMainBoardStrInfo);
				 pMainBoardStrInfo = NULL;
			 }
			 pMainBoardStrInfo = TransString("Connected Failure......oh sorry!");
			 SetLabelText(pMainBoardStrInfo,pMainBoardLblInfo);
   			 SetLabelFont(pMainBoardFntWrong, pMainBoardLblInfo);
			 DisplayPicture(pMainBoardBarInfo);
			 DisplayPicture(pMainBoardBtnBack);
			 DisplayLabel(pMainBoardLblInfo);
         }
    }
    else if(ret == REOPENSERIAL)
    {
        SerialClose();
        if(pMainBoardStrInfo != NULL)

		{
			free(pMainBoardStrInfo);
			pMainBoardStrInfo = NULL;
		}
		pMainBoardStrInfo = TransString("close the serial device!");
		SetLabelText(pMainBoardStrInfo,pMainBoardLblInfo);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblInfo);
		DisplayPicture(pMainBoardBarInfo);
		DisplayPicture(pMainBoardBtnBack);
		DisplayLabel(pMainBoardLblInfo);

		SetPictureBitmap(BmpFileDirectory"btn_disable.bmp", pMainBoardBtnF11);
		DisplayPicture(pMainBoardBtnF11);
		DisplayLabel(pMainBoardLblF11);
		SetPictureEnable(0, pMainBoardBtnF11);
    }
	else
	{
		printf("--mainboard connect ---ret = %d---\n",ret);
        if(pMainBoardStrInfo != NULL)

		{
			free(pMainBoardStrInfo);
			pMainBoardStrInfo = NULL;
		}
		pMainBoardStrInfo = TransString("unknown err!");
		SetLabelText(pMainBoardStrInfo,pMainBoardLblInfo);
		SetLabelFont(pMainBoardFntWrong, pMainBoardLblInfo);
		DisplayPicture(pMainBoardBarInfo);
		DisplayPicture(pMainBoardBtnBack);
		DisplayLabel(pMainBoardLblInfo);
	}

	return ret;
}

/***
  * 功能：
        得到RTC时间
  * 参数：
        1.int iRtcFd:   RTC设备文件描述符
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
***/
static RTCVAL* Rtc_GetTime(int iRtcFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    RTCVAL *pRtc = NULL;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if (iRtcFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //尝试分配内存
        pRtc = (RTCVAL *)malloc(sizeof(RTCVAL));
        if (NULL == pRtc)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //得到RTC时间
        iErr = ioctl(iRtcFd, RTC_RD_TIME, pRtc);
       if (iErr < 0)
        {
            iErr = -3;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pRtc); 
    case -2:
    case -1:
        pRtc = NULL;
    default:
        break;
    }

    return pRtc;
}

/*
**函数说明:
	关闭EEPROM设备，并释放由E2prom_open函数分配的内存空间
**参数说明:
	无
**返回值:
	0	--成功
	-1	--失败
*/
static int E2prom_close(void)
{
	int ret;

	ret = close(iE2prom);
	iE2prom = -1;
	free(EepromData.msgs);
	
	return ret;
}

/*
**函数说明:
	打开EEPROM设备，并分配IIC通讯单元内存
**参数说明:
	无
**返回值:
	成功--返回EEPROM设备的文件描述符
	失败--返回负数
*/
static int E2prom_open(void)
{
	if(iE2prom == -1)
	{
		iE2prom = open(EEPROMFILE, O_RDWR);

		if(iE2prom < 0)
		{
			return -1;
			printf( "FUN:open--can't open eeprom dev\n");
		}
		else
		{
			EepromData.nmsgs = 2;
			EepromData.msgs = (struct i2c_msg*)malloc(EepromData.nmsgs * sizeof(struct i2c_msg));
			if(!EepromData.msgs)
			{
				printf("GuiMemAlloc err\n");
				return -1;
				E2prom_close();
			}
		}
		return 0;
	}
	else
	{
		printf("Eeprom busy..\n");
		return -1;
	}
}
/*
**函数说明:
	EEPROM忙查询,
	EEPROM数据写入操作后，后续所有操作前都要调用该函数进行查询
	直到设备就绪或时间溢出
**参数说明:
	无
**返回值:
	ret < 0 ----设备忙
	其它----就绪
*/
static int E2prom_polling(void)
{
	int ret;
	unsigned char Buff;
	
	//参数检查
	if(iE2prom == -1)
	{
		printf("EEPROM DEV not opened\n");
		return -1;
	}

	EepromData.nmsgs = 1;
	EepromData.msgs[0].len = 1;
	EepromData.msgs[0].addr = EEPROMADDR;
	EepromData.msgs[0].flags = I2C_M_RD;				//read
	EepromData.msgs[0].buf = &Buff;

	//写保护
	ioctl(iE2prom, I2C_SLAVE, EEPROMADDR);
	
	//数据传输
	ret = ioctl(iE2prom, I2C_RDWR, (unsigned long)&EepromData);	
	
	return ret;
}
/*
**函数说明:
	EEPROM忙查询
**参数说明:
	无
**返回值:
	成功返回0
	失败返回-1
*/
static int Poll_E2prom(void)
{
	int ret, retries=20;

	do
	{
		ret = E2prom_polling();
	}while((ret<0) && (retries-->0));
	
	if((ret<0) && (retries<=0))
	{
		printf("Eeprom poll no response, failed\n");
		return -1;
	}

	return 0;
}
/*
**函数说明:
	EEPROM数据页写函数
**参数说明:
	unsigned int pagenum: 待写入页地址号
	void *buff: 数据缓冲区，指向128byte的缓冲区
**返回值:
	<0:		失败
	其它:	成功
*/
static int E2prom_write_page(unsigned short pagenum, unsigned char  *buff)
{
	int ret;
	unsigned char *pBuff;
	
	//参数检查
	if((pagenum >= PAGECOUNT) || (!buff))
	{
		printf("FUN:write page--para err\n");
		return -1;
	}
	if(iE2prom == -1)
	{
		printf("EEPROM DEV not opened\n");
		return -1;
	}

	printf("uspagenum = %d\n", pagenum);
              
	//分配缓冲区
	pBuff = (unsigned char *)malloc(PAGESIZE + 2);
	if(!pBuff)
	{
		printf("malloc err\n");
		return -1;
	}

	//拷贝数据，设置地址
	pBuff[0] = (unsigned char)(pagenum>>1);	//high address
	pBuff[1] = (unsigned char)(pagenum<<7);	//low address
	memcpy(&pBuff[2], buff, PAGESIZE);

	EepromData.nmsgs = 1;
	EepromData.msgs[0].len = PAGESIZE + 2;
	EepromData.msgs[0].addr = EEPROMADDR;
	EepromData.msgs[0].flags = 0;				//write
	EepromData.msgs[0].buf = pBuff;

	//写保护
	ioctl(iE2prom, I2C_SLAVE, EEPROMADDR);
	ret = ioctl(iE2prom, I2C_EEPROM_WP, 0);
	
	//数据传输
	ret = ioctl(iE2prom, I2C_RDWR, (unsigned long)&EepromData);
	if(ret < 0)
	{
		printf("E2prom page write failed\n");
	}
	
	//写保护
	ioctl(iE2prom, I2C_EEPROM_WP, 1);
	
	//释放内存
	free(pBuff);
	return ret;
}
/*
**函数说明:
	EEPROM数据页读函数
**参数说明:
	unsigned int pagenum: 待写入页号
	void * buff: 待写入数据缓冲区，指向128byte的缓冲
**返回值:
	ret < 0: 失败
	其它:成功
*/
static int E2prom_read_page(unsigned short pagenum, unsigned char *buff)
{
	int ret;
	unsigned char *pBuff;
	
	//参数检查
	if((pagenum >= PAGECOUNT) || (!buff))
	{
		printf("FUN:write page--para err\n");
		return -1;
	}
	if(iE2prom == -1)
	{
		printf("EEPROM DEV not opened\n");
		return -1;
	}

	printf("uspagenum = %d\n", pagenum);

	//分配内存，设置参数
	pBuff=(unsigned char*)malloc(2);
	pBuff[0] = (unsigned char)(pagenum>>1);	//high address
	pBuff[1] = (unsigned char)(pagenum<<7);	//low address
	
	EepromData.nmsgs = 2;
	EepromData.msgs[0].len = 2;
	EepromData.msgs[0].addr = EEPROMADDR;
	EepromData.msgs[0].flags = 0;				//write
	EepromData.msgs[0].buf = pBuff;		
	EepromData.msgs[1].len = PAGESIZE;
	EepromData.msgs[1].addr = EEPROMADDR;
	EepromData.msgs[1].flags = I2C_M_RD;		//read
	EepromData.msgs[1].buf = buff;		

	//数据传输
	ret = ioctl(iE2prom, I2C_RDWR, (unsigned long)&EepromData);
	if(ret<0)
	{
		printf("E2prom page read failed\n");
		return -1;
	}

	//释放内存
	free(pBuff);
	return 0;
}

/*
**函数说明:
	EEPROM数据页写检查函数
**参数说明:
	unsigned int pagenum: 待写入页地址号
	void *buff: 数据缓冲区，指向128byte的缓冲区
**返回值:
	成功		---- 	0
	页错误	----	-EPAGEFAULT
	设备忙	----	-EDEVBUSY
*/
static int E2prom_write_check(unsigned short pagenum, unsigned char  *pBuff)
{
	int ret, retries;
	PAGE temp;

	//polling
	if(Poll_E2prom())
	{
		return -EDEVBUSY;
	}
	
	//page wirte
	retries = 10;
	do{
		ret = E2prom_write_page(pagenum, pBuff);
		retries--;
	}while((ret<0) && (retries>0));
	if((ret<0) && (retries<=0)){
		printf("1.write check failed, Write PageNum:%d\n", pagenum);
		return -EDEVBUSY;
	}	

	//polling
	if(Poll_E2prom())
	{
		return -EDEVBUSY;
	}

	//read back
	retries = 10;
	do{
		ret = E2prom_read_page(pagenum, (unsigned char*)&temp);
		retries--;
	}while((ret<0) &&(retries>0));
	if((ret<0) && (retries <= 0)){
		printf("3. write check failed, read back PageNum:%d\n",pagenum);
		return -EDEVBUSY;
	}

	//data check
	ret = memcmp(pBuff, &temp, PAGESIZE);

	if(!ret){
		return 0;
	}else{
		return -EPAGEFAULT;
	}
}

//enter键响应处理函数
static void KeyEnterCallBack(int iKeyFlag)
{
	switch (iKeyFlag)
	{
	case 0://connect mcu
		printf("-----%s-----%d------connect mcu-----\n", __func__, __LINE__);
		MainBoardBtnF1_Down(NULL, 0, NULL, 0);
		MainBoardBtnF1_Up(NULL, 0, NULL, 0);
		break;
	case 1://voltage check
		printf("-----%s-----%d------voltage check-----\n", __func__, __LINE__);
		MainBoardBtnF11_Down(NULL, 0, NULL, 0);
		MainBoardBtnF11_Up(NULL, 0, NULL, 0);
		break;
	case 2://eeprom check
		printf("-----%s-----%d------eeprom check-----\n", __func__, __LINE__);
		MainBoardBtnF21_Down(NULL, 0, NULL, 0);
		MainBoardBtnF21_Up(NULL, 0, NULL, 0);
		break;
	case 3://usb check
		printf("-----%s-----%d------usb check-----\n", __func__, __LINE__);
		MainBoardBtnF31_Down(NULL, 0, NULL, 0);
		MainBoardBtnF31_Up(NULL, 0, NULL, 0);
		break;
	case 4://key check
		printf("-----%s-----%d------key check-----\n", __func__, __LINE__);
		MainBoardBtnF51_Down(NULL, 0, NULL, 0);
		MainBoardBtnF51_Up(NULL, 0, NULL, 0);
		break;
	case 5://RTC check
		printf("-----%s-----%d------RTC check-----\n", __func__, __LINE__);
		MainBoardBtnF71_Down(NULL, 0, NULL, 0);
		MainBoardBtnF71_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}