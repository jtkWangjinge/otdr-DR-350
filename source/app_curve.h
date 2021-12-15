/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_curve.h  
* 摘    要：  定义应用程序曲线显示数据结构与函数
*
* 当前版本：  v1.1.0 (重写数据结构，与GUI的交互使用循环队列)
* 作    者：  
* 完成日期：
*******************************************************************************/


#ifndef _APP_CURVE_H_
#define _APP_CURVE_H_


/**************************************
* 为实现APP_CURVE而需要引用的其他头文件
**************************************/
#include "guibase.h"
#include "guipen.h"
#include "guipicture.h"
#include "guilabel.h"
#include "guidraw.h"
#include "guiglobal.h"
#include "wnd_global.h"
#include "app_global.h"
#include "app_math.h"
#include "app_middle.h"
#include "app_eventsearch.h"
#include <limits.h>

#include "app_algorithm_support.h"

/**************************
* 为实现APP_CURVE而定义的宏
**************************/
#define 	MAX_PIXEL			(800)			//屏幕像素最大宽度

#define 	CURVE_MAX			(2)				//最多支持2个波形
#define 	F_NAME_MAX			(PATH_MAX)		//文件名最大字符长度
#define 	DIV_NUM_X			(6)				//曲线x坐标系格数
#define 	DIV_NUM_Y			(9)				//曲线y坐标系格数
#define RETURNLOSS_MAXVALUE     80.0f			//回波损耗最大值

//全屏绘图区域
#define 	COOR_B_SX 		(45)			//全屏模式左上角x
#define 	COOR_B_SY		(89)			//全屏模式左上角y
#define 	COOR_B_EX		(627)			//全屏模式右下角x
#define 	COOR_B_EY		(351)			//全屏模式右下角y

//非全屏绘图区域
#define 	COOR_S_SX		(45)			//普通模式左上角x
#define 	COOR_S_SY		(89)			//普通模式左上角y
#define 	COOR_S_EX		(627)			//普通模式右下角x
#define 	COOR_S_EY		(281)			//普通模式右下角y

//预览区域
#define 	PRE_COOR_SX		(681)			//预览区左上角x
#define 	PRE_COOR_SY		(381)			//预览区左上角y
#define 	PRE_COOR_EX		(799)			//预览区右下角x
#define 	PRE_COOR_EY		(479)			//预览区右下角y

//当前曲线标识
#define 	CURR_CURVE		(0xffff)		//当前曲线


//颜色
#define 	COLOR_WHITE		(0xffffff)		//白色
#define		COLOR_BLACK		(0x212121)		//黑色
#define 	COLOR_GREEN		(0x00ff00)		//绿色
#define		COLOR_RED		(0xff0000)		//红色
#define		COLOR_BLUE		(0x088094)		//蓝色
#define 	COLOR_GRAY		(0xB4B4B4)		//灰色


/******************************
* 为实现APP_CURVE而定义数据结构
******************************/
//曲线操作方式定义
typedef enum _curve_ctrl_type
{
    EN_CTL_NULL	   = 0,						//无操作
    EN_CTL_INIT	   = 1,						//初识化
    EN_CTL_SWITCH  = 2,						//波形切换
    EN_CTL_NEWDATA = 3,						//新数据
   	EN_CTL_ZOOMIN  = 4,         			//窗口放大
    EN_CTL_ZOOMOUT = 5,         			//窗口缩小
    EN_CTL_RECOVER = 6,         			//恢复1:1比例
    EN_CTL_DOWN	   = 7,						//点击
    EN_CTL_MOVE	   = 8,						//移动
    EN_CTL_SCRATCH = 9,         			//划屏取窗口
    EN_CTL_MT	   = 10,					//双点放大
    EN_CTL_DRAG    = 11,         			//窗口移动
    EN_CTL_ZOOMVAL = 12,					//直接设置放大倍数
    EN_CTL_ENDOFDAQ= 13,                    //采集结束
	EN_CTL_MAGNIFIER= 14,					//显示放大镜图标
} CTRL_CMD;

