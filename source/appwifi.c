
#include "appwifi.h"
#include "app_global.h"

#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>

#include "guiconfig.h"

//#define WIFI_LOG_INFO

pthread_t wifi_scan_tid = -1;			//扫描热点线程
pthread_t wifi_connect_tid = -1;			//连接wifi线程

static NETWORK_INFO *NetInfo = NULL;		//网络信息
static NETWORK_INFO_SINGLE CurrentNetInfo;	//当前连接的网络
static NETWORK_CONNECT ConnectNetInfo;		//需要连接的网络

static char IPAdress[128];
static int iConnectQuit = 0;				//连接过程的退出标志
//网络配置文件信息，保存了连接过的热点的bssid level flags key ssid timestamp，用于自动连接
#ifndef VIEW100
static const char *config_path = "/etc/wireless/network_save";
#else
static const char *config_path = "/mnt/sdcard/network_save";
#endif

extern int mysystem(char *pCmd);

int save_network_config(NETWORK_INFO_SINGLE info, char * pwdbuff);

static int isHexChar(char c)
{
    char lc = c | 0x20;
    return  (c >= '0' && c <= '9') || (lc >= 'a' && lc <= 'f');
}

static int isHexEscapeSequence(const char* s)
{
    return (s[0] == '\\') &&
            (s[1] == 'x') &&
            isHexChar(s[2]) &&
            isHexChar(s[3]);
}
//去掉中文ssid的‘\x’
static char* escapeSequenceFilter(const char* s, char* ds)
{
    if (!s || !ds)
        return NULL;

    while (*s)
    {
        if (isHexEscapeSequence(s))
        {
            char hex[4];
            hex[0] = s[2];
            hex[1] = s[3];
            hex[2] = 0;

            unsigned int c;
            sscanf(hex, "%02x", &c);
            *ds++ = c;
            s += 4;
        }
        else
        {
            *ds++ = *s++;
        }
    }
    *ds = '\0';
    return ds;
}


//读取一行存入buffer, 除去 '\r' & '\n'
static char * mfgets(char * buffer, int size, FILE * stream)
{
    int index, ret;

    for (index = 0; index < size; index++)
    {
        ret = fgetc(stream);

        if ('\n' == ret || '\r' == ret)
        {
            buffer[index] = 0;
            return buffer;
        }
        else if (EOF == ret)
        {
            buffer[index] = 0;

            if (!index)
            {
                return NULL;
            }
            else
            {
                return buffer;
            }
        }

        buffer[index] = (char)ret;
    }

    buffer[size - 1] = 0;
    return buffer;
}

//using vfork instead of fork to avoid blocking
int vsystem(const char * cmdstring)
{
    pid_t pid;
    int status;

    if (cmdstring == NULL)
    {
        return 1;
    }

    if ((pid = vfork()) < 0)
    {
        status = -1;
    }
    else if (pid == 0) //child
    {
        execl("/bin/sh", "sh", "-c", cmdstring, NULL);
        _exit(127); //get here when 'execl' failed & return value '127'
    }
    else
    {
        while (waitpid(pid, &status, 0) == -1)
        {
            if (errno != EINTR) //not interrupted by signal
            {
                status = -1;
                break;
            }
        }
    }

    return status;
}
//打印LOG信息到SDCARD
void WIFI_LOG(char *log)
{
	const char *wifi_log = MntDataDirectory"/wifi_log.txt";
	FILE *stream;

	if(!log)
		return;

	stream = fopen(wifi_log,"a+");

	if(stream == NULL )
		return;

	fprintf(stream,"%s\n",log);

	fclose(stream);
}
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
int calcsignallevel(int rssi, int numLevels)
{
    const int min_value = 155, max_value = 200;
    float inputRange, outputRange;

    if (rssi <= min_value)
    {
        return 0;
    }
    else if (rssi >= max_value)
    {
        return numLevels - 1;
    }
    else
    {
        inputRange = (max_value - min_value);
        outputRange = (numLevels - 1);
        return (int)((float)(rssi - min_value) * outputRange / inputRange);
    }
}

/***
  * 功能：
        获取本机IP
  * 参数：

  * 返回：
        成功返回指针，失败返回NULL
  * 备注：
***/
char* getHostIP(void)
{
	struct ifaddrs *ifaddr, *ifa;  
    int family, s;  
    char host[64];
	static char hostIP[128];  

  	memset(hostIP,0,sizeof(hostIP));
	
    if (getifaddrs(&ifaddr) == -1) {  
        perror("getifaddrs");
		return NULL;
    }  
  
    /* Walk through linked list, maintaining head pointer so we 
     *               can free list later */  
  
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {  
        if (ifa->ifa_addr == NULL)  
            continue;  
  
        family = ifa->ifa_addr->sa_family;  
  
        /* Display interface name and family (including symbolic 
         *                   form of the latter for the common families) */  
  /*
        printf("%s  address family: %d%s\n",  
                ifa->ifa_name, family,  
                (family == AF_PACKET) ? " (AF_PACKET)" :  
                (family == AF_INET) ?   " (AF_INET)" :  
                (family == AF_INET6) ?  " (AF_INET6)" : "");  
  */
        /* For an AF_INET* interface address, display the address */  
  
        if (family == AF_INET || family == AF_INET6) {  
            s = getnameinfo(ifa->ifa_addr,  
                    (family == AF_INET) ? sizeof(struct sockaddr_in) :  
                    sizeof(struct sockaddr_in6),  
                    host, sizeof(host), NULL, 0, NI_NUMERICHOST);  
            if (s != 0) {
				LOG(LOG_ERROR, "getnameinfo() failed: %s\n", gai_strerror(s));
				return NULL;
            }

			strcpy(hostIP,host);
        }  
    }  
  
    freeifaddrs(ifaddr);  
	return hostIP;
}


