//
// Created by ILICH on 09.02.2021.
//
#ifndef __UTILS_H_
#define __UTILS_H_

#include "editor.h"

/* Move text with controls functions */
void LineUp(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData );
void LineDown(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData );
void PageUp(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData );
void PageDown(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData );

/* Mapping from text representation to scroll space functions */
int textWidthToHScroll(TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData, int minScroll, int maxScroll );
int textHeightToVScroll(TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData, int minScroll, int maxScroll );

/* Inverse mapping functions */
int hScrollToTextWidth(TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData, int scrollPos, int minScroll, int maxScroll );
int vScrollToTextHeight( int endYLeftUp, int scrollPos, int minScroll, int maxScroll );

/* Invalidate screen function */
void invalidateScreen(HWND hWnd, TEXT_RENDER_DATA *textRenderData, TEXTMETRIC *textmetric );

#endif // __UTILS_H_