//附加控制命令
typedef enum _curve_add_cmd
{
    EN_ADD_REFSH   = 1,						//刷新波形
    EN_ADD_ADDTION = 2,						//附加信息
    EN_ADD_MEATIME = 3,						//测量时间
    //EN_ADD_END = 4,                         //采集结束
} ADD_CMD;

//光标移到方式
typedef enum _move_type
{
	EN_MOVE_NULL	= 0,					//无操作
	EN_MOVE_ANTI 	= 1,					//旋钮逆时针
	EN_MOVE_CLOCK	= 2,					//旋钮顺时针
	EN_MOVE_TOUCH	= 3,					//触摸滑动
	EN_MOVE_SETVAL	= 4,					//设置值
} CURSOR_CMD;

//光标移到方式
typedef enum _maker_type
{
	EN_MAKER_NULL	= 0,					//无操作
	EN_MAKER_ANTI 	= 1,					//旋钮逆时针
	EN_MAKER_CLOCK	= 2,					//旋钮顺时针
	EN_MAKER_DOWN	= 3,					//触摸点击
	EN_MAKER_MOVE	= 4,					//触摸滑动
	EN_MAKER_PATCH	= 5,					//链接方式
	EN_MAKER_MEAS	= 6,					//光标测量内容改变
	EN_MAKER_AUTO	= 7,					//自动MARK
} MAKER_CMD;

//标记线链接方式
typedef enum _maker_link
{
	EN_PATCH_a 		= 0,					//a
	EN_PATCH_Aa 	= 1,					//aA
	EN_PATCH_Bb		= 2,					//Bb
	EN_PATCH_b		= 3,					//b
	EN_PATCH_ab		= 4,					//ab
	EN_PATCH_ALL	= 5,					//all
} MAKER_LINK;

//标记线的计算方式
typedef enum _marker_methd
{
	EN_METHD_LSA	= 1,					//4点法
	EN_METHD_TPA	= 2,					//2点法
} MAKER_METHD;

//标记线测量内容,有用作索引，所以从0开始
typedef enum _marker_meas
{
    EN_MEAS_LOSS    = 0,                    //损耗
    EN_MEAS_ATTE    = 1,                    //衰减
    EN_MEAS_REFC    = 2,                    //反射率
    EN_MEAS_ORL     = 3,                    //光回损
} MAKER_MEAS;

//显示对象状态
typedef enum _display_status
{
	EN_STATUS_BUSY 	= 1,					//忙
	EN_STATUS_FREE 	= 2,					//空闲
} DISPLAY_STATUS;

//显示对象大小
typedef enum _display_size
{
	EN_SIZE_LARGE 	= 1,					//全屏
	EN_SIZE_NOMAL 	= 2,					//普通
} DISPLAY_SIZE;

//显示对象截图
typedef enum _display_shot
{
	EN_SHOT_YES  	= 1,					//显示截图
	EN_SHOT_NO   	= 2,					//不显示截图
} DISPLAY_SHOT;

typedef enum _curve_thd_mode
{
    EN_NOT_DRAW_CURVE_MODE = 0,             //计算参数但不显示波形
    EN_NORMAL_MODE  = 1,                    //计算参数并显示波形
    EN_SOLA_MODE    = 2,                    //SLOA不计算参数也不显示波形
} CURVE_THD_MODE;

//光标控制类型
typedef struct _cursor_ctl
{
	CURSOR_CMD 		enType;					//移动类型
	INT32  			iValue;					//附加值
} CURSOR_CTL;

//放大控制值
typedef struct _scale_val
{
	float			fXScale;				//x轴
	float			fYScale;				//y轴
} SCALE_VAL;

//移到控制值
typedef struct _move_val
{
	INT32			iPosX;					//x位置
	INT32			iPosY;					//y位置
} MOVE_VAL;

//曲线控制附加值定义
typedef union _line_ctl_val
{
	SCALE_VAL 		sScaleVal;				//放大
	MOVE_VAL		sMoveVal;				//移动
} CTL_VAL;

//曲线控制消息
typedef struct _line_ctl
{
	CTRL_CMD 		enType;					//控制方式
	CTL_VAL			uValue;					//值
} LINE_CTL;