/**
 *对wifi列表排序
**/
int sort_wifilist()
{
   NETWORK_INFO_SINGLE *Tempbuffer = NULL;
   int i=0,j=0,k=0;
   int APcount = NetInfo->count;

   Tempbuffer = (NETWORK_INFO_SINGLE *)malloc(sizeof(NETWORK_INFO_SINGLE));

	if(APcount <=1 )
		return 0;
	
	//过滤同名热点
	for(;i<APcount-1;i++)
	{
		 for(j=i+1;j<APcount;j++)
		 {
			 if(0 == strcmp(NetInfo->pNetSingleInfo[i].ssid,NetInfo->pNetSingleInfo[j].ssid))
			 {

				
			 	if(NetInfo->pNetSingleInfo[i].signallevel < NetInfo->pNetSingleInfo[j].signallevel)
				{
					if(NetInfo->dhcpready && 
						0 == strcmp(NetInfo->pNetSingleInfo[i].bssid,CurrentNetInfo.bssid) && 
						0 == strcmp(NetInfo->pNetSingleInfo[i].ssid,CurrentNetInfo.ssid))
					{
						//若有同名网络，且已连接同名热点强度较弱，则断开网络
						wifi_Disconnect();
					}
					memcpy(&NetInfo->pNetSingleInfo[i],&NetInfo->pNetSingleInfo[j],sizeof(NETWORK_INFO_SINGLE));
				}					
				else
				{
					if(NetInfo->dhcpready && 
						0 == strcmp(NetInfo->pNetSingleInfo[j].bssid,CurrentNetInfo.bssid) && 
						0 == strcmp(NetInfo->pNetSingleInfo[j].ssid,CurrentNetInfo.ssid))
					{
						//若有同名网络，且已连接同名热点强度较弱，则断开网络
						wifi_Disconnect();
					}
				}
				
				if(j != (APcount-1))
			 	{
				 	memcpy(&NetInfo->pNetSingleInfo[j],&NetInfo->pNetSingleInfo[APcount-1],sizeof(NETWORK_INFO_SINGLE));
					j--;
			 	}
				NetInfo->count = --APcount;

			 }
		 }
	}

	i=0;
	j=0;

	//将已连接的热点放在第一个
   if(NetInfo->dhcpready)
   {
		for(;i<APcount;i++)
		{
			if(0 == strcmp(NetInfo->pNetSingleInfo[i].bssid,CurrentNetInfo.bssid) && 
				0 == strcmp(NetInfo->pNetSingleInfo[i].ssid,CurrentNetInfo.ssid) &&
				 i != 0)
			{
				memset(Tempbuffer,0,sizeof(Tempbuffer));
				memcpy(Tempbuffer,&NetInfo->pNetSingleInfo[i],sizeof(NETWORK_INFO_SINGLE));
				memcpy(&NetInfo->pNetSingleInfo[i],&NetInfo->pNetSingleInfo[0],sizeof(NETWORK_INFO_SINGLE));
				memcpy(&NetInfo->pNetSingleInfo[0],Tempbuffer,sizeof(NETWORK_INFO_SINGLE));
				break;
			}
		}
		CurrentNetInfo = NetInfo->pNetSingleInfo[0];
		i=1;
   }

	//排序
   for(;i<APcount;i++)
   {
   		k=i;
		for(j=i;j<APcount;j++)
		{
			if(NetInfo->pNetSingleInfo[k].signallevel < NetInfo->pNetSingleInfo[j].signallevel)
			{
				k=j;
			}
		}
		
		if(k!=i)
		{
			memset(Tempbuffer,0,sizeof(Tempbuffer));
			memcpy(Tempbuffer,&NetInfo->pNetSingleInfo[i],sizeof(NETWORK_INFO_SINGLE));
			memcpy(&NetInfo->pNetSingleInfo[i],&NetInfo->pNetSingleInfo[k],sizeof(NETWORK_INFO_SINGLE));
			memcpy(&NetInfo->pNetSingleInfo[k],Tempbuffer,sizeof(NETWORK_INFO_SINGLE));
		}
   }
   
   free(Tempbuffer);
   return 0;
}


