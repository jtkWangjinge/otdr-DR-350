/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_wifi.h
* 摘    要：  实现wifi模块数据结构的创建和函数的声明
*
* 取代版本：
* 原 作 者：
* 完成日期：
*
* 
*******************************************************************************/

#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_BIT0 0x01
#define WIFI_BIT1 0x02


typedef int (*WIFICALLBACK)(char );

//定义无线网络信息结构体
typedef struct _network_info_single
{
    char bssid[18];     //无线网卡mac地址6c:59:40:04:dc:64
    char signallevel;   //信号强度，5个等级0~4，范围定义为-100~-55
    char flags;         //安全加密方式，0-ESS-无 1-"WPA/WPA2-PSK TKIP/AES"-wpa加密
    char ssid[33];      //ap名称,最大长度32字节
} __attribute__((packed))NETWORK_INFO_SINGLE;

typedef struct _network_info
{
	char wifienable;						//wifi使能
    char connectready;  					//网络连接状态
    char dhcpready;     					//DHCP获取状态
    char listready;     					//网络列表准备好
    int count;          					//搜索到ap数，限制最大为30
    NETWORK_INFO_SINGLE * pNetSingleInfo;   //网络参数区
    WIFICALLBACK wifiScanFunc;				//当扫描了网络列表后执行此函数，传入的参数为获取的AP个数，最多30个。
    WIFICALLBACK wifiConnectFunc;			//当连接热点后执行此函数，参数为连接状态，0为连接热点失败。(注:该函数内不要调用会加锁的函数，否则会死锁)
    WIFICALLBACK wifiGetIPFunc;				//当获取分配的IP后执行此函数，参数为获取IP状态，0为获取失败。(注:该函数内不要调用会加锁的函数，否则会死锁)
    pthread_mutex_t wifiMutex;				//互斥锁
} __attribute__((packed))NETWORK_INFO;

//连接WIFI结构体
typedef struct _network_connect
{
	NETWORK_INFO_SINGLE TargetAP;
	char Password[128];	
} __attribute__((packed))NETWORK_CONNECT;

//using vfork instead of fork to avoid blocking
int vsystem(const char * cmdstring);

/*设置使能
参数:
	enable:1、每隔2秒扫描一次周围wifi热点
		   0、停止扫描
*/

/**
 *计算wifi信号值
 *
 *参数:
 	rssi:转换前
    numLevels: 5
 *
 *return:
    0~4
 *
**/
int calcsignallevel(int rssi, int numLevels);

void setWifiEnable(int enable);
/*获得使能*/
int getWifiEnable(void);

/*设置扫描热点后的回调函数
参数:
	callbackfunc:wifi列表刷新后调用的函数，并向该函数传入热点数量，最多30个
*/
void setWifiScanFunc(WIFICALLBACK callbackfunc);

/*设置连接热点后的回调函数
参数:
	callbackfunc:连接热点后的回调函数(参数为1，连接成功；参数为0，连接失败，可判断为密码错误)
*/
void setWifiConnectFunc(WIFICALLBACK callbackfunc);

/*设置获取IP后的回调函数
参数:
	callbackfunc:获取IP后的回调函数(参数为1，成功获取ip；参数为0，获取IP失败)
*/
void setWifiGetIPFunc(WIFICALLBACK callbackfunc);

/*获得是否正在连接网络
返回值:
	1:正在连接网络
	0:没有正在连接
*/
char getConnectready(void);

/*获得DHCP获取状态
返回值:
	1:已获得ip，可访问外网
	0:没有获得ip，无法访问外网
*/
char getDhcpready(void);

/*获得网络列表是否准备好
返回值:
	1:网络列表准备好
	0:网络列表没准备好
*/
char getListready(void);

/*获得搜索到的ap数
返回值:
	搜索到的热点数量
*/
int getCount(void);

/*获得ip地址*/
char* getIP(void);

/*获得当前连接的网络信息*/
NETWORK_INFO_SINGLE getConnectNetInfo(void);

/*获得网络参数表
返回值:
	返回一个指向网络列表的指针，使用完毕需free掉该指针
*/
NETWORK_INFO_SINGLE * getNetworkInfo(void);

//忘记密码
int forget_network_config(void);

/**
 *对wifi列表排序
**/
int sort_wifilist();

/*WIFI初始化*/
int wifi_Init();

/*连接网络
参数:
	targetAP:需要连接的热点
	password:密码
	*/
int wifi_Connect(NETWORK_INFO_SINGLE targetAP, char *password);

/*退出连接网络线程*/
int wifi_Connect_Cancel(void);

/*自动连接网络*/
int wifi_Auto_Connect(WIFICALLBACK ConnectCallback,WIFICALLBACK GetIPCallback);

/*
检查网络信息是否存储在配置文件中

参数:
	info:需要查询的网络
	pwdbuff:存储密码的缓存
	length:缓存大小
返回:
	0:没有找到
	1:找到并提取
*/
int check_if_network_saved(NETWORK_INFO_SINGLE info, char * pwdbuff, int length);

/*断开连接*/
int wifi_Disconnect();

/*退出WIFI模块，回收分配的内存*/
int wifi_Quit();

#ifdef __cplusplus
}
#endif

#endif

