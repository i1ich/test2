//
// Created by ILICH on 09.02.2021.
//
#include <stdio.h>
#include <windows.h>
#include "wndproc.h"
#include "utils.h"

/* Reboot file function */
void Reboot(HWND hWnd, TEXT_RENDER_DATA *renderData )
{
    renderData->curLineInStr = 0;
    renderData->xLeftUp = 0;
    renderData->yLeftUp = 0;

    SetScrollPos(hWnd, SB_HORZ, 0, TRUE);
    SetScrollPos(hWnd, SB_VERT, 0, TRUE);
}
/* Add menu line function */
void AddMenus(HWND hwnd) {

    HMENU hMenubar;
    HMENU hMenu;

    hMenubar = CreateMenu();
    hMenu = CreateMenu();

    AppendMenuW(hMenu, MF_STRING, MENU_BARS_ON, L"&Bars on");
    AppendMenuW(hMenu, MF_STRING, MENU_BARS_OFF, L"&Bars off");
    AppendMenuW(hMenu, MF_STRING, MENU_OPEN, L"&Open");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, MENU_QUIT, L"&Quit");

    AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) hMenu, L"&File");
    SetMenu(hwnd, hMenubar);
}

/* WM_PAINT callback function */
void OnPaint(HWND hWnd, TEXT_DATA *td, TEXT_RENDER_DATA *renderData, TEXTMETRIC *tm, MODE m )
{
    int i;
    PAINTSTRUCT paintstruct;
    BeginPaint(hWnd, &paintstruct);

    if (m == BARS_ON)
        for (i = renderData->yLeftUp; i < min(renderData->symbolsPerH + renderData->yLeftUp, td->strCount); i++)
            TextOut(paintstruct.hdc, 0, (i - renderData->yLeftUp) * tm->tmHeight,
                    td->text + td->strOffsets[i] + renderData->xLeftUp,
                    min(strTextLength(td, i) - renderData->xLeftUp,
                        renderData->symbolsPerW));
    else
    {
        int printed = 0; // num of printed strings
        int overgone = 0;
        for (i = renderData->yLeftUp; printed < renderData->symbolsPerH && i < min(renderData->symbolsPerH + renderData->yLeftUp, td->strCount); i++)
        {
            int
                j = 0,
                strTL = strTextLength(td, i);
            int strCount = linesInCurStr(strTL, renderData);
            for (j = (1 - overgone) * renderData->curLineInStr;
                    j < strCount - 1 &&
                    printed < renderData->symbolsPerH; j++, printed++)
                TextOut(paintstruct.hdc, 0, printed * tm->tmHeight,
                        td->text + td->strOffsets[i] + renderData->symbolsPerW * j,
                        renderData->symbolsPerW);

            for (; j == strCount - 1 && printed < renderData->symbolsPerH; j++, printed++)
                TextOut(paintstruct.hdc, 0, printed * tm->tmHeight,
                        td->text + td->strOffsets[i] + renderData->symbolsPerW * j,
                        strTL - (strCount - 1) * renderData->symbolsPerW);
            overgone = 1;
        }
    }
    EndPaint(hWnd, &paintstruct);
}

/* WM_SIZE callback function */
void OnSize(HWND hWnd, TEXT_RENDER_DATA *trd, TEXTMETRIC *tm, int newW, int newH, MODE m )
{
    int
        oldW = trd->symbolsPerW,
        oldH = trd->symbolsPerH;
    RECT rc;

    trd->symbolsPerW = newW / tm->tmAveCharWidth;
    trd->symbolsPerH = newH / tm->tmHeight;

    if (m == BARS_ON)
    {
        // vertical
        rc.top = 0;
        rc.bottom = max(newH, oldH);
        rc.left = (min(trd->symbolsPerW, oldW) - 1) * tm->tmAveCharWidth;
        rc.right = (max(trd->symbolsPerW, oldW) + 1) * tm->tmAveCharWidth ;
        InvalidateRect(hWnd, &rc, TRUE);

        // horizontal
        rc.top = (min(trd->symbolsPerH, oldH) - 1) * tm->tmHeight;
        rc.bottom = (max(trd->symbolsPerH, oldH) + 1) * tm->tmHeight;
        rc.left = 0;
        rc.right = newW;
        InvalidateRect(hWnd, &rc, TRUE);
    }
    else
        invalidateScreen(hWnd, trd, tm);
}

