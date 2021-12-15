
//log out 总开关
#define ENABLE_DEBUG_LOG                    1
//各个模块log开关
#define ENABLE_CURVE_DEBUG                  0       //app_curve.c
#define ENABLE_DATA_PROCESS_DEBUG           1       //app_frmotdr.c DataProcessThread
#define ENABLE_DATA_ACQUI_DEBUG             1       //app_frmotdr.c DataAcquisitionThread
#define ENABLE_WND_OTDR_DEBUG               0       //wnd_frmotdr.c
#define ENBALE_ALGORITHM_DEBUG              1       //app_eventsearch.c & app_sigcombine.c
#define ENABLE_SOLA_DEBUG                   1       //app_sola.c & wnd_frmsola.c


//LOG输出的颜色值定义(利用printf原生的格式控制功能)
#define DEBUG_LOG_COLOR_BLACK               "\033[30m"      //黑色
#define DEBUG_LOG_COLOR_RED                 "\033[31m"      //红色
#define DEBUG_LOG_COLOR_GREEN               "\033[32m"      //绿色
#define DEBUG_LOG_COLOR_YELLOW              "\033[33m"      //黄色
#define DEBUG_LOG_COLOR_BLUE                "\033[34m"      //蓝色
#define DEBUG_LOG_COLOR_CARMINE             "\033[35m"      //洋红
#define DEBUG_LOG_COLOR_CYAN                "\033[36m"      //青色
#define DEBUG_LOG_COLOR_WHITE               "\033[37m"      //白色

#define DEBUG_LOG_COLOR_CLEAR               "\033[0m"       //清除颜色


#if (ENABLE_DEBUG_LOG == 1)
#define DEBUG_LOG(color, format, ...) \
	do {\
        fprintf(stderr, color "[%s|%s,%d] " format DEBUG_LOG_COLOR_CLEAR, \
				__FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#else
#define DEBUG_LOG(color, format, ...)
#endif


#if (ENABLE_CURVE_DEBUG == 1)
#define CURVE_DEBUG(format, ...)                DEBUG_LOG(DEBUG_LOG_COLOR_RED, format, ##__VA_ARGS__)
#else
#define CURVE_DEBUG(format, ...)
#endif

#if (ENABLE_DATA_PROCESS_DEBUG == 1)
#define  DATA_PROCESS_DEBUG(format, ...)        DEBUG_LOG(DEBUG_LOG_COLOR_BLUE, format, ##__VA_ARGS__)
#else
#define  DATA_PROCESS_DEBUG(format, ...)
#endif

#if (ENABLE_DATA_ACQUI_DEBUG == 1)
#define  DATA_ACQUI_DEBUG(format, ...)          DEBUG_LOG(DEBUG_LOG_COLOR_YELLOW, format, ##__VA_ARGS__)
#else
#define  DATA_ACQUI_DEBUG(format, ...)
#endif

#if (ENABLE_WND_OTDR_DEBUG == 1)
#define  WND_OTDR_DEBUG(format, ...)            DEBUG_LOG(DEBUG_LOG_COLOR_GREEN, format, ##__VA_ARGS__)
#else
#define  WND_OTDR_DEBUG(format, ...)
#endif

#if (ENBALE_ALGORITHM_DEBUG == 1)
#define  ALGORITHM_DEBUG(format, ...)           DEBUG_LOG(DEBUG_LOG_COLOR_CARMINE, format, ##__VA_ARGS__)
#else
#define  ALGORITHM_DEBUG(format, ...)
#endif

#if (ENABLE_SOLA_DEBUG == 1)
#define SOLA_DEBUG(format, ...)                 DEBUG_LOG(DEBUG_LOG_COLOR_CYAN, format, ##__VA_ARGS__)
#else
#define SOLA_DEBUG(format, ...)
#endif