//附加控制选项消息值
typedef union _add_val
{
	int 			iVaule;					//值
	void 			*pAddition;				//附加数据
} ADD_VAL;

//附加控制选项消息
typedef struct _add_ctl
{
	ADD_CMD			enCmd;					//命令
	ADD_VAL			uValue;					//值
} ADD_CTL;

//标记控制系
typedef struct _maker_ctl
{
	MAKER_CMD 		enType;					//控制方式
	INT32 			iValue;					//值
} MAKER_CTL;

//区段事件控制
typedef struct _event_marker_ctl
{
	CURSOR_CMD 		enType;					//控制方式
	INT32 			iLeftValue;			    //左侧值
	INT32 			iRightValue;		    //右侧值
	INT32           iMaxFlag;               //最大化标志位
	INT32           iTouchMove;             //触摸屏移动标记线
} EVENT_MARKER_CTL;

//附加信息
typedef struct _curve_add
{
	GUIVISIBLE *pLblAddtion;				//附加信息文本的显示
	GUIVISIBLE *pBgAddtion;					//附加信息背景的显示
} CURVE_ADD;

//定义曲线显示坐标结构体
typedef struct _otdr_rect
{
    long 			lSX;      				//左上角横坐标
    long 			lSY;      				//左上角纵坐标
    long 			lEX;     				//右下角横坐标
    long 			lEY;     				//右下角纵坐标
} OTDR_RECT;

//画线所需的数据点对象
typedef struct _data_dot
{
	INT32 			iIndex;					//索引
	INT32 			iValue;					//值
	double			fDist;					//实际距离(m)
	
	INT32			iPosX;					//x 坐标
	INT32			iPosY;					//y 坐标
} DATA_DOT;


//波形图的光标对象
typedef struct _cursor
{
	DATA_DOT 		dDot;					//光标数据信息
		
	UINT32			uiNeedUpdate;			//光标更新标志
} CURSOR;


//参考起始点对象
typedef struct _refrence
{
	UINT32			uiIsVaild;				//参考起始点有效标志
	DATA_DOT 		dDot;					//参考起始点数据

	UINT32			uiNeedUpdate;			//参考起始点更新标志
} REFERENCE;


//标记线对象
typedef struct _marker
{
	MAKER_LINK		enPitchOn;				//标记线的关联方式
    MAKER_MEAS      enMeas;                 //标记线测量的内容，用于控制标记线显示几条
	MAKER_METHD		enMethd;				//损耗测量方法
	MAKER_METHD		enMethdAtten;				//衰减测量方法

	UINT32			uiNum;					//标记线个数 (常量:4)
	DATA_DOT		dDot[4];				//标记线数据
	UINT32			uiColor[4];				//标记线颜色
    UINT32          uiVisible[4];           //标记线是否可见

	float			fAttenAB;				//A-B的衰减
	float           fAtten_aABb;            //aABb的衰减，新增
	float   		fLoss_aABb;				//标记点aA-Bb的回波损耗
	float           fLoss_AB;               //标记点A-B的回波损耗，新增
	float   		fReturnLoss;			//标记点A-B的端到端损耗
	float   		fReflect_AB;			//标记点A-B之间的反射率
    float           fTotalReutrnLoss;       //总回波损耗
    
	UINT32			uiaAUpdated;			//
	UINT32			uiBbUpdated;			//
	SLINE			lLineA;					//
	SLINE			lLineB;					//

	UINT32			uiPosiUpdate;			//标记更新标志
	UINT32			uiLossUpdate;			//损耗等信息更新
} MARKER;

//事件标记线对象
typedef struct _event_marker
{
    DATA_DOT 		dDot[2];		        //事件标记线数据信息
		
	UINT32			uiNeedUpdate;			//事件标记线更新标志
} EVENT_MARKER;