/*!
获得wifi热点信息

return:
	0:获得热点信息成功
	1:scan失败
	2:出错
	3:没有扫描到热点
*/
int get_network_info(void)
{
    int index;
    char buffer[256];
    FILE * fstream;
	static int wait_time = 1;//开机第一次扫描需等待时间更长
    const int max_network = 30;
    const char * listpath = "/tmp/wifi/network_list";
    const char * get_network_file = "/tmp/wifi/get_network_info";
    NetInfo->listready = 0;
	
#ifdef WIFI_LOG_INFO
    if (mysystem("wpa_cli scan > /mnt/sdcard/wifi_log.txt"))
#else
    if (mysystem("wpa_cli scan > /dev/null"))
#endif
    {
        perror("wpa_cli scan");
        return 1;
    }


    //get network with not encrypted or wpa encrypted,excluding the connected network
    if(wait_time)
    {
		usleep(3000000);
		wait_time = 0;
	}
	else
	{
    	usleep(1500000);    //necessary time wait
	}

	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "wpa_cli scan_result | grep [[]ESS[]] > %s", listpath);
    mysystem(buffer);
	
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "wc -l %s > %s", listpath, get_network_file);
    mysystem(buffer);
	
    if (NULL == (fstream = fopen(get_network_file, "r")))
    {
        perror("fopen");
        return 2;
    }
	
	memset(buffer,0,sizeof(buffer));
    fgets(buffer, sizeof(buffer), fstream);
    fclose(fstream);

    NetInfo->count = strtol(buffer, NULL, 10);

#ifdef WIFI_LOG_INFO
    LOG(LOG_INFO, "count = %d\n", NetInfo->count);

    memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "AP Count ***************  %d", NetInfo->count);
	WIFI_LOG(buffer);
#endif

#if 0
	if(1 == NetInfo->count)
	{
		if(1 == NetInfo->dhcpready)
		{
			char Gateway[32] = {0};
			char *gatewayinfo = "/tmp/wifi/gateway";
			char *pingreturn = "/tmp/wifi/pingreturn";
			
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"cat /proc/net/route | grep wlan0 | awk -F' ' '{print $3}' > %s",gatewayinfo);
			mysystem(buffer);

			if (NULL == (fstream = fopen(gatewayinfo, "r")))
		    {
		        perror("fopen gateway:");
				mysystem("rmmod 8189es");
				mysystem("insmod /lib/modules/2.6.30/8189es.ko");
		        return 2;
		    }
			
			memset(buffer,0,sizeof(buffer));
		    while(NULL != mfgets(buffer, sizeof(buffer), fstream))
		    {
				struct in_addr gateway;
				char *temp = NULL;
				if(0 == strcmp(buffer,"00000000"))
				{
					memset(buffer,0,sizeof(buffer));
					continue;
				}
				
				gateway.s_addr = 0;
				temp = (char *)&gateway.s_addr;
				signed int i = 3;
				for(;i>=0;i--)
				{
					if(buffer[i*2] <= 57)
						*temp += (buffer[i*2] - 48) * 16;
					else
						*temp += (buffer[i*2] - 65 + 10) * 16;

					if(buffer[i*2+1] <= 57)
						*temp += (buffer[i*2+1] - 48);
					else
						*temp += (buffer[i*2+1] - 65 + 10);
					temp++;
				}
				
				strcpy(Gateway,inet_ntoa(gateway));
				break;
				
		    }

			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"ping -c1 -w1 %s ; echo $? > %s",Gateway,pingreturn);
			mysystem(buffer);
			
			if (NULL == (fstream = fopen(pingreturn, "r")))
		    {
		        perror("fopen");
		        return 2;
		    }
			
			memset(buffer,0,sizeof(buffer));
		    fgets(buffer, sizeof(buffer), fstream);
		    fclose(fstream);

			int pingresult;
		    pingresult = strtol(buffer, NULL, 10);

			if(pingresult != 0)
			{
				mysystem("date >> /mnt/sdcard/wifi_log.txt");
				WIFI_LOG("lost AP");
				mysystem("rmmod 8189es");
				mysystem("insmod /lib/modules/2.6.30/8189es.ko");
				wait_time = 1;
		        return 2;
			}

		}
		else
		{
			mysystem("date >> /mnt/sdcard/wifi_log.txt");
			WIFI_LOG("one AP");
			mysystem("rmmod 8189es");
			mysystem("insmod /lib/modules/2.6.30/8189es.ko");
			wait_time = 1;
		}
	}
