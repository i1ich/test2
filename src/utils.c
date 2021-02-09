//
// Created by ILICH on 09.02.2021.
//
#include "utils.h"

int textWidthToHScroll(TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData, int minScroll, int maxScroll )
{
    if (textData->maxStrWidth - textRenderData->symbolsPerW - 1 > 0)
        return minScroll + textRenderData->xLeftUp * (float)(maxScroll - minScroll) /
                           (textData->maxStrWidth - textRenderData->symbolsPerW - 1);
    return minScroll + textRenderData->xLeftUp * (float)(maxScroll - minScroll) /
                       (textRenderData->symbolsPerW - 1);
}

int textHeightToVScroll(TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData, int minScroll, int maxScroll )
{
    if (textData->strCount - 1 - textRenderData->symbolsPerH > 0)
        return minScroll + textRenderData->yLeftUp * (float)(maxScroll - minScroll) /
                           (textData->strCount - 1 - textRenderData->symbolsPerH);
    return minScroll + textRenderData->yLeftUp * (float)(maxScroll - minScroll) /
                       (textData->strCount - 1);
}

int hScrollToTextWidth(TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData, int scrollPos, int minScroll, int maxScroll )
{
    if (textData->maxStrWidth - textRenderData->symbolsPerW - 1 > 0)
            return (float)(scrollPos - minScroll) / (maxScroll - minScroll) *
                (textData->maxStrWidth - textRenderData->symbolsPerW - 1);

    return 0;
}

int vScrollToTextHeight( int endYLeftUp, int scrollPos, int minScroll, int maxScroll )
{
    return (float)(scrollPos - minScroll) / (maxScroll - minScroll) * endYLeftUp;
}

void LineUp(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData )
{
    if (m == BARS_ON)
        textRenderData->yLeftUp = max(0, textRenderData->yLeftUp - 1);
    else
    {
        int strTL;
        if (textRenderData->curLineInStr <= 0)
        {
            textRenderData->yLeftUp--;
            if (textRenderData->yLeftUp < 0)
            {
                textRenderData->yLeftUp = 0;
                textRenderData->curLineInStr = 0;
                return;
            }
            strTL = strTextLength(textData, textRenderData->yLeftUp);
            textRenderData->curLineInStr = max(0, linesInCurStr(strTL, textRenderData) - 1);
        }
        else
            textRenderData->curLineInStr--;
    }
}

void LineDown(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData )
{
    int endYLeftUp, endCurLineInStr;
    endOfDocument(m, textData, textRenderData, &endYLeftUp, &endCurLineInStr);
    if (m == BARS_ON)
        textRenderData->yLeftUp = min(textRenderData->yLeftUp + 1, endYLeftUp);
    else
    {
        int strTL = strTextLength(textData, textRenderData->yLeftUp);

        if (textRenderData->curLineInStr >= linesInCurStr(strTL, textRenderData) - 1 ||
            textRenderData->yLeftUp == endYLeftUp)
        {
            textRenderData->yLeftUp = min(endYLeftUp, textRenderData->yLeftUp + 1);
            textRenderData->curLineInStr = (textRenderData->yLeftUp == endYLeftUp) * endCurLineInStr;
        }
        else if (!(textRenderData->yLeftUp == endYLeftUp &&
                   textRenderData->curLineInStr == endCurLineInStr))
            textRenderData->curLineInStr++;
    }
}

void PageUp(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData )
{
    if (m == BARS_ON)
        textRenderData->yLeftUp -= min(textRenderData->yLeftUp, textRenderData->symbolsPerH - 1);
    else
    {
        int skipped = 0, toSkip = textRenderData->symbolsPerH - 1; // how many strings are printed
        for (; skipped < toSkip;)
        {
            int skippedOnIteration = min(textRenderData->curLineInStr + 1, toSkip - skipped);

            if (skippedOnIteration == toSkip - skipped && textRenderData->curLineInStr != 0)
                textRenderData->curLineInStr -= skippedOnIteration;
            else
            {
                textRenderData->yLeftUp--;
                /* start of document */
                if (textRenderData->yLeftUp < 0)
                {
                    textRenderData->yLeftUp = 0;
                    textRenderData->curLineInStr = 0;
                    return;
                }
                int strTL = strTextLength(textData, textRenderData->yLeftUp);
                textRenderData->curLineInStr = linesInCurStr(strTL, textRenderData) - 1;
            }

            skipped += skippedOnIteration;
        }
    }
}

void PageDown(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *textRenderData )
{
    int endYLeftUp, endCurLineInStr;
                endOfDocument(m, textData, textRenderData, &endYLeftUp, &endCurLineInStr);

    if (m == BARS_ON)
        textRenderData->yLeftUp = min(endYLeftUp,
                                      textRenderData->yLeftUp + textRenderData->symbolsPerH - 1);
    else
    {
        int skipped = 0, toSkip = textRenderData->symbolsPerH - 1; // how many strings are printed
        for (; skipped < toSkip;)
        {
            int strTL = strTextLength(textData, textRenderData->yLeftUp);
            int lineCount = linesInCurStr(strTL, textRenderData);
            int skippedOnIteration = min(lineCount - 1 - textRenderData->curLineInStr + 1, toSkip - skipped);

            textRenderData->curLineInStr =
                /*(skippedOnIteration == lineCount - textRenderData->curLineInStr) * 0 +*/
                (skippedOnIteration == toSkip - skipped) * (textRenderData->curLineInStr + skippedOnIteration);
            skipped += skippedOnIteration;
            textRenderData->yLeftUp += (textRenderData->curLineInStr == 0);

            // finish of document
            if (textRenderData->yLeftUp >= endYLeftUp)
            {
                textRenderData->yLeftUp = endYLeftUp;
                textRenderData->curLineInStr = endCurLineInStr;
            }
        }
    }
}

void invalidateScreen(HWND hWnd, TEXT_RENDER_DATA *textRenderData, TEXTMETRIC *textmetric )
{
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = (textRenderData->symbolsPerW + 1) * textmetric->tmAveCharWidth;
    rc.bottom = (textRenderData->symbolsPerH + 1) * textmetric->tmHeight;
    InvalidateRect(hWnd, &rc, TRUE);
}