//曲线坐标轴对象
typedef struct _coordinate
{
	UINT32			iXNum;					//x 刻度个数
	double			fMinX;					//x 轴最小值
	double			fDivX;					//x 分度数值
	double			fMaxX;					//x 轴最大值
	
	UINT32			iYNum;					//y 刻度个数
	double			fMinY;					//y	轴最小值
	double			fDivY;					//y 轴分度值
	double			fMaxY;					//y 轴最大值

	UINT32			uiNeedUpdate;			//刻度更新标志
} COORDINATE;


//事件绘制对象(暂未使用)
typedef struct _eventer
{
	UINT32			uiNum;					//需要显示的个数
	INT8 			ppNum[MAX_EVT_NUM][4];	//事件序号
	DATA_DOT		dDot[MAX_EVT_NUM];		//事件数据信息
	
	UINT32 			uiNeedUpdate;			//事件更新标志
} EVENTER;


//预览区对象
typedef struct _preview_trace
{
 	UINT32			uiColor;				//波形的颜色
 	OTDR_RECT		Rect;		        	//波形显示区域
  	
	UINT16			*pPixVal;				//特征值
	UINT32			*pPixIndex;				//索引值
	INT32			iPixValNum;				//特征值数量	

   	UINT16			uiNeedUpdate;       	//更新标志
}PREVIEW_TRACE, *PPREVIEW_TRACE;


//曲线对外提供参数结构
typedef struct _curve_paramter
{
	time_t			ulDTS;					//时间

	UINT32			enTime;					//平均时间
	UINT32			enWave;					//波长
	UINT32 			enRange;				//量程
	UINT32			enPluse;				//脉宽
	
	UINT32			uiAvgTime;				//平均时间
	INT32			iFactor;				//比例因子
	float 			fDistance;				//总长
	float			fSmpRatio;				//采样比率
	float			fSmpIntval;				//采样间隔

	float			fRefractive;			//反射率
	float			fBackScatter;			//背向散射
	float			fLossThr;				//熔接损耗阈值
	float			fReturnThr;				//反射损耗阈值
	float			fEndThr;				//末端阈值
} CURVE_PRARM;


//数据输入信息
typedef struct _curve_data_in
{
	UINT32			uiCnt;					//数据个数
	UINT16 			*pData;					//数据缓冲
} DATA_IN;


//原始数据输出信息
typedef struct _data_out
{
	UINT16			*pValue;				//特征值
	UINT32			*pIndex;				//特征值索引值
	INT32			iValNum;				//特征值数量
	
	UINT32			uiDrawCnt;				//画线长度
	INT32			iLeftOffset;			//画线数据x轴偏移
	INT32			iBotOffset;				//画线数据y轴偏移
	float			fDataPix;				//像素 / 点
	float			fValPix;				//像素 / dB

	UINT32			uiColor;				//画线颜色
	UINT32			uiNeedUpdate;			//更新标志
} DATA_OUT;


/***********************
* 定义OTDR曲线显示的参数
***********************/
typedef struct _curve_info
{
	UINT32			uiIsVaild;				//曲线是否有效
	UINT32			uiAnalysised;			//事件分析标识

	CURVE_PRARM		pParam;					//参数
	char			strFName[F_NAME_MAX];	//文件名

	DATA_IN			dIn;					//数据输入
	DATA_OUT		dOut;					//数据输出
	
	REFERENCE		rReference;				//参考起始点
	CURSOR			cCursor;				//光标
	MARKER			mMarker;				//标记线
	EVENT_MARKER    eEventMarker;           //事件标记线
	COORDINATE		cCoor;					//坐标系
	EVENTS_TABLE	Events;		        	//事件列表信息
	PPREVIEW_TRACE  pPreview;	       		//预览区波形显示
}CURVE_INFO, *PCURVE_INFO;


//显示对象控制参数
typedef struct _display_para
{
	DISPLAY_SIZE enSize;					//显示大小
	DISPLAY_SHOT enShot;					//截图图标
	MAKER_METHD  enMethd;					//损耗测量算法
	MAKER_METHD  enMethdAtten;			    //衰减测量算法
	UINT32		 uiNeedUpdate;				//更新标志
} DISPLAY_PARA;


