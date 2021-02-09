//
// Created by ILICH on 09.02.2021.
//

#ifndef __WIN_H
#define __WIN_H

#include <windows.h>

/* Initialize window function */
BOOL initWnd( HINSTANCE hInst, LPSTR lpszArgument,
              int nCmdShow, WNDPROC wndProc, HBRUSH *hBr );

/* Run message loop function */
int runMsgLoop( HBRUSH hBr );


#endif //__WIN_H
