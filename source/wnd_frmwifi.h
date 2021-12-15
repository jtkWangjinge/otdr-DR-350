/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifi.h
* 摘    要：  实现wifi的连接
*             
*
* 当前版本：  v1.0.0
* 作    者：  xiazhizhu
* 完成日期：  2016-4-7
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _WND_FRMWIFI_H_
#define _WND_FRMWIFI_H_


#define WIFI_SSID_LEN  33	//wifi ssid存储到EEPROM中的最大长度
#define WIFI_PSK_LEN   66	//wifi password存储到EEPROM中的最大长度


//WIFI连接过程记录
typedef enum _connect_progress
{
	EN_WIFI_DISCONNECT		= 1,					//未连接
	EN_WIFI_AUTHENTICATOR	= 2,					//身份验证
	EN_WIFI_GETIP			= 3,					//获得IP
	EN_WIFI_CONNECT			= 4,					//已连接
} CONNECT_PROGRESS;


/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmwifi的初始化函数，建立窗体控件、注册消息处理
int FrmWiFiInit(void *pWndObj);
//窗体frmwifi的退出函数，释放所有资源
int FrmWiFiExit(void *pWndObj);
//窗体frmwifi的绘制函数，绘制整个窗体
int FrmWiFiPaint(void *pWndObj);
//窗体frmwifi的循环函数，进行窗体循环
int FrmWiFiLoop(void *pWndObj);
//窗体frmwifi的挂起函数，进行窗体挂起前预处理
int FrmWiFiPause(void *pWndObj);
//窗体frmwifi的恢复函数，进行窗体恢复前预处理
int FrmWiFiResume(void *pWndObj);

/***
  * 功能：
        保存连接成功的热点的ssid和psk。以供下次自动连接
  * 参数：
        1.char * ssid:	需要保存的SSID
        2.char * psk:	需要保存的PSK
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/
int savepsk(char * ssid, char * psk);
/***
  * 功能：
        删除保存的无法连接的ssid和psk
  * 参数：
        1.char * ssid:	需要删除的SSID
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/
int deletepsk(char * ssid);

int wifi_ScanCallback(char AP_Count);

#endif	//_WND_FRMWIFI_H_