//显示对象
typedef struct _display_info            
{
	DISPLAY_STATUS	enStatus;				//显示对象状态
	GUIMUTEX		mStatusMutex;			//状态锁

	UINT32			uiDrawCurve;			//是否画曲线
	DISPLAY_PARA 	CtlPara;				//控制参数
	
	GUIPICTURE		*pLargeBg;				//背景1
	GUIPICTURE		*pSmallBg;				//背景2
	GUIPICTURE		*pPreBgShot;			//预览背景1
	GUIPICTURE		*pPreBgNoml;			//预览背景2
	GUIPICTURE		*pCurveID[CURVE_MAX];	//波形标识图标
	GUIPICTURE		*pPicIsPass[CURVE_MAX];	//波形是否通过图标
	GUILABEL		*pLabIsPass[CURVE_MAX];	//波形是否通过文本
	GUILABEL 		*pLCoorX[DIV_NUM_X+1];	//x坐标系文字1
	GUILABEL		*pLCoorY[DIV_NUM_Y+1];	//y坐标系文字1
	GUILABEL 		*pSCoorX[DIV_NUM_X+1];	//x坐标系文字2
	GUILABEL		*pSCoorY[DIV_NUM_Y-1];	//y坐标系文字2

	OTDR_RECT   	LargeRect;		    	//全屏模式
	OTDR_RECT   	SmallRect;		    	//普通模式
	
	OTDR_RECT   	Rect;		    		//波形显示区域
	GUIPICTURE		*pBg;					//曲线背景
	GUIPICTURE		*pPreBg;				//缩略图背景
	GUILABEL		**ppCoorX;				//x轴标签
	GUILABEL		**ppCoorY;				//y轴标签
	GUIPICTURE		*pPicFile;				//文件名
	GUILABEL		*pLblFile;				//文件名
	GUILABEL		*pLblDot;				//省略号
	
	INT32 			iMeasTime;				//平均测量时间
	UINT32			uiReadSor;				//读sor标志
	UINT32          uiEndOfDaq;             //采集完毕，表明OTDR测量过程结束需要发送结束消息到UI
	CURVE_ADD		*pAttach;				//附加信息

	float           xScale;					//横坐标放大比例
	float           yScale;					//纵坐标放大比例
	INT32			xSclBase;				//横轴放大基准
	INT32			ySclBase;				//纵轴放大基准
	INT32          	xShift;             	//横坐标平移值
	INT32          	yShift;             	//纵坐标平移值
	OTDR_RECT   	Scratch;          		//划屏获取的矩形

	float 			fDrawLen;				//画线区域的光纤长度
	INT32           iDrawHeight;        	//当前显示窗口的高度
	
	INT32           iCurveNum;				//曲线数量
	INT32           iCurCurve;				//当前曲线索引
    UINT32          uiIsSaved;              //测量结果是否已经保存
    PCURVE_INFO     pCurve[CURVE_MAX];		//曲线数据

	GUIMUTEX        mMutex;					//显示对象互斥锁
}DISPLAY_INFO, *PDISPLAY_INFO;


//曲线对象的绘制标志
typedef struct _curve_draw_flgs
{
	UINT32			uiBackground;			//曲线背景
	UINT32			uiCoord;				//坐标系
	UINT32			uiCurve;				//曲线
	UINT32			uiCursor;				//光标
	UINT32			uiCursorDist;			//光标距离
	UINT32			uiMarker;				//标记点
	UINT32			uiEventMarker;		    //事件标记点
	UINT32			uiEvent;				//事件点
	UINT32			uiReference;			//参考起始点
	UINT32 			uiScratch;				//选择框
	UINT32			uiPreview;				//预览图
	UINT32			uiFileName;				//文件名
	UINT32			uiIsPass;				//是否通过
	UINT32 			uiMagnifier;			//放大镜图标
	
	GUIMUTEX    	mMutex;					//互斥锁
}CURVE_DRAW_FLGS, *PCURVE_DRAW_FLGS;
//
typedef struct _unit_converter
{
    int iFlag;                              //单位选择标志位
    float fDist;                            //距离
    char cUnit[10];                         //单位名称
}CONVERTER_UNIT, *PCONVERTER_UNIT;
/*********************
* 曲线绘制线程相关接口
*********************/
//初始化显示对象
int InitDisplayInfo(PDISPLAY_INFO pDisplay);

