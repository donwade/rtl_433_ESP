#ifndef _WXUI
#define _WXUI

#include <M5GFX.h> 
#include <WxCommon.h>
#include <WxUI.h>
#include "gfxfont.h"

typedef int TFT_COLOUR;

extern char *getHHMMSS(uint32_t utc);
extern unsigned long getUTC();

#if 0
extern void WxWindDrawItem(const char *valueName,
                    TFT_COLOUR gfxColour, 
                    float value, 
                    float valueMin, 
                    float valueMax);
#endif

void WxWindDrawItem2(ITEM &item);

#endif