#endif
    if (!NetInfo->count)
    {
        LOG(LOG_ERROR, "No AP was found!\n");

        //mysystem("date >> /mnt/sdcard/wifi_log.txt");
#ifdef WIFI_LOG_INFO
		WIFI_LOG("No AP");
#endif
		//mysystem("rmmod 8189es");
		//mysystem("insmod /lib/modules/2.6.30/8189es.ko");
		//wait_time = 1;
		
		NetInfo->listready = 1;
        return 3;
    }


    if (NetInfo->count > max_network) { NetInfo->count = max_network; }

    //alloc space to save network list
    free(NetInfo->pNetSingleInfo);
    NetInfo->pNetSingleInfo = malloc(NetInfo->count * sizeof(NETWORK_INFO_SINGLE));
    memset(NetInfo->pNetSingleInfo, 0, NetInfo->count * sizeof(NETWORK_INFO_SINGLE));
    //bssid
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "awk -F'\t' '{print $1}' %s > %s", listpath, get_network_file);
    mysystem(buffer);

    if (NULL == (fstream = fopen(get_network_file, "r")))
    {
        perror("fopen");
        return 2;
    }

    index = 0;
	
	memset(buffer,0,sizeof(buffer));
    while (NULL != mfgets(buffer, sizeof(buffer), fstream))
    {
        if (index >= NetInfo->count)
        {
            break;
        }

        strncpy(NetInfo->pNetSingleInfo[index].bssid, buffer, sizeof(NetInfo->pNetSingleInfo[index].bssid) - 1);
        index++;
		memset(buffer,0,sizeof(buffer));
    }

    fclose(fstream);
    //signal level
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "awk -F'\t' '{print $3}' %s > %s", listpath, get_network_file);
    mysystem(buffer);

    if (NULL == (fstream = fopen(get_network_file, "r")))
    {
        perror("fopen");
        return 2;
    }

    index = 0;
	
	memset(buffer,0,sizeof(buffer));
    while (NULL != mfgets(buffer, sizeof(buffer), fstream))
    {
        if (index >= NetInfo->count)
        {
            break;
        }

        NetInfo->pNetSingleInfo[index].signallevel = strtol(buffer, NULL, 10);
        index++;
		memset(buffer,0,sizeof(buffer));
    }

    fclose(fstream);
    //encrypt flags
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "awk -F'[' '{print $2}' %s | awk -F']' '{print $1}' > %s", listpath, get_network_file);
    mysystem(buffer);

    if (NULL == (fstream = fopen(get_network_file, "r")))
    {
        perror("fopen");
        return 2;
    }

    index = 0;

	memset(buffer,0,sizeof(buffer));
    while (NULL != mfgets(buffer, sizeof(buffer), fstream))
    {
        if (index >= NetInfo->count)
        {
            break;
        }

        if (!strncmp(buffer, "WPA", 3))
        {
            NetInfo->pNetSingleInfo[index].flags = 1;
        }
        else
        {
            NetInfo->pNetSingleInfo[index].flags = 0;
        }

        index++;
		memset(buffer,0,sizeof(buffer));
    }

    fclose(fstream);
    //ssid
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "awk -F'\t' '{print $5}' %s > %s", listpath, get_network_file);
    mysystem(buffer);

    if (NULL == (fstream = fopen(get_network_file, "r")))
    {
        perror("fopen");
        return 2;
    }

    index = 0;

	memset(buffer,0,sizeof(buffer));
    while (NULL != mfgets(buffer, 256, fstream))
    {
        if (index >= NetInfo->count)
        {
            break;
        }
        char buff[256] = {0};
        memset(buff, 0, sizeof(buff));
        escapeSequenceFilter(buffer, buff);
        strncpy(NetInfo->pNetSingleInfo[index].ssid, buff, sizeof(NetInfo->pNetSingleInfo[index].ssid) - 1);
        index++;
		memset(buffer,0,sizeof(buffer));
    }

    fclose(fstream);
	sort_wifilist();
#ifdef WIFI_LOG_INFO
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "AP Count %d", NetInfo->count);
	WIFI_LOG(buffer);
    //mysystem("cat /tmp/wifi/network_list >> /mnt/sdcard/wifi_log.txt");
#endif
    NetInfo->listready = 1;
    return 0;
}

//注意：极限情况下,dst的长度应该为src的4倍
static void WifiSsidSpecialCharReplace(char* src, char* dst)
{
    char* tmpSrc = src;
    char* tmpDst = dst;
    while('\0' != *tmpSrc)
    {
        if(*tmpSrc != '\'')
        {
            *tmpDst = *tmpSrc;
            tmpDst++;
        }
        else
        {
            strcpy(tmpDst,"'\\''");
            tmpDst += 4;
        }
        tmpSrc++;
    }
    return;
}

/*!
	连接wifi.密码中单引号的剔除在调用处处理
*/
int connect_to_network(NETWORK_INFO_SINGLE * singleInfo, char * pwdbuffer)
{
    char buffer[256] = {0};
    char tmpBuf[256] = {0};
    
    mysystem("wpa_cli remove_n 0");
	mysystem("wpa_cli add_n");
    WifiSsidSpecialCharReplace(singleInfo->ssid, tmpBuf);
    sprintf(buffer, "wpa_cli set_n 0 ssid '\"%s\"'", tmpBuf);
    mysystem(buffer);

	memset(buffer,0,sizeof(buffer));
    if (singleInfo->flags)
    {
        sprintf(buffer, "wpa_cli set_n 0 psk '\"%s\"'", pwdbuffer);
    }
    else
    {
        sprintf(buffer, "wpa_cli set_n 0 key_mgmt NONE");
    }
		
    mysystem(buffer);
	
	mysystem("wpa_cli enable_n 0");
    mysystem("wpa_cli select_n 0");
    return 0;
}

/*获取IP地址*/
int get_IP(void)
{
    mysystem("udhcpc -i wlan0 -n -q -t20 -T1 -A1");
	return 0;
}

/**
 *检查wifi连接状态
 *
 *return:
 *  0-失败，
 *  1-连接成功
 *	-1-异常
**/
int check_network_connect_completed(void)
{
	int iReturn = 0;
    FILE * fstream;
    char buffer[128] = {0};
    const char * network_status = "/tmp/wifi/network_status";
	const char * network_status_completed = "/tmp/wifi/network_status_completed";

	if(!iConnectQuit)
	{
	    sprintf(buffer, "wpa_cli status > %s", network_status);
	    mysystem(buffer);
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "awk -F'=' '/wpa_state/ {print $2}' %s > %s", network_status,network_status_completed);
	    mysystem(buffer);
	}

	if(!iConnectQuit)
	{
	    if (NULL == (fstream = fopen(network_status_completed, "r")))
	    {
	        perror("fopen complete:");
	        return -1;
	    }
		
		memset(buffer,0,sizeof(buffer));
		while(NULL != mfgets(buffer,sizeof(buffer),fstream))
		{
			if (!strncmp(buffer, "COMPLETED", 9) || !strncmp(buffer, "ASSOCIATED", 10))
	        {
	            iReturn = 1;
				break;
	        }
			memset(buffer,0,sizeof(buffer));
		}
		
	    fclose(fstream);
	}
	
    return iReturn;
}