//释放显示对象
void ReleaseDisplayInfo(PDISPLAY_INFO pDisplay);

//曲线画图线程
void *DrawCurveThread(void * pThreadArg);


/*************************
* 曲线元素绘制标志操作函数
*************************/
//设置绘制背景标志
void SetDrawBg(UINT32 uiFlg);

//设置绘制曲线标志
void SetDrawCurveData(UINT32 uiFlg);

//设置绘制光标标志
void SetDrawCursor(UINT32 uiFlg);

//设置光标距离绘制标志
void SetDrawCursorDist(UINT32 uiFlg);

//设置绘制marker标志
void SetDrawMarker(UINT32 uiFlg);

//设置绘制event marker标志
void SetDrawEventMarker(UINT32 uiFlg);

//设置绘制事件标志
void SetDrawEvent(UINT32 uiFlg);

//设置绘制参考起始点标志
void SetDrawReference(UINT32 uiFlg);

//设置绘制拖拽选择框
void SetDrawScratch(UINT32 uiFlg);

//设置绘制预览图
void SetDrawPreview(UINT32 uiFlg);

//设置绘制坐标系
void SetDrawCoord(UINT32 uiFlg);

//设置绘制文件名
void SetDrawFileName(UINT32 uiFlg);

//设置绘制坐标系标志
void SetDrawIsPass(UINT32 uiFlg);

//设置放大镜图标绘制标志
void SetDrawMagnifier(UINT32 uiFlg);

/***************
* 曲线绘相关函数
***************/
//设置是否画线标志
void SetCurveThreadMode(int iFlag);

//得到线程状态
DISPLAY_STATUS  CurrDisplayStatus();

//读取 sor 标识
void SetDisplaySor(PDISPLAY_INFO pDisplay, UINT32 iFlag);

/*****************************
* 曲线绘制线程控制命令发送函数
*****************************/
//光标控制
int SendCursorCtlMsg(CURSOR_CMD enMove, INT32 iValue);

//曲线移动控制
int SendLineCtlMoveMsg(CTRL_CMD enCtlType, INT32 iPosX, INT32 iPosY);

//曲线缩放控制
int SendLineCtlScaleMsg(CTRL_CMD enCtlType, float fXScale, float fYScale);

//标记线控制
int SendMakerCtlMsg(CTRL_CMD enCtlType, INT32 iValue);

//事件标记线控制
int SendEventMakerCtlMsg(CURSOR_CMD enCtlType, INT32 iLeftValue, INT32 iRightValue, INT32 iMaxFlag, INT32 isTouchMove);

//附加消息控制
int SendCurveAdditionCmd(ADD_CMD enCmd, INT32 iValue);

/*************************
* 提供给外部的其他操作函数
*************************/
//当前的曲线是否有效
UINT32 CurveIsVaild(int iPos, PDISPLAY_INFO pDisplay);

//当前的波形是否分析完成
UINT32 CurveIsAnalysised(int iPos, PDISPLAY_INFO pDisplay);

//获得当前曲线个数
int GetCurveNum(PDISPLAY_INFO pDisplay);

//设置当前曲线的指针
int SetCurrCurvePos(int iPos, PDISPLAY_INFO pDisplay);

//设置当前曲线的指针
int GetCurrCurvePos(PDISPLAY_INFO pDisplay);

//切换曲线
int NextCurve(PDISPLAY_INFO pDisplay);

//获得曲线参数
int GetCurvePara(int iPos, PDISPLAY_INFO pDisplay, CURVE_PRARM *pPara);

//设置曲线参数
int SetCurvePara(int iPos, PDISPLAY_INFO pDisplay, CURVE_PRARM *pPara);

//获得当前波形的数据
int GetCurveData(int iPos, PDISPLAY_INFO pDisplay, DATA_IN *pIn);

//设置当前波形的数据
int SetCurveData(int iPos, PDISPLAY_INFO pDisplay, DATA_IN *pIn);

