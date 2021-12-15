#ifndef _WND_FRMSOLALINEDEFINE_H
#define _WND_FRMSOLALINEDEFINE_H
#include "guiglobal.h"
#include "wnd_global.h"

int linkDefineFrmInit(void *pWndObj);
int linkDefineFrmMainExit(void *pWndObj);
int linkDefineFrmMainPaint(void *pWndObj);
int linkDefineFrmMainLoop(void *pWndObj);
int linkDefineFrmMainPause(void *pWndObj);
int linkDefineFrmMainResume(void *pWndObj);

GUIWINDOW* CreateSolaLineDefineWindow();
#endif // WND_FRMSOLALINEDEFINE_H