/* WM_KEYDOWN callback function */
void OnKeyDown(HWND hWnd, WPARAM wParam,
               TEXT_DATA *textData,
               TEXT_RENDER_DATA *renderData, TEXTMETRIC *tm, MODE m )
{
    int minScroll, maxScroll;
    GetScrollRange(hWnd, SB_VERT, &minScroll, &maxScroll);

    switch (wParam)
    {
    case VK_RIGHT:
        if (m == BARS_OFF)
            break;
            renderData->xLeftUp =
                    (textData->maxStrWidth - renderData->symbolsPerW - 1 > 0) *
                    min(renderData->xLeftUp + 1, textData->maxStrWidth - renderData->symbolsPerW - 1);
        invalidateScreen(hWnd, renderData, tm);
        break;
    case VK_LEFT:
        if (m == BARS_OFF)
            break;
            renderData->xLeftUp = max(0, renderData->xLeftUp - 1);
        invalidateScreen(hWnd, renderData, tm);
        break;
    case VK_UP:
        LineUp(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        break;
    case VK_DOWN:
        LineDown(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        break;
    case VK_PRIOR:
        PageUp(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        break;
    case VK_NEXT:
        PageDown(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        break;
    case VK_HOME:
        if (!GetAsyncKeyState(VK_CONTROL))
        {
            if (m == BARS_OFF)
                break;
            renderData->xLeftUp = 0;
        }
        else
        {
            renderData->yLeftUp = 0;
            renderData->curLineInStr = 0;
        }
        invalidateScreen(hWnd, renderData, tm);
        break;
    case VK_END:
        if (!GetAsyncKeyState(VK_CONTROL))
        {
            int maxStrW = findMaxStrWidth(textData, renderData->yLeftUp, min(renderData->yLeftUp + renderData->symbolsPerH, textData->strCount));
            renderData->xLeftUp = max(0, maxStrW - renderData->symbolsPerW);
        }
        else
        {
            int yUp, curLine;
            endOfDocument(m, textData, renderData, &yUp, &curLine);
            renderData->yLeftUp = yUp;
            renderData->curLineInStr = curLine;
        }
        invalidateScreen(hWnd, renderData, tm);
        break;
    }

    SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(textData, renderData, minScroll, maxScroll), TRUE);
    SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(textData, renderData, minScroll, maxScroll), TRUE);
}

/* WM_VSCROLL callback function. */
void OnVScroll(HWND hWnd, WPARAM wParam, TEXT_DATA *textData, TEXT_RENDER_DATA *renderData, TEXTMETRIC *tm, MODE m )
{
    int minScroll, maxScroll;
    int pos = HIWORD(wParam);
    int oldPos = GetScrollPos(hWnd, SB_VERT);
    GetScrollRange(hWnd, SB_VERT, &minScroll, &maxScroll);

    switch (LOWORD(wParam))
    {
    case SB_THUMBTRACK:
        if (oldPos != pos)
        {
            int endYLeftUp, endCurLine;
            endOfDocument(m, textData, renderData, &endYLeftUp, &endCurLine);

            renderData->yLeftUp = vScrollToTextHeight(endYLeftUp, pos, minScroll, maxScroll);

            if (pos == maxScroll)
                renderData->curLineInStr = endCurLine;
        }
        invalidateScreen(hWnd, renderData, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(textData, renderData, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEDOWN:
        LineDown(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(textData, renderData, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEUP:
        LineUp(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(textData, renderData, minScroll, maxScroll), TRUE);
        break;
    case SB_PAGEUP:
        PageUp(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(textData, renderData, minScroll, maxScroll), TRUE);
        break;
    case SB_PAGEDOWN:
        PageDown(m, textData, renderData);
        invalidateScreen(hWnd, renderData, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(textData, renderData, minScroll, maxScroll), TRUE);
        break;
    }
}

/* WM_HSCROLL callback function. */
void OnHScroll(HWND hWnd, WPARAM wParam, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData, TEXTMETRIC *textmetric, MODE m )
{
    int minScroll, maxScroll;
    int pos = HIWORD(wParam);

    if (m == BARS_OFF)
        return;

    GetScrollRange(hWnd, SB_HORZ, &minScroll, &maxScroll);

    switch (LOWORD(wParam))
    {
    case SB_THUMBTRACK:
        textRenderData->xLeftUp = hScrollToTextWidth(textData, textRenderData, pos, minScroll, maxScroll);
        invalidateScreen(hWnd, textRenderData, textmetric);
        SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(textData, textRenderData, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEDOWN:
        textRenderData->xLeftUp =
                (textData->maxStrWidth - textRenderData->symbolsPerW - 1 > 0) *
                min(textRenderData->xLeftUp + 1, textData->maxStrWidth - textRenderData->symbolsPerW - 1);
        invalidateScreen(hWnd, textRenderData, textmetric);
        SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(textData, textRenderData, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEUP:
        textRenderData->xLeftUp = max(0, textRenderData->xLeftUp - 1);
        invalidateScreen(hWnd, textRenderData, textmetric);
        SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(textData, textRenderData, minScroll, maxScroll), TRUE);
        break;
    }
}

//
//  FUNCTION: WindowProcedure(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//  ...
//
LRESULT CALLBACK WindowProcedure( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    static TEXT_DATA textData = {0};
    static TEXT_RENDER_DATA renderData = {0};
    static TEXTMETRIC textmetric;
    static HFONT hFont;
    static int minScroll = 0, maxScroll = 2000;
    static int fontSize = 26;
    static MODE m = BARS_ON;
    static OPENFILENAME ofn = {0};

    //char szFile[100];
    wchar_t szFile[260];

    HDC hDC;
    HMENU hMenu;

    switch (message)                  /* handle the msg */
    {
    case WM_CREATE:
        hDC = GetDC(hWnd);

        hFont = CreateFont(
                             fontSize,
                             0,
                             0,
                             0,
                             FW_NORMAL,
                             FALSE,
                             FALSE,
                             FALSE,
                             ANSI_CHARSET,
                             OUT_TT_PRECIS,
                             CLIP_CHARACTER_PRECIS,
                             CLEARTYPE_QUALITY,
                             FIXED_PITCH,
                             TEXT("Courier"));

        SelectObject(hDC, hFont);
        SetTextColor(hDC, RGB(169, 183, 198));
        SetBkColor(hDC, RGB(43, 43, 43));

        GetTextMetrics(hDC, &textmetric);
        readFile(((CREATESTRUCT *)lParam)->lpCreateParams, &textData);
        SetScrollRange(hWnd, SB_VERT, minScroll, maxScroll, TRUE);
        SetScrollRange(hWnd, SB_HORZ, minScroll, maxScroll, TRUE);
        Reboot(hWnd, &renderData);
        AddMenus(hWnd);
        break;
    case WM_DESTROY:
        freeTextData(&textData);
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    case WM_PAINT:
        OnPaint(hWnd, &textData, &renderData, &textmetric, m);
        break;
    case WM_SIZE:
        OnSize(hWnd, &renderData, &textmetric, LOWORD(lParam), HIWORD(lParam), m);
        break;
    case WM_CLOSE:
        freeTextData(&textData);
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    case WM_KEYDOWN:
        OnKeyDown(hWnd, wParam, &textData, &renderData, &textmetric, m);
        break;
    case WM_VSCROLL:
        OnVScroll(hWnd, wParam, &textData, &renderData, &textmetric, m);
        break;
    case WM_HSCROLL:
        OnHScroll(hWnd, wParam, &textData, &renderData, &textmetric, m);
        break;
    case WM_COMMAND:
        hMenu = GetMenu(hWnd);
        switch (LOWORD(wParam))
        {
        case MENU_BARS_ON:
            CheckMenuItem(hMenu, MENU_BARS_ON, MF_CHECKED);
            CheckMenuItem(hMenu, MENU_BARS_OFF, MF_UNCHECKED);
            m = BARS_ON;
            ShowScrollBar(hWnd, SB_HORZ, TRUE);
            ShowScrollBar(hWnd, SB_VERT, TRUE);
            invalidateScreen(hWnd, &renderData, &textmetric);
            break;
        case MENU_BARS_OFF:
            CheckMenuItem(hMenu, MENU_BARS_ON, MF_UNCHECKED);
            CheckMenuItem(hMenu, MENU_BARS_OFF, MF_CHECKED);
            ShowScrollBar(hWnd, SB_HORZ, FALSE);
            ShowScrollBar(hWnd, SB_VERT, FALSE);
            m = BARS_OFF;
                renderData.curLineInStr = 0;
            invalidateScreen(hWnd, &renderData, &textmetric);
            break;
        case MENU_OPEN:

                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFile = szFile;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            GetOpenFileName(&ofn);
            if (!readFile(ofn.lpstrFile, &textData))
                MessageBox(hWnd, "File is not found or no memory", NULL, MB_OK);
            Reboot(hWnd, &renderData);
            invalidateScreen(hWnd, &renderData, &textmetric);
            break;
        case MENU_QUIT:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case WM_QUIT:
        freeTextData(&textData);
        break;
    default:                      /* for msg that we don't deal with */
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
