#ifndef _WXUI
#define _WXUI

#include <M5GFX.h> 
#include <WxData.h>
#include <WxUI.h>
#include "gfxfont.h"

typedef int TFT_COLOUR;

extern char *getHHMMSS(uint32_t utc);

#if 0
extern void WxWindDrawItem(const char *valueName,
                    TFT_COLOUR gfxColour, 
                    float value, 
                    float valueMin, 
                    float valueMax);
#endif

void WxWindDrawItem2(ITEM &item);

#endif