/**
 *检查获取IP状态
 *
 *return:
 *  0-失败，
 *  1-连接成功
 *	-1-异常
**/
int check_network_connect_ip(void)
{
	int iReturn = 0;
    FILE * fstream;
    char buffer[128];
    const char * network_status = "/tmp/wifi/network_status";
	const char * network_status_getip = "/tmp/wifi/network_status_getip";
	
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "wpa_cli status > %s", network_status);
    mysystem(buffer);
	
	memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "grep ip_address %s > %s", network_status, network_status_getip);
    mysystem(buffer);

    if (NULL == (fstream = fopen(network_status_getip, "r")))
    {
        perror("fopen getip:");
		return -1;
    }
	
	while(NULL != mfgets(buffer,sizeof(buffer),fstream))
	{
		if (!strncmp(buffer, "ip_address", 10))
        {
            iReturn = 1;
			break;
        }
		memset(buffer,0,sizeof(buffer));
	}

    fclose(fstream);
    return iReturn;
}


/*获得网络信息线程*/
void * thread_wifi_fetch_network_info(void * arg)
{
	int ret = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	while(1)
	{
		
		while(!NetInfo->wifienable)
		{
			usleep(50000);
		}
		pthread_mutex_lock(&NetInfo->wifiMutex);
		
#ifdef WIFI_LOG_INFO
		WIFI_LOG("get network info1: check connect status");
#endif
		if(NetInfo->wifienable)
		{
			if(0 == check_network_connect_completed())
			{
				NetInfo->dhcpready = 0;
			}
		}
		
		pthread_testcancel();
		
#ifdef WIFI_LOG_INFO
		WIFI_LOG("get network info2: get network info");
#endif
		if(NetInfo->wifienable)
		{
    		ret = get_network_info();
		}
		pthread_mutex_unlock(&NetInfo->wifiMutex);
#ifdef WIFI_LOG_INFO
		WIFI_LOG("get network info3: callback");
#endif
		if(NetInfo->wifienable)
		{
			if((ret == 0 || ret == 3) && NetInfo->wifiScanFunc != NULL)
			{
				(*NetInfo->wifiScanFunc)(NetInfo->count);
			}
		}
		
#ifdef WIFI_LOG_INFO
		WIFI_LOG("get network info4: delay");
#endif
		int time = 8;
		while(--time && NetInfo->wifienable)
		{
			sleep(1);
		}
	}
}

/*退出获得网络信息线程*/
int thread_wifi_fetch_network_info_cancel(void)
{
	if(wifi_scan_tid == -1)
	{
		return -1;
	}

	pthread_cancel(wifi_scan_tid);
	
	return 0;
}



/*连接wifi线程*/
void * thread_connect_network(void * arg)
{
	int i=0;
	int ret = 0;
    char tmpBuf[256] = {0};
	
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	pthread_mutex_lock(&NetInfo->wifiMutex);
	
    NetInfo->connectready = 1;
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("thread_connect_network1: start!!!");
#endif
	
	if(!iConnectQuit)
	{
	    WifiSsidSpecialCharReplace(ConnectNetInfo.Password, tmpBuf);
    	connect_to_network(&ConnectNetInfo.TargetAP, tmpBuf);
	}
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("thread_connect_network2: check password ");
#endif
	i=10;
	while(--i && !iConnectQuit)
	{
    	ret = check_network_connect_completed();
		
		if(!iConnectQuit)
		{
			if(ret == 0)
			{
				int j = 100;
				while(--j && !iConnectQuit)
				{
					usleep(10000);
				}
			}
			else if(ret == 1)
			{
				break;
			}
		}
	}
	

	if(!iConnectQuit)
	{
		if(ret == 1)
		{
#ifdef WIFI_LOG_INFO
			WIFI_LOG("thread_connect_network3: password right go to callback");
#endif
			if(NetInfo->wifiConnectFunc) NetInfo->wifiConnectFunc(1);
		}
		else
		{
#ifdef WIFI_LOG_INFO
			WIFI_LOG("thread_connect_network3: password error go to callback");
#endif
			if(NetInfo->wifiConnectFunc) NetInfo->wifiConnectFunc(0);
			goto __exit;
		}
	}
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("thread_connect_network4: start get ip");
#endif
	if(!iConnectQuit)
	{
		get_IP();
	}
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("thread_connect_network5: check get ip");
#endif
	i=10;
	while(--i && !iConnectQuit)
	{
    	ret = check_network_connect_ip();
		if(!iConnectQuit)
		{
			if(ret == 0)
			{
				int j = 100;
				while(--j && !iConnectQuit)
				{
					usleep(10000);
				}
			}
			else if(ret == 1)
			{
				break;
			}	
		}
	}
	
	
	//mysystem("echo connect 6 >> /mnt/sdcard/wifi_log.txt");
	if(!iConnectQuit)
	{
		if(ret == 1)
		{
#ifdef WIFI_LOG_INFO
			WIFI_LOG("thread_connect_network6: get ip success,save info and callback");
#endif
			NetInfo->dhcpready=1;
			CurrentNetInfo = ConnectNetInfo.TargetAP;
			save_network_config(ConnectNetInfo.TargetAP,tmpBuf);
			if(!iConnectQuit)
			{
				memset(IPAdress,0,sizeof(IPAdress));
				strcpy(IPAdress,getHostIP());
			}
			
			if(NetInfo->wifiGetIPFunc && !iConnectQuit) NetInfo->wifiGetIPFunc(1);
		}
		else
		{
#ifdef WIFI_LOG_INFO
			WIFI_LOG("thread_connect_network6: get ip fail,go to callback");
#endif
			NetInfo->dhcpready=0;
			if(NetInfo->wifiGetIPFunc && !iConnectQuit) NetInfo->wifiGetIPFunc(0);
		}
	}
	//mysystem("echo connect 7 >> /mnt/sdcard/wifi_log.txt");

	
__exit:
	NetInfo->connectready = 0;
	
	if(iConnectQuit)
	{
		wifi_Disconnect();
	}
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("thread_connect_network7；over");
#endif
	pthread_mutex_unlock(&NetInfo->wifiMutex);
	wifi_connect_tid = -1;
    pthread_exit(NULL);
}

