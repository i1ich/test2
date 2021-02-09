//
// Created by ILICH on 09.02.2021.
//
#include <stdio.h>
#include <stdlib.h>
#include "editor.h"

int symsCount( char *arr, int len, char sym )
{
    int i;
    int cnt = 0;
    for (i = 0; i < len; i++)
        cnt += arr[i] == sym;

    return cnt;
}

BOOL readFile(char const *name, TEXT_DATA *textData )
{
    int i, curStr = 1, len;
    char *buf;
    FILE *F = fopen(name, "rb");

    if (F == NULL)
        return FALSE;

    fseek(F, 0, SEEK_END);
    len = ftell(F);
    fseek(F, SEEK_SET, 0);

    freeTextData(textData);

    buf = malloc(len + 1);
    if (buf == NULL)
        return FALSE;

    int resLen = fread(buf, 1, len, F);
    if (resLen != len)
    {
        free(buf);
        return FALSE;
    }

    textData->strCount = symsCount(buf, len, '\n') + 1;
    textData->text = buf;
    // +1 cause save last sym offset
    textData->strOffsets = malloc(sizeof(int) * (textData->strCount + 1));

    if (textData->strOffsets == NULL)
    {
        free(textData->text);
        return FALSE;
    }

    textData->strOffsets[0] = 0;
    for (i = 1; i < len; i++)
        if (buf[i] == '\n')
            textData->strOffsets[curStr++] = i + 1;
    textData->strOffsets[textData->strCount] = len;

    textData->maxStrWidth = findMaxStrWidth(textData, 0, textData->strCount);
    return TRUE;
}

void freeTextData(TEXT_DATA *textData )
{
    if (textData->strOffsets != NULL)
    {
        free(textData->strOffsets);
        textData->strOffsets = NULL;
    }

    if (textData->text != NULL)
    {
        free(textData->text);
        textData->text = NULL;
    }
}

int strByteLength(TEXT_DATA *textData, int number )
{
    return textData->strOffsets[number + 1] - textData->strOffsets[number];
}

int strTextLength(TEXT_DATA *textData, int number )
{
    return textData->strOffsets[number + 1] - textData->strOffsets[number] - (textData->text[textData->strOffsets[number + 1] - 2] == '\r');
}

int linesInCurStr(int strTL, TEXT_RENDER_DATA *textRenderData )
{
    return strTL / textRenderData->symbolsPerW + (strTL % textRenderData->symbolsPerW != 0);
}

int findMaxStrWidth(TEXT_DATA *textData, int yStart, int yEnd )
{
    int i;
    int maxLen = 0;
    for (i = yStart; i < yEnd; i++)
        if (textData->strOffsets[i + 1] - textData->strOffsets[i] > maxLen)
            maxLen = strTextLength(textData, i);

    return maxLen;
}

void endOfDocument(MODE m, TEXT_DATA *textData, TEXT_RENDER_DATA *trd,
                   int *endYLeftUp, int *endCurLineInStr )
{
    int passed = 0;
    int passedOnIteration;
    int lines;
    if (m == BARS_OFF)
    {
        *endYLeftUp = textData->strCount - 1;
        for (; passed < trd->symbolsPerH;)
        {
            int
                strTL = strTextLength(textData, *endYLeftUp);

            lines = linesInCurStr(strTL, trd);
            passedOnIteration = min(lines, trd->symbolsPerH - passed);
            passed += passedOnIteration;

            if (passed == trd->symbolsPerH)
                break;

            *endYLeftUp -= (passedOnIteration == lines);
            if (*endYLeftUp < 0)
            {
                *endYLeftUp = 0;
                passedOnIteration = lines; // for *endCurLineInStr = ... make 0
                break;
            }
        }
        *endCurLineInStr = lines - passedOnIteration;
    }
    else
    {
        *endYLeftUp = max(0, textData->strCount - trd->symbolsPerH);
        *endCurLineInStr = 0;
    }
}
