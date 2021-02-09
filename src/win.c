//
// Created by ILICH on 09.02.2021.
//
#include <stdio.h>
#include "win.h"
/* Init Main window function*/
BOOL initWnd( HINSTANCE hInst, LPSTR lpszArgument,
              int nCmdShow, WNDPROC wndProc, HBRUSH *hBr )
{
    HWND hWnd;
    WNDCLASSEX wincl;// = {0};
    LPCSTR szClassName = "MainWnd";

    /* The Window structure */
    wincl.hInstance = hInst;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = wndProc;
    wincl.style = CS_OWNDC;
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = NULL;
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = szClassName;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;

    /* Dark version color */

    *hBr = CreateSolidBrush(RGB(43, 43, 43));
    wincl.hbrBackground = *hBr;
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return FALSE;



    /* The class is registered, let's create the program*/
    hWnd = CreateWindowEx (
           0,
           szClassName,
           "Text  editor",
           WS_OVERLAPPEDWINDOW |
           WS_VSCROLL |
           WS_HSCROLL,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           NULL,
           NULL,
           hInst,
           lpszArgument
           );

    if (hWnd == NULL)
        return FALSE;

    /* Make the window visible on the screen */
    UpdateWindow(hWnd);
    ShowWindow(hWnd, nCmdShow);

    return TRUE;
}
/* Main program loop function */
int runMsgLoop( HBRUSH hBr )
{
    MSG msg;
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&msg, NULL, 0, 0))
    {
        /* Translate virtual-key msg into character msg */
        TranslateMessage(&msg);
        /* Send message to WindowProcedure */
        DispatchMessage(&msg);
    }

    DeleteObject((HGDIOBJ)hBr);

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return msg.wParam;
}