/*退出连接网络线程*/
int wifi_Connect_Cancel(void)
{
	if(wifi_connect_tid == -1)
	{
		return -1;
	}

	iConnectQuit = 1;	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("connect cancel start !!!");
#endif
	pthread_join(wifi_connect_tid,NULL);
#ifdef WIFI_LOG_INFO
	WIFI_LOG("connect cancel over !!!");
#endif
	iConnectQuit = 0;
	return 0;
}


//忘记密码
int forget_network_config(void)
{
	char buffer[256] = {0};
    char tmpBuf[256] = {0};
    WifiSsidSpecialCharReplace(ConnectNetInfo.TargetAP.ssid, tmpBuf);
	sprintf(buffer, "sed -i '/^%s.*%s\t/d' %s", ConnectNetInfo.TargetAP.bssid, tmpBuf, config_path);	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("forget ap info");
	WIFI_LOG(buffer);
#endif
	mysystem(buffer);

	return 0;
}



//保存网络配置, 包括 bssid 信号 加密方式 密码 ssid 连接的时间.密码中单引号的剔除在调用处。
int save_network_config(NETWORK_INFO_SINGLE info, char * pwdbuff)
{
    char buffer[256];
    char tmpBuf[256]={0};
    int total, ret;
    FILE * stream;
    time_t itime;
    struct tm tm_time;
    const int record_limit = 1000;  //maximum saving items
    total = 0;
	char *fileline = "/tmp/wifi/config_file_line";

	memset(tmpBuf,0,sizeof(tmpBuf));
    WifiSsidSpecialCharReplace(info.ssid, tmpBuf);
    
#ifdef WIFI_LOG_INFO
	WIFI_LOG("save info start");
	mysystem("df -h | grep root >> "MntDataDirectory"/wifi_log.txt");
	
	
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"save %s %s %s",info.bssid,tmpBuf,pwdbuff);
	WIFI_LOG(buffer);
	
#endif
    //get the total record count
    
	if(!iConnectQuit)
	{
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "wc -l %s > %s", config_path,fileline);
	    mysystem(buffer);
	}

	
	if(!iConnectQuit)
	{
		if(NULL == (stream = fopen(fileline,"r")))
		{
			perror("fopen config_file_line:");
			return -1;
		}

	   	mfgets(buffer, sizeof(buffer), stream);
		
		total = atoi(buffer);
		fclose(stream);
	}

	if(!iConnectQuit)
	{
	    //current timestamp
	    time(&itime);
	    localtime_r(&itime, &tm_time);
	    //comparison
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "grep '%s' %s  2>/dev/null | grep '%s'", info.bssid, config_path, tmpBuf);
	    ret = mysystem(buffer);
	}

	if(!iConnectQuit)
	{
	    //new record
	    if (ret)
	    {
	        if (total == record_limit)
	        {
				memset(buffer,0,sizeof(buffer));
	            sprintf(buffer, "sed -i '1d' %s", config_path);
	            mysystem(buffer);
	        }
	    }
	    //exist record
	    else
	    {
			memset(buffer,0,sizeof(buffer));
	        sprintf(buffer, "sed -i '/^%s.*%s\t/d' %s", info.bssid, tmpBuf, config_path);
	        mysystem(buffer);
	    }
	}
	
	if(!iConnectQuit)
	{
	    //the open network save the key as '*'
	    if (!info.flags)
	    {
	        strcpy(pwdbuff, "*");
	    }

	    //append config
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "echo '""%s\t"/*bssid*/"%d\t"/*level*/"%d\t"/*flags*/"%s\t"/*key*/"%s\t"/*ssid*/\
	            "%04d-%02d-%02d %02d:%02d:%02d"/*timestamp 2015-09-18 09:08:05*/"' >> %s",
	            info.bssid, info.signallevel, info.flags, pwdbuff, tmpBuf,
	            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
	            config_path);
	    mysystem(buffer);
		
#ifdef WIFI_LOG_INFO
	    //mysystem("cat /etc/wireless/network_save >> /mnt/sdcard/wifi_log.txt");
#endif
	}
    return 1;
}