//设置文件名
int SetCurveFile(int iPos, PDISPLAY_INFO pDisplay, char *strFile);

//获得文件名
int GetCurveFile(int iPos, PDISPLAY_INFO pDisplay, char *strFile);

//获得曲线名
int GetCurveName(int iPos, PDISPLAY_INFO pDisplay, char *strFile);

//增加一条曲线
int DisplayAddCurve(PDISPLAY_INFO pDisplay, CURVE_PRARM *pPara, DATA_IN *pIn);

//设置参考起始点
void SetReference(INT32 iPos, PDISPLAY_INFO pDisplay, INT32 iIndex);

//清除参考起始点
void ClearReference(INT32 iPos, PDISPLAY_INFO pDisplay);

//参考起始点是否有效
UINT32 ReferenceIsVaild(INT32 iPos, PDISPLAY_INFO pDisplay);

//得到光标
int GetCursor(int iPos, PDISPLAY_INFO pDisplay);

//获取事件标记
int GetEventMarker(int iPos, PDISPLAY_INFO pDisplay);

//获取标记
int GetCurveMarker(int iPos, PDISPLAY_INFO pDisplay, MARKER *pMarker);

//判断点击是否在标记线上
int TouchOnMaker(int iPos, PDISPLAY_INFO pDisplay, INT32 iTchX);

//事件操作函数
int GetCurveEvents(int iPos, PDISPLAY_INFO pDisplay, EVENTS_TABLE *pEvents);

//回写事件列表
int SetCurveEvents(int iPos, PDISPLAY_INFO pDisplay, EVENTS_TABLE *pEvents);

//判断点击是否在事件上
int TouchOnEvent(int iPos, PDISPLAY_INFO pDisplay, int iTchX);

//设置显示对象控制参数
int SetDisplayCtlPara(PDISPLAY_INFO pDisplay, DISPLAY_PARA *pPara);

//得到显示对象控制参数
int GetDisplayCtlPara(PDISPLAY_INFO pDisplay, DISPLAY_PARA *pPara);

//设置otdr坐标系为普通模式
int SetCoorNomal(PDISPLAY_INFO pDisplay);

//设置otdr坐标系为伸展模式
int SetCoorLarge(PDISPLAY_INFO pDisplay);

int SetSavedFlag(PDISPLAY_INFO pDisplay, UINT32 saved);
UINT32 IsCurveSaved(PDISPLAY_INFO pDisplay);

//储存中清除指定的波形
int ClearSaveOtdrCurve(int iCurvePos);

//清除所有的波形
int ClearAllCurve(void);

//设置参考起始点
int SetStartRef(float *pInSignal, int iSigLen, int iBlind, int iDataIndex,
				EVENTS_TABLE *pEvtTab, PDISPLAY_INFO pDisplay);

//清除参考起始点
int ClearStartRef(float *pInSignal, int iSigLen, EVENTS_TABLE *pEvtTab,	
				  PDISPLAY_INFO pDisplay);
//转换距离单位
float UnitConverter(int iFlag, float fDist);
void UnitConverterName(int iFlag, float fEnd, char* cBuf);
//PCONVERTER_UNIT UnitConverter(PCONVERTER_UNIT pConverter_unit);
/*************
* 算法相关函数
*************/
//根据数据点数计算真实距离
inline float CalcRealDist(int iInputValue, float fSmpIntval);
//计算回波损耗值
float ReturnLoss(UINT16 *pData, INT32 m, INT32 n, float fRLCoef, float fSamRatio);

//更新通过/不通过标志
void changeIsPassUI(int isPass, int CurveIndex);

//获得当前曲线测量时间 成功返回0. 失败返回非0
int getCurveMeasureDate(char *date, char *time, int dateFormat);

//显示激光器发射中
int DrawLaserIsOn(int isChange);

//提取数据的特征值(提供给pdf报告使用，重新绘制曲线)
int GetEigenValue(UINT16 *pDest, UINT32 *pDestIndex, UINT32 iDestLen,
						 UINT16 *pSrc, UINT32 iSrcLen, UINT16 *pSrcLimit);

#endif	//_APP_CURVE_H_

