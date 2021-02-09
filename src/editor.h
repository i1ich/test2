//
// Created by ILICH on 09.02.2021.
//
#ifndef __EDITOR_H_
#define __EDITOR_H_

#include <windows.h>

#include "menu.h"

/* Render mode */
typedef enum
{
    BARS_ON = MENU_BARS_ON,
    BARS_OFF = MENU_BARS_OFF
} MODE;

typedef struct tagTEXT_DATA {
    char *text;
    int *strOffsets;
    int strCount;
    int maxStrWidth;
} TEXT_DATA;

typedef struct tagTEXT_RENDER_DATA {
    int xLeftUp, yLeftUp;
    int symbolsPerW, symbolsPerH;
    int curLineInStr;
} TEXT_RENDER_DATA;

/* Read file functions */
BOOL readFile(char const *name, TEXT_DATA *textData );

/* Find width of the widest string in text function */
int findMaxStrWidth(TEXT_DATA *textData, int yStart, int yEnd );

/* Free memory of textData function */
void freeTextData(TEXT_DATA *textData );

/* Find string length in bytes function */
int strByteLength(TEXT_DATA *textData, int number );

/* Find string length in chars function */
int strTextLength(TEXT_DATA *textData, int number );

/* Count of lines to be rendered in BARS_OFF mode */
int linesInCurStr(int strTL, TEXT_RENDER_DATA *textRenderData );

/* Find document end function */
void endOfDocument(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *trd,
                   int *endYLeftUp, int *endCurLineInStr );

#endif // __EDITOR_H_