/*自动连接网络*/
int wifi_Auto_Connect(WIFICALLBACK ConnectCallback,WIFICALLBACK GetIPCallback)
{
    int index;
    FILE * stream;
    char buffer[256], pwdbuff[32];
    char tmpBuf[256] = {0};
    const char * compare_path = "/tmp/wifi/compare_result";
	const char * get_data = "/tmp/wifi/get_compare_result";
    NETWORK_INFO_SINGLE info;
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("auto connect");
#endif

	mysystem("> /tmp/wifi/compare_result");
	if(!iConnectQuit)
	{
	    for (index = 0; index < NetInfo->count; index++)
	    {
	        //record in file format is 'bssid level flags key ssid timestamp',we will compare 'bssid flags ssid'
			memset(buffer,0,sizeof(buffer));
            memset(tmpBuf,0,sizeof(tmpBuf));
            WifiSsidSpecialCharReplace(NetInfo->pNetSingleInfo[index].ssid, tmpBuf);
	        sprintf(buffer, "grep -w '^%s[[:space:]].*[[:space:]]%d.*%s' %s  2>/dev/null >> %s",
	                NetInfo->pNetSingleInfo[index].bssid, NetInfo->pNetSingleInfo[index].flags, tmpBuf,
	                config_path, compare_path);
	        mysystem(buffer);
			if(iConnectQuit)
				break;
	    }
	}

    //sort result in descending order by level flags timestamp
    if(!iConnectQuit)
	{
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "sort -t$'\t' -k2,3 -k6 -r %s -o %s", compare_path, compare_path);
	    mysystem(buffer);
	    //bssid
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "head -n 1  %s | awk -F'\t' '{print $1}' > %s", compare_path,get_data);
	    mysystem(buffer);
    }


    if(NULL == (stream = fopen(get_data, "r")))
    {
		perror("[auto connect bssid] open:");
		return 0;
	}

    if (!mfgets(info.bssid, sizeof(info.bssid), stream))
    {
        perror("[auto connect bssid]mfgets");
        goto __exit_close;
    }

	if(iConnectQuit)
        goto __exit_close;

    fclose(stream);
    //level
    if(!iConnectQuit)
	{
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "head -n 1  %s | awk -F'\t' '{print $2}' > %s", compare_path,get_data);
	    mysystem(buffer);
    }
    if(NULL == (stream = fopen(get_data, "r")))
    {
		perror("[auto connect level] open:");
		return 0;
	}

    if (!mfgets(buffer, sizeof(buffer), stream))
    {
        perror("[auto connect level]mfgets");
        goto __exit_close;
    }
	if(iConnectQuit)
        goto __exit_close;

    info.signallevel = (char)strtol(buffer, NULL, 10);
    fclose(stream);
    //flags
    if(!iConnectQuit)
	{
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "head -n 1  %s | awk -F'\t' '{print $3}' > %s", compare_path,get_data);
	    mysystem(buffer);
    }
    if(NULL == (stream = fopen(get_data, "r")))
    {
		perror("[auto connect flags] open:");
		return 0;
	}

    if (!mfgets(buffer, sizeof(buffer), stream))
    {
        perror("[auto connect flags]mfgets");
        goto __exit_close;
    }
	if(iConnectQuit)
        goto __exit_close;

    info.flags = (char)strtol(buffer, NULL, 10);
    fclose(stream);
    //password
    if(!iConnectQuit)
	{
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "head -n 1  %s | awk -F'\t' '{print $4}' > %s", compare_path,get_data);
	    mysystem(buffer);
    }
    if(NULL == (stream = fopen(get_data, "r")))
    {
		perror("[auto connect pwdbuff] open:");
		return 0;
	}

    if (!mfgets(pwdbuff, sizeof(pwdbuff), stream))
    {
        perror("[auto connect pwdbuff]mfgets");
        goto __exit_close;
    }
	if(iConnectQuit)
        goto __exit_close;

    fclose(stream);
    //ssid
    if(!iConnectQuit)
	{
		memset(buffer,0,sizeof(buffer));
	    sprintf(buffer, "head -n 1  %s | awk -F'\t' '{print $5}' > %s", compare_path,get_data);
	    mysystem(buffer);
    }
    if(NULL == (stream = fopen(get_data, "r")))
    {
		perror("[auto connect ssid] open:");
		return 0;
	}

    if (!mfgets(info.ssid, sizeof(info.ssid), stream))
    {
        perror("[auto connect ssid]mfgets");
        goto __exit_close;
    }
	if(iConnectQuit)
        goto __exit_close;
	
	if(NetInfo->connectready == 0)
	{
		setWifiConnectFunc(ConnectCallback);
		setWifiGetIPFunc(GetIPCallback);
		
		wifi_Connect(info, pwdbuff);
	}

__exit_close:
    fclose(stream);

	return 0;
}

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

