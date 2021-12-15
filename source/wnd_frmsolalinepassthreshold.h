#ifndef _WND_FRMSOLALINEPASSTHRESHOLD_H
#define _WND_FRMSOLALINEPASSTHRESHOLD_H
#include "guiglobal.h"
#include "wnd_global.h"

int linkbyFrmMainInit(void *pWndObj);
int linkbyFrmMainExit(void *pWndObj);
int linkbyFrmMainPaint(void *pWndObj);
int linkbyFrmMainLoop(void *pWndObj);
int linkbyFrmMainPause(void *pWndObj);
int linkbyFrmMainResume(void *pWndObj);

GUIWINDOW* CreateSolaLinePassThresholdWindow();
#endif // WND_FRMSOLALINEPASSTHRESHOLD_H