int check_if_network_saved(NETWORK_INFO_SINGLE info, char * pwdbuff, int length)
{
    char buffer[256];
    char tmpBuf[256] = {0};
	char *check_save = "/tmp/wifi/check_save";
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("cheak network save");
#endif
    FILE * stream;
	memset(buffer,0,sizeof(buffer));
    memset(tmpBuf,0,sizeof(tmpBuf));
    WifiSsidSpecialCharReplace(info.ssid, tmpBuf);
    sprintf(buffer, "grep -w '^%s[[:space:]].*[[:space:]]%d.*%s' %s 2>/dev/null | awk -F'\t' '{print $4}' > %s",
            info.bssid, info.flags, tmpBuf, config_path, check_save);
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG(buffer);
#endif

	mysystem(buffer);
	
    stream = fopen(check_save, "r");

    if (NULL == stream)
    {
        perror("[check_if_network_saved]fopen");
        return 0;
    }

    if (!mfgets(pwdbuff, length, stream))
    {
#ifdef WIFI_LOG_INFO
		WIFI_LOG("check fail");
#endif
        fclose(stream);
        return 0;
    }
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("check success");
#endif

    fclose(stream);
    return 1;
}


/*设置使能*/
void setWifiEnable(int enable)
{
	NetInfo->wifienable = enable;
}

/*获得使能*/
int getWifiEnable(void)
{
	return NetInfo->wifienable;
}

/*设置扫描热点后的回调函数*/
void setWifiScanFunc(WIFICALLBACK callbackfunc)
{
	NetInfo->wifiScanFunc = callbackfunc;
}

/*设置连接热点后的回调函数*/
void setWifiConnectFunc(WIFICALLBACK callbackfunc)
{
	NetInfo->wifiConnectFunc= callbackfunc;
}

/*设置获取IP后的回调函数*/
void setWifiGetIPFunc(WIFICALLBACK callbackfunc)
{
	NetInfo->wifiGetIPFunc= callbackfunc;
}

/*获得DHCP获取状态*/
char getDhcpready(void)
{
	char ret;
	//pthread_mutex_lock(&NetInfo->wifiMutex);
	ret = NetInfo->dhcpready;
	//pthread_mutex_unlock(&NetInfo->wifiMutex);
	return ret;
}

/*获得是否正在连接网络*/
char getConnectready(void)
{
	char ret;
	ret = NetInfo->connectready;
	return ret;
}

/*获得网络列表是否准备好*/
char getListready(void)
{
	char ret;
	ret = NetInfo->listready;
	return ret;
}

/*获得搜索到的ap数*/
int getCount(void)
{
	int ret;
	ret = NetInfo->count;
	return ret;
}

/*获得ip地址*/
char* getIP(void)
{
	return IPAdress;
}

/*获得当前连接的网络信息*/
NETWORK_INFO_SINGLE getConnectNetInfo(void)
{
	return CurrentNetInfo;
}
/*获得网络参数表*/
NETWORK_INFO_SINGLE * getNetworkInfo(void)
{
	NETWORK_INFO_SINGLE *ret = NULL;
	//pthread_mutex_lock(&NetInfo->wifiMutex);
	
	if(NetInfo->count)
	{
		ret = (NETWORK_INFO_SINGLE *)malloc(NetInfo->count * sizeof(NETWORK_INFO_SINGLE));
		memcpy(ret,NetInfo->pNetSingleInfo,NetInfo->count * sizeof(NETWORK_INFO_SINGLE));
	}
	
	//pthread_mutex_unlock(&NetInfo->wifiMutex);
	return ret;
}

/*!
	WIFI初始化
*/
int wifi_Init()
{
	int iReturn = 0;
	int err = 0;

	if(-1 == access("/tmp/wifi",F_OK))
	{
		mysystem("mkdir /tmp/wifi");
	}
	
#ifdef WIFI_LOG_INFO
	WIFI_LOG("***********************************WIFI Init**************************************");
#endif
	NetInfo = (NETWORK_INFO *)malloc(sizeof(NETWORK_INFO));
	memset(NetInfo,0,sizeof(NETWORK_INFO));

	pthread_mutex_init(&NetInfo->wifiMutex, NULL);
	
	err = pthread_create(&wifi_scan_tid,NULL,thread_wifi_fetch_network_info,NULL);
	
	if(err != 0)
	{
		perror("create thread_wifi_fetch_network_info");
		iReturn = -1;
	}
	
	return iReturn;
}

/*
*连接网络
*/
int wifi_Connect(NETWORK_INFO_SINGLE targetAP, char *password)
{
	int err;
	int iReturn = 0;
	
	if(targetAP.flags && password == NULL)
	{
		return 1;
	}

	//有连接线程未结束
	if(NetInfo->connectready || iConnectQuit)
		return 1;
	
	memset(&ConnectNetInfo.TargetAP,0,sizeof(NETWORK_INFO_SINGLE));
	memset(&ConnectNetInfo.Password[0],0,sizeof(ConnectNetInfo.Password));
	
	ConnectNetInfo.TargetAP = targetAP;
	if(password)
	{
		memcpy(ConnectNetInfo.Password,password,strlen(password));
	}
	
	err = pthread_create(&wifi_connect_tid,NULL,thread_connect_network,NULL);
	
	if(err != 0)
	{
		perror("create thread_wifi_fetch_network_info");
		iReturn = -1;
	}
	return iReturn;
}

/*
*断开连接
*/
int wifi_Disconnect()
{
    mysystem("wpa_cli remove_network 0");
	memset(IPAdress,0,sizeof(IPAdress));
	NetInfo->dhcpready = 0;
	return 0;
}

/*
*退出WIFI模块，回收分配的内存
*/
int wifi_Quit()
{
	NetInfo->wifienable = 0;
	thread_wifi_fetch_network_info_cancel();
	return 0;
}